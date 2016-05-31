

#ifndef _my_SYNC_H_
#define _my_SYNC_H_

#include "atomic_ops.h"
#include <sys/types.h>

/* status keeps status of thread if its started or not
   count keeps track of number of times same thread tries an operation
   th_id is thread id
   prnth_id is parent thread id
*/
struct my_mutex_struct {
  volatile unsigned long status;
  long long count;
  pthread_t th_id;
  pthread_t prntth_id;
};


typedef struct my_mutex_struct my_mutex_t;


int my_mutex_init(my_mutex_t *mutex);
int my_mutex_unlock(my_mutex_t *mutex);
int my_mutex_destroy(my_mutex_t *mutex);

int my_mutex_lock(my_mutex_t *mutex);
int my_mutex_trylock(my_mutex_t *mutex);


/*Spinlock Starts here*/

/* status keeps status of thread if its started or not
   count keeps track of number of times same thread tries an operation
   th_id is thread id
   prnth_id is parent thread id
*/
struct my_spinlock_struct {
  volatile unsigned long status;
  long long count;
  pthread_t th_id;
  pthread_t prntth_id;
};

typedef struct my_spinlock_struct my_spinlock_t;

int my_spinlock_init(my_spinlock_t *lock);
int my_spinlock_destroy(my_spinlock_t *lock);
int my_spinlock_unlock(my_spinlock_t *lock);

int my_spinlock_lockTAS(my_spinlock_t *lock);
int my_spinlock_lockTTAS(my_spinlock_t *lock);
int my_spinlock_trylock(my_spinlock_t *lock);


/*Queuelock Starts here*/

/* status keeps status of thread if its started or not
   count keeps track of number of times same thread tries an operation
   th_id is thread id
   prnth_id is parent thread id
   current keeps track of current thread processing
   next keeps track of next thread to be processed
*/

struct my_queuelock_struct {
  /* FILL ME IN! */
	volatile unsigned long current;
	volatile unsigned long next;
	volatile unsigned long status;
	long long count;
	pthread_t th_id;
  	pthread_t prntth_id;
};

typedef struct my_queuelock_struct my_queuelock_t;

int my_queuelock_init(my_queuelock_t *lock);
int my_queuelock_destroy(my_queuelock_t *lock);
int my_queuelock_unlock(my_queuelock_t *lock);

int my_queuelock_lock(my_queuelock_t *lock);
int my_queuelock_trylock(my_queuelock_t *lock);


#endif
