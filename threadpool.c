//这是一个线程池代码

//threadpool.hpp
//#ifndef __M_POOL_H__
//#define __M_POOL_H__ 
#include<iostream>
#include<queue>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

#define MAX_THREAD 3
#define MAX_QUEUE 10

class MyTask
{
  private:
    int data;
  public:
    MyTask()
    {}
    ~MyTask()
    {}
    void SetData(int _data)
    {
      data = _data;
    }
    void Run()
    {
      int t = rand()%5;
      printf("thread:%lu sleep %d sec,execute data:%d\n",pthread_self(),t,data);
      sleep(t);
    }
};
class MyThreadPool
{
  private:
    int max_thread; //线程池中最大线程数
    int cur_thread; //当前线程池中的线程数
    int keep_running; //用于停止线程池中线程的标志
    int max_queue;  //队列的最大节点数
    pthread_t *tid; //线程的线程ID
    std::queue<MyTask *> task_list;
    pthread_mutex_t mutex;
    pthread_cond_t empty;
    pthread_cond_t full;
    void ThreadLock()
    {
      pthread_mutex_lock(&mutex);
    }
    void ThreadUnlock()
    {
      pthread_mutex_unlock(&mutex);
    }
    void ConsumerWait()
    {
      pthread_cond_wait(&empty,&mutex);
    }
    void ConsumerNotify()
    {
      pthread_cond_signal(&empty); 
    }
    void ProducterWait()
    {
      pthread_cond_wait(&full,&mutex);
    }
    void ProducterNotify()
    {
      pthread_cond_signal(&full);
    }
    bool ThreadIsRunning()
    {
      return (keep_running == 0?false:true);
    }
    void ThreadExit()
    {
      cur_thread--;
      //线程池中的线程退出时通知一下主线程
      printf("thread:%lu exit\n",pthread_self());
      ProducterNotify();
      pthread_exit(NULL);
    }
    bool QueueIsEmpty()
    {
      return (task_list.size()==max_queue?true:false);
    }
    bool QueueIsFull()
    {
      return (task_list.size()==max_queue?true:false);
    }
    void PopTask(MyTask **task)
    {
      *task = task_list.front();
      task_list.pop();
      return;
    }
    void PushTask(MyTask* task)
    {
      task_list.push(task);
    }
    static void *thread_routine(void* arg)
    {
      MyThreadPool* pthreadpool = (MyThreadPool*)arg;
      while(1)
      {
        pthreadpool->ThreadLock();
        //若当前是运行状态，并且没有任务，则挂起等待
        while(pthreadpool->QueueIsEmpty()&&pthreadpool->ThreadIsRunning())
        {
          pthreadpool->ConsumerWait();
        }
        //若当前是非运行状态，且没有任务则退出线程
        //若当前是非运行状态，但是队列中有任务，则需要将任务执行完毕后才能退出
        if(!pthreadpool->ThreadIsRunning()&&pthreadpool->QueueIsEmpty())
        {
          pthreadpool->ThreadUnlock();
          pthreadpool->ThreadExit();
        }
        //能够走下来，则任务队列不为空，那么获取一个任务执行
        MyTask *task;
        pthreadpool->PopTask(&task);
        pthreadpool->ProducterNotify();
        pthreadpool->ThreadUnlock();
        //执行任务，不能在锁内，否则导致其他线程饥饿
        task->Run();
      }
      return NULL;
    }
  public:
    MyThreadPool(int _max_thread = MAX_THREAD,int _max_queue = MAX_QUEUE):\
      max_thread(_max_thread),max_queue(_max_queue),cur_thread(_max_thread),keep_running(1)
  {
    int i=0;

    printf("create threaed:%d-%d\n",max_thread,max_queue);
    tid = (pthread_t*)malloc(sizeof(pthread_t)* _max_thread);
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&full,NULL);
    pthread_cond_init(&empty,NULL);
    //创建固定数量的线程，等待执行任务
    for(int i=0;i<_max_thread;i++)
    {
      int ret = pthread_create(&tid[i],NULL,thread_routine,(void*)this);
      if(ret!=0)
      {
        printf("create thread error\n");
        exit(0);
      }
      pthread_detach(tid[i]);
    }
  }
  ~MyThreadPool()
  {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
  }
  //向线程池任务队列中添加任务
  bool AddTaskToPool(MyTask *task)
  {
    ThreadLock();
    while(QueueIsFull())
    {
      ProducterWait();
    }
    PushTask(task);
    printf("add task to pool\n");
    ConsumerNotify();
    ThreadUnlock();
    return true;
  }
  void StopThreadPoll()
  {
    //若已经调用过线程池退出，则返回
    if(keep_running==0)
    {
      return;
    }
    ThreadLock();
    keep_running = 0;
    //如果还有线程没有退出，则挂起等待
    //等待所有线程将队列中的所有任务都执行完毕后并且退出
    while(cur_thread>0)
    {
      ProducterWait();
    }
    ThreadUnlock();
  }
};


//#endif

int main()
{
  MyTask task[10];
  int i;

  MyThreadPool pool;
  for(int i=0;i<10;i++)
  {
    task[i].SetData(i);
    pool.AddTaskToPool(&task[i]);
  }

  pool.StopThreadPoll();
  return 0;
}
