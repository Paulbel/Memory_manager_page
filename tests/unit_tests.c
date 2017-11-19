#include <stdio.h>
#include "unit_tests.h"
#include <malloc.h>



void _malloc_test(int testNum, int szBlock, int resultCode, char *definition) {
    VA ptr;
    printf("Test %d) %s: ", testNum, definition);
    int returnValue = _malloc(&ptr, szBlock);
    if (returnValue == resultCode)
        printf("PASSED\n");
    else
        printf("FAILED\n");
}

void _free_test(int testNum, VA ptr, int resultCode, char *definition) {
    printf("Test %d) %s: ", testNum, definition);
    int returnValue = _free(ptr);
    if (returnValue == resultCode)
        printf("PASSED\n");
    else
        printf("FAILED\n");
}

void _write_test(int testNum, VA ptr, void *buffer, size_t size, int resultCode, char *definition) {
    printf("Test %d) %s: ", testNum, definition);
    int returnValue = _write(ptr, buffer, size);
    if (returnValue == resultCode)
        printf("PASSED\n");
    else
        printf("FAILED\n");
}

void _read_test(int testNum, VA ptr, void *buffer, size_t size, int resultCode, char *definition) {
    printf("Test %d) %s: ", testNum, definition);
    int returnValue = _read(ptr, buffer, size);
    if (returnValue == resultCode)
        printf("PASSED\n");
    else
        printf("FAILED\n");
}

void _init_test(int testNum, int n, int szPage, int resultCode, char *definition) {
    printf("Test %d) %s: ", testNum, definition);
    int returnValue = _init(n, szPage);
    if (returnValue == resultCode)
        printf("PASSED\n");
    else
        printf("FAILED\n");
}

void run_init_tests() {
    printf("\n--------_init tests--------\n");
    _init_test(1, 5, 32, 0, "Good parameters");
    _init_test(2, -2, 32, -1, "Negative block on page number");
    _init_test(3, 5, -15, -1, "Negative page number");
}

void run_malloc_tests() {
    _init(4, 4);// начальные условия: 4 стр, размер 4
    printf("\n--------_malloc tests--------\n");

    _malloc_test(1, -2, -1, "Negative block size");//1
    _malloc_test(2, 1, 0, "Page table creation");//2
    _malloc_test(3, 3, 0, "Filling block that already has info");//3
    _malloc_test(4, 4, 0, "Finding another free block in memory");//4
    _malloc_test(5, 9, 0, "Finding free block in HDD");//5
    _malloc_test(6, 3, 0, "Filling block that already has info in HDD");//6
    _malloc_test(7, 13, -2, "Memory lack");//6

}

void run_free_tests() {
//    freeAll();
    // freeGlobalVars();
    //начальные условия
    printf("\n--------_free tests--------\n");
    _init(10, 4);
    VA addr = 0;
    _malloc(&addr, 16);
    _free_test(1, addr, 0, "Good parameters");
    // VA a = (VA) 10100000000000000000000000000000000;
    //_free_test(1,&a,-1);
}

int equals(char *array1, char *array2, int size) {
    for (int index = 0; index < size; index++) {
        if (array1[index] != array2[index]) {
            return WRONG_PARAMETRS;
        }
    }
    return UNKNOWN_ERROR;
}


void run_write_tests() {
    printf("\n--------_write tests--------\n");
    _init(2, 4);
    char *z = "ABCDEFGHIJKLMNO";
    VA addr = 0;
    _malloc(&addr, 20);
    _write_test(1, addr, z, -1, -1, "Negative block size");
    VA badAddr = (char *) -2;
    _write_test(2, badAddr, z, -1, -1, "Bad address(negative)");
    _write_test(3, addr, z, 4, 0, "In memory");
    _write_test(4, addr + 8, z, 4, 0, "On HDD");
    _write_test(5, addr + 8, z, 6, 0, "Array");
    _write_test(6, addr + 25, z, 8, -2, "Address is out of block");
}

void run_read_tests() {
    printf("\n--------_read tests--------\n");
    _init(2, 4);
    char *z = "ABCDEFGHIJKLMNO";
    VA addr = 0;
    _malloc(&addr, 20);

    _write(addr, z, 15);
    _read_test(1, addr, z, -1, -1, "Negative block size");
    VA badAddr = (char *) -2;

    _write_test(2, badAddr, z, -1, -1, "Bad address(negative)");
    _write_test(3, addr, z, 4, 0, "In memory");
    _write_test(4, addr + 8, z, 4, 0, "On HDD");
    _write_test(5, addr + 8, z, 6, 0, "Array");
    _write_test(6, addr + 25, z, 8, -2, "Address is out of block");
}


void run_address_arithmetic_tests() {
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
}

void assert(){

}
