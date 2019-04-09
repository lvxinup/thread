//基于环形队列的生产消费模型
#include<unistd.h>
#include<semaphore.h>
#include<vector>
#include<stdlib.h>
#include<iostream>
#include<pthread.h>

#define NUM 16

class RingQueue
{
  private:
    std::vector<int> q;
    int cap;
    sem_t data_sem; //表示队列中数据的数量
    sem_t space_sem;  //表示队列中空间的大小
    int consume_step; 
    int product_step;
  public:
    RingQueue(int _cap = NUM):q(_cap),cap(_cap)
    {
      sem_init(&data_sem,0,0);
      sem_init(&space_sem,0,cap);
      consume_step = 0;
      product_step = 0;
    } 
    void PutData(const int&data)
    {
      sem_wait(&space_sem);
      q[product_step] = data;
      product_step++;
      product_step %= cap;
      sem_post(&data_sem);
    }
    void GetData(int &data)
    {
      sem_wait(&data_sem);
      data = q[product_step];
      product_step++;
      product_step %= cap;
      sem_post(&space_sem);
    }
    ~RingQueue()
    {
      sem_destroy(&data_sem);
      sem_destroy(&space_sem);
    }
};

void* consumer(void* arg) //消费者
{
  RingQueue* rqp = (RingQueue*)arg;
  int data;
  for(;;)
  {
    rqp->GetData(data);
    std::cout<<"Consume data done: "<<data<<std::endl;
    sleep(1);
  }
}

void* producter(void* arg)  //生产者
{
  RingQueue* rqp = (RingQueue*)arg;
  srand((unsigned long)time(NULL));
  for(;;)
  {
    int data = rand()%1024;
    rqp->PutData(data);
    std::cout<<"Product data done: "<<data<<std::endl;
  }
}

int main()
{
  RingQueue rq;
  pthread_t c,p;
  
  pthread_create(&c,NULL,consumer,(void*)&rq);
  pthread_create(&p,NULL,producter,(void*)&rq);

  pthread_join(c,NULL);
  pthread_join(p,NULL);

  return 0;
}
