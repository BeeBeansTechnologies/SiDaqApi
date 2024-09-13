/*
C Simple Link List
*/
//#ifdef ＿MSC＿VER
#include "pch.h"
//#endif
#include <stdlib.h>
#include "squeue.h"



#define SQDATA_DATA( sd_ptr ) ( sd_ptr->_data_ptr)
#define SQDATA_SIZE( sd_ptr ) ( sd_ptr->_size )

/**
 * Data struct stored in queues.
 * If copy_data is not 0, store a copy of data_ptr value.
*/
struct SQData *  NewSQData(void * data_ptr, int size_in_bytes, int copy_data)
{
    struct SQData * sd_ptr = (struct SQData * )malloc(sizeof(struct SQData));
     sd_ptr->_parent = NULL;
     sd_ptr->_next = NULL;
     sd_ptr->_prev = NULL;
     sd_ptr->_size = size_in_bytes;
     if( copy_data ){
        sd_ptr->_data_ptr = malloc(size_in_bytes);
        memcpy(sd_ptr->_data_ptr, data_ptr, size_in_bytes);
     }else{
         sd_ptr->_data_ptr = (void *)data_ptr;
     }
    return  sd_ptr;
}

/// <sumaary>
/// Check next queue value without get data.
/// </summary>
/// <param name="que_ptr">Pointer to SQueue</param>
/// <param name="pPrev">Pointer to the base SQdata (NULL: get the first data)</param>
/// <returns>SQData *　Pointer to Next SQData of pPrev</returns>
const SQData* SQDataNext(struct SQueue* que_ptr, const SQData* pPrev)
{
    struct SQData* pRet = NULL;
    if (que_ptr) {
        if (que_ptr->_count == 0) return NULL;
        LOCK(que_ptr->_lock);
        if (pPrev == NULL) {
            pRet = que_ptr->_top;
        }
        else {
            SASSERT(pPrev->_parent == que_ptr);
            pRet = pPrev->_next;
        }
        if( pRet ) SASSERT(pRet->_parent == que_ptr);
        UNLOCK(que_ptr->_lock);
        return pRet;
    }
    return NULL; // not reachable.
}

/// <summary>
/// Get data value and size of SQdata.
/// If sq_data_ptr or data_size is NULL, data_size is undefined.
/// </summary>
/// <param name="sq_data_ptr">Pointer to SQData by SQDataNext()</param>
/// <param name="data_size">Data size of SQData value</param>
/// <returns>void * Pointer to SQData value</returns>
const void* SQDataPtr(const SQData* sq_data_ptr, int* data_size)
{
    if (sq_data_ptr && data_size) {
        *data_size = sq_data_ptr->_size;
        return sq_data_ptr->_data_ptr;
    }
    return NULL;    
}


void  DeleteSQData(struct SQData * sd_ptr)
{
    if( sd_ptr &&  sd_ptr->_data_ptr ){
        if(  sd_ptr->_next == NULL &&  sd_ptr->_prev == NULL ){
             sd_ptr->_size = 0;
             sd_ptr->_data_ptr = NULL;
            free( sd_ptr);
            return;
        }
    }
    printf("ERROR : DeleteSQData callled with invalid  sd_ptr");
}

struct SQueue *NewSQueue()
{
    struct SQueue *que_ptr = (struct SQueue *) malloc(sizeof(struct SQueue));
    que_ptr->_top = NULL;
    que_ptr->_last = NULL;
    que_ptr->_lock = CREATE_LOCK();
    que_ptr->_count = 0;
    return que_ptr;
}

void DeleteSQueue(struct SQueue *que_ptr)
{
    if( que_ptr && que_ptr->_lock != NULL){
        LOCK_TYPE lock_save = que_ptr->_lock;
        int data_size = -1;
        while (true) {
            void* delete_ptr = SQGet(que_ptr, &data_size, -1);
            if (delete_ptr) {
                free(delete_ptr);
            }
            else {
                break;
            }
        }
        LOCK(lock_save);
        que_ptr->_lock = NULL;
        free(que_ptr);
        UNLOCK(lock_save);
        DELETE_LOCK(lock_save);
        return;
    }
    printf("ERROR : DeleteQueue callled with invalid que_ptr");
};

void PrintSQData(struct SQData * sd_ptr)
{
    //printf("===  sd_ptr = %08lX == \n", (long) sd_ptr);
    //printf(" sd_ptr->_parent %08lX\n", (long) sd_ptr->_parent);
    //printf(" sd_ptr->_next %08lX\n", (long) sd_ptr->_next);
    //printf(" sd_ptr->_prev %08lX\n", (long) sd_ptr->_prev);
    //printf(" sd_ptr->_size %lu\n", (long) sd_ptr->_size);
    //printf(" sd_ptr->_data_ptr %08lX\n", (long) sd_ptr->_data_ptr);
    // printf(" sd_ptr->data %08lX\n", (long)& sd_ptr->data[0]);
    // printf(" sd_ptr->data %c\n",  sd_ptr->data[0]);
}

