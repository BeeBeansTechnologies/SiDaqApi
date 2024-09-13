// SiDaqApi.cpp : Defines the initialization routines for the DLL.

#include "pch.h"
#include "framework.h"
#include "SiDaqApi.h"
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CSiDaqApiApp

BEGIN_MESSAGE_MAP(CSiDaqApiApp, CWinApp)
END_MESSAGE_MAP()


// CSiDaqApiApp construction

CSiDaqApiApp::CSiDaqApiApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}


// The one and only CSiDaqApiApp object

CSiDaqApiApp theApp;


// CSiDaqApiApp initialization
BOOL CSiDaqApiApp::InitInstance()
{
	CWinApp::InitInstance();

	//if (!AfxSocketInit())
	//{
	//	AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	//	return FALSE;
	//}

	return TRUE;
}

int g_wsa_init = 0;//WSA Initlize calling count.
WSADATA g_sidaq_wsa_data;

void set_socket_error(SiDaqMngr* sidaq_mngr, int nCode) {
    if (sidaq_mngr) {
        sidaq_mngr->sock_param.sock_stat.last_error = nCode;
    }
}

void set_sidaq_error(SiDaqMngr* sidaq_mngr, int sidaq_error_code, const char* message, int winsock_error_code = 0) {
    assert(sidaq_mngr != NULL && sizeof(sidaq_mngr->error_message) > strlen(message));
    sidaq_mngr->error_code = sidaq_error_code;
    strcpy_s(sidaq_mngr->error_message,
        sizeof(sidaq_mngr->error_message),
        message);
    set_socket_error(sidaq_mngr, winsock_error_code);
}

int decode_framer_error(int framer_code) {
    switch (framer_code)
    {
    case FRAMER_NO_ERROR:
        return SIDAQ_NO_ERROR;
    case FRAMER_INVALID_PARAMETER:
        return SIDAQ_INVALID_PARAMETER;
    case FRAMER_MEMORY_ALLOCATED_FAILED:
        return SIDAQ_MEMORY_ALLOCATED_FAILED;
    case FRAMER_EXCEED_FRAME_QUEUE_COUNT:
        return SIDAQ_EXCEED_FRAME_QUEUE_COUNT;
    case FRAMER_GET_FRAME_LENGTH_FAILED:
        return SIDAQ_GET_FRAME_LENGTH_FAILED;
    case FRAMER_EXCEED_MAX_FRAME_LENGTH:
        return SIDAQ_EXCEED_MAX_FRAME_LENGTH;
    case FRAMER_BUF_SIZE_ERROR:
        return SIDAQ_FRAMER_BUF_SIZE_ERROR;
    default:
        return SIDAQ_UNDEFINED_ERROR;
    }
}

const char* decode_framer_error_string(int framer_code) {
    switch (framer_code)
    {
    case FRAMER_NO_ERROR:
        return "No Error";
    case FRAMER_INVALID_PARAMETER:
        return "Framer invalid parameter";
    case FRAMER_MEMORY_ALLOCATED_FAILED:
        return "Framer memory allocatd failed";
    case FRAMER_EXCEED_FRAME_QUEUE_COUNT:
        return "Exceed frame queue count";
    case FRAMER_GET_FRAME_LENGTH_FAILED:
        return "Get Frame length failed";
    case FRAMER_EXCEED_MAX_FRAME_LENGTH:
        return "Exceed max frame lentgh";
    case FRAMER_BUF_SIZE_ERROR:
        return "framer buffer size error";
    default:
        return "Framer undefined error";
    }
}

int decode_sock_queue_error(int framer_code) {
    switch (framer_code)
    {
    case SOCKQUEUE_NO_ERROR:
        return SIDAQ_NO_ERROR;
    case SOCKQUEUE_MEMORY_ALLOCATED_FAILED:
        return SIDAQ_MEMORY_ALLOCATED_FAILED;
    case SOCKQUEUE_EXCEED_RECV_QUEUE_COUNT:
        return SIDAQ_EXCEED_RECV_QUEUE_COUNT;
    default:
        return SIDAQ_UNDEFINED_ERROR;
    }
}


