#pragma once

#ifndef _sock_thread_h_
#define _sock_thread_h_

#include "squeue_c/squeue.h"
#define DEFAULT_RECV_BUFF_SIZE 65536*4
#define DEFAULT_RECV_QUEUE_COUNT_LIMIT 0 //0:No Limit


struct SockStat_s {
	unsigned long long tx_bytes;
	unsigned long long rx_bytes;
	unsigned long long rx_duration;//Total receive time
	int last_error;
};

typedef enum _SockQueueErrorCode {
	SOCKQUEUE_NO_ERROR = 0,
	SOCKQUEUE_EXCEED_RECV_QUEUE_COUNT = 1,
	SOCKQUEUE_MEMORY_ALLOCATED_FAILED = 2,

}SockQueueErroeCode;

struct SockThreadLparam {
	BOOL _run;
	SOCKET device_socket;
	int squeue_index;//index for received data 0 for command reply, 1 for events.
	int squeue_count;
	struct SQueue** squeue_list_ptr;// 3 Squeue ptr, 0, 1 for receive, 2 for tx.
	struct SockStat_s sock_stat;
	UINT recv_buf_size;
	UINT queue_limit;
	int queue_error_code;
	bool pause_flg;
};

#define SOCKTHREAD_IS_RUN(sock_thread) (sock_thread->_run)

DWORD WINAPI socket_loop_thread(LPVOID lParam);
const char* decode_error(int nCode);
int is_allowed_error(int nCode);

#endif //_sock_thread_h_