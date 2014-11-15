#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include "thread_pool.h"

/**
 *  @struct threadpool_task
 *  @brief the work struct
 *
 *  Feel free to make any modifications you want to the function prototypes and structs
 *
 *  @var function Pointer to the function that will perform the task.
 *  @var argument Argument to be passed to the function.
 */

#define MAX_THREADS 20
#define STANDBY_SIZE 8

typedef struct {
    void (*function)(void *);
    void *argument;
} pool_task_t;


struct pool_t {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t threads[MAX_THREADS];
  pool_task_t *queue;
  int thread_count;
  int task_queue_size_limit;
};

static void *thread_do_work(void *pool);


/*
 * Create a threadpool, initialize variables, etc
 *
 */
pool_t *pool_create(int queue_size, int num_threads)
{

  pool_t *threadPool = malloc(sizeof(pool_t));

  assert(num_threads <= MAX_THREADS);

  //Initialize threadpool variables with default values
  pthread_mutex_init(&threadPool->lock,NULL);
  pthread_cond_init(&threadPool->notify,NULL);
  threadPool->thread_count = 0;

  //Initualize threadpool variables with provided values
  threadPool->task_queue_size_limit = queue_size;
  threadPool->thread_count = num_threads;

  //Create threads running the thread_do_work routine
  int i = 0;
  for (i = 0; i < threadPool->thread_count; i++)
  {
    pthread_create(&threadPool->threads[i], NULL, thread_do_work, threadPool);
  }

  return threadPool;
}


/*
 * Add a task to the threadpool
 *
 */
int pool_add_task(pool_t *pool, void (*function)(void *), void *argument)
{
    int err = 0;
        
    return err;
}



/*
 * Destroy the threadpool, free all memory, destroy treads, etc
 *
 */
int pool_destroy(pool_t *pool)
{
    // Initialize return value to 0 (no error)
    int err = 0;

    // For each thread...
    int i;
    for (i=0; i < pool->thread_count; i++)
    {
      // Cancel the thread
      err = pthread_cancel(pool->threads[i]);
      // Return with the error number if there is an error
      if (err != 0)
        return err;
    }
    // Return 0 for no error
    return err;
}



/*
 * Work loop for threads. Should be passed into the pthread_create() method.
 *
 */
static void *thread_do_work(void *pool)
{ 
  pool_t *threadpool = (pool_t*)pool;
  while(1) {

    while(threadpool->thread_count == 0) {
      pthread_cond_wait(&(threadpool->notify), &(threadpool->lock));
    }
      
  }

  pthread_exit(NULL);
  return(NULL);
}