DWORD framer_thread(LPVOID sidaq_mngr_ptr) {
    SiDaqMngr* sidaq_mngr = (SiDaqMngr*)sidaq_mngr_ptr;
    int data_size = 0, wait_msec = 50;
    int error_flg = 0;
    int frame_count = 0;
    _RPTN(_CRT_WARN, "framer_thread started %d tmm_mngr %llx\n", wait_msec, (long long)sidaq_mngr_ptr);
    sidaq_mngr->_sidaq_framer->_error_code = FRAMER_NO_ERROR;
    while (sidaq_mngr->status != SIDAQ_IDLE && sidaq_mngr->status != SIDAQ_RESET) {
        if (sidaq_mngr->sock_param.squeue_index == 0) {
            u_char* byte_data = (u_char*)SQGet(sidaq_mngr->sock_param.squeue_list_ptr[sidaq_mngr->sock_param.squeue_index], &data_size, wait_msec);
            if (byte_data) {
                if (data_size > 0) {
                    //put reply queue
                    frame_count = RecvSiDaqData(sidaq_mngr->_sidaq_framer, byte_data, data_size, sidaq_mngr->_eventFrame);
                    _RPTN(_CRT_WARN, "Event framer_thread received data size %d bytes\n", data_size);
                }
                free(byte_data);

                //error check
                if (sidaq_mngr->_sidaq_framer->_error_flg == 0) {
                    if (sidaq_mngr->_sidaq_framer->_error_code != FRAMER_NO_ERROR) {
                        //error occured
                        sidaq_mngr->_sidaq_framer->_error_flg = -1;
                        sidaq_mngr->status = SIDAQ_SYS_ERROR;
                        set_sidaq_error(sidaq_mngr, decode_framer_error(sidaq_mngr->_sidaq_framer->_error_code), decode_framer_error_string(sidaq_mngr->_sidaq_framer->_error_code));
                        // close socket
                        if (sidaq_mngr->sock_param.device_socket >= 0) {
                            sidaq_mngr->sock_param._run = FALSE;
                            WaitForSingleObject(sidaq_mngr->sock_thread, 500);
                            closesocket(sidaq_mngr->sock_param.device_socket);
                            sidaq_mngr->sock_param.device_socket = INVALID_SOCKET;
                        }
                        // continue framing
                    }
                    else if (sidaq_mngr->sock_param.queue_error_code != SOCKQUEUE_NO_ERROR) {
                        //socket error occured
                        sidaq_mngr->_sidaq_framer->_error_flg = -1;
                        sidaq_mngr->status = SIDAQ_SYS_ERROR;
                        set_sidaq_error(sidaq_mngr, decode_sock_queue_error(sidaq_mngr->sock_param.queue_error_code), "Socket Receive Queue Error");
                    }
                }
            }
            else {
                if (sidaq_mngr->status == SIDAQ_STOPPING) {
                    // rest frame
                    if (sidaq_mngr->_sidaq_framer->_rest > 0) {
                        // put rest frame
                        SQPut(sidaq_mngr->_eventFrame, sidaq_mngr->_sidaq_framer->_buff_ptr, sidaq_mngr->_sidaq_framer->_rest);
                        sidaq_mngr->_sidaq_framer->_rest = 0;
                    }
                    break;
                }
                else {
                    // check socket
                    if (!SIDAQ_IS_SOCKET_OPEN(sidaq_mngr)) {
                        if (sidaq_mngr->_sidaq_framer->_error_flg == 0) {
                            if (sidaq_mngr->sock_param.queue_error_code != SOCKQUEUE_NO_ERROR) {
                                //socket error occured
                                sidaq_mngr->_sidaq_framer->_error_flg = -1;
                                sidaq_mngr->status = SIDAQ_SYS_ERROR;
                                set_sidaq_error(sidaq_mngr, decode_sock_queue_error(sidaq_mngr->sock_param.queue_error_code), "Socket Receive Queue Error");
                            }
                        }
                        // rest frame
                        if (sidaq_mngr->_sidaq_framer->_rest > 0) {
                            // put rest frame 
                            SQPut(sidaq_mngr->_eventFrame, sidaq_mngr->_sidaq_framer->_buff_ptr, sidaq_mngr->_sidaq_framer->_rest);
                            sidaq_mngr->_sidaq_framer->_rest = 0;
                        }
                        break;
                    }

                    //continue
                }
            }
        }
        else {
            Sleep(10);
        }
    }
    // thread loop end
    if (sidaq_mngr->_sidaq_framer->_error_flg < 0) {
        // force stop measurement
        SiDaqStop(sidaq_mngr);
    }
    else if(sidaq_mngr->status == SIDAQ_RESET) {
        // No Process
    }
    else {
        sidaq_mngr->status = SIDAQ_IDLE;
    }
    _RPTN(_CRT_WARN, "framer_thread exited %d\n", wait_msec);

    return 0;
}

