#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"

struct semaphore {
	/* TODO: Phase 1 */
  size_t count;
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
  return new_semaphore;
    
}

int sem_destroy(sem_t sem)
{
	/* TODO: Phase 1 */
  if (sem == NULL)
  {
    return -1;
  }
  
  free(sem);
  return 0;
}

int sem_down(sem_t sem)
{
	/* TODO: Phase 1 */
  
  return 1;
}

int sem_up(sem_t sem)
{
	/* TODO: Phase 1 */
  return 1;
}

size_t get_sem_count(sem_t sem)
{
  return sem->count;
}
