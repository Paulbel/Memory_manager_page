#include "mmemory.h"
#include "paging.h"
#include <malloc.h>
#include <string.h>




struct page *memory = NULL;
struct page *HDD = NULL;
unsigned int BLOCK_ON_PAGE_NUMBER;
unsigned int PAGE_MEMORY_NUMBER;
unsigned int PAGE_HDD_NUMBER = 4;
struct pageInfo *pageTable = NULL;


struct pageNode {
    int index;
    int value;
};

int findFreePageIndexInMemory() {
    for (int pageIndex = 0; pageIndex < PAGE_MEMORY_NUMBER; pageIndex++) {
        if (memory[pageIndex].hasVirtualPage == false) {
            return pageIndex;
        }
    }
    return WRONG_PARAMETRS;
}

int findFreePageIndexInHDD() {
    for (int pageIndex = 0; pageIndex < PAGE_HDD_NUMBER; pageIndex++) {
        if (HDD[pageIndex].hasVirtualPage == false) {
            return pageIndex;
        }
    }
    return WRONG_PARAMETRS;
}


int findNumberPagesToMove() {
    int length = 0;
    for (int pageIndex = 0; pageIndex < PAGE_MEMORY_NUMBER; pageIndex++) {
        if (memory[pageIndex].hasVirtualPage == true) {
            length++;
        }
    }
}

int *createIndexArray(int length) {
    struct pageNode *nodeArray = calloc(length, sizeof(struct pageNode));
    int index = 0;
    int pageIndex = 0;
    while (index < PAGE_MEMORY_NUMBER) {
        if (memory[index].hasVirtualPage == true) {
            nodeArray[pageIndex].index = index;
            nodeArray[pageIndex].value = memory[index].timesRead;
            pageIndex++;
        }
        index++;
    }
    struct pageNode node;
    for (int nodeIndex = 0; nodeIndex < (length - 1); nodeIndex++) {
        for (int index = 0; index < length - nodeIndex - 1; index++) {
            if (nodeArray[index].value > nodeArray[index + 1].value) /* For decreasing order use < */
            {
                node = nodeArray[index];
                nodeArray[index] = nodeArray[index + 1];
                nodeArray[index + 1] = node;
            }
        }
    }

    int *array = calloc(length, sizeof(int));

    for (int pageNodeIndex = 0; pageNodeIndex < length; pageNodeIndex++) {
        array[pageNodeIndex] = nodeArray[pageNodeIndex].index;
    }
    return array;
}


int moveToHDD(int pageIndex) {
    struct pageInfo *pageInfo = pageTable;
    while (pageInfo->nextDescriptor != NULL) {
        if (pageInfo->offsetPage == pageIndex && pageInfo->isUse == true) {
            break;
        }
        pageInfo = pageInfo->nextDescriptor;
    }
    int newPageIndex = findFreePageIndexInHDD();
    if (newPageIndex == -1) {
        return WRONG_PARAMETRS;
    }

    //workWithHdd();
    struct page tempPage = HDD[newPageIndex];
    HDD[newPageIndex] = memory[pageIndex];
    memory[pageIndex] = tempPage;
    pageInfo->offsetPage = newPageIndex;
    pageInfo->isUse = false;

    return newPageIndex;
}


int moveToMemory(struct pageInfo *descriptor) {
    int newPageIndex = findFreePageIndexInMemory();
    if (newPageIndex == -1) {
        return WRONG_PARAMETRS;
    }
    //workWithHdd();
    struct page tempPage = memory[newPageIndex];
    memory[newPageIndex] = HDD[descriptor->offsetPage];
    HDD[descriptor->offsetPage] = tempPage;
    descriptor->isUse = true;
    return newPageIndex;
}


int allocateBlocksOnPage(int blockIndex, int szBlock, struct page *page) {
    int spaceRemaining = szBlock;
    while (spaceRemaining > 0 && page->maxSizeFreeBlock > 0) {
        page->isFreeBlock[blockIndex] = false;
        page->maxSizeFreeBlock--;
        blockIndex++;
        spaceRemaining--;
    }
    return spaceRemaining;
}