std::map<int, std::string> _g_status_name;

void initialize_status_message() {
    if (_g_status_name.size() == 0) {
        _g_status_name[SIDAQ_IDLE] = "Idle";// = 0, // Connected. wait for operation.
        _g_status_name[SIDAQ_STOPPING] = "Stopping";// = 40,
        _g_status_name[SIDAQ_RESET] = "Reset";// = 41,
        _g_status_name[SIDAQ_RUN] = "Running";// = 32,  //Sparse
        _g_status_name[SIDAQ_SYS_ERROR] = "System Error";// = 60,
    }
};



// // notWSACleanup when calling WSAStartup() on the application side.

/// @brief Create a manager struct connect to SiTCP
/// @param ip_address IP address of the connection destination
/// @param tcp_port TCP port of the destination
/// @param udp_port UDP port of the destination
/// @param not_wsa_cleanup TRUE: Do not run WSAStartup() on this API.
/// @return Pointer to the manager struct
SiDaq_API SiDaqMngr* SiDaqInitialize(const char* ip_address, int tcp_port, int udp_port, bool not_wsa_cleanup)
{
    int i__ = 0;
    SiDaqMngr* sidaq_mngr = NULL;
    DWORD dwThreadId = 0;
    char service[128];
    int ret = -1;
    sprintf_s(service, "%d", tcp_port);
    if (ip_address) {
        sidaq_mngr = (SiDaqMngr*)malloc(sizeof(SiDaqMngr));
        if (sidaq_mngr == NULL) return NULL;
        memset(sidaq_mngr, 0, sizeof(SiDaqMngr));
        sidaq_mngr->sock_thread = NULL;
        sidaq_mngr->_reply = NULL;
        sidaq_mngr->_framer_thread = NULL;
        sidaq_mngr->_sidaq_framer = NULL;
        set_sidaq_error(sidaq_mngr, SIDAQ_NO_ERROR, SYS_ERR_NO_ERROR);
        strcpy_s(sidaq_mngr->ip_address, ip_address);
        sidaq_mngr->status = SIDAQ_IDLE;
        sidaq_mngr->tcp_port = tcp_port;
        sidaq_mngr->udp_port = udp_port;
        sidaq_mngr->_sitcp = NULL;

        memset(&sidaq_mngr->sock_param.sock_stat, 0, sizeof(sidaq_mngr->sock_param.sock_stat));

        if (!not_wsa_cleanup && g_wsa_init == 0) {//execute once per one process
            if (WSAStartup(MAKEWORD(2, 0), &g_sidaq_wsa_data) != 0) {
                sidaq_mngr->status = SIDAQ_SYS_ERROR;
                set_sidaq_error(sidaq_mngr, SIDAQ_NO_ERROR, SYS_ERR_SOCK_INIT, WSAGetLastError());//"could not initialize socket. WSAStartup Failed.");
                return sidaq_mngr;
            }
            g_wsa_init++;
        }

        sidaq_mngr->_sitcp = new SiTcpRbcp(sidaq_mngr->ip_address, udp_port);
        memset(sidaq_mngr->sitcp_register, 0, sizeof(sidaq_mngr->sitcp_register));
        auto read_sitcp_data = SiDaqRbcpReadBytes(sidaq_mngr, SiTCP_REG_AREA_ADDRESS, SiTCP_RBCP_REG_LENGTH);
        if (read_sitcp_data == NULL) {
            //Test Connection Failed
            if (sidaq_mngr->_sitcp) delete sidaq_mngr->_sitcp;
            sidaq_mngr->_sitcp = NULL;
            sidaq_mngr->status = SIDAQ_SYS_ERROR;
            set_sidaq_error(sidaq_mngr, SIDAQ_INITIAL_CONNECT_FAILED, SYS_ERR_IVALID_ADDR);//"could not initialize SiTCP. confirm ip address and device.");
            return sidaq_mngr;
        }

        sidaq_mngr->sock_param._run = TRUE;
        sidaq_mngr->sock_param.squeue_count = 1;// only event raw data
        sidaq_mngr->sock_param.squeue_index = 0;
        sidaq_mngr->sock_param.squeue_list_ptr = (struct SQueue**)malloc(sizeof(struct SQueue*) * sidaq_mngr->sock_param.squeue_count);
        if (sidaq_mngr->sock_param.squeue_list_ptr) {
            for (i__ = 0; i__ < sidaq_mngr->sock_param.squeue_count; i__++) {
                sidaq_mngr->sock_param.squeue_list_ptr[i__] = NewSQueue();
            }
        }
        else {
            free(sidaq_mngr);
            return NULL;
        }
        

        sidaq_mngr->_eventFrame = NewSQueue();//queue of event framing

        return sidaq_mngr;
    }
    return NULL;
}

