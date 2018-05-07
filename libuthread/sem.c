#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"

struct semaphore {
	/* TODO: Phase 1 */
	queue_t semQueue;
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
	
	if (sem == NULL)
	{
		return -1;
	}
/*	
	if (sem -> count == 0)
	{
		//Add thread to list of threads waiting for resource
		//Set thread to blocked state (not eligible for scheduling)
	}
*/
	return 0;
}

int sem_up(sem_t sem)
{
	/* TODO: Phase 1 */
	if (sem == NULL)
	{
		return -1;
	}
/*	
	if (sem -> count == 0)
	{
		//If other threads are waiting on the released semaphore
		//Then first thread is set to unblocked and eligible to run later
	}
*/	
	return 0;
}

size_t get_sem_count(sem_t sem)
{
  return sem -> count;
}