VA convertToVA(int pageNumber, int offset) {
    VA addr = 0;
    unsigned long long pageNumberLong = pageNumber;

    pageNumberLong = pageNumberLong << (8 * (sizeof(unsigned long long) / 2));

    unsigned long long offsetLong = offset;
    unsigned long long result = (unsigned long long) addr;

    result = result | pageNumberLong;
    result = result | offsetLong;

    addr = (VA) result;
    return addr;
}

void printMemory() {
    printf("%s \n", "MEMORY:");
    for (int pageIndex = 0; pageIndex < PAGE_MEMORY_NUMBER; pageIndex++) {
        printf("%d page (%d times read)\n", pageIndex, memory[pageIndex].timesRead);
        for (int blockIndex = 0; blockIndex < BLOCK_ON_PAGE_NUMBER; blockIndex++) {

            if (memory[pageIndex].isFreeBlock[blockIndex] == true) {
                printf("%d) _ ", blockIndex);
            } else {
                printf("%d) %c ", blockIndex, memory[pageIndex].blocks[blockIndex].info);
            }
        }
        printf("\n");
    }

    printf("%s \n", "HDD:");
    for (int pageIndex = 0; pageIndex < PAGE_HDD_NUMBER; pageIndex++) {
        printf("%d page (%d times read)\n", pageIndex, HDD[pageIndex].timesRead);
        for (int blockIndex = 0; blockIndex < BLOCK_ON_PAGE_NUMBER; blockIndex++) {

            if (HDD[pageIndex].isFreeBlock[blockIndex] == true) {
                printf("%d) _ ", blockIndex);
            } else {
                printf("%d) %c ", blockIndex, HDD[pageIndex].blocks[blockIndex].info);

            }
        }
        printf("\n");
    }


}


int _init(int n, int szPage) {
    if (n <= 0 || szPage <= 0) {
        return WRONG_PARAMETRS;
    }
    BLOCK_ON_PAGE_NUMBER = (unsigned int) szPage;
    PAGE_MEMORY_NUMBER = (unsigned int) n;
    memory = calloc(n, sizeof(struct page));
    HDD = calloc(PAGE_HDD_NUMBER, sizeof(struct page));
    struct page *currentPage;
    for (int pageIndex = 0; pageIndex < n; pageIndex++) {
        currentPage = &memory[pageIndex];
        currentPage->maxSizeFreeBlock = (unsigned int) szPage;
        currentPage->blocks = calloc((size_t) szPage, sizeof(struct block));
        currentPage->hasVirtualPage = false;
        currentPage->isFreeBlock = calloc((size_t) szPage, sizeof(bool));
        for (int blockIndex = 0; blockIndex < szPage; blockIndex++) {
            currentPage->isFreeBlock[blockIndex] = true;
        }
    }
    for (int pageIndex = 0; pageIndex < PAGE_HDD_NUMBER; pageIndex++) {
        currentPage = &HDD[pageIndex];
        currentPage->maxSizeFreeBlock = (unsigned int) szPage;
        currentPage->blocks = calloc((size_t) szPage, sizeof(struct block));
        currentPage->hasVirtualPage = false;
        currentPage->isFreeBlock = calloc((size_t) szPage, sizeof(bool));
        for (int blockIndex = 0; blockIndex < szPage; blockIndex++) {
            currentPage->isFreeBlock[blockIndex] = true;
        }
    }
    //printf("_init: \n ");
    //printMemory();
    return SUCCESS;
}


