all:create join detach ticket mutex cond prodcons sem rwlock threadpool
create:create.c
	g++ -o $@ $^ -lpthread
join:join.c
	g++ -o $@ $^ -lpthread 
detach:detach.c
	g++ -o $@ $^ -lpthread 
ticket:ticket.c	
	gcc -o $@ $^ -lpthread
mutex:mutex.c
	gcc -o $@ $^ -lpthread 
cond:cond.c
	gcc -o $@ $^ -lpthread 
prodcons:prodcons.c
	g++ -o $@ $^ -lpthread 
sem:sem.c
	g++ -o $@ $^ -lpthread 
rwlock:rwlock.c
	g++ -std=c++11 -Wall -Werror $^ -o $@ -lpthread
threadpool:threadpool.c
	g++ -std=c++11 -o $@ $^ -lpthread -lrt
.PHONY:clean
clean:
	rm -rf create join detach ticket mutex cond prodcons sem rwlock threadpool
