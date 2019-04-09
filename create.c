#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<string.h>

void* rout(void* arg)
{
  //线程入口函数
  while(1)
  {
    printf("i am thread ! i am sleep!!! thread id: %ld\n",pthread_self());
    sleep(3);
  }
}

int main(void)
{
  pthread_t tid;
  int ret;
  if((ret=pthread_create(&tid,NULL,rout,NULL))!=0)
  {
    fprintf(stderr,"pthread_create: %s",strerror(ret));
    exit(EXIT_FAILURE);
  }
  while(1)
  {
    printf("i am main thread!! thread id: %ld\n",pthread_self());
    sleep(1);
  }
}
