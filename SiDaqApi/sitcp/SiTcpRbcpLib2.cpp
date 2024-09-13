//
// SiTcpRbcpLib2.cpp
// ~~~~~~~~~~~~~~  
//
// Copyright (C) 2019  Bee Beans Technologies Co.,Ltd.
//
// Released under the MIT License.
// (See accompanying file License.txt)
//

#include "pch.h"
#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32 
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define closesocket(s) close((s))
#define INVALID_SOCKET (-1)
#define SOCKET int
#endif

#include "SiTcpRbcpLib2.h"

using namespace SiTcpRbcpLib2;
 
    // RBCP packet format

    // Bit 7         Bit 0
    //  | Ver. | Type |
    //  | CMD  | FLAG |
    //  |     I D     |
    //  | Data Length |
    //  | Adr[31:24]  |
    //  | Adr[23:16]  |
    //  | Adr[15: 8]  |
    //  | Adr[ 7: 0]  |
    //  |   Data 0    |
    //  .    ...      .
    //  |   Data N    |
    //

int get_ip_from_host(const char* hostname, u_long* saddr) {
    struct addrinfo hints, * res;
    int err;

    //WSADATA data;
    //WSAStartup(MAKEWORD(2, 0), &data);

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    if ((err = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
        printf("error %d\n", err);
        return -1;
    }

    *saddr = ((struct sockaddr_in*)(res->ai_addr))->sin_addr.S_un.S_addr;
    //        printf("ip address : %s\n", inet_ntoa(addr));
    freeaddrinfo(res);
    return 0;

}



struct sockaddr_in SiTcpRbcp::make_sockaddr_in(std::string hostAddr, unsigned int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (get_ip_from_host(hostAddr.c_str(), & addr.sin_addr.S_un.S_addr)) {
    }else{
#ifdef _WIN32
        inet_pton(AF_INET, hostAddr.c_str(), &addr.sin_addr.S_un.S_addr);
#else
        addr.sin_addr.s_addr = inet_addr(hostAddr.c_str());
#endif
    }
    return addr;
}

void SiTcpRbcp::send_packet(int length) {
    int res = send(sock, (char *)rbcp_buf, length, 0);

    if (res != length) {
        closesocket(sock);
        throw SiTcpRbcpException(SITCPRBCP_ERR_SOCKETERROR, "send() failed"); 
    }
}

int SiTcpRbcp::recv_packet(void) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    int result;
    if ((result = select((int)(sock + 1), &fds, NULL, NULL, &timeout)) < 0) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_SOCKETERROR, "select() failed");
    } 
    else if(result == 0) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_TIMEOUT, "select() timeout");
    }

    int size = recv(sock, (char *)rbcp_buf, MAX_PKT_LEN, 0);
    if(size < 0) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_SOCKETERROR, "recv() failed");
    }
    return size;
}

void SiTcpRbcp::make_header(unsigned int addr, unsigned int length, bool read) {
    rbcp_buf[0] = SITCPRBCP_HEADER_VERTYPE;
    rbcp_buf[1] = read ? SITCPRBCP_HEADER_CMD_READ: SITCPRBCP_HEADER_CMD_WRITE;
    rbcp_buf[2] = id;
    rbcp_buf[3] = length & 0xff;
    *(unsigned int *)(&rbcp_buf[4]) = htonl(addr);
}

