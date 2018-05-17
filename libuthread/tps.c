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

//Struct for storing memory pages
struct memPage
{
	int count;	//Used to keep track of memory pages, needed in write function
	void* start_ptr;
};

//Struct to hold TPS information
struct TPS
{
	pthread_t TID;
	struct memPage *memPage_ptr;
};

//TPS queue struct
struct TPS_STORE
{
	queue_t TPS_queue;
};

struct TPS_STORE *TPS_store = 0;

//Iterator function to search for TPS
int tps_search(void *data, void *arg)
{
	struct TPS *a = (struct TPS*)data;
	pthread_t match = (pthread_t)arg;

	return  (a->TID == match);
}

//Iterator function to search for memory page address
int page_address_search(void *data, void *match_arg)
{
  struct TPS *tps = (struct TPS*)data;
  void* match = (void*) match_arg;
  return (tps->memPage_ptr->start_ptr == match);
}

static void segv_handler(int sig, siginfo_t *si, void *context)
{
  void *p_fault = (void*)((uintptr_t)si->si_addr & ~(TPS_SIZE - 1));
	
	void *found = NULL;
  queue_iterate(TPS_store->TPS_queue, page_address_search, p_fault, (void**)&found);
    
  if (found != NULL)
  {
    fprintf(stderr, "TPS protection error! \n");
  }
  
  signal(SIGSEGV, SIG_DFL);
  signal(SIGBUS, SIG_DFL);

  raise(sig);

}

//Initalizes TPS queue
int tps_init(int segv)
{
	if (TPS_store != 0) { return -1; }
	else
	{
		TPS_store = malloc(sizeof(struct TPS_STORE));
		TPS_store->TPS_queue = queue_create();   
	}
  
    //Checks for segfault
  if (segv) 
  {
    struct sigaction sa;
  
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = segv_handler;
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
  }
	return 0;
}

//Creates a new TPS
int tps_create(void)
{
	struct TPS *tps = malloc(sizeof(struct TPS));
	if (tps == NULL) 
  { 
    return -1;
  }
	
    //Get TID.
    //Search for TPS using queue_iterate and tpsSearch iterator function
    //Return error if TPS not found
	tps->TID = pthread_self();
	struct TPS *tps_found = NULL;
	
	//Search for TPS using queue_iterate and tps_search iterator function
	queue_iterate(TPS_store->TPS_queue, tps_search, (void*)tps->TID, (void**)&tps_found);
	if (tps_found != NULL)
  { 
    return -1; 
  }
	
	tps->memPage_ptr = (struct memPage*)malloc(sizeof(struct memPage));
	if (tps->memPage_ptr == NULL) 
  { 
    return -1; 
  }
	
	//PROT_NONE reserves a region of address space for future use
	//addr ptr NULL, TPS_SIZE = 4096, PRO_NONE, fildescriptor 0, offset 0
	tps->memPage_ptr->start_ptr = mmap(NULL, TPS_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANON, 0, 0);
	if (tps->memPage_ptr->start_ptr == MAP_FAILED)
  { 
    return -1;
  }

    //Increment memory page count and enqueue the new TPS
	tps->memPage_ptr->count = 1;
	queue_enqueue(TPS_store->TPS_queue, tps);
  
	return 0;
}

int tps_destroy(void)
{
    //Get TID and search the TPS queue for a TID match.  Return error if TPS not found
	struct TPS *tps_found = NULL;
	pthread_t tid = (pthread_t)pthread_self();
	queue_iterate(TPS_store->TPS_queue, tps_search, (void*)tid, (void**)&tps_found);
    //Delete TPS from queue
	queue_delete(TPS_store->TPS_queue, tps_found);
  return ((tps_found == NULL) ? -1 : 0);
}

