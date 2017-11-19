#include <stdio.h>
#include <time.h>
#include "load_tests.h"
#include "../mmemory.h"
#include <malloc.h>

void loadTests(){
    FILE *fp;
    fp = fopen("C:\\Users\\Sinelnikov\\Desktop\\memory_manager-master\\test.txt", "w+");

    time_t actual_time,actual_time1;
    double actual_time_sec;


    int pageNumber = 5;
    int pageSize = 8;

    _init(pageNumber,pageSize);
    VA va;
    char * z;
    char a = 'a';

    for (int infoSize = 1; infoSize <= pageSize*pageNumber; infoSize++) {
        printf("%d\n",infoSize);
        z = calloc(infoSize, sizeof(char));
        for (int i = 0; i < infoSize; ++i) {
            z[i] = a;
        }
        actual_time1 = clock();
        _malloc(&va,infoSize);
        _write(va,z,infoSize);
        actual_time = clock();
        //_read(va,z,pageToAddNumber*pageSize);
        actual_time_sec = difftime(actual_time,actual_time1);

        fprintf(fp,"%d;%.0f\n",infoSize,actual_time_sec);
        a++;
    }

    pageSize = 2;
    pageNumber  = 20;
    a = 'a';
    _init(pageNumber,pageSize);
    for (int infoSize = 1; infoSize <= pageSize*pageNumber; infoSize++) {
        printf("%d\n",infoSize);
        z = calloc(infoSize, sizeof(char));
        for (int i = 0; i < infoSize; ++i) {
            z[i] = a;
        }
        actual_time1 = clock();
        _malloc(&va,infoSize);
        _write(va,z,infoSize);
        actual_time = clock();
        //_read(va,z,pageToAddNumber*pageSize);
        actual_time_sec = difftime(actual_time,actual_time1);
        fprintf(fp,"%d;%.0f\n",infoSize,actual_time_sec);
        a++;
    }
    fclose(fp);
}

