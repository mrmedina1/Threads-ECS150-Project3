#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <tps.h>
#include <sem.h>


void *latest_mmap_addr;
static pthread_t tid0;
static pthread_t tid1;

void *__real_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
void *__wrap_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    latest_mmap_addr = __real_mmap(addr, len, prot, flags, fildes, off);
    return latest_mmap_addr;
}

void test_tps_create()
{
    assert(tps_create() == 0);
    tps_destroy();
}

void test_tps_destroy()
{
    tps_create();
    assert(tps_destroy() == 0);
}

void test_tps_write_and_read(size_t offset)
{
    tps_create();

    char *write_buffer = "abracadabra";  
    size_t buf_len = strlen(write_buffer);
    char read_buffer[buf_len];
    assert(tps_write(offset, buf_len, write_buffer) == 0);
    assert(tps_read(offset, buf_len, read_buffer) == 0);
    assert(strncmp(write_buffer, read_buffer, buf_len) == 0);  

    tps_destroy();
}

void test_tps_write_invalid_offset()
{
    tps_create();

    size_t offset = 1000000;
    char *write_buffer = "abracadabra";  
    size_t buf_len = strlen(write_buffer);
    assert(tps_write(offset, buf_len, write_buffer) != 0);

    tps_destroy();
}

void test_tps_write_invalid_buffer()
{
    tps_create();

    size_t offset = 0;
    char *write_buffer = NULL;
    assert(tps_write(offset, 10, write_buffer) != 0);

    tps_destroy();
}

void test_internal_tps_protection_error()
{
    tps_create();

    char* tps_addr = latest_mmap_addr;
    tps_addr[0] = '\0';

    tps_destroy();
}

void test_tps_clone()
{
    assert(tps_clone(tid0) == 0);
}

void *my_another_thread(void* arg)
{
    test_tps_clone();
    printf("Pass: test_tps_clone\n");
    return NULL;
}

void *my_thread(void* arg)
{
    // test_tps_create();
    printf("\nPass: test_tps_create\n");
    tps_create();

    size_t offset = 0;
    test_tps_write_and_read(offset);
    printf("Pass: test_tps_write_and_read\n");

    offset = 10;
    test_tps_write_and_read(offset);
    printf("Pass: test_tps_write_and_read with non-zero offset\n");

    offset = 4000;
    test_tps_write_and_read(offset);
    printf("Pass: test_tps_write_and_read with edge offset\n");

    test_tps_write_invalid_offset();
    printf("Pass: test_tps_write_invalid_offset\n");
  
    test_tps_write_invalid_buffer();
    printf("Pass: test_tps_write_invalid_buffer\n");
  
    test_tps_destroy();
    printf("Pass: test_tps_destroy\n");

    // Create tps, because it is needed for clone
    tps_create();

	  pthread_create(&tid1, NULL, my_another_thread, NULL);
	  pthread_join(tid1, NULL);

    // Cleanip
    tps_destroy();

    // Since this is a destructive tester, call it in the end
    printf("\nWarning! Running destructive tester.  Expect Segmentation fault\n");
    test_internal_tps_protection_error();

    return NULL;
}



int main(int argc, char **argv)
{
	/* Init TPS just once*/
	tps_init(1);

	/* Create thread 0 and wait */
	pthread_create(&tid0, NULL, my_thread, NULL);
	pthread_join(tid0, NULL);

	return 0;
}