void SiTcpRbcp::check_packet(int length, int recv_len, bool read) {
    if (length+SITCPRBCP_HEADER_SIZE != recv_len) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_INVALIDPACKET, "unexpected packet length");
    }
    else if (rbcp_buf[0] != SITCPRBCP_HEADER_VERTYPE) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_INVALIDPACKET, "unexpected header value");
    }
    else if ( ( read && ((rbcp_buf[1] & SITCPRBCP_HEADER_CMD_MASK) != SITCPRBCP_HEADER_CMD_READ)) ||
            (!read && ((rbcp_buf[1] & SITCPRBCP_HEADER_CMD_MASK) != SITCPRBCP_HEADER_CMD_WRITE)) ) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_INVALIDPACKET, "unexpected header value");
    } 
    else if ((rbcp_buf[1] & SITCPRBCP_HEADER_FLAG_ACK) != SITCPRBCP_HEADER_FLAG_ACK) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_INVALIDPACKET, "unexpected header value");
    }
    else if ((rbcp_buf[1] & SITCPRBCP_HEADER_FLAG_BUSERR) == SITCPRBCP_HEADER_FLAG_BUSERR) {
        if (read) {
            throw SiTcpRbcpException(SITCPRBCP_ERR_BUSERROR, "read failed");
        }
        else {
            throw SiTcpRbcpException(SITCPRBCP_ERR_BUSERROR, "write failed");
        }
    }
    else if (rbcp_buf[2] != id) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_INVALIDPACKET, "unexpected id value");
    }
    else if (rbcp_buf[3] != length) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_INVALIDPACKET, "unexpected data length");
    }
}

void SiTcpRbcp::rw_memory(unsigned int addr, unsigned int length, bool read) {
    id = (id + 1) & 0xff;
    make_header(addr, length, read);  // already copy data to buffer when write.

    int send_len = read ? SITCPRBCP_HEADER_SIZE: (length+SITCPRBCP_HEADER_SIZE);
    send_packet(send_len);
    int recv_len = recv_packet();

    check_packet(length, recv_len, read);
}

unsigned char* SiTcpRbcp::ReadBytes(unsigned char* buffer, unsigned int addr, unsigned int length) {
    if (length > MAX_DATA_LEN) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_INVALID_PARAM, "too large length");
    }
    rw_memory(addr, length, true);

    if (buffer == nullptr) {
        return (unsigned char *)&(rbcp_buf[SITCPRBCP_HEADER_SIZE]);
    }

    memcpy(buffer, rbcp_buf + SITCPRBCP_HEADER_SIZE, length);

    return buffer;
}

void SiTcpRbcp::WriteBytes(unsigned char* buffer, unsigned int addr, unsigned int length) {
    if (length > MAX_DATA_LEN) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_INVALID_PARAM, "too large length");
    }
    memcpy(rbcp_buf + SITCPRBCP_HEADER_SIZE, buffer, length);
    rw_memory(addr, length, false);
}

void SiTcpRbcp::SetTimeout(unsigned int msec) {
    timeout.tv_sec = msec / 1000;
    timeout.tv_usec = (msec - (timeout.tv_sec * 1000)) * 1000;

    if( setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(struct timeval)) != 0 ) {
        closesocket( sock );
        throw SiTcpRbcpException(SITCPRBCP_ERR_SOCKETERROR, "setsocketopt() failed");
    }
}

unsigned int SiTcpRbcp::GetTimeout(void) {
    return (unsigned int)(timeout.tv_sec * 1000 + timeout.tv_usec / 1000);
}

SiTcpRbcp::SiTcpRbcp(const char *hostAddr, const unsigned short port, unsigned int timeout /*= 200*/) {   // ms
//#ifdef _WIN32 
//    if (ref++ == 0) {
//        WSADATA wsaData;
//        if (WSAStartup(2, &wsaData) != 0) {
//            throw SiTcpRbcpException(SITCPRBCP_ERR_SOCKETERROR, "WSAStartup() failed");
//        }
//    }
//#else
//#endif
    this->id = 0;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef _WIN32
    if (sock == INVALID_SOCKET ) {
#else
    if (sock < 0) {
#endif
        throw SiTcpRbcpException(SITCPRBCP_ERR_SOCKETERROR, "socket() failed");
    }
    SetTimeout(timeout);
    struct sockaddr_in sock_addr = make_sockaddr_in(hostAddr, port);
    if (connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) != 0) {
        throw SiTcpRbcpException(SITCPRBCP_ERR_SOCKETERROR, "connect() failed");
    }
};

SiTcpRbcp::~SiTcpRbcp() {
//#ifdef _WIN32 
//    if (--ref == 0) {
//        WSACleanup();
//    }
//#else
//
//#endif
    closesocket( sock );
}

