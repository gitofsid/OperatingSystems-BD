
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>

//find the time difference 
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

//global variables
pthread_mutex_t num_mutex;
pthread_cond_t var1,var2;
struct timespec stop,start;
unsigned long long result = 0;
int iterations = 200;

int num = 0;

//switches num value from 1 to 0
void *num_switcher_1to0()
{
	int i = 0;
	while ( i < iterations) {
		pthread_mutex_lock(&num_mutex);
		//while num is 0 keep waiting for the signal
		while (num == 0)
			pthread_cond_wait(&var1, &num_mutex);

		//switch to 0
		num = 0;

		//stop the clock as now wait is over and thread is switched
		clock_gettime(CLOCK_MONOTONIC, &stop);

		//keep accumulating result
		result= result + timespecDiff(&stop,&start);

		//send signal to other halted thread
		pthread_cond_signal(&var2);
		pthread_mutex_unlock(&num_mutex);
		i++;
	}
}

//switches num value from 0 to 1
void *num_switcher_0to1()
{
	int i = 0;
	while ( i < iterations) {
		pthread_mutex_lock(&num_mutex);

		//while num is 1 keep waiting for the signal
		while (num == 1)
			pthread_cond_wait(&var2, &num_mutex);

		//switch to 1
		num = 1;
		
		//start the clock as we are about to send the signal for switch
		clock_gettime(CLOCK_MONOTONIC, &start);
		pthread_cond_signal(&var1);	
		pthread_mutex_unlock(&num_mutex);
		i++;
	}
}

int main(void)
{
	int rt1, rt2;
	pthread_t t1, t2;
	cpu_set_t set;
   	int cpu = 0;

   	CPU_ZERO(&set);

   	CPU_SET(cpu, &set);

   	//initialize to 0 to avoid gabage values
   	start.tv_sec = 0, stop.tv_sec = 0;
	start.tv_nsec = 0, stop.tv_nsec = 0;

	//set the running core to core 0
   	if (sched_setaffinity(0, sizeof(set), &set) == -1)
        perror("sched_setaffinity not set");

    //start thread 1
	if( (rt1=pthread_create( &t1, NULL, &num_switcher_1to0, NULL)) )
		printf("Thread 1 creation failed: %d\n", rt1);

	//start thread 2
	if( (rt2=pthread_create( &t2, NULL, &num_switcher_0to1, NULL)) )
		printf("Thread 2 creation failed: %d\n", rt2);

	//join
	pthread_join( t1, NULL);
    pthread_join( t2, NULL);

    //print the final average cost of switch
    printf("Cost of thread switching is %llu ns\n", result/iterations);

    return 0; 		
}