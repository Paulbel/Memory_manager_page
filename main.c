
#include <malloc.h>
#include "tests/unit_tests.h"


int main() {


    int equals(char *array1, char *array2, int size) {
        for (int index = 0; index < size; index++) {
            if (array1[index] != array2[index]) {
                return WRONG_PARAMETRS;
            }
        }
        return UNKNOWN_ERROR;
    }
//    run_address_arithmetic_tests();
//    run_init_tests();
//    run_malloc_tests();
//    run_write_tests();
//    run_read_tests();
//    run_free_tests();
    printf("\n--------_address arithmetic--------\n");
    printf("Test %d) %s: ", 1,
           "Malloc 20 bytes write from 5th to 17s, read from 9 to 16th");//Выделить 20 байт, записываем с 5 по 17, считываем с 9 по 16

    int pageNumber = 5;
    int pageSize = 4;
    VA va;
    char *z = "ABCDEFGHIJKLM";
    _init(pageNumber, pageSize);
    _malloc(&va, 20);
    _write(va + 5, z, 13);
    void *read;
    read = calloc(9, sizeof(char));
    _read(va + 9, read, 8);
    char *result = "EFGHIJKL";
    if (equals(result, read, 8) == 1) {
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }
    printf("Test %d) %s: ", 2,
           "Read from 7th to 9th");//Считываем с 7ого по девятый

    result = "CDE";
    _read(va + 7, read, 3);
    if (equals(result, read, 3) == 1) {
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }

    return SUCCESS;
}
