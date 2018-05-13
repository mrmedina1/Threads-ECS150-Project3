#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "queue.h"
#include "thread.h"
#include "tps.h"

/* TODO: Phase 2 */

struct TPS
{
	pthread_t TID;
	struct memPage *memPage_ptr;
};

struct memPage
{
	int count;
	void* start_ptr;
};

struct TPS_STORE
{
	queue_t TPS_queue;
};

struct TPS_STORE *TPS_store = 0;

//Iterator function to search for TPS
int tpsSearch(void *data, void *arg)
{
	struct TPS *a = (struct TPS*)data;
	pthread_t match = (pthread_t)arg;

	if (a->TID == match)
		return 1;
	return 0;
}

//Initalizes TPS queue
int tps_init(int segv)
{
	if (TPS_store != 0) { return -1; }
	else
	{
		TPS_store = malloc(sizeof(struct TPS_STORE));
		TPS_store -> TPS_queue = queue_create();   
	}
	/* TODO: Phase 2 */
	return 0;
}

int tps_create(void)
{
	/* TODO: Phase 2 */
	struct TPS *new_tps = malloc(sizeof(struct TPS));
	if (new_tps == NULL) { return -1; }
	
	new_tps -> TID = pthread_self();
	struct TPS *tps_found = NULL;
	
	//Search for TPS using queue_iterate and tpsSearch iterator function
	queue_iterate(TPS_store -> TPS_queue, tpsSearch, (void*)new_tps -> TID, (void**)&tps_found);
	if(tps_found != NULL){ return -1; }
	
	new_tps -> memPage_ptr = (struct memPage*)malloc(sizeof(struct memPage));
	if (new_tps -> memPage_ptr == NULL) { return -1; }
	
	//PROT_NONE reserves a region of address space for future use
	//addr ptr NULL, TPS_SIZE = 4096, PRO_NONE, fildescriptor 0, offset 0
	new_tps -> memPage_ptr -> start_ptr = mmap(NULL, TPS_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANON, 0, 0);
	if(new_tps -> memPage_ptr -> start_ptr == MAP_FAILED){ return -1; }
	
	new_tps -> memPage_ptr -> count = 1;

	queue_enqueue(TPS_store -> TPS_queue, new_tps);
  
	return 0;
}

int tps_destroy(void)
{
	/* TODO: Phase 2 */
	struct TPS *tps_found = NULL;
	pthread_t tid = (pthread_t)pthread_self();
	queue_iterate(TPS_store -> TPS_queue, tpsSearch, (void*)tid, (void**)&tps_found);
	if(tps_found == NULL){ return -1; }
	
	assert(queue_delete(TPS_store -> TPS_queue, tps_found) != -1);
	
	return 0;
}

int tps_read(size_t offset, size_t length, char *buffer)
{
	/* TODO: Phase 2 */
	int outOfBounds = (length + offset);
	
	struct TPS *tps_found = NULL;
	pthread_t tid = (pthread_t)pthread_self();
	queue_iterate(TPS_store -> TPS_queue, tpsSearch, (void*)tid, (void**)&tps_found);
	if(tps_found == NULL || outOfBounds > TPS_SIZE || buffer == NULL){ return -1; }
	
	memcpy(buffer, tps_found->memPage_ptr->start_ptr + offset, length);
	
	assert(strcmp(tps_found->memPage_ptr->start_ptr + offset, buffer) == 0);
	
	return 0;
}

int tps_write(size_t offset, size_t length, char *buffer)
{
	/* TODO: Phase 2 */
  return 0;
}

int tps_clone(pthread_t tid)
{
	/* TODO: Phase 2 */
  return 0;
}

