#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>

// find the time difference in ns
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}


int main(void)
{
        int     fd_1[2], fd_2[2], rcvd_bytes;
        pid_t   child_pid;
        char    string_ch[] = "C"; //a byte tp be sent or read
        char    string_pa[] = "P"; //a byte tp be sent or read
        char    readbuffer[5]; 
        cpu_set_t set;
        int pCPU = 0, cCPU = 0; //both processes will run on cpu 0

        struct timespec start,stop;
        unsigned long long result;

        CPU_ZERO(&set); //clear the current set cpu

        //pipes created and will be inherited by child
        if(pipe(fd_1) == -1 || pipe(fd_2) == -1) //create two pipes
        {
            perror("pipe failed to open");
            exit(1);
        }

        if((child_pid = fork()) == -1) //forking here
        {
            perror("fork failed");
            exit(1);
        }

        //fork success and pipe success
        if(child_pid == 0) // this is child
        {
            CPU_SET(cCPU, &set); //setting cpu to 0
            int i;

            if (sched_setaffinity(0, sizeof(set), &set) == -1) //run this process on cpu 0
                perror("sched_setaffinity not set");
 
            close(fd_1[1]); //for reading only
            close(fd_2[0]); //for writing only


            while(read(fd_1[0],readbuffer,sizeof(readbuffer)) > 0) //reading success
            {
                //printf("\nIn child process:\n");
                //printf("Received string from main: %s\n", readbuffer);
                //printf("Pid of this child process: %d\n", (int)getpid());
                write(fd_2[1], string_ch, strlen(string_ch)+1);
            } 

            exit(0);
        }   else // parent process
        {
            int i =0 ;
            unsigned long long sum = 0;

            CPU_SET(pCPU, &set);

            if (sched_setaffinity(0, sizeof(set), &set) == -1) //set this process to run on cpu0
                perror("sched_setaffinity not set");
            
            while ( i < 200) {
                

                close(fd_1[0]); //for writing only
                close(fd_2[1]); //for reading only

                if(write(fd_1[1], string_pa, strlen(string_pa)+1) != -1) //write in parent
                {
                    clock_gettime(CLOCK_MONOTONIC, &start);
                    read(fd_2[0], readbuffer, sizeof(readbuffer));
                    clock_gettime(CLOCK_MONOTONIC, &stop);
                    sum = sum + timespecDiff(&stop,&start);
                    //printf("\nIn parent process:\n");
                    //printf("Received string from child: %s\n", readbuffer);
                    //printf("Child pid: %d Parent pid: %d\n", child_pid, (int)getpid());
                }
                i++;

            }
            printf("\nCLOCK_MONOTONIC Measured from pa to ch: %llu\n",sum/(2*200)); //there are two context switch and we are meaureing is once so divide by 2
        }




        return(0);
}

