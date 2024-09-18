//
// SiDaqApi.h
// ~~~~~~~~~~~~~~  
//
// Copyright (C) 2024  Bee Beans Technologies Co.,Ltd.
//
// Released under the MIT License.
// (See accompanying file LICENSE)
//

// main header file for the SiDaqApi DLL

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSiDaqApiApp
// See SiDaqApi.cpp for the implementation of this class
//

class CSiDaqApiApp : public CWinApp
{
public:
	CSiDaqApiApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


#include <WinSock2.h>// u_short
#include <atlstr.h >//CString

#ifdef SIDAQAPI_EXPORTS
#define SiDaq_API __declspec(dllexport)
#else
#define SiDaq_API __declspec(dllimport)
#endif
#include "sitcp/SiTcpRbcpLib2.h"
#include "squeue_c/squeue.h"
#include "sock_thread.h"
#include "sidaq_framer.h"

using namespace SiTcpRbcpLib2;

const int SiDAQ_HOST_NAME_MAX = 256; // max host name char size.
const int SiDAQ_ERROR_MSG = 1024;
const int SiTCP_RBCP_SIZE = 255; // SiTCP reply max size
const int SiTCP_RBCP_REG_LENGTH = 64;
const u_int SiTCP_REG_AREA_ADDRESS = 0xFFFFFF00; // SiTCP

typedef enum _SiDaqStatus {
	SIDAQ_IDLE = 0, // Connected. wait for operation.
	SIDAQ_STOPPING = 40,
	SIDAQ_RESET = 41,
	SIDAQ_RUN = 32,
	SIDAQ_SYS_ERROR = 60,
	SIDAQ_MNGR_INVALID = -1
}SiDaqStatus;

typedef enum _SiDaqErrorCode {
	SIDAQ_NO_ERROR = 0,
	SIDAQ_EXCEED_RECV_QUEUE_COUNT = 1,
	SIDAQ_MEMORY_ALLOCATED_FAILED = 2,
	SIDAQ_EXCEED_FRAME_QUEUE_COUNT = 3,
	SIDAQ_GET_FRAME_LENGTH_FAILED = 4,
	SIDAQ_EXCEED_MAX_FRAME_LENGTH = 5,
	SIDAQ_INITIAL_CONNECT_FAILED = 6,
	SIDAQ_INVALID_PARAMETER = 7,
	SIDAQ_FRAMER_BUF_SIZE_ERROR = 8,
	SITCP_READ_ERROR = 101,
	SITCP_WRITE_ERROR =102,
	SIDAQ_UNDEFINED_ERROR = -1,
	WINSOCK_ERROR = 10000,

}SiDaqErroeCode;

#define SIDAQ_IS_SOCKET_OPEN(sidaq_mngr) (sidaq_mngr->sock_param.device_socket!=INVALID_SOCKET)


#define SYS_ERR_NO_ERROR		"No Error"
#define SYS_ERR_SOCK_INIT		"could not initialize socket. WSAStartup Failed."
#define SYS_ERR_IVALID_ADDR		"could not initialize SiTCP. confirm ip address and device."
#define SYS_ERR_TIME_STOP		"SiDaqStop() called but is not Running"

#define DEFAULT_TCP_RECV_BUF_BYTE_SIZE -1 //0: No set buffer

typedef struct _SiDaqMngr {
	SiDaqStatus status; // Manager Status
	char ip_address[SiDAQ_HOST_NAME_MAX];
	int tcp_port;
	int udp_port;
	unsigned char sitcp_register[SiTCP_RBCP_SIZE];
	int total_byte;
	struct SockThreadLparam sock_param;
	DWORD sock_thread_id;
	SOCKET device_socket;
	HANDLE sock_thread;
	int error_code;
	char error_message[SiDAQ_ERROR_MSG];
	SiTcpRbcp* _sitcp;
	SiDaqFramer* _sidaq_framer;
	HANDLE _framer_thread;
	DWORD _framer_thread_id;
	struct SQueue* _reply;//reply frame
	struct SQueue* _eventFrame;//reply frame
	int (*lengthFunc)(unsigned char*, int);
}SiDaqMngr;

extern "C" {

	SiDaq_API SiDaqMngr* SiDaqInitialize(const char* ip_address, int tcp_port, int udp_port, bool not_wsa_cleanup);
	SiDaq_API void SiDaqTerminate(SiDaqMngr* sidaq_mngr, bool not_wsa_cleanup);
	SiDaq_API int SiDaqGetDataCount(SiDaqMngr* sidaq_mngr);
	SiDaq_API int SiDaqGetFrameDataCount(SiDaqMngr* sidaq_mngr);
	SiDaq_API unsigned char* SiDaqGetData(SiDaqMngr* sidaq_mngr, int* data_len_ptr, int wait_msec);
	SiDaq_API void SiDaqDeleteData(unsigned char* data_ptr);
	SiDaq_API const char* SiDaqErrorMessage(SiDaqMngr* sidaq_mngr);
	SiDaq_API int SiDaqGetSocketErrorCode(SiDaqMngr* sidaq_mngr);
	SiDaq_API SiDaqStatus SiDaqGetStatus(SiDaqMngr* sidaq_mngr);
	SiDaq_API const char* SiDaqStatusText(SiDaqMngr* sidaq_mngr);
	SiDaq_API SiDaqStatus SiDaqRun(SiDaqMngr* sidaq_mngr, int(*lengthFunc)(unsigned char*, UINT, UINT) = NULL, UINT header_len = 0, UINT fixed_frame_len = 0, int rcv_buf_bytes = DEFAULT_TCP_RECV_BUF_BYTE_SIZE, UINT recv_queue_limit = DEFAULT_RECV_QUEUE_COUNT_LIMIT, UINT frame_queue_limit = DEFALT_FRAME_QUEUE_COUNT_LIMIT, UINT max_recv_buff = DEFAULT_RECV_BUFF_SIZE, UINT max_frame_len = DEFAULT_MAX_FRAME_LEN);
	SiDaq_API bool SiDaqStop(SiDaqMngr* sidaq_mngr);
	SiDaq_API unsigned char* SiDaqRbcpReadBytes(SiDaqMngr* sidaq_mngr, unsigned int addr, unsigned int length);
	SiDaq_API int SiDaqRbcpWriteBytes(SiDaqMngr* sidaq_mngr, unsigned char* buffer, unsigned int addr, unsigned int length);
	SiDaq_API void SiDaqPauseRecvData(SiDaqMngr* sidaq_mngr);
	SiDaq_API void SiDaqResumeRecvData(SiDaqMngr* sidaq_mngr);
	SiDaq_API void SiDaqResetQueue(SiDaqMngr* sidaq_mngr);
}