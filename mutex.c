//这是一个练习死锁的代码
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

int ticket = 100;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

void* route1(void* arg)
{
  char* id = (char*)arg;
  while(1)
  {
    pthread_mutex_lock(&mutex1);
    if(ticket>0)
    {
      sleep(1);
      printf("%s sells ticket:%d\n",id,ticket);
      ticket--;
    }
    pthread_mutex_lock(&mutex2);
    if(ticket>0)
    {
      sleep(1);
      printf("%s sells ticket:%d\n",id,ticket);
      ticket--;
    }
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    sleep(3);
  }
}
void* route2(void* arg)
{
  char* id = (char*)arg;
  while(1)
  {
    //pthread_mutex_lock(&mutex2);  // 加锁顺序不一致时，会构成死锁
    pthread_mutex_lock(&mutex1);  //解决方法
    if(ticket>0)
    {
      sleep(1);
      printf("%s sells ticket:%d\n",id,ticket);
      ticket--;
    }
    //pthread_mutex_lock(&mutex1); // 加锁顺序不一致时，会构成死锁。
    pthread_mutex_lock(&mutex2);  //解决方法
    if(ticket>0)
    {
      sleep(1);
      printf("%s sells ticket:%d\n",id,ticket);
      ticket--;
    }
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    sleep(3);
  }
}

int main()
{
  pthread_t t1,t2;

  pthread_create(&t1,NULL,route1,"thread 1");
  pthread_create(&t2,NULL,route2,"thread 2");

  pthread_join(t1,NULL);
  pthread_join(t2,NULL);

  pthread_mutex_destroy(&mutex1);
  pthread_mutex_destroy(&mutex2);
  return 0;
}
