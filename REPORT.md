```Authors: Sahana Melkris, Matt Medina```

# Project 3

### Introduction

In this project we have implemented two aspects of threads:

1. Semaphores

2. Per-thread protected memory regions

Semaphores provide for an efficient thread synchronization through use of 
waiting queues.  With protected memory regions specific to an individual thread, 
threads will be able to generate a Thread Private Storage (TPS) that is 
non-accessible from other threads due to exclusive API access features.

### Report

This project makes use of a user implemented queue, rather than the one provided 
for the project.  The implementation of the queue can be seen in queue.c.

##### sem.c

sem.c contains a ```semaphore struct``` with a queue and a count to keep track 
of the available resources that can be taken.  When all resources have been used 
up the running thread is put into a blocked state until resources become 
available.  Sempahores make use of a critical section.  The critical section 
allows allows for mutually exclusive access to a shared resource, and when 
implemented properly, prevents a deadlock from occuring.

```sem_create(size_t count)``` creates a new semaphore of initial count

```sem_destroy(sem_t sem)``` destroys a semaphore sem

```sem_down(sem_t sem)``` takes a resource from semaphore sem

While the semaphore resource count is 0, the thread is enqueued and 
blocked.  The critical section for sem_down can be seen in the following code 
block.

	```c
	//Crititcal section for sem_down
	enter_critical_section();
	while (sem->count == 0)
	{
		queue_enqueue(sem->blocked_queue, (pthread_t*)pthread_self());
		thread_block();
	}  
	sem->count -= 1;
	exit_critical_section();
	```

```sem_up(sem_t sem)``` releases a resource from semaphore sem

If there are still resources that can be taken the critical section is exited.  
If there are resources in the queue then dequeue the thread and unblock it.
The critical section for sem_up can be seen in the following code 
block:

	```c
	//Crititcal section for sem_up
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
	```
	
```get_sem_count(sem_t sem)``` returns the semaphore resource count of sem

##### tps.c

The Thread Private Storage API is used to protect threads from accessing other 
threads.  It allows for protected access to read and write operations that can 
only be performed from within the API.

A ```TPS struct``` is used to hold the thread ID and a pointer to the memory 
page of the TPS.  The ```memPage struct``` is used to store memory page 
information.  It holds a starting page pointer and a count that keeps track of 
how many memory pages there are.  The ```TPS _STORE``` contains the 
```TPS_queue``` that is used to store the various TPS's.

```tps_init(int segv)``` Initializes the TPS queue and checks on two types of 
seg faults, both SIGSEGV and SIGBUS.  This is useful to know whether you had an 
actual segmentation fault, or a seg fault caused by trying to access a protected 
TPS.

```tps_create(void)``` Creates a new TPS as long as the TPS has not already been 
created and enqueues it into the TPS queue.  The first page to the new TPS is 
created using a mapped pages memory (mmap) with the PROT_NONE flag which 
reserves a region of address space for future use.

```tps_destroy(void)``` 

```tps_read(size_t offset, size_t length, char *buffer)``` Reads a specified 
length of bytes from from current thread's TPS starting at the offset and stores 
it into the data buffer.  Returns errors for out of bounds indexing, an empty 
buffer, or if the TPS to read from is not found in the TPS queue.

Memory protections are set to PROT_READ before the memory is copied into the 
buffer, then the memory protections are set back to PROT_NONE to protect the 
region of address space from unwanted access.  The code for this can be seen as 
follows: 
	
	```c
	//Change protection permissions before and after copying
	mprotect(tps_found->memPage_ptr->start_ptr, TPS_SIZE, PROT_READ);
	memcpy(buffer, tps_found->memPage_ptr->start_ptr + offset, length);
	mprotect(tps_found->memPage_ptr->start_ptr, TPS_SIZE, PROT_NONE);
	```

##### Testing

sem_count.c

sem_buffer.c

sem_prime.c

tps.c

##### Sub Report 4

##### Sub Report 5

##### Makefile

### Limitations

* Has only been compiled through GCC.

### Sources
* Source1

http://google.com

* Source2

http://google.com