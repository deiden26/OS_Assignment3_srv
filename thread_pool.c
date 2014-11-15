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

typedef struct pool_task_t pool_task_t;

struct pool_task_t {
    void (*function)(int *);
    void *argument;
    struct pool_task_t *next;
};

struct pool_t {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t threads[MAX_THREADS];
  pool_task_t *queue;
  int thread_count;
  int task_count;
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
  threadPool->task_count = 0;
  threadPool->queue = NULL;

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
int pool_add_task(pool_t *pool, void (*function)(int *), void *argument)
{
    //Initialize err to no error
    int err = 0;

    //Create the task
    pool_task_t *newTask = malloc(sizeof(pool_task_t));
    newTask->function = function;
    newTask->argument = argument;
    newTask->next = NULL;

    //Lock the pool
    pthread_mutex_lock(&(pool->lock));

    //Increment the task count
    pool->task_count++;

    //Insert the new task at the end of the list
    pool_task_t *task = pool->queue;

    //Special case: empty list
    if (task == NULL)
      pool->queue = newTask;
    else
    {
      //Get the last task in the que
      while(task->next != NULL)
        task = task->next;
      //Insert new task
      task->next = newTask;
    }

    //Release the lock (we are done editing the queue)
    pthread_mutex_unlock(&(pool->lock));

    //Signal that there is a new task
    pthread_cond_signal(&(pool->notify));

        
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

    //For each task...
    pool_task_t *task = pool->queue;
    pool_task_t *tasktoDel;
    while(task != NULL)
    {
      //Save the location of the task
      tasktoDel = task;
      //Get the location of the next task
      task = task->next;
      //Free the task object's memory
      free(task);
    }

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
  //Cast pool as pool_t
  pool_t *threadPool = (pool_t*)pool;

  //Work on this loop until destroyed
  while(1) {

    //Lock the pool so you can check the task count
    pthread_mutex_lock(&(threadPool->lock));

    while(threadPool->task_count == 0) {
      //If there are no available tasks, wait (also release your lock)
      pthread_cond_wait(&(threadPool->notify), &(threadPool->lock));
    }
    //Now you have the lock and there is a task in the que. First, remove the task
    pool_task_t *task = threadPool->queue;
    threadPool->queue = threadPool->queue->next;

    //Next, decrement the task count
    threadPool->task_count--;

    //Release lock (we are done using the threadpool data)
    pthread_mutex_unlock(&(threadPool->lock));

    //Execute the function
    task->function((int*)task->argument);

    //Free the task
    free(task);
  }

  pthread_exit(NULL);
  return(NULL);
}
