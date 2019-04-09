//这是一个演示读写锁的代码
#include<vector>
#include<sstream>
#include<cstdio>
#include<cstdlib>
#include<unistd.h>
#include<pthread.h>

volatile int ticket = 1000;
pthread_rwlock_t rwlock;

void* reader(void* arg)
{
  char* id = (char*)arg;
  while(1)
  {
    pthread_rwlock_rdlock(&rwlock);
    if(ticket<=0)
    {
      pthread_rwlock_unlock(&rwlock);
      break;
    }
    printf("%s: %d\n",id,ticket);
    pthread_rwlock_unlock(&rwlock);
    usleep(1);
  }
  return nullptr;
}

void* writer(void* arg)
{
  char* id = (char*)arg;
  while(1)
  {
    pthread_rwlock_wrlock(&rwlock);
    if(ticket<=0)
    {
      pthread_rwlock_unlock(&rwlock);
      break;
    }
    printf("%s: %d\n",id,--ticket);
    pthread_rwlock_unlock(&rwlock);
    usleep(1);
  }
  return nullptr;
}

struct ThreadAttr
{
  pthread_t tid;
  std::string id;
};

std::string create_reader_id(std::size_t i)
{
  //利用 ostringstream 进行 string 拼接
  std::ostringstream oss("thread reader ",std::ios_base::ate);
  oss<<i;
  return oss.str();
}

std::string create_writer_id(std::size_t i)
{
  //利用 ostringstream 进行 string 拼接
  std::ostringstream oss("thread writer ",std::ios_base::ate);
  oss<<i;
  return oss.str();
}

void init_readers(std::vector<ThreadAttr>& vec)
{
  for(std::size_t i=0;i<vec.size();++i)
  {
    vec[i].id = create_reader_id(i);

    pthread_create(&vec[i].tid,nullptr,reader,(void*)vec[i].id.c_str());
  }
}

void init_writers(std::vector<ThreadAttr>& vec)
{
  for(std::size_t i=0;i<vec.size();++i)
  {
    vec[i].id = create_writer_id(i);

    pthread_create(&vec[i].tid,nullptr,writer,(void*)vec[i].id.c_str());
  }
}

void join_threads(std::vector<ThreadAttr> const& vec)
{
  //我们按创建的 逆序 来进行线程的回收
  for(std::vector<ThreadAttr>::const_reverse_iterator it = vec.rbegin();it!=vec.rend();++it)
  {
    pthread_t const& tid = it->tid;
    pthread_join(tid,nullptr);
  }
}

void init_rwlock()
{
#if 0 //写优先
  pthread_rwlockattr_t attr;
  pthread_rwlock_init(&attr);
  pthread_rwlockattr_setkind_up(&attr,PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
  pthread_rwlock_init(&rwlock,&attr);
  pthread_rwlockattr_destroy(&attr);
#else //读优先，会造成写饥饿
  pthread_rwlock_init(&rwlock,nullptr);
#endif 
}

int main()
{
  //测试效果不明显的情况下，可以加大reader_nr
  //但也不能太大，超过一定阈值后系统就调度不了主线程了
  const std::size_t reader_nr = 1000;
  const std::size_t writer_nr = 2;

  std::vector<ThreadAttr> readers(reader_nr);
  std::vector<ThreadAttr> writers(writer_nr);

  init_rwlock();

  init_readers(readers);
  init_writers(writers);

  join_threads(writers);
  join_threads(readers);

  pthread_rwlock_destroy(&rwlock);
}