int _malloc(VA *ptr, size_t szBlock) {
    if (szBlock > 10000000) {
        return WRONG_PARAMETRS;
    }
    int freeBlockIndex = 0;
    int pageIndex = 0;
    int descrIndex = 0;
    int dataRemaining = szBlock;
    struct pageInfo *lastPageDescriptor = pageTable;
    if (pageTable != NULL) {
        while (lastPageDescriptor->nextDescriptor != NULL) {
            descrIndex++;
            lastPageDescriptor = lastPageDescriptor->nextDescriptor;
        }
        pageIndex = lastPageDescriptor->offsetPage;
        struct page *lastPage;
        if (lastPageDescriptor->isUse == true) {
            lastPage = &memory[pageIndex];
        } else {
            lastPage = &HDD[pageIndex];
        }
        if (lastPage->maxSizeFreeBlock > 0) {
            for (int blockIndex = 0; blockIndex < BLOCK_ON_PAGE_NUMBER; blockIndex++) {
                if (lastPage->isFreeBlock[blockIndex] == true) {
                    freeBlockIndex = blockIndex;
                    break;
                }
            }
            dataRemaining = allocateBlocksOnPage(freeBlockIndex, dataRemaining, lastPage);
        } else {
            descrIndex++;
        }

    }
    while (dataRemaining > 0) {
        struct page *pageToFill;
        struct pageInfo *pageDescriptor = malloc(sizeof(struct pageInfo));
        if (lastPageDescriptor != NULL) {
            lastPageDescriptor->nextDescriptor = pageDescriptor;
        } else {
            pageTable = pageDescriptor;
        }


        int freePageIndex = findFreePageIndexInMemory();
        if (freePageIndex != -1) {
            pageToFill = &memory[freePageIndex];
            pageDescriptor->isUse = true;
        } else {
            freePageIndex = findFreePageIndexInHDD();
            if (freePageIndex != -1) {
                pageToFill = &HDD[freePageIndex];
                pageDescriptor->isUse = false;
            } else {
                return MEMORY_LACK;
            }
        }
        pageToFill->hasVirtualPage = true;
        pageDescriptor->offsetPage = freePageIndex;
        pageDescriptor->nextDescriptor = NULL;
        dataRemaining = allocateBlocksOnPage(0, dataRemaining, pageToFill);
        lastPageDescriptor = pageDescriptor;
    }
    *ptr = convertToVA(descrIndex, freeBlockIndex);
    return SUCCESS;
}

int getPageNumberFromAddress(VA address) {
    unsigned long long longAdress = (unsigned long long) address;
    unsigned long long offset = longAdress & (4294967295);
    int newPageNumber = 0;
    while (offset > BLOCK_ON_PAGE_NUMBER) {
        offset -= BLOCK_ON_PAGE_NUMBER;
        newPageNumber++;
        longAdress -= BLOCK_ON_PAGE_NUMBER;
    }

    return (int) (longAdress >> 32) + newPageNumber;
}

int getOffsetFromAdress(VA address) {
    unsigned long long longAdress = (unsigned long long) address;
    unsigned long long offset = longAdress & (4294967295);
    while (offset > BLOCK_ON_PAGE_NUMBER) {
        offset -= BLOCK_ON_PAGE_NUMBER;
    }
    return (int) offset;
}


int _read(VA ptr, void *pBuffer, size_t szBuffer) {
    if (szBuffer > 100000||szBuffer > PAGE_MEMORY_NUMBER*BLOCK_ON_PAGE_NUMBER) {
        return WRONG_PARAMETRS;
    }
    int infoRemaining = (int) szBuffer;
    char info[szBuffer + 1];

    int pageNumber = getPageNumberFromAddress(ptr);
    int offset = getOffsetFromAdress(ptr);
    int currentBlockIndex = 0;
    struct pageInfo *currentDescriptor = pageTable;
    while (pageNumber > 0) {
        currentDescriptor = currentDescriptor->nextDescriptor;
        pageNumber--;
    }

    struct page *currentPage;

    int indexInSequence = 0;
    int *indexArray = NULL;
    int length = 0;

    while (infoRemaining > 0) {
        if (currentDescriptor->isUse == false) {
            if (moveToMemory(currentDescriptor) == -1) {
                if (indexArray == NULL) {
                    length = findNumberPagesToMove();
                    indexArray = createIndexArray(length);
                }
                if (indexInSequence >= length) {
                    return WRONG_PARAMETRS;
                }
                //printf("READ: move, size = %d,\n", szBuffer);
                moveToHDD(indexArray[indexInSequence]);
                indexInSequence++;
                currentDescriptor->offsetPage = moveToMemory(currentDescriptor);
                currentDescriptor->isUse = true;
            }

        }
        currentPage = &memory[currentDescriptor->offsetPage];
        currentPage->timesRead++;
        while (offset < BLOCK_ON_PAGE_NUMBER && infoRemaining > 0) {
            info[currentBlockIndex] = currentPage->blocks[offset].info;
            offset++;
            infoRemaining--;
            currentBlockIndex++;
        }
        offset = 0;
        currentDescriptor = currentDescriptor->nextDescriptor;

    }
    info[szBuffer] = '\0';
    strcpy(pBuffer, info);

    return SUCCESS;
}