/// @brief Delete the manager struct and release the allocated resources
/// @param sidaq_mngr Pointer to manager struct
/// @param notWSACleanup TRUE: Do not run WSACleanup() on this API.
/// @return 
SiDaq_API void  SiDaqTerminate(SiDaqMngr* sidaq_mngr, bool notWSACleanup)
{
    int i__ = 0;
    if (sidaq_mngr) {
        //Stop
        SiDaqStop(sidaq_mngr);
        if (g_wsa_init > 0) {
            g_wsa_init--;
            if (g_wsa_init == 0 && !notWSACleanup) {
                WSACleanup();
            }
        }


        for (i__ = 0; i__ < sidaq_mngr->sock_param.squeue_count; i__++) {
            DeleteSQueue(sidaq_mngr->sock_param.squeue_list_ptr[i__]);
        }
        if (sidaq_mngr->_sitcp) delete sidaq_mngr->_sitcp;
        if (sidaq_mngr->_reply) DeleteSQueue(sidaq_mngr->_reply);
        if (sidaq_mngr->_eventFrame) DeleteSQueue(sidaq_mngr->_eventFrame);
        if (sidaq_mngr->_sidaq_framer) DellSiDaqFramer(sidaq_mngr->_sidaq_framer);
        if (sidaq_mngr->sock_param.squeue_list_ptr) free(sidaq_mngr->sock_param.squeue_list_ptr);
        free(sidaq_mngr);
    }
}

/// @brief Get one received event data
/// @param sidaq_mngr Pointer to manager struct
/// @param data_len_ptr Pointer to event data length
/// @param wait_msec Positive: wait for specified (millisecondes). Negative:wait until data is obtained. 
/// @return Pointer to data
SiDaq_API unsigned char* SiDaqGetData(SiDaqMngr* sidaq_mngr, int* data_len_ptr, int wait_msec)
{
    unsigned char* data_ptr = (unsigned char*)SQGet(sidaq_mngr->_eventFrame, data_len_ptr, wait_msec);
    return data_ptr;
}

/// @brief Release data. data obtained with SiDaqGetData must be released with this API.
/// @param data_ptr Pointer to data
/// @return 
SiDaq_API void SiDaqDeleteData(unsigned char* data_ptr)
{
    free(data_ptr);
}

/// @brief Get number of data queues before framing
/// @param sidaq_mngr Pointer to manager struct
/// @return Number of data
SiDaq_API int SiDaqGetDataCount(SiDaqMngr* sidaq_mngr)
{
    if (sidaq_mngr == NULL) return 0;
    return SQUEUE_COUNT(sidaq_mngr->sock_param.squeue_list_ptr[sidaq_mngr->sock_param.squeue_index]);
}

/// @brief Get number of data queues after framing
/// @param sidaq_mngr Pointer to manager struct
/// @return Number of data
SiDaq_API int SiDaqGetFrameDataCount(SiDaqMngr* sidaq_mngr)
{
    if (sidaq_mngr == NULL) return 0;
    return SQUEUE_COUNT(sidaq_mngr->_eventFrame);
}

/// @brief Get the latest error message stored in the manager struct
/// @param sidaq_mngr Pointer to manager struct
/// @return Error message string
const char* SiDaqErrorMessage(SiDaqMngr* sidaq_mngr) {
    return sidaq_mngr->error_message;
}

/// @brief Get the latest Windows Socket error code stored in the manager struct
/// @param sidaq_mngr Pointer to manager struct
/// @return Windows Socket error code
int SiDaqGetSocketErrorCode(SiDaqMngr* sidaq_mngr) {
    if (sidaq_mngr == NULL) return 0;
    return sidaq_mngr->sock_param.sock_stat.last_error;
}

