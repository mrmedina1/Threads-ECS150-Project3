#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"

struct semaphore {
	/* TODO: Phase 1 */
  size_t count;
  queue_t blocked_queue; 
};

sem_t sem_create(size_t count)
{
	/* TODO: Phase 1 */
  struct semaphore* new_semaphore = malloc(sizeof(struct semaphore));
  if (new_semaphore == NULL)
  {
    return NULL;
  }

  new_semaphore->count = count;
  new_semaphore->blocked_queue = queue_create();
  return new_semaphore;
    
}

int sem_destroy(sem_t sem)
{
	/* TODO: Phase 1 */
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

int sem_down(sem_t sem)
{
	/* TODO: Phase 1 */
  if (sem == NULL)
  {
    return -1;
  }
    
  enter_critical_section();
  while (sem->count == 0)
  {
    queue_enqueue(sem->blocked_queue, (pthread_t*)pthread_self());
    thread_block();
  }  
  sem->count -= 1;
  exit_critical_section();
  return 0;
}

int sem_up(sem_t sem)
{
/* TODO: Phase 1 */
  if (sem == NULL)
  {
    return -1;
  }

  enter_critical_section();
  if (sem->count != 0)
  {
    sem->count += 1;
    exit_critical_section();
    return 0;
  }
  sem->count += 1;
  if (queue_length(sem->blocked_queue) > 0)
  {
    pthread_t tid_to_unblock;
    queue_dequeue(sem->blocked_queue, (void**)&tid_to_unblock);
    thread_unblock(tid_to_unblock);
  }
  exit_critical_section();
  return 0;
}

size_t get_sem_count(sem_t sem)
{
  return sem->count;
}
