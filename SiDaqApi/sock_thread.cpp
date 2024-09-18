//
// sock_thread.cpp
// ~~~~~~~~~~~~~~  
// 
// Copyright (C) 2024  Bee Beans Technologies Co.,Ltd.
//
// Released under the MIT License.
// (See accompanying file LICENSE)
//
#include "pch.h"
#include <stdio.h>
#include <windows.h>
#include <WinSock2.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <crtdbg.h> // debug message
#include "squeue_c/squeue.h"
#include "sock_thread.h"


struct timeval _g_SELECT_DEFAULT = {
    0,
    50000,//500msec
};



DWORD WINAPI socket_loop_thread(LPVOID lParam) {

    struct SockThreadLparam* sock_thread_lparam = (struct SockThreadLparam*)lParam;
    SOCKET device_socket = sock_thread_lparam->device_socket;
    struct SockStat_s* sock_stat = &sock_thread_lparam->sock_stat;
    fd_set rfds, wfds, readfds, writefds;
    clock_t t_start;// , t_end;
    int recv_len = 0, ret_val = 0, tx_len = 0, queue_count = 0;
    int buf_size = sock_thread_lparam->recv_buf_size;
    struct SQueue* queue_ptr = sock_thread_lparam->squeue_list_ptr[sock_thread_lparam->squeue_index];
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(device_socket, &readfds);
    FD_SET(device_socket, &writefds);
    char* recvbuff = (char*)malloc(buf_size);
    if (recvbuff == NULL) return -1;
    _RPTN(_CRT_WARN, "Starting socket thread %u\n", device_socket);
    sock_thread_lparam->queue_error_code = SOCKQUEUE_NO_ERROR;
    sock_thread_lparam->pause_flg = false;
    while (SOCKTHREAD_IS_RUN(sock_thread_lparam)) {
        if (device_socket < 0 || device_socket == INVALID_SOCKET) break;
        memcpy(&rfds, &readfds, sizeof(fd_set));
        memcpy(&wfds, &writefds, sizeof(fd_set));
        queue_ptr = sock_thread_lparam->squeue_list_ptr[sock_thread_lparam->squeue_index];
        if ((ret_val = select(0, &rfds, &wfds, NULL, &_g_SELECT_DEFAULT)) > 0) {
            if (FD_ISSET(device_socket, &rfds)) {// enable reading
                recv_len = -1;//no data.
                t_start = clock();// (NULL);
                while (TRUE) {
                    if (sock_thread_lparam->pause_flg == false) {


                        memset(recvbuff, 0, buf_size);
                        recv_len = recv(device_socket, recvbuff, buf_size, 0);
                        if (recv_len == 0) {
                            sock_stat->last_error = WSAGetLastError();
                            printf("ERROR receive 0 bytes last_error %d \n", sock_stat->last_error);
                            closesocket(device_socket);
                            sock_thread_lparam->device_socket = device_socket = INVALID_SOCKET;

                            break;//recive loop
                        }
                        else if (recv_len < 0) {// no data to read.
                            auto last_error = WSAGetLastError();
                            if (is_allowed_error(last_error) < 0) {
                                //end TCP Connection
                                sock_stat->last_error = last_error;
                                closesocket(device_socket);
                                sock_thread_lparam->device_socket = device_socket = INVALID_SOCKET;
                                break;
                            }

                        }
                        else {// put data to framing process
                            queue_count = SQPut(queue_ptr, recvbuff, recv_len);
                            if (queue_count < 0) {
                                //memory allocation error
                                sock_thread_lparam->queue_error_code = SOCKQUEUE_MEMORY_ALLOCATED_FAILED;
                                closesocket(device_socket);
                                sock_thread_lparam->device_socket = device_socket = INVALID_SOCKET;

                                break;//recive loop
                            }
                            else if (sock_thread_lparam->queue_limit != 0 && (UINT)queue_count > sock_thread_lparam->queue_limit) {
                                // Queue Limit Error
                                sock_thread_lparam->queue_error_code = SOCKQUEUE_EXCEED_RECV_QUEUE_COUNT;
                                closesocket(device_socket);
                                sock_thread_lparam->device_socket = device_socket = INVALID_SOCKET;

                                break;//recive loop
                            }

                            sock_stat->rx_bytes += (unsigned long long)recv_len;
                            _RPTN(_CRT_WARN, "recv bytes %llu queue %llx\n", sock_stat->rx_bytes, (long long)queue_ptr);
                        }
                    }
                    else {
                        if (!SOCKTHREAD_IS_RUN(sock_thread_lparam)) {
                            break;
                        }
                        Sleep(0);
                    }
                }
                _RPTN(_CRT_WARN, "end loop(%d) recved byets %llu in %llu msec\n", recv_len, sock_stat->rx_bytes, sock_stat->rx_duration);
            }

            Sleep(0);
        }

        else if (ret_val == SOCKET_ERROR) {
            sock_stat->last_error = WSAGetLastError();
            break;
        }

    }
    free(recvbuff);
    return 0;
}


const char* decode_error(int nCode)
{
    const char* pRet = NULL;
    switch (nCode) {
    case WSANOTINITIALISED:
        pRet = "Before this function can be used, the WSAStartup call must succeed.";
        break;
    case WSAEFAULT:
        pRet = "Either the Windows socket implementation could not allocate the necessary resources for internal operations or readfds, writefds, exceptfds, or timeval parameters are not part of the user address space.";
        break;
    case WSAENETDOWN:
        pRet = "Network subsystem failed.";
        break;
    case WSAEINVAL:
        pRet = "Either the timeout value is invalid or all three descriptor parameters are null.";
        break;
    case WSAEINTR:
        pRet = "A blocked operation was interrupted by a call to WSACancelBlockingCall.";
        break;
    case WSAEINPROGRESS:
        pRet = "Either a blocking Windows Socket 1.1 call is in progress or the service provider is handling the callback function.";
        break;
    case WSAENOTSOCK:
        pRet = "One of the descriptor sets contains an entry that is not a socket.";
        break;
    case WSAEWOULDBLOCK:
        pRet = "Resource temporarily unavailable.";
        break;
    default:
        pRet = "Undefined Error.";
    }
    return pRet;
}

int is_allowed_error(int nCode) {
    int ret = -1;
    switch (nCode) {
    case WSAEWOULDBLOCK:
        ret = 0;
        break;
    default:
        break;
    }

    return ret;
}