/// @brief Get the current manager status
/// @param sidaq_mngr Pointer to manager struct
/// @return Current manager status
SiDaq_API SiDaqStatus SiDaqGetStatus(SiDaqMngr* sidaq_mngr)
{
    if (sidaq_mngr) {
        return sidaq_mngr->status;
    }
    return SIDAQ_MNGR_INVALID;
}

/// @brief Get the current manager status as a string
/// @param sidaq_mngr Pointer to manager struct
/// @return Current manager status string
const char* SiDaqStatusText(SiDaqMngr* sidaq_mngr)
{
    initialize_status_message();
    return _g_status_name[SiDaqGetStatus(sidaq_mngr)].c_str();
}

/// @brief TCP connection to the device and start measurement
/// @param sidaq_mngr Pointer to manager struct
/// @param lengthFunc Pointer to function to get event frame length from received data (default: NULL)
/// @param header_len Length of header required to get event data length (default: 0)
/// @param fixed_frame_len Length of fixed length framing (default:0)
/// @param rcv_buf_bytes TCP receive buffer size (default:0)
/// @param recv_queue_limit Upper limit on number of data queues before framing (default:0, no upper limit)
/// @param frame_queue_limit Upper limit on number of data queues after framing (default:0, no upper limit)
/// @param max_recv_buff Maximum receive data size (default: 65536*4)
/// @param max_frame_len Maximum frame length (default:262144)
/// @return Current manager status
SiDaq_API SiDaqStatus SiDaqRun(SiDaqMngr* sidaq_mngr, int (*lengthFunc)(unsigned char*, UINT, UINT), UINT header_len, UINT fixed_frame_len, int rcv_buf_bytes, UINT recv_queue_limit, UINT frame_queue_limit, UINT max_recv_buff, UINT max_frame_len)
{
    struct sockaddr_in device_addr;
    u_long nobock = 1;
    struct addrinfo hints, * pResult;
    int ret = -1;

    set_sidaq_error(sidaq_mngr, SIDAQ_NO_ERROR, SYS_ERR_NO_ERROR);

    //パラメータチェック
    if (header_len >= max_frame_len) {
        set_sidaq_error(sidaq_mngr, SIDAQ_INVALID_PARAMETER, "INVALID_HEADER_LENGTH");
        sidaq_mngr->status = SIDAQ_SYS_ERROR;
        return sidaq_mngr->status;
    }

    SOCKET device_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (device_socket == INVALID_SOCKET) {
        auto last_error = WSAGetLastError();
        set_sidaq_error(sidaq_mngr, WINSOCK_ERROR, decode_error(last_error), last_error);
        set_socket_error(sidaq_mngr, last_error);
        sidaq_mngr->status = SIDAQ_SYS_ERROR;
        return sidaq_mngr->status;
    }

    ret = ioctlsocket(device_socket, FIONBIO, &nobock);
    device_addr.sin_family = AF_INET;
    device_addr.sin_port = htons((u_short)sidaq_mngr->tcp_port);

    ret = inet_pton(AF_INET, sidaq_mngr->ip_address, &device_addr.sin_addr.S_un.S_addr);
    if (ret != 1) {
        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_INET;
        if ((ret = getaddrinfo(sidaq_mngr->ip_address, NULL, &hints, &pResult)) != 0) {
            printf("error %d\n", ret);
            if (pResult) ::freeaddrinfo(pResult);
            auto last_error = WSAGetLastError();
            set_sidaq_error(sidaq_mngr, WINSOCK_ERROR, decode_error(last_error), last_error);
            set_socket_error(sidaq_mngr, last_error);
            sidaq_mngr->status = SIDAQ_SYS_ERROR;
            return sidaq_mngr->status;
        }
        if (pResult) {
            device_addr.sin_addr.S_un.S_addr = ((struct sockaddr_in*)(pResult->ai_addr))->sin_addr.s_addr;
            ::freeaddrinfo(pResult);
        }
        else {
            auto last_error = WSAGetLastError();
            set_sidaq_error(sidaq_mngr, WINSOCK_ERROR, "Invalid IP Address", last_error);
            set_socket_error(sidaq_mngr, last_error);
            sidaq_mngr->status = SIDAQ_SYS_ERROR;
            return sidaq_mngr->status;
        }
    }


    ret = connect(device_socket, (struct sockaddr*)&device_addr, sizeof(device_addr));
    if (ret != 0) {
        int last_error = WSAGetLastError();
        if (last_error != WSAEWOULDBLOCK) {//This operation is OK because it is a non-blocking socket.
            _RPTN(_CRT_WARN, "Connect Error %s\n", decode_error(last_error));
            set_sidaq_error(sidaq_mngr, WINSOCK_ERROR, decode_error(last_error));
            set_socket_error(sidaq_mngr, last_error);
            sidaq_mngr->status = SIDAQ_SYS_ERROR;
            return sidaq_mngr->status;
        }
        else {
            _RPTN(_CRT_WARN, "Connect Error %s\n", decode_error(last_error));
        }

    }

    if (rcv_buf_bytes >= 0) {
        ret = setsockopt(device_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&rcv_buf_bytes, sizeof(int));
        if (ret < 0) {
            auto last_error = WSAGetLastError();
            set_sidaq_error(sidaq_mngr, WINSOCK_ERROR, decode_error(last_error), last_error);
            sidaq_mngr->status = SIDAQ_SYS_ERROR;
            return sidaq_mngr->status;
        }
    }

    sidaq_mngr->sock_param.device_socket = device_socket;
    sidaq_mngr->sock_param._run = TRUE;
    sidaq_mngr->sock_param.recv_buf_size = max_recv_buff;
    sidaq_mngr->sock_param.queue_limit = recv_queue_limit;
    sidaq_mngr->sock_param.queue_error_code = SOCKQUEUE_NO_ERROR;
    sidaq_mngr->sock_thread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        socket_loop_thread,       // thread function name
        &sidaq_mngr->sock_param,          // argument to thread function 
        0,                      // use default creation flags 
        &sidaq_mngr->sock_thread_id);

    g_wsa_init++;
    if(sidaq_mngr->_reply == NULL )sidaq_mngr->_reply = NewSQueue();//queue of reply frames.
    if (sidaq_mngr->_sidaq_framer == NULL)sidaq_mngr->_sidaq_framer = NewSiDaqFramer();
    sidaq_mngr->_sidaq_framer->_header_len = header_len;
    sidaq_mngr->_sidaq_framer->_queue_limit = frame_queue_limit;
    sidaq_mngr->_sidaq_framer->_max_frame_len = max_frame_len;
    sidaq_mngr->_sidaq_framer->_lengthFunc = lengthFunc;
    sidaq_mngr->_sidaq_framer->_fixed_frame_len = fixed_frame_len;
    sidaq_mngr->_sidaq_framer->_error_flg = 0;
    sidaq_mngr->_sidaq_framer->_error_code = FRAMER_NO_ERROR;

    sidaq_mngr->_framer_thread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        framer_thread,       // thread function name
        (LPVOID)sidaq_mngr,          // argument to thread function 
        0,                      // use default creation flags 
        &sidaq_mngr->_framer_thread_id);

    //Runも実施
    int sequence_index = 0;
    sidaq_mngr->status = SIDAQ_RUN;
    return sidaq_mngr->status;
}

/// @brief Stop measurement and disconnect TCP connection
/// @param sidaq_mngr Pointer to manager struct
/// @return 
SiDaq_API bool SiDaqStop(SiDaqMngr* sidaq_mngr)
{
    if (sidaq_mngr->status == SIDAQ_RUN) {
        sidaq_mngr->status = SIDAQ_STOPPING;

        if (sidaq_mngr) {
            //Stop
            sidaq_mngr->sock_param._run = FALSE;
            WaitForSingleObject(sidaq_mngr->sock_thread, 500);
            if (sidaq_mngr->sock_param.device_socket >= 0) {
                closesocket(sidaq_mngr->sock_param.device_socket);
                sidaq_mngr->sock_param.device_socket = INVALID_SOCKET;
            }

            //WaitForSingleObject(sidaq_mngr->_framer_thread, INFINITE);//

            //sidaq_mngr->status = SIDAQ_IDLE;

            return true;

        }
    }
    else if (sidaq_mngr->status == SIDAQ_SYS_ERROR) {
        //abnormal stop
        if (sidaq_mngr) {
            //Stop
            sidaq_mngr->sock_param._run = FALSE;
            WaitForSingleObject(sidaq_mngr->sock_thread, 500);
            if (sidaq_mngr->sock_param.device_socket >= 0) {
                closesocket(sidaq_mngr->sock_param.device_socket);
                sidaq_mngr->sock_param.device_socket = INVALID_SOCKET;
            }

            if (sidaq_mngr->_sidaq_framer != NULL) {
                //stop framer thread
                if (sidaq_mngr->_sidaq_framer->_error_flg >= 0) {
                    //WaitForSingleObject(sidaq_mngr->_framer_thread, INFINITE);//
                }
            }
            

            return true;

        }

    }
    else if (sidaq_mngr->status == SIDAQ_IDLE) {
        return true;
    }
    return false;
}

/// @brief Read SiTCP internal registers by RBCP
/// @param sidaq_mngr Pointer to manager struct
/// @param addr Head address to be read
/// @param length Length of data to be read
/// @return Result of reading
SiDaq_API unsigned char* SiDaqRbcpReadBytes(SiDaqMngr* sidaq_mngr, unsigned int addr, unsigned int length) {
    try {
        if (sidaq_mngr->_sitcp == NULL) return NULL;
        return sidaq_mngr->_sitcp->ReadBytes(sidaq_mngr->sitcp_register, addr, length);
    }
    catch (SiTcpRbcpException e) {
        set_sidaq_error(sidaq_mngr, SITCP_WRITE_ERROR, e.what());
        return NULL;
    }

}

/// @brief Write to SiTCP internal register by RBCP
/// @param sidaq_mngr Pointer to manager struct
/// @param buffer Data to write to
/// @param addr Head address to write to
/// @param length Length of data to write to
/// @return -1: write failure
SiDaq_API int SiDaqRbcpWriteBytes(SiDaqMngr* sidaq_mngr, unsigned char* buffer, unsigned int addr, unsigned int length) {
    try {
        if (sidaq_mngr->_sitcp == NULL) return -1;
        sidaq_mngr->_sitcp->WriteBytes(buffer, addr, length);
        return 0;
    }
    catch (SiTcpRbcpException e) {
        set_sidaq_error(sidaq_mngr, SITCP_WRITE_ERROR, e.what());
        return -1;
    }

}

/// @brief If in Run state, pause socket receive processing
/// @param sidaq_mngr Pointer to manager struct
/// @return 
SiDaq_API void SiDaqPauseRecvData(SiDaqMngr* sidaq_mngr) {
    if (sidaq_mngr == NULL) return;
    if (sidaq_mngr->sock_param._run) {
        sidaq_mngr->sock_param.pause_flg = true;
    }
}

/// @brief If in Run state, resume socket receive processing
/// @param sidaq_mngr Pointer to manager struct
/// @return 
SiDaq_API void SiDaqResumeRecvData(SiDaqMngr* sidaq_mngr) {
    if (sidaq_mngr == NULL) return;
    if (sidaq_mngr->sock_param._run) {
        sidaq_mngr->sock_param.pause_flg = false;
    }
}

/// @brief Deletes all data stored in the receive data queue and event queue
/// @param sidaq_mngr Pointer to manager struct
/// @return 
SiDaq_API void SiDaqResetQueue(SiDaqMngr* sidaq_mngr) {
    if (sidaq_mngr == NULL) return;
    SiDaqStatus status = sidaq_mngr->status;
    if (status == SIDAQ_STOPPING || status == SIDAQ_SYS_ERROR) {
        sidaq_mngr->status = SIDAQ_RESET;
        int data_size = -1;
        SQueue* que_ptr = NULL;
        // delete recv queue
        que_ptr = sidaq_mngr->sock_param.squeue_list_ptr[sidaq_mngr->sock_param.squeue_index];
        while (true) {
            void* delete_ptr = SQGet(que_ptr, &data_size, -1);
            if (delete_ptr) {
                free(delete_ptr);
            }
            else {
                break;
            }
        }

        //delere frame queue
        que_ptr = sidaq_mngr->_eventFrame;
        while (true) {
            void* delete_ptr = SQGet(que_ptr, &data_size, -1);
            if (delete_ptr) {
                free(delete_ptr);
            }
            else {
                break;
            }
        }

        if (status == SIDAQ_SYS_ERROR) {
            sidaq_mngr->status = SIDAQ_SYS_ERROR;
        }
        else {
            sidaq_mngr->status = SIDAQ_IDLE;
        }
        
    }
}

