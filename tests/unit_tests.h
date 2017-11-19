#include <time.h>
#include "../mmemory.h"

void _init_test(int testNum, int n, int szPage, int resultCode, char * definition);
void _malloc_test(int testNum, int szBlock, int resultCode, char * definition);
void _free_test(int testNum, VA ptr, int resultCode,char * definition);
void _write_test(int testNum, VA ptr, void* buffer, size_t size, int resultCode,  char * definition);
void _read_test(int testNum, VA ptr, void *buffer, size_t size, int resultCode, char *definition);
void run_init_tests();
void run_malloc_tests();
void run_free_tests();
void run_write_tests();
void run_read_tests();
void run_address_arithmetic_tests();

