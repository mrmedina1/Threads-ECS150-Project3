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
//  struct page *page_ptr;
};

/*struct page
{
  void* start_ptr;
  int count;
};*/

struct TPS_STORE
{
  queue_t TPS_queue;
};

struct TPS_STORE *TPS_store = 0;

int tps_init(int segv)
{
  if (TPS_store != 0) { return -1; }
  else
  {
    TPS_store = malloc(sizeof(struct TPS_STORE));
    TPS_store->TPS_queue = queue_create();   
  }
	/* TODO: Phase 2 */
  return 1;
}

int tps_create(void)
{
	/* TODO: Phase 2 */
  struct TPS *new_tps = malloc(sizeof(struct TPS));
  if (new_tps == NULL) { return -1; }
  
  new_tps->TID = pthread_self();
   
  return 1;
}

int tps_destroy(void)
{
	/* TODO: Phase 2 */
  return 1;
}

int tps_read(size_t offset, size_t length, char *buffer)
{
	/* TODO: Phase 2 */
  return 1;
}

int tps_write(size_t offset, size_t length, char *buffer)
{
	/* TODO: Phase 2 */
  return 1;
}

int tps_clone(pthread_t tid)
{
	/* TODO: Phase 2 */
  return 1;
}

