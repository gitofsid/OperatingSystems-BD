#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//for calculation of time elapsed
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

//an empty function
void dumb_func(void)
{
    //do nothing
}

int main(void)
{
    struct timespec start;
    struct timespec stop;
    unsigned long long result; //64 bit integer
    int i = 0, sum = 0;

    //initialize to 0 to avoid gabage values
    start.tv_sec = 0, stop.tv_sec = 0;
    start.tv_nsec = 0, stop.tv_nsec = 0;

    //looping 200 times to find average value of function call
    while(i < 200) {
        //start clock, call fucntion and stop clock
        clock_gettime(CLOCK_MONOTONIC, &start);
        dumb_func();
        clock_gettime(CLOCK_MONOTONIC, &stop);

        //accumulate time
        sum += timespecDiff(&stop,&start);
        i++;

    }

    //report average
    result = sum/200;
    printf("Minimal function call cost is: %llu\n",result);

    return 0;

}