//Read a certain amount of bytes at offset of current threads TPS
int tps_read(size_t offset, size_t length, char *buffer)
{
    //error checks for outOfBounds index on TPS size, 
    //or an empty buffer.
	int outOfBounds = length + offset;
	if ((outOfBounds > TPS_SIZE) || (buffer == NULL))
  {
    return -1;
  }

  //Get TID and search the TPS queue for a TID match
  //Error checks for TID not found in queue, 
	struct TPS *tps_found = NULL;
	pthread_t tid = (pthread_t)pthread_self();
	queue_iterate(TPS_store->TPS_queue, tps_search, (void*)tid, (void**)&tps_found);
	if (tps_found == NULL) 
  { 
    return -1;
  }
	
	//Set the found TPS protections to read, copy the data, then reset
	//protections so that the memory cannot be accessed
	mprotect(tps_found->memPage_ptr->start_ptr, TPS_SIZE, PROT_READ);
	memcpy(buffer, tps_found->memPage_ptr->start_ptr + offset, length);
	mprotect(tps_found->memPage_ptr->start_ptr, TPS_SIZE, PROT_NONE);
	
	return 0;
}

//Write a certain amount of bytes at offset of current threads TPS
int tps_write(size_t offset, size_t length, char *buffer)
{
	int outOfBounds = length + offset;
	if ((outOfBounds > TPS_SIZE) || (buffer == NULL))
  {
    return -1;
  }

	//Get TID and search the TPS queue for a TID match
  //Error checks for TID not found in queue, outOfBounds index on TPS size, 
  //or an empty buffer.
	struct TPS *tps_found = NULL;
	pthread_t tid = (pthread_t)pthread_self();
	queue_iterate(TPS_store->TPS_queue, tps_search, (void*)tid, (void**)&tps_found);
	if (tps_found == NULL) 
  { 
    return -1; 
  }

  //Check if the found TPS has been cloned
	if (tps_found->memPage_ptr->count > 1)
	{
		struct memPage *new_memPage = (struct memPage*)malloc(sizeof(struct memPage));
		tps_found->memPage_ptr->count -= 1;
		new_memPage->start_ptr =  mmap(NULL, TPS_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANON, 0, 0);
		
    //Set read protection for the found TPS and write protections for the
    //new memory page
		mprotect(tps_found->memPage_ptr->start_ptr, TPS_SIZE, PROT_READ);
		mprotect(new_memPage->start_ptr, TPS_SIZE, PROT_WRITE);

    //Copy the found TPS memory page to the new memory page
		memcpy(new_memPage->start_ptr, tps_found->memPage_ptr->start_ptr, TPS_SIZE);

    //Set protections back so that the memory cannot be accessed
		mprotect(tps_found->memPage_ptr->start_ptr, TPS_SIZE, PROT_NONE);
		mprotect(new_memPage->start_ptr, TPS_SIZE, PROT_NONE);
		
    //Readjust the found TPS pointer and increment the memory page count
		tps_found->memPage_ptr = new_memPage;
		tps_found->memPage_ptr->count = 1;
	}

  //Set the found TPS protections to write, copy the data, then reset
  //protections so that the memory cannot be accessed
	mprotect(tps_found->memPage_ptr->start_ptr, TPS_SIZE, PROT_WRITE);
	memcpy(tps_found->memPage_ptr->start_ptr + offset, buffer, length);
	mprotect(tps_found->memPage_ptr->start_ptr, TPS_SIZE, PROT_NONE);
	
	return 0;
}

//Handles ptr redirection
int tps_clone(pthread_t tid)
{
  //Search for tps of 'tid' parameter
	struct TPS *tps_found = NULL;
	queue_iterate(TPS_store->TPS_queue, tps_search, (void*)tid, (void**)&tps_found);
	if (tps_found == NULL)
  {
    return -1;
  }
	
  //Search for the current TID and return error if 'tid' parameter wasn't found
	struct TPS *tps_current_found = NULL;
	pthread_t tid_current = (pthread_t)pthread_self();
	queue_iterate(TPS_store->TPS_queue, tps_search, (void*)tid_current, (void**)&tps_current_found);
	if (tps_current_found != NULL)
  {
    return -1;
  }
	
	struct TPS *cloned_tps = (struct TPS*)malloc(sizeof(struct TPS));
	if (cloned_tps == NULL)
  {
    return -1;
  }
	
	//Clone the TPS.  memPage pointer is reassigned
	cloned_tps->TID = tid_current;
	cloned_tps->memPage_ptr = tps_found->memPage_ptr;
	
	//Increment memory page counter
	tps_found->memPage_ptr->count += 1;
	
	//Enqueue cloned TPS
	queue_enqueue(TPS_store->TPS_queue, cloned_tps);

	return 0;
}

