#include <stdio.h>
#include <stdint.h>
#include <time.h>

// this function find the elapsed time after timer started till its stopped. timespec structure 
// has two variables tv_sec and tv_nsec with values in second and nanosecond but 
// the difference is reported in ns
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}


int main()
{
struct timespec start; //instatiate start of timespec type
struct timespec stop; //instatiate stop of timespec type
unsigned long long result; //64 bit integer - this will store the elapsed time

clock_gettime(CLOCK_REALTIME, &start); //this clock type will try to match time on wall clock and give real time at moment to store in start
sleep(1); //sleep the current process for 1 sec
clock_gettime(CLOCK_REALTIME, &stop); //this clock type will try to match time on wall clock and give real time at moment to store in stop

result=timespecDiff(&stop,&start); //call the timespecDiff function to find the elapsed time between start and stop

printf("CLOCK_REALTIME Measured: %llu\n",result); //print the difference when REALTIME clock is used

clock_gettime(CLOCK_MONOTONIC, &start); //this clock type will get the time from an unspecified point in time (for more accuracy) store in start
sleep(1);
clock_gettime(CLOCK_MONOTONIC, &stop); //this clock type will get the time from an unspecified point in time (for more accuracy) store in stop

result=timespecDiff(&stop,&start);

printf("CLOCK_MONOTONIC Measured: %llu\n",result); //print the difference when MONOTONIC clock is used

clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); //this clock type will get how much cpu time the process is taking at moment and store in start
sleep(1);
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);  //this clock type will get how much cpu time the process is taking at moment and store in stop

result=timespecDiff(&stop,&start);

printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result); //print the difference when PROCESS_CPUTIME_ID clock is used

clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);  //this clock type will get how much cpu time a thread is taking at moment and store in start
sleep(1);
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop); //this clock type will get how much cpu time a thread is taking at moment and store in stop

result=timespecDiff(&stop,&start);

printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result); //print the difference when THREAD_CPUTIME_ID clock is used


}