using System;
using System.Runtime.InteropServices;
public enum SiDaqStatus
{
    SIDAQ_IDLE = 0, // Connected. wait for operation.
    SIDAQ_STOPPING = 40,
    SIDAQ_RUN = 32,  //Sparse
    SIDAQ_SYS_ERROR = 60,
    SIDAQ_MNGR_INVALID = -1
};

public static class SiDaqApi
{
    public const UInt32 DEFAULT_RECV_BUFF_SIZE = 262144;
    public const UInt32 DEFAULT_RECV_QUEUE_COUNT_LIMIT = 0;
    public const UInt32 DEFAULT_MAX_FRAME_LEN = 262144;
    public const UInt32 DEFAULT_FRAME_QUEUE_COUNT_LIMIT = 0;
    public const int DEFAULT_TCP_RECV_BUF_BYTE_SIZE = -1;

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqInitialize", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr SiDaqInitialize(string ipAddress, int tcpPort, int udpPort, bool notWSACleanup);

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqTerminate",  CallingConvention = CallingConvention.Cdecl)]
    public static extern void SiDaqTerminate(IntPtr sidaqMngr, bool notWSACleanup);


    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqDeleteData", CallingConvention = CallingConvention.Cdecl)]
    public static extern void SiDaqDeleteData(IntPtr data_ptr);

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqGetData", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr SiDaqGetData(IntPtr sidaqMngr, ref int data_len_ptr, int wait_msec);

    public  static byte [] SiDaqGetDataCopy(IntPtr sidaqMngr, ref int data_len_ptr, int wait_msec)
        {
            int data_len = -1;
            IntPtr recv_data_ptr = SiDaqGetData(sidaqMngr, ref data_len, wait_msec);
            data_len_ptr = data_len;
            try
            {
                if (recv_data_ptr != null)
                {
                    byte[] ret_data = null;
                    if (data_len > 0)
                    {
                        ret_data = new byte[data_len];
                        Marshal.Copy(recv_data_ptr, ret_data, 0, data_len);
                    }
                    return ret_data;
                }
                return null;
            }
            finally
            {
                if(recv_data_ptr != null) SiDaqDeleteData(recv_data_ptr);
            }

        }


    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqGetDataCount", CallingConvention = CallingConvention.Cdecl)]
    public static extern int SiDaqGetDataCount(IntPtr sidaqMngr );

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqGetFrameDataCount", CallingConvention = CallingConvention.Cdecl)]
    public static extern int SiDaqGetFrameDataCount(IntPtr sidaqMngr);

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqErrorMessage", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern IntPtr SiDaqErrorMessage(IntPtr sidaqMngr);

    public static string SiDaqErrorMesageStr(IntPtr sidaqMngr)
    {
        return Marshal.PtrToStringAnsi(SiDaqErrorMessage(sidaqMngr));
    }

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqGetStatus", CallingConvention = CallingConvention.Cdecl)]
    public static extern SiDaqStatus SiDaqGetStatus(IntPtr sidaqMngr);

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqStatusText", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern IntPtr SiDaqStatusText(IntPtr sidaqMngr);

    public static string SiDaqStatusTextStr(IntPtr sidaqMngr)
    {
        return Marshal.PtrToStringAnsi(SiDaqStatusText(sidaqMngr));

    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate int LengthFunc(IntPtr recv_data, UInt32 data_len, UInt32 header_len);

    [DllImport("SiDaqAPI.dll", EntryPoint = "SiDaqRun", CallingConvention = CallingConvention.Cdecl)]
    public static extern SiDaqStatus SiDaqRun(IntPtr sidaqMngr, [MarshalAs(UnmanagedType.FunctionPtr)] LengthFunc lengthFunc = null,
        UInt32 header_len = 0,
        UInt32 fixed_frame_len = 0,
        int rcv_buf_bytes = DEFAULT_TCP_RECV_BUF_BYTE_SIZE,
        UInt32 recv_queue_limit = DEFAULT_RECV_QUEUE_COUNT_LIMIT,
        UInt32 frame_queue_limit = DEFAULT_FRAME_QUEUE_COUNT_LIMIT,
        UInt32 max_recv_buff = DEFAULT_RECV_BUFF_SIZE,
        UInt32 max_frame_len = DEFAULT_MAX_FRAME_LEN);

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqStop", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern bool SiDaqStop(IntPtr sidaqMngr);

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqRbcpReadBytes", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern IntPtr SiDaqRbcpReadBytes(IntPtr sidaqMngr, UInt32 addr, UInt32 length);

    public static byte[] SiDaqRbcpReadBytesArray(IntPtr sidaqMngr, UInt32 addr, UInt32 length)
    {
        var ptr = SiDaqRbcpReadBytes(sidaqMngr, addr, length);
        if (ptr == IntPtr.Zero) return new byte[0];
        byte[] ret_data = new byte[length];
        Marshal.Copy(ptr, ret_data, 0, (int)length);

        return ret_data;

    }

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqRbcpWriteBytes", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern int SiDaqRbcpWriteBytes(IntPtr sidaqMngr, [MarshalAs(UnmanagedType.LPArray)] byte[] buffer, UInt32 addr, UInt32 length);

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqPauseRecvData", CallingConvention = CallingConvention.Cdecl)]
    public static extern void SiDaqPauseRecvData(IntPtr data_ptr);

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqResumeRecvData", CallingConvention = CallingConvention.Cdecl)]
    public static extern void SiDaqResumeRecvData(IntPtr data_ptr);

    [DllImport("SiDaqApi.dll", EntryPoint = "SiDaqResetQueue", CallingConvention = CallingConvention.Cdecl)]
    public static extern void SiDaqResetQueue(IntPtr data_ptr);
}
