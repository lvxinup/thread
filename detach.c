#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>


void* thread_run(void *arg)
{
  pthread_detach(pthread_self());
  printf("thread detaching ...\n");
  return NULL;
}

int main()
{
  pthread_t tid;
  if(pthread_create(&tid,NULL,thread_run,NULL)!=0)
  {
    printf("create thread error\n");
    return 1;
  }
  
  int ret = 0;
  sleep(1);//让线程先分离，再等待

  if(pthread_join(tid,NULL)==0)
  {
    printf("pthread wait success\n");
  }
  else 
  {
    printf("pthread wait failed \n");
    ret = 1;
  }
  return ret;
}
