#include <stdbool.h>
#include "mmemory.h"

struct block {
    //struct block*	pNext;	// следующий свободный или
    // занятый блок
    char  info;
    //unsigned int	szBlock;	// размер блока
};

struct page {
//    struct block*	pFirstFree;		// первый свободный блок
//    struct block*	pFirstUse;		// первый занятый блок
    unsigned int	maxSizeFreeBlock;	// макс. размер сводного блока
    bool hasVirtualPage;
    struct block * blocks;
    bool * isFreeBlock;
    unsigned int timesRead;

};

struct pageInfo {
    unsigned int offsetPage;		// смещение страницы от начала
    // памяти или файла
    bool			isUse;		// флаг, описывающий находится ли
    // страница в памяти или на диске
    struct pageInfo * nextDescriptor;

};
