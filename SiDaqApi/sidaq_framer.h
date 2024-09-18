//
// sidaq_framer.h
// ~~~~~~~~~~~~~~  
//
// Copyright (C) 2024  Bee Beans Technologies Co.,Ltd.
//
// Released under the MIT License.
// (See accompanying file LICENSE)
//
#ifndef _SIDAQ_FRAMER_H_
#define _SIDAQ_FRAMER_H_

#include "squeue_c/squeue.h"

#ifndef u_char
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
#endif


#define DEFAULT_MAX_FRAME_LEN 262144
#define FRAME_LEN_ERROR -1
#define DEFALT_FRAME_QUEUE_COUNT_LIMIT 0 //0:No Limit

/**VALUE SWAP MACRO*/
#define SWAPLONGLONG(_long_value64_)	(((0xff00000000000000LL&_long_value64_)>>56)|\
										 ((0x00ff000000000000LL&_long_value64_)>>40)|\
										 ((0x0000ff0000000000LL&_long_value64_)>>24)|\
										 ((0x000000ff00000000LL&_long_value64_)>>8)|\
										 ((0x00000000ff000000LL&_long_value64_)<<8)|\
										 ((0x0000000000ff0000LL&_long_value64_)<<24)|\
										 ((0x000000000000ff00LL&_long_value64_)<<40)|\
										 ((0x00000000000000ffLL&_long_value64_)<<56))

#define SWAPLONG(_long_value_)	(((_long_value_&0xff000000)>>24)|\
								 ((_long_value_&0x00ff0000)>>8) |\
								 ((_long_value_&0x0000ff00)<<8) |\
						 		 ((_long_value_&0x000000ff)<<24) )


#define SWAPSHORT(_short_value_)	(((_short_value_&0x00ff)<<8)|((_short_value_&0xff00)>>8))

typedef enum _FramerErrorCode {
	FRAMER_NO_ERROR = 0,
	FRAMER_INVALID_PARAMETER = 1,
	FRAMER_MEMORY_ALLOCATED_FAILED = 2,
	FRAMER_EXCEED_FRAME_QUEUE_COUNT = 3,
	FRAMER_GET_FRAME_LENGTH_FAILED = 4,
	FRAMER_EXCEED_MAX_FRAME_LENGTH = 5,
	FRAMER_BUF_SIZE_ERROR = 6,
}FramerErroeCode;

typedef struct _SiDaqFramer {
	int _rest;//Data length remaining from previous framing processing
	int _buff_size;
	unsigned char* _buff_ptr;
	int (*_lengthFunc)(unsigned char*, UINT, UINT);
	int _header_len;
	int _error_flg;
	int _error_code;
	UINT _queue_limit;
	UINT _max_frame_len;
	UINT _fixed_frame_len = 0;
}SiDaqFramer;

SiDaqFramer* NewSiDaqFramer();
void DellSiDaqFramer(SiDaqFramer* framer_ptr);

int RecvSiDaqData(SiDaqFramer* framer_ptr, unsigned char* data_ptr, int nLen, struct SQueue* que_ptr);

void PrintFramer(SiDaqFramer* framer_ptr, const char* prefix_ptr);

#endif // _SIDAQ_FRAMER_H_

