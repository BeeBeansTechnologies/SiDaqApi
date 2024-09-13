#pragma once
#ifndef _SQUEUE_H_
#define _SQUEUE_H_
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef _MSC_VER

#define LOCK_TYPE HANDLE
#define CREATE_LOCK() CreateMutex(NULL, FALSE, NULL)
#define DELETE_LOCK(lock) CloseHandle(lock)
#define LOCK(lock)  WaitForSingleObject(lock, INFINITE)
#define WAIT_LOCK(lock, msec)  WaitForSingleObject(lock, (DWORD)msec)
//#define WAIT_TIMEOUT same on windows api
#define UNLOCK(lock) ReleaseMutex(lock);
#define SASSERT(condition) assert(condition)
#define SLEEP(msec) Sleep(msec)

#else 

#define LOCK_TYPE void *
#define CREATE_LOCK() malloc(4)
#define DELETE_LOCK(lock) free(lock)
#define LOCK(lock) ;
#define WAIT_LOCK(lock, msec) 
#define WAIT_TIMEOUT -1
#define UNLOCK(lock) ;
#define SASSERT(condition) assert(condition)
#define SLEEP(msec)  sleep(msec)

#endif 

#define DEFAULT_DATA 4

// extern struct SQueue;

struct SQData {
    struct SQueue* _parent;
    struct SQData* _next;
    struct SQData* _prev;
    int _size;
    void* _data_ptr;
};

struct SQueue {
    struct SQData *_top;
    struct SQData *_last;
    LOCK_TYPE _lock;
    int _count;
};

struct SQueue *  NewSQueue();
void  DeleteSQueue(struct SQueue *que_ptr);
#define SQUEUE_COUNT(que_ptr) (que_ptr->_count)

//#define SQUEUE_SIZE( que_ptr) ( que_ptr->_size)
// struct SQData *NewMessage(int size);
int  SQPut(struct SQueue * que_ptr, void *data_ptr, int data_size);
void *  SQGet(struct SQueue * que_ptr, int *data_size, int wait_msec);

const SQData* SQDataNext(struct SQueue* que_ptr, const SQData* pPrev);
const void* SQDataPtr(const SQData* sq_data_ptr, int* data_size);


void PrintSQueue(struct SQueue * que_ptr);
void PrintSqueueLink(struct SQueue * que_ptr, const char *prefix_ptr);


#endif //_SQUEUE_H_