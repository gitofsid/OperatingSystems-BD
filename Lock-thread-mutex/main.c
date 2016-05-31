
#include "main.h"



void *fnC()
{
    int i;
    for(i=0;i<1000000;i++)
    {   
        c++;
    }   
}


/* this is the regular pthread mutex test
based on builtin mutex lock*/
void *pthreadMutexTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		pthread_mutex_lock(&count_mutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_mutex_unlock(&count_mutex);    
	
    }   
}


/* this is the regular pthread spin mutex test
	based on builtin mutex lock*/
void *pthreadSpinTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		pthread_spin_lock(&count_spin);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_spin_unlock(&count_spin);    
	
    }   
}
 
/* this is test and test and set implementation for the custom
spink lock*/

void *mySpinLockTTASTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_spinlock_lockTTAS(&mspinlock);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_spinlock_unlock(&mspinlock);   
	
    }
}


/* this is test and set test for custom spin lock */
void *mySpinLockTASTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_spinlock_lockTAS(&mspinlock);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_spinlock_unlock(&mspinlock);   
	
    }
}

/* this is test and test and set test for custom mutex lock */
void *myMutexLockTest() //this is TTAS
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_mutex_lock(&mmutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_mutex_unlock(&mmutex);   
	
    }
}

/* this is test for ticket based queue lock */
void *myQueueLockTest() //queue
{
	int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_queuelock_lock(&mqueue);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_queuelock_unlock(&mqueue);   	
    }

}


int runTest(int testID)
{

	
	int i;
	int rt;

	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);
	if(threads == NULL) {
		printf("Cant allocate memory in runTest");
		return -1;
	}


/*You must create all data structures and threads for you experiments*/

	if (testID == 0 || testID == 1 ) /*Pthread Mutex*/ //all and pthreadmutex
	{
		c=0;
		pthread_mutex_init(&count_mutex, 0); //init
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
		 if( rt=(pthread_create( threads+i, NULL, &pthreadMutexTest, NULL)) )
		{
			printf("Thread creation failed: %d\n", rt);
			return -1;	
		}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run Pthread (Mutex) Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Mutex time(ms): %llu\n",result/numItterations);

	}

	if(testID == 0 || testID == 2) /*Pthread Spinlock*/ //all and pthreadspinlock
	{
	/*Pthread Spinlock goes here*/
		c=0;
		pthread_spin_init(&count_spin, 0);
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
		 if( rt=(pthread_create( threads+i, NULL, &pthreadSpinTest, NULL)) )
		{
			printf("Thread creation failed: %d\n", rt);
			return -1;	
		}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run Pthread (SpinMutex) Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Spin Mutex time(ms): %llu\n",result/numItterations);
	}

	if(testID == 0 || testID == 3) /*MySpinlockTAS*/
	{
	/* mySpinlock TAS goes here*/
		c=0;
		my_spinlock_init(&mspinlock);
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
		 if( rt=(pthread_create( threads+i, NULL, &mySpinLockTASTest, NULL)) )
		{
			printf("Thread creation failed: %d\n", rt);
			return -1;	
		}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run MySpinlockTAS Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("MySpinlockTAS time(ms): %llu\n",result/numItterations);
	}

	/*....you must implement the other tests....*/
	if(testID == 0 || testID == 4) /*MySpinlockTTAS*/
	{
		c=0;
		my_spinlock_init(&mspinlock);
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
		 if( rt=(pthread_create( threads+i, NULL, &mySpinLockTTASTest, NULL)) )
		{
			printf("Thread creation failed: %d\n", rt);
			return -1;	
		}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run MySpinlockTTAS Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("MySpinlockTTAS time(ms): %llu\n",result/numItterations);

	}

	if(testID == 0 || testID == 5) //MyMutexTTASTest
	{
		c=0;
		my_mutex_init(&mmutex);
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
		 if( rt=(pthread_create( threads+i, NULL, &myMutexLockTest, NULL)) )
		{
			printf("Thread creation failed: %d\n", rt);
			return -1;	
		}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run MyMutex (TTAS) Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("MyMutexLock (TTAS) time(ms): %llu\n",result/numItterations);

	}

	if(testID == 0 || testID == 6) //queue or ticket based run
	{
		c=0;
		my_queuelock_init(&mqueue);
		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
		
		 if( rt=(pthread_create( threads+i, NULL, &myQueueLockTest, NULL)) )
		{
			printf("Thread creation failed: %d\n", rt);
			return -1;	
		}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run MyQueue Total Count: %lld\n", c);
		result=timespecDiff(&stop,&start);
		printf("MyQueue time(ms): %llu\n",result/numItterations);

	}

	return 0;
}


//example to test how tas works
int testAndSetExample()
{
	volatile long test = 0; //Test is set to 0
	printf("Test before atomic OP:%ld\n",test);
	tas(&test);
	printf("Test after atomic OP:%ld\n",test);
}

//example to test how cas works
int compareAndSwapExample()
{
	volatile unsigned long test = 0;
	printf("Test before swap OP:%ld\n", test);
	unsigned long old =  cas(&test,0,1);
	printf("Test after swap OP:%ld and old:%ld\n", test,old);
}

int processInput(int argc, char *argv[])
{

/*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock*/
	/*You must write how to parse input from the command line here, your software should default to the values given below if no input is given*/

	// if number of arguments are greater than 1 create a string with all arguments in it
	// then use sscanf for pattern comparison and use new values instead of default
	if (argc > 1) {
		int i;
		char inputargs[50] = "";
		for (i = 1; i < argc; i++) {
			strcat(inputargs,argv[i]);
			strcat(inputargs," ");
		}

		if(sscanf(inputargs, "-t %d -i %d -o %d -c %d -d %d",&numThreads,&numItterations,&workOutsideCS,&workInsideCS,&testID) != 5) {
			printf("Invalid inputs. Using defaults");
		} else {
			printf("my values -t %d -i %d -o %d -c %d -d %d\n",numThreads,numItterations,workOutsideCS,workInsideCS,testID);
			return 0;
		}
	}

	// use default if no argument is given or less or more than required are given
	numThreads=4;
	numItterations=1000000;
	testID=0;
	workOutsideCS=0;
	workInsideCS=1;


	printf("my values -t %d -i %d -o %d -c %d -d %d\n",numThreads,numItterations,workOutsideCS,workInsideCS,testID);
	
	return 0;
}


int main(int argc, char *argv[])
{


	printf("Usage of: %s -t #threads -i #Itterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTTAS, 6=myQueueLock, \n");	
	
	//testAndSetExample(); //Uncomment this line to see how to use TAS
	//compareAndSwapExample();

	/*
		Test before atomic OP:0
		Test after atomic OP:1
		Test before swap OP:0
		Test after swap OP:1 and old:0 */
	
	processInput(argc,argv);
	runTest(testID);
	return 0;

}