void PrintSQueue(struct SQueue *que_ptr){
    //printf("que_ptr = %08lX\n", (long)que_ptr);
    //printf("que_ptr->_top %08lX\n", (long)que_ptr->_top);
    //printf("que_ptr->_last %08lX\n", (long)que_ptr->_last);
    //printf("que_ptr->_count %lu\n", (long)que_ptr->_count);
    int count = 0;
}

void PrintSqueueLink(struct SQueue *que_ptr, const char *pPrefix)
{
    int count = 1;
    struct SQData *pMsg = NULL;
    //printf("----- %s ----- \n", pPrefix);
    //printf("SQueue: %08lX =Top=> %08lX \n", (long)que_ptr, (long)que_ptr->_top);
    //printf("SQueue:       =Last<= %08lX\n", (long)que_ptr->_last);
    //printf("SQueue: count:%u\n", que_ptr->_count);
    pMsg = que_ptr->_top;
    while(1){
        if( pMsg == NULL ) break;;
        //printf("  %u:SQData: %08lX <=Parent %08lX \n", count, (long)pMsg, (long)pMsg->_parent);
        //printf("  %u:SQData: %08lX =>next %08lX \n", count, (long)pMsg, (long)pMsg->_next);
        //printf("  %u:SQData: %08lX <=prev %08lX \n", count, (long)pMsg, (long)pMsg->_prev);
        count++;
        pMsg = pMsg->_next;
    }
}



int _sque_put(struct SQueue *que_ptr, struct SQData * sd_ptr )
{
    LOCK(que_ptr->_lock);
    struct SQData *prevLast = que_ptr->_last;
    que_ptr->_last =  sd_ptr;
     sd_ptr->_parent = que_ptr;
     sd_ptr->_prev = prevLast;
    if( prevLast ) prevLast->_next =  sd_ptr;
    if( que_ptr->_top == NULL )  que_ptr->_top =  sd_ptr;
    que_ptr->_count += 1;
    UNLOCK(que_ptr->_lock);
    // PrintSqueueLink(que_ptr, "Put");
    return que_ptr->_count;
}

struct SQData * _sque_get(struct SQueue *que_ptr, int wait_msec)
{
    SASSERT(que_ptr != NULL);
    if( que_ptr ){
        if( que_ptr->_count == 0 ) return NULL;
        if (wait_msec < 0) {
            LOCK(que_ptr->_lock);
        }
        else if( WAIT_LOCK(que_ptr->_lock, wait_msec) == WAIT_TIMEOUT ){
            return NULL;
        }
        struct SQData *pRet = que_ptr->_top;
        if (pRet) {
            SASSERT(pRet->_parent == que_ptr);
            que_ptr->_top = pRet->_next;
            if (que_ptr->_top) que_ptr->_top->_prev = NULL;
            pRet->_next = NULL;
            pRet->_prev = NULL;
            que_ptr->_count -= 1;
            if (que_ptr->_count == 0) {
                que_ptr->_top = NULL;
                que_ptr->_last = NULL;
            }
            pRet->_parent = NULL;
        }
    // PrintSqueueLink(que_ptr, "Get");
        UNLOCK(que_ptr->_lock);
        return pRet;
    }
    return NULL; // not reachable.
}

/// @brief Put data to SQueue. SQueue stores a copy data of data_ptr value,  so it must be freed when SQGet().
/// @param que_ptr Pointer to SQueue
/// @param data_ptr Pointer to data to store
/// @param data_size data size to store
/// @return queue size > 0 or negative for failed
int  SQPut(struct SQueue * que_ptr, void *data_ptr, int data_size){
    struct SQData *sd_ptr = NewSQData(data_ptr, data_size, 1);
    if( sd_ptr ){
        _sque_put(que_ptr, sd_ptr);
        return SQUEUE_COUNT(que_ptr);
    }
    return -1;
}

/// @brief Get data from SQueue
/// @param que_ptr  Pointer to Squeue
/// @param data_size  Pointer to get data size
/// @param wait_msec Positive: wait for specified (millisecondes). Negative:wait until data is acquired. 
/// @return Pointer to data.
/// NULL and data_size = 0 : Queue has no data,
/// data size < 0 : Error,
/// data_size = NULL : Undefined
void * SQGet(struct SQueue * que_ptr, int *data_size, int wait_msec)
{
    if( que_ptr == NULL || data_size == NULL ) return NULL;
    while (wait_msec > 1) {
        if (SQUEUE_COUNT(que_ptr) > 0)  break;
        SLEEP(1);
        wait_msec -= 1;
    }
    if(SQUEUE_COUNT(que_ptr)>0){
        struct SQData *sd_ptr = _sque_get(que_ptr, wait_msec);
        if( sd_ptr ){
            void *data_ptr = SQDATA_DATA( sd_ptr );
            *data_size = SQDATA_SIZE( sd_ptr );
            DeleteSQData(sd_ptr);
            return data_ptr;
        }
        else if (wait_msec < 0) {
            *data_size = -2;  // internal error: Although queue count > 0, queue has no data. 
        }
        else {
            *data_size = 0;
        }
        return NULL;
    }else{
        *data_size = 0;
        return NULL;
    }

}