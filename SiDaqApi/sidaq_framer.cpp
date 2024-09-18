//
// sidaq_framer.cpp
// ~~~~~~~~~~~~~~  
// 
// Copyright (C) 2024  Bee Beans Technologies Co.,Ltd.
//
// Released under the MIT License.
// (See accompanying file LICENSE)
//
#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "squeue_c/squeue.h"
#include "sidaq_framer.h"


SiDaqFramer* NewSiDaqFramer()
{
    SiDaqFramer* framer_ptr = (SiDaqFramer*)malloc(sizeof(SiDaqFramer));
    if (framer_ptr) {
        memset(framer_ptr, 0, sizeof(SiDaqFramer));
        framer_ptr->_buff_size = DEFAULT_MAX_FRAME_LEN;
        framer_ptr->_buff_ptr = (unsigned char*)malloc(framer_ptr->_buff_size);
        if (framer_ptr->_buff_ptr) {
            memset(framer_ptr->_buff_ptr, 0, framer_ptr->_buff_size);
        }
        framer_ptr->_error_flg = 0;
    }
    return framer_ptr;

}
void PrintFramer(SiDaqFramer* framer_ptr, const char* prefix_ptr)
{
    int i = 0;
    if (prefix_ptr) {
        printf(" - %s -----\n", prefix_ptr);
    }
    for (i = 0; i < 8; i++) {
        printf(" %02X", framer_ptr->_buff_ptr[i]);
    }
    printf("---\n");
}

void DellSiDaqFramer(SiDaqFramer* pDelFramer)
{
    if (pDelFramer && pDelFramer->_buff_ptr) {
        free(pDelFramer->_buff_ptr);
        pDelFramer->_buff_ptr = NULL;
        free(pDelFramer);
    }

}