int _write(VA ptr, void *pBuffer, size_t szBuffer) {
    if (szBuffer > 100000||szBuffer > PAGE_MEMORY_NUMBER*BLOCK_ON_PAGE_NUMBER) {
        return WRONG_PARAMETRS;
    }
    int infoRemaining = (int) szBuffer;
    char *info = pBuffer;
    int pageNumber = getPageNumberFromAddress(ptr);
    int offset = getOffsetFromAdress(ptr);
    int currentBlockIndex = 0;
    struct pageInfo *currentDescriptor = pageTable;
    while (pageNumber > 0) {
        if (currentDescriptor->nextDescriptor != NULL) {
            currentDescriptor = currentDescriptor->nextDescriptor;
            pageNumber--;
        } else {
            return MEMORY_LACK;
        }
    }

    struct page *currentPage;

    int indexInSequence = 0;
    int *indexArray = NULL;
    int length = 0;

    while (infoRemaining > 0) {
        if (currentDescriptor->isUse == false) {
            if (
                    moveToMemory(currentDescriptor)
                    == -1) {
                if (indexArray == NULL) {
                    length = findNumberPagesToMove();
                    indexArray = createIndexArray(length);
                }
                if (indexInSequence >= length) {
                    return WRONG_PARAMETRS;
                }
                moveToHDD(indexArray[indexInSequence]);
                indexInSequence++;
                currentDescriptor->offsetPage = moveToMemory(currentDescriptor);
                currentDescriptor->
                        isUse = true;
            }

        }
        currentPage = &memory[currentDescriptor->offsetPage];
        currentPage->timesRead++;
        while (offset < BLOCK_ON_PAGE_NUMBER && infoRemaining > 0) {
            currentPage->blocks[offset].
                    info = info[currentBlockIndex];
            offset++;
            infoRemaining--;
            currentBlockIndex++;
        }
        offset = 0;
        currentDescriptor = currentDescriptor->nextDescriptor;

    }
//printf("_write: \n");
//printMemory();
//memcpy(info, pBuffer, szBuffer);
    return SUCCESS;
}

int _free(VA ptr) {
    int pageNumber = getPageNumberFromAddress(ptr);
    int blockIndex = getOffsetFromAdress(ptr);
    struct pageInfo *currentDescriptor = pageTable;
    while (pageNumber > 0) {
        if (currentDescriptor->nextDescriptor != NULL) {
            currentDescriptor = currentDescriptor->nextDescriptor;
        } else {
            return WRONG_PARAMETRS;
        }
        pageNumber--;
    }
    struct page *page;
    if (currentDescriptor->isUse = true) {
        page = &memory[currentDescriptor->offsetPage];
    } else {
        page = &HDD[currentDescriptor->offsetPage];
    }
    for (int blockIndex = 0; blockIndex < BLOCK_ON_PAGE_NUMBER; blockIndex++) {
        page->isFreeBlock[blockIndex] = true;
        page->hasVirtualPage = false;
        page->maxSizeFreeBlock = BLOCK_ON_PAGE_NUMBER;
        page->timesRead = 0;
    }
    return SUCCESS;
}


