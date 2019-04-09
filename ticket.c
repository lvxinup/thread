#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>

pthread_mutex_t mutex;

int ticket = 100;

void* route(void* arg)
{
  char *id = (char*)arg;
  while(1)
  {
    pthread_mutex_lock(&mutex);
    if(ticket>0)
    {
      usleep(1000);
      printf("%s sells ticket:%d\n",id,ticket);
      ticket--;
      pthread_mutex_unlock(&mutex);
    }
    else 
    {
      pthread_mutex_unlock(&mutex);
      break;
    }
  }
}

int main()
{
  pthread_t t1,t2,t3,t4;

  pthread_create(&t1,NULL,route,"thread 1");
  pthread_create(&t2,NULL,route,"thread 2");
  pthread_create(&t3,NULL,route,"thread 3");
  pthread_create(&t4,NULL,route,"thread 4");

  pthread_join(t1,NULL);
  pthread_join(t2,NULL);
  pthread_join(t3,NULL);
  pthread_join(t4,NULL);
  
}
