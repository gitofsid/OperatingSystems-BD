

#define _REENTRANT

#include <unistd.h>
#include "sync.h"
#include <stdio.h>
#include <stdlib.h>



#define TRUE 1


/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *mutex)
{
	mutex = (my_spinlock_t *)malloc (sizeof(my_spinlock_t *));
	if(mutex == NULL) {
		printf("Spinlock: Cant allocate memory in my_spinlock_init\n");
		return -1;
	}

  	mutex->status = 0; 
  	mutex->count = 0;
  	mutex->th_id = pthread_self();
  	mutex->prntth_id = pthread_self();

  	return 0;

}

int my_spinlock_destroy(my_spinlock_t *mutex)
{
	if(mutex == NULL) {
		printf("Spinlock: Invalid Mutex in my_spinlock_destroy\n");
		return -1;
	}

	free(mutex);
	return 0;
}

int my_spinlock_unlock(my_spinlock_t *mutex)
{
	if(mutex == NULL) {
		printf("Spinlock: Invalid mutex in my_spinlock_unlock\n");
		return -1;
	}

	mutex->status = 0;
	mutex->count = 0;
	mutex->th_id = mutex->prntth_id;

	return 0;

}

int my_spinlock_lockTAS(my_spinlock_t *mutex)
{
	//For the spinlock the first version will directly use the TAS atomic operations
	if(mutex == NULL) {
		printf("Spinlock: Invalid mutex in my_spinlock_lockTAS\n");
		return -1;
	}

	while(TRUE) {
		if(mutex->count == 0) { //if new lock request
			while(tas(&mutex->status) == 1) { //its set so dont do anything
				//do nothing
				;
			}
			mutex->count++;
			mutex->th_id = pthread_self();
			return 0;
		} else if(mutex->th_id == pthread_self()) {//if locking thread tryng again
			printf("Spinklock: Already locked by this in my_spinlock_lockTAS\n");
			return 0;
		}
	}
	return -1;
}


int my_spinlock_lockTTAS(my_spinlock_t *mutex)
{
	//The second version will employ a Test-And-TestAnd-Set (TTAS) technique.
	if(mutex == NULL) {
		printf("Spinlock: Invalid mutex in my_spinlock_lockTTAS\n");
		return -1;
	}

	while(TRUE) {
		if(mutex->count == 0) { //if new lock request
			while(mutex->status == 1) {//its set so dont do anything
				//do nothing
				;
			}

			if(tas(&mutex->status) == 0) { //checking for status first, if previous val is 0 then write 1
				mutex->count++;
				mutex->th_id = pthread_self();
				return 0;
			}
		} else if(mutex->th_id == pthread_self()) {//if locking thread tryng again
			printf("Spinlock: Already locked by this in my_spinlock_lockTTAS\n");
			return 0;
		}
	}
	return -1;
}

int my_spinlock_trylock(my_spinlock_t *mutex)
{
	if(mutex == NULL) {
		printf("Spinlock: Invalid mutex in my_spinlock_trylock\n");
		return -1;
	}

	if(mutex->status == 0) {//try to take lock
		if(tas(&mutex->status) == 0) { //if initial value returned is 0 that means its set
			mutex->count++;
			mutex->th_id = pthread_self();
			return 0;
		}		
	}
	printf("Spinkock: Lock is already taken in my_spinlock_trylock\n");
	return -1;
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *mutex)
{
	mutex = (my_mutex_t *)malloc (sizeof(my_mutex_t *));
	if(mutex == NULL) {
		printf("Mutexlock: Cant allocate memory in my_mutex_init\n");
		return -1;
	}

  	mutex->status = 0; 
  	mutex->count = 0;
  	mutex->th_id = pthread_self();
  	mutex->prntth_id = pthread_self();

  	return 0;

}

int my_mutex_destroy(my_mutex_t *mutex)
{
	if(mutex == NULL) {
		printf("Mutexlock: invalid mutex in my_mutex_destroy\n");
		return -1;	
	} 

	free(mutex);
	return 0;
}

int my_mutex_unlock(my_mutex_t *mutex)
{
	if(mutex == NULL) {
		printf("Mutexlock: Invalid mutex in my_mutex_unlock\n");
		return -1;
	}

	mutex->status = 0;
	mutex->count = 0;
	mutex->th_id = mutex->prntth_id;

	return 0;
}