int RecvSiDaqData(SiDaqFramer* framer_ptr, unsigned char* data_ptr, int data_len, struct SQueue* pSQueue)
{
    int frame_count = 0;//SQUEUE_SIZE(pSQueue);///return value extracted frames
    int event_queue_count = 0;
    unsigned short short_len = 0;
    int event_len = 0;
    unsigned char* read_ptr = data_ptr;
    int copy_len = 0;
    if (data_len < 0) {
        return 0;
    }
    if (framer_ptr->_rest > 0) {//If there is any remaining, concatenate and get 1 frame(if any) and shift the buffer
        copy_len = (framer_ptr->_buff_size - framer_ptr->_rest);//Maximum size that can be copied = total maximum frame size
        if (copy_len < 0) {
            //frame buf error
            framer_ptr->_error_code = FRAMER_BUF_SIZE_ERROR;
            return -1;
        }
        if (data_len < copy_len) {
            copy_len = data_len;//If the received data is smaller than the number of bytes that can be copied
        }
        memcpy(&framer_ptr->_buff_ptr[framer_ptr->_rest], data_ptr, copy_len);
        copy_len = framer_ptr->_rest + copy_len;//Maximum size in buffer
        if (framer_ptr->_lengthFunc != NULL && framer_ptr->_header_len != 0) {
            event_len = framer_ptr->_lengthFunc(framer_ptr->_buff_ptr, copy_len, framer_ptr->_header_len);
        }
        else if (framer_ptr->_fixed_frame_len != 0) {
            event_len = framer_ptr->_fixed_frame_len;
        }
        else {
            event_len = copy_len;
        }
        if (event_len < 0) {
            //failed frame length
            framer_ptr->_error_code = FRAMER_GET_FRAME_LENGTH_FAILED;
            // put null framer
            framer_ptr->_lengthFunc = NULL;
            framer_ptr->_header_len = 0;
            framer_ptr->_fixed_frame_len = 0;
            event_len = copy_len;
        }

        if ((UINT)(event_len + framer_ptr->_header_len) > framer_ptr->_max_frame_len) {
            //max frame len error
            framer_ptr->_error_code = FRAMER_EXCEED_MAX_FRAME_LENGTH;
            // put null framer
            framer_ptr->_lengthFunc = NULL;
            framer_ptr->_header_len = 0;
            framer_ptr->_fixed_frame_len = 0;
            event_len = copy_len;
        }

        if (copy_len >= event_len + framer_ptr->_header_len) {//Create one frame with remaining data.
            event_queue_count = SQPut(pSQueue, framer_ptr->_buff_ptr, event_len + framer_ptr->_header_len);
            if (event_queue_count < 0) {
                //memory allocation error
                framer_ptr->_error_code = FRAMER_MEMORY_ALLOCATED_FAILED;
                return -1;

            }
            if (framer_ptr->_queue_limit != 0 && (UINT)event_queue_count >= framer_ptr->_queue_limit) {
                //queue limit error
                framer_ptr->_error_code = FRAMER_EXCEED_FRAME_QUEUE_COUNT;
            }
            frame_count++;
            
            if (data_len > event_len + framer_ptr->_header_len - framer_ptr->_rest) {
                read_ptr = &data_ptr[event_len + framer_ptr->_header_len - framer_ptr->_rest];
                data_len -= (event_len + framer_ptr->_header_len - framer_ptr->_rest); //Remaining number of bytes after processing one frame by adding the rest to the received data. 

            }
            else {//just size. 
                //framer_ptr->_rest = 0;
                read_ptr = NULL;
                data_len = 0;
            }
            framer_ptr->_rest = 0;
        }
        else {//frame did not compoleted.
            framer_ptr->_rest = copy_len;
            read_ptr = NULL;
            data_len = 0;
        }
    }

    while (read_ptr != NULL) {

        if (data_len == 0 || data_len < framer_ptr->_header_len) {// not enough length for HD field
            //            if( data_len > 0 ){# issue #18
            framer_ptr->_rest = data_len;
            framer_ptr->_buff_ptr[0] = read_ptr[0];
            //          }
            break;
        }

        if (framer_ptr->_lengthFunc != NULL && framer_ptr->_header_len) {
            event_len = framer_ptr->_lengthFunc(read_ptr, data_len, framer_ptr->_header_len);
        }
        else if (framer_ptr->_fixed_frame_len != 0) {
            event_len = framer_ptr->_fixed_frame_len;
        }
        else {
            event_len = data_len;
        }

        if (event_len < 0) {
            //failed frame length
            framer_ptr->_error_code = FRAMER_GET_FRAME_LENGTH_FAILED;
            // put null framer
            framer_ptr->_lengthFunc = NULL;
            framer_ptr->_header_len = 0;
            framer_ptr->_fixed_frame_len = 0;
            event_len = data_len;
        }

        if ((UINT)(event_len + framer_ptr->_header_len) > framer_ptr->_max_frame_len) {
            //max frame len error
            framer_ptr->_error_code = FRAMER_EXCEED_MAX_FRAME_LENGTH;
            framer_ptr->_lengthFunc = NULL;
            framer_ptr->_header_len = 0;
            framer_ptr->_fixed_frame_len = 0;
            event_len = data_len;
        }

        if (data_len >= event_len + framer_ptr->_header_len) {
            event_queue_count = SQPut(pSQueue, read_ptr, event_len + framer_ptr->_header_len);
            if (event_queue_count < 0) {
                //memory allocation error
                framer_ptr->_error_code = FRAMER_MEMORY_ALLOCATED_FAILED;
                return -1;

            }
            if (framer_ptr->_queue_limit != 0 && (UINT)event_queue_count >= framer_ptr->_queue_limit) {
                //queue limit error
                framer_ptr->_error_code = FRAMER_EXCEED_FRAME_QUEUE_COUNT;
                return -1;
            }
            frame_count++;
            read_ptr = &read_ptr[event_len + framer_ptr->_header_len];
            data_len -= event_len + framer_ptr->_header_len;
        }
        else {
            framer_ptr->_rest = data_len;
            memcpy(framer_ptr->_buff_ptr, read_ptr, data_len);
            read_ptr = NULL;
            data_len = 0;
        }
    }
    
    return frame_count;
}
