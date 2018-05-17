#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"

//Struct to hold semaphore information
struct semaphore {
  size_t count;
  queue_t blocked_queue; 
};

//Created a returns a new semaphore
sem_t sem_create(size_t count)
{
  struct semaphore* new_semaphore = malloc(sizeof(struct semaphore));
  if (new_semaphore == NULL)
  {
    return NULL;
  }

  new_semaphore->count = count;
  new_semaphore->blocked_queue = queue_create();
  return new_semaphore;
}

//Destroys semaphore sem
int sem_destroy(sem_t sem)
{
  if (sem == NULL)
  {
    return -1;
  }
  if (queue_destroy(sem->blocked_queue) == -1)
  {
    return -1;
  }
  free(sem);
  return 0;
}

//Takes a resource from semaphore sem
int sem_down(sem_t sem)
{
  if (sem == NULL)
  {
    return -1;
  }
  
  //Critical section allows for mutually exclusive access to a shared resource
  enter_critical_section();
  
  //Enqueue and block thread while there are no resources available
  while (sem->count == 0)
  {
    queue_enqueue(sem->blocked_queue, (pthread_t*)pthread_self());
    thread_block();
  }  
  sem->count -= 1;
  
  exit_critical_section();
  
  return 0;
}

//Releases a resource from semaphore sem
int sem_up(sem_t sem)
{
  if (sem == NULL)
  {
    return -1;
  }

  //Critical section allows for mutually exclusive access to a shared resource
  enter_critical_section();
  
  //If there are resources, increment count and exit the critical section
  if (sem->count != 0)
  {
    sem->count += 1;
    exit_critical_section();
    return 0;
  }
  sem->count += 1;
  
  //If there are resources in the queue, dequeue thread and unblock it
  if (queue_length(sem->blocked_queue) > 0)
  {
    pthread_t tid_to_unblock;
    queue_dequeue(sem->blocked_queue, (void**)&tid_to_unblock);
    thread_unblock(tid_to_unblock);
  }
  exit_critical_section();

  return 0;
}

//Return sem count
size_t get_sem_count(sem_t sem)
{
  return sem->count;
}