int my_mutex_lock(my_mutex_t *mutex) //TTAS
{

	if(mutex == NULL) {
		printf("Mutexlock: Invalid mutex in my_mutex_lock\n");
		return -1;
	}

	//exponential back off with delay
	useconds_t backoff_delay = 5;

	while(TRUE) {
		if(mutex->count == 0) { //if new lock request
			while(mutex->status == 1) {//its set so dont do anything
				//do nothing
				;
			}

			if(tas(&mutex->status) == 0) { //checking for status first, if previous val is 0 then write 1
				mutex->count++;
				mutex->th_id = pthread_self();
				return 0;
			}
		} else if(mutex->th_id == pthread_self()) {//if locking thread tryng again
			printf("Mutexlock: Already locked by this in my_mutex_lock\n");
			return 0;
		}
		usleep(backoff_delay); //sleep for delay time
		if (backoff_delay < 40) {
			backoff_delay *= 2;
		}
	}
	return -1;
}

int my_mutex_trylock(my_mutex_t *mutex)
{
	if(mutex == NULL) {
		printf("Mutexlock: Invalid mutex in my_mutex_trylock\n");
		return -1;
	}

	if(mutex->status == 0) {//try to take lock
		if(tas(&mutex->status) == 0) { //if initial value returned is 0 that means its set
			mutex->count++;
			mutex->th_id = pthread_self();
			return 0;
		}		
	}
	printf("Mutexlock: Lock is already taken in my_mutex_trylock\n");
	return -1;
}

/*
 * Queue Lock
 */

int my_queuelock_init(my_queuelock_t *mutex)
{
	mutex = (my_queuelock_t *)malloc (sizeof(my_queuelock_t *));
	if(mutex == NULL) {
		printf("Queuelock: Cant allocate memory in my_queuelock_init\n");
		return -1;
	}

	mutex->count = 0;
	mutex->current = 0;
	mutex->next = 0;
	mutex->status = 0;
	mutex->th_id = pthread_self();
  	mutex->prntth_id = pthread_self();

  	return 0;
}

int my_queuelock_destroy(my_queuelock_t *mutex)
{
	if(mutex == NULL) {
		printf("Queuelock: invalid mutex in my_queuelock_destroy\n");
		return -1;	
	} 

	free(mutex);
	return 0;
}

int my_queuelock_unlock(my_queuelock_t *mutex)
{
	if(mutex == NULL) {
		printf("Queuelock: Invalid mutex in my_queuelock_unlock\n");
		return -1;
	}

	mutex->current++;
	mutex->th_id = mutex->prntth_id;
	mutex->count = 0;
	mutex->status = 0;

	return 0;
}

int my_queuelock_lock(my_queuelock_t *mutex)
{
	if(mutex == NULL) {
		printf("Queuelock: Invalid mutex in my_queuelock_lock\n");
		return -1;
	}

	// for queue lock we will be using the functionality of checking
	// thread status using TTAS as well as the ticket number check
	// this ensure the thread safe execution with the ability of 
	// queue based fairness
	unsigned long next_to_serve, previous;

	while(TRUE) {
		if(mutex->count == 0) { //if new lock request
			while(mutex->status == 1) {}; //if already processed then wait
			if(tas(&mutex->status) == 0) { // else set to 1 that its been addressed 
				previous = mutex->next; //last value of ticket
				next_to_serve = cas(&mutex->current,previous,mutex->next++); //fetch next ticket

				while(mutex->current != next_to_serve) { //check if current to serve is next ticket
					sleep(next_to_serve - mutex->current); //if not equal then wait
					if (mutex->current == next_to_serve) { // recheck 
						mutex->count++; //increase count addressing the thread
						mutex->th_id = pthread_self();
						return 0;
					}
				}

				mutex->count++; //increase 
				mutex->th_id = pthread_self();

				return 0;
			}

		} else if(mutex->th_id == pthread_self()) {//if locking thread tryng again return 0
			printf("Queuelock: Already locked by this in my_queuelock_lock\n");
			return 0;
		}
	}
	return -1;
}

int my_queuelock_trylock(my_queuelock_t *mutex)
{
	if(mutex == NULL) {
		printf("Queuelock: Invalid mutex in my_queuelock_trylock\n");
		return -1;
	}

	if(mutex->status == 0) {
		if(tas(&mutex->status) == 0) {
			unsigned long next_to_serve;

			next_to_serve = cas(&mutex->current+1,mutex->next,mutex->next++);
			if (mutex->current == next_to_serve) {
				mutex->count++;
				mutex->th_id = pthread_self();
				return 0;
			}
		}
	}

	printf("Queuelock: Lock is already taken in my_queuelock_trylock\n");
	return -1;
}

