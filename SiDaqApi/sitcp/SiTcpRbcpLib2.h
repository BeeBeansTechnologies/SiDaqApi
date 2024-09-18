//
// SiTcpRbcpLib2.h
// ~~~~~~~~~~~~~~  
//
// Copyright (C) 2019  Bee Beans Technologies Co.,Ltd.
//
// Released under the MIT License.
// (See accompanying file LICENSE)
//
#ifndef _SITCPRBCPLIB_H_
#define _SITCPRBCPLIB_H_

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

namespace SiTcpRbcpLib2
{
    enum SITCPRBCP_ERR_NUM{
        SITCPRBCP_ERR_TIMEOUT,
        SITCPRBCP_ERR_BUSERROR,
        SITCPRBCP_ERR_INVALIDPACKET,
        SITCPRBCP_ERR_SOCKETERROR,
        SITCPRBCP_ERR_INVALID_PARAM,
        SITCPRBCP_ERR_UNKNOWN
    };

    class SiTcpRbcpException: public std::exception  {
        protected:
            SITCPRBCP_ERR_NUM error_number;
            std::string message;
        public:
            SiTcpRbcpException(SITCPRBCP_ERR_NUM num, std::string mes) : error_number(num) {
                std::string type;
                switch(error_number) {
                    case SITCPRBCP_ERR_TIMEOUT:
                        type = "Timeout:";
                        break;
                    case SITCPRBCP_ERR_BUSERROR: 
                        type = "Bus Error:";
                        break;
                    case SITCPRBCP_ERR_INVALIDPACKET:
                        type = "Invalid Packet:";
                        break;
                    case SITCPRBCP_ERR_SOCKETERROR:
                        type = "Socket Error:";
                        break;
                    case SITCPRBCP_ERR_INVALID_PARAM:
                        type = "Invalid Parameter:";
                        break;
                    default:
                        type = "Unknown:";
                        break;
                }
                message = type + " " + mes;
            }

            const char *what() const noexcept {
                return message.c_str();
            }

            SITCPRBCP_ERR_NUM GetErrorNumber() { return error_number; }
    };

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


    class SiTcpRbcp {
        public: 
            static const int MAX_DATA_LEN = 255;

        protected:
            static const int SITCPRBCP_HEADER_SIZE = 8;
            static const int MAX_PKT_LEN = SITCPRBCP_HEADER_SIZE + MAX_DATA_LEN;
            unsigned char rbcp_buf[MAX_PKT_LEN] = { 0 };

            static int ref;
            static const unsigned char SITCPRBCP_HEADER_VERTYPE     = 0xff;
            static const unsigned char SITCPRBCP_HEADER_CMD_MASK    = 0xf0;
            static const unsigned char SITCPRBCP_HEADER_CMD_READ    = 0xc0;
            static const unsigned char SITCPRBCP_HEADER_CMD_WRITE   = 0x80;
            static const unsigned char SITCPRBCP_HEADER_FLAG_ACK    = 0x08;
            static const unsigned char SITCPRBCP_HEADER_FLAG_BUSERR = 0x01;

            int id;
            struct timeval timeout;
            SOCKET sock;

            struct sockaddr_in make_sockaddr_in(std::string hostAddr, unsigned int port);

            void send_packet(int length);
            int recv_packet(void);
            void make_header(unsigned int addr, unsigned int length, bool read);
            void check_packet(int length, int recv_len, bool read);
            void rw_memory(unsigned int addr, unsigned int length, bool read);
    public:
            unsigned char* ReadBytes(unsigned char* buffer, unsigned int addr, unsigned int length);
            void WriteBytes(unsigned char* buffer, unsigned int addr, unsigned int length);
            void SetTimeout(unsigned int msec);
            unsigned int GetTimeout(void);
            SiTcpRbcp(const char* hostAddr, const unsigned short port, unsigned int timeout = 200);
            virtual ~SiTcpRbcp();
        protected:  // To prevent copying objects
            void operator =(const SiTcpRbcp&) {}
            SiTcpRbcp(const SiTcpRbcp&);
    };

    //int SiTcpRbcp::ref = 0;
}
#endif