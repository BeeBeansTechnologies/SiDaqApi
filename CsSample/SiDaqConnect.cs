using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;
using static SiDaqApi;


namespace SiDaqCsSample
{
    public partial class SiDaqConnect : Form
    {
        IntPtr Sidaq_Mngr;
        Thread recvthread_object;
        ulong _data_bytes;
        System.Windows.Forms.Timer _updateTimer;

        DateTime StartTime = DateTime.Now;
        DateTime EndTime = DateTime.Now;

        private SiDaqApi.LengthFunc LengthFunc;
        private UInt32 HeaderLen = 0;
        private UInt32 FixedLen = 0;
        private UInt32 LimitRecvCount = 0;
        private UInt32 LimitFrameCount = 0;

        private GUIStatus GUICurrentStatus = GUIStatus.GUI_NO_CONNECTION;

        private bool WriteFileFlg = false;
        private string WriteFilePath = string.Empty;

        private int GetHeaderLengthFuncSample(IntPtr recv_data_ptr, UInt32 data_len, UInt32 header_len) 
        {
            //Get Data Length
            try
            {
                if (data_len < header_len) return -1;
                if (header_len == 0) return (int)data_len;
                var ret_data = new byte[header_len];
                Marshal.Copy(recv_data_ptr, ret_data, 0, (int)header_len);
                //Big Endian
                var headerBytelen = header_len > 4? (int)header_len : 4;
                var header = new byte[headerBytelen];
                for (int i = 0; i < header_len; i++)
                {
                    header[i] = ret_data[header_len - 1 - i];
                }

                var ret = System.BitConverter.ToUInt32(header, 0);

                if(ret == 0)
                {
                    return -1;
                }
                return (int)ret;
            }
            catch(Exception ex)
            {
                Debug.Write(ex.ToString());
                return -1;
            }

        }

        public SiDaqConnect()
        {
            InitializeComponent();
            ControlInitialize();
            ParamInitialize();
            
        }
        void on_TimerUpdate(Object myObject,
                                            EventArgs myEventArgs)
        {
            bool remain_flg  = false;
            this._updateTimer.Stop();
            var status = SiDaqApi.SiDaqGetStatus(this.Sidaq_Mngr);
            var error_str = SiDaqApi.SiDaqErrorMesageStr (this.Sidaq_Mngr);
            this.SetTextBoxStatus(SiDaqApi.SiDaqStatusTextStr(this.Sidaq_Mngr));
            this.SetTextMessage(error_str, textBoxError);

            if (status == SiDaqStatus.SIDAQ_RUN)
            {
                this.EndTime = DateTime.Now;
                var duration = (this.EndTime - this.StartTime).TotalSeconds;
                //calc byte rate
                if (duration != 0) 
                {
                    double mbps = this._data_bytes * 8 / duration / 1000000;
                    this.SetTextMessage($"data receive rate : {mbps.ToString("e3")} Mbps", textBoxGbps);
                }

                if (this._data_bytes < 1000)
                {
                    this.SetTextMessage("data received " + this._data_bytes.ToString("N3") + " Bytes", textMessage);
                }
                else if (this._data_bytes < 1000 * 1000)
                {
                    this.SetTextMessage("data received " + (this._data_bytes / (double)1000).ToString("N3") + " KBytes", textMessage);
                }
                else if (this._data_bytes < 1000 * 1000 * 1000)
                {
                    this.SetTextMessage("data received " + (this._data_bytes / (double)(1000 * 1000)).ToString("N3") + " MBytes", textMessage);
                }
                else
                {
                    this.SetTextMessage("data received " + (this._data_bytes / (double)(1000 * 1000 * 1000)).ToString("N3") + " GBytes", textMessage);
                }
            }

            var recv_count = SiDaqApi.SiDaqGetDataCount(this.Sidaq_Mngr);
            var frame_count = SiDaqApi.SiDaqGetFrameDataCount(this.Sidaq_Mngr);

            // Display Monitoring Status
            this.SetTextMessage(recv_count.ToString(), textBoxCount);
            this.SetTextMessage(frame_count.ToString(), textBoxFrameCount);

            if(status == SiDaqStatus.SIDAQ_SYS_ERROR && this.GUICurrentStatus == GUIStatus.GUI_RUNNING)
            {
                GUICurrentStatus = GUIStatus.GUI_STOPPING;
                UpdateGUIControlStatus(GUICurrentStatus);
                AddOpeLog("Force Stopping…", DateTime.Now);
            }

            if(GUICurrentStatus == GUIStatus.GUI_STOPPING)
            {
                if (recv_count == 0 && frame_count == 0)
                {
                    for (int i = 0; i < 10; i++)
                    {
                        recv_count = SiDaqApi.SiDaqGetDataCount(this.Sidaq_Mngr);
                        frame_count = SiDaqApi.SiDaqGetFrameDataCount(this.Sidaq_Mngr);
                        if (recv_count == 0 && frame_count == 0)
                        {
                            remain_flg = false;
                            Thread.Sleep(100);
                        }
                        else
                        {
                            remain_flg = true;
                            break;
                        }
                    }

                    if (remain_flg == false)
                    {
                        GUICurrentStatus = GUIStatus.GUI_IDLE;
                        UpdateGUIControlStatus(GUICurrentStatus);
                        AddOpeLog("Run Stop", DateTime.Now);
                    }
                    
                }
            }

            this._updateTimer.Start();
        }

        private void ControlInitialize()
        {
            comboBoxRunMode.DataSource = Enum.GetValues(typeof(FrameLenFunction));
            comboBoxRunMode.SelectedIndex = 0;// NULL
            FrameControlInitialize((FrameLenFunction)comboBoxRunMode.SelectedValue);

            this.GUICurrentStatus = GUIStatus.GUI_NO_CONNECTION;
            this.UpdateGUIControlStatus(this.GUICurrentStatus);

            this.textBoxFilePath.Text = AppDomain.CurrentDomain.BaseDirectory;
        }

        private void ParamInitialize()
        {
            Sidaq_Mngr = (IntPtr)0;// IntPtr.Zero;
            this._data_bytes = 0;
            this.StartTime = DateTime.Now;
            this.EndTime = DateTime.Now;

            this.LengthFunc = null;
            this.HeaderLen = 0;
            this.FixedLen = 0;
    }

        private void FrameControlInitialize(FrameLenFunction selected)
        {
            switch (selected)
            {
                case FrameLenFunction.NULL:
                    headerFrameLen.Enabled = false;
                    fixedFrameLen.Enabled = false;
                    this.LengthFunc = null;
                    break;
                case FrameLenFunction.Header:
                    headerFrameLen.Enabled = true;
                    fixedFrameLen.Enabled = false;
                    this.LengthFunc = this.GetHeaderLengthFuncSample;
                    break;
                case FrameLenFunction.Fixed:
                    headerFrameLen.Enabled = false;
                    fixedFrameLen.Enabled = true;
                    this.LengthFunc = null;
                    break;
                default:
                    headerFrameLen.Enabled = false;
                    fixedFrameLen.Enabled = false;
                    this.LengthFunc = null;
                    break;

            }
        }

        private bool CheckPortNoText(string portNoText, out int portNo)
        {
            portNo = -1;
            if (!Int32.TryParse(portNoText, out portNo))
            {
                MessageBox.Show("Port must be integer");
                return false;
            }
            else if(portNo < 0)
            {
                MessageBox.Show("Port must be positive");
                return false;
            }

            return true;
        }

        private bool GetFrameLengthParam(out string errMes)
        {
            errMes = string.Empty;
            if (this.headerFrameLen.Enabled)
            {
                if (!UInt32.TryParse(this.headerFrameLen.Text, out this.HeaderLen))
                {
                    errMes = "header length must be integer";
                    return false;
                }
            }
            else
            {
                this.HeaderLen = 0;
            }

            if (this.fixedFrameLen.Enabled)
            {
                if (!UInt32.TryParse(this.fixedFrameLen.Text, out this.FixedLen))
                {
                    errMes = "fixed frame length must be integer";
                    return false;
                }
                else if (this.HeaderLen < 0)
                {
                    errMes = "fixed frame length must be positive";
                    return false;
                }
            }
            else
            {
                this.FixedLen = 0;
            }


            return true;
        }

        private bool GetLimitCountParam(out string errMes)
        {
            errMes = string.Empty;
            if (this.checkBoxFrameCount.Checked)
            {
                if (!UInt32.TryParse(this.textBoxLimitFrameCount.Text, out this.LimitFrameCount))
                {
                    errMes = "limit count must be integer";
                    return false;
                }
            }
            else
            {
                this.LimitFrameCount = 0;
            }

            if (this.checkBoxRecvCount.Checked)
            {
                if (!UInt32.TryParse(this.textBoxLimitRecvCout.Text, out this.LimitRecvCount))
                {
                    errMes = "limit count must be integer";
                    return false;
                }
            }
            else
            {
                this.LimitRecvCount = 0;
            }

            return true;
        }

        private void AddOpeLog(string logString, DateTime logTime)
        {
            if (InvokeRequired)
            {
                this.Invoke((MethodInvoker)(() =>
                {
                    this.AddOpeLog(logString, logTime);
                }));
            }
            else
            {
                if (opeLog.Lines.Count() > 20)
                {
                    opeLog.Clear();
                }
                var fullLogString = $"{logTime.ToString("yyyy/MM/dd HH:mm:ss")}: {logString}";
                this.opeLog.AppendText(fullLogString + Environment.NewLine);
            }

        }

        private void UpdateGUIControlStatus(GUIStatus guistatus)
        {
            switch (guistatus)
            {
                case GUIStatus.GUI_NO_CONNECTION:
                    this.ipAddress.Enabled = true;
                    this.tcpPort.Enabled = true;
                    this.udpPort.Enabled = true;
                    this.buttonConnect.Text = "Connect";
                    this.buttonRunStop.Text = "Run";
                    this.buttonRunStop.Enabled = false;
                    this.buttonConnect.Enabled = true;
                    this.comboBoxRunMode.Enabled = false;
                    this.headerFrameLen.Enabled = false;
                    this.fixedFrameLen.Enabled = false;
                    this.checkBoxFrameCount.Enabled = false;
                    this.checkBoxRecvCount.Enabled = false;
                    this.textBoxLimitRecvCout.Enabled = false;
                    this.textBoxLimitFrameCount.Enabled = false;
                    this.checkBoxWriteFile.Enabled = false;
                    this.textBoxFilePath.Enabled = false;
                    this.buttonBrowse.Enabled = false;
                    this.buttonRbcpRead.Enabled = false;
                    this.buttonRbcpWrite.Enabled = false;
                    this.buttonReset.Enabled = false;
                    break;
                case GUIStatus.GUI_IDLE:
                    this.ipAddress.Enabled = false;
                    this.tcpPort.Enabled = false;
                    this.udpPort.Enabled = false;
                    this.buttonConnect.Text = "Disconnect";
                    this.buttonRunStop.Text = "Run";
                    this.buttonRunStop.Enabled = true;
                    this.buttonConnect.Enabled = true;
                    this.comboBoxRunMode.Enabled = true;
                    FrameControlInitialize((FrameLenFunction)this.comboBoxRunMode.SelectedValue);
                    this.checkBoxFrameCount.Enabled = true;
                    this.checkBoxRecvCount.Enabled = true;
                    this.textBoxLimitRecvCout.Enabled = true;
                    this.textBoxLimitFrameCount.Enabled = true;
                    this.checkBoxWriteFile.Enabled = true;
                    this.textBoxFilePath.Enabled = true;
                    this.buttonBrowse.Enabled = true;
                    this.buttonRbcpRead.Enabled = true;
                    this.buttonRbcpWrite.Enabled = true;
                    this.buttonReset.Enabled = false;
                    break;
                case GUIStatus.GUI_RUNNING:
                    this.ipAddress.Enabled = false;
                    this.tcpPort.Enabled = false;
                    this.udpPort.Enabled = false;
                    this.buttonConnect.Text = "Disconnect";
                    this.buttonRunStop.Text = "Stop";
                    this.buttonRunStop.Enabled = true;
                    this.buttonConnect.Enabled = false;
                    this.comboBoxRunMode.Enabled = false;
                    this.headerFrameLen.Enabled = false;
                    this.fixedFrameLen.Enabled = false;
                    this.checkBoxFrameCount.Enabled = false;
                    this.checkBoxRecvCount.Enabled = false;
                    this.textBoxLimitRecvCout.Enabled = false;
                    this.textBoxLimitFrameCount.Enabled = false;
                    this.checkBoxWriteFile.Enabled = false;
                    this.textBoxFilePath.Enabled = false;
                    this.buttonBrowse.Enabled = false;
                    this.buttonRbcpRead.Enabled = false;
                    this.buttonRbcpWrite.Enabled = false;
                    this.buttonReset.Enabled = false;
                    break;
                case GUIStatus.GUI_STOPPING:
                    this.ipAddress.Enabled = false;
                    this.tcpPort.Enabled = false;
                    this.udpPort.Enabled = false;
                    this.buttonConnect.Text = "Disconnect";
                    this.buttonRunStop.Text = "Stop";
                    this.buttonRunStop.Enabled = false;
                    this.buttonConnect.Enabled = false;
                    this.comboBoxRunMode.Enabled = false;
                    this.headerFrameLen.Enabled = false;
                    this.fixedFrameLen.Enabled = false;
                    this.checkBoxFrameCount.Enabled = false;
                    this.checkBoxRecvCount.Enabled = false;
                    this.textBoxLimitRecvCout.Enabled = false;
                    this.textBoxLimitFrameCount.Enabled = false;
                    this.checkBoxWriteFile.Enabled = false;
                    this.textBoxFilePath.Enabled = false;
                    this.buttonBrowse.Enabled = false;
                    this.buttonRbcpRead.Enabled = false;
                    this.buttonRbcpWrite.Enabled = false;
                    this.buttonReset.Enabled = true;
                    break;
                default:
                    break;
            }
        }

        private void connect_Click(object sender, EventArgs e)
        {
            this.Enabled = false;
            Cursor preCursor = Cursor.Current;
            Cursor.Current = Cursors.WaitCursor;
            if (this.Sidaq_Mngr == (IntPtr)0)
            {
                if( this.tcpPort.Text.Length <= 0 && this.ipAddress.Text.Length <=  0)
                {
                    MessageBox.Show("IP Address, TCP Port must should be input");
                    this.Enabled = true;
                    Cursor.Current = preCursor;
                    return;
                }

                if(!CheckPortNoText(this.tcpPort.Text, out var tcpPortNo))
                {
                    this.Enabled = true;
                    Cursor.Current = preCursor;
                    return;
                }

                if (!CheckPortNoText(this.udpPort.Text, out var udpPortNo))
                {
                    this.Enabled = true;
                    Cursor.Current = preCursor;
                    return;
                }
                this.AddOpeLog("Connect Start", DateTime.Now);
                Sidaq_Mngr = SiDaqApi.SiDaqInitialize(this.ipAddress.Text, tcpPortNo, udpPortNo, false);
                if (Sidaq_Mngr != (IntPtr)0)
                {
                    string status_str = SiDaqApi.SiDaqStatusTextStr(Sidaq_Mngr);
                    if (status_str == "System Error")
                    {
                        MessageBox.Show("Could not connect. Check device IP Address, TCP Port");
                        this.Enabled = true;
                        Cursor.Current = preCursor;
                        this.GUICurrentStatus = GUIStatus.GUI_NO_CONNECTION;
                        this.UpdateGUIControlStatus(this.GUICurrentStatus);
                        return;
                    }
                    this.GUICurrentStatus = GUIStatus.GUI_IDLE;
                    this.UpdateGUIControlStatus(this.GUICurrentStatus);
                    SetTextMessage(SiDaqApi.SiDaqErrorMesageStr(this.Sidaq_Mngr), textBoxError);
                    FrameControlInitialize((FrameLenFunction)this.comboBoxRunMode.SelectedValue);
                    this._updateTimer = new System.Windows.Forms.Timer();
                    this._updateTimer.Tick += new EventHandler(this.on_TimerUpdate);
                    this._updateTimer.Enabled = true;
                    this._updateTimer.Interval = 500;

                }
            }
            else
            {
                //Terminate
                this.AddOpeLog("Connect End", DateTime.Now);
                SiDaqApi.SiDaqTerminate(Sidaq_Mngr, false);
                this.GUICurrentStatus = GUIStatus.GUI_NO_CONNECTION;
                this.UpdateGUIControlStatus(this.GUICurrentStatus);
                Sidaq_Mngr = (IntPtr)0;
                if(this._updateTimer != null) this._updateTimer.Stop();
                
            }
            Cursor.Current = preCursor;
            this.Enabled = true;

        }

        private void SiDaqConnect_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (this.Sidaq_Mngr != (IntPtr)0) {
                SiDaqApi.SiDaqTerminate(Sidaq_Mngr, false);
                Sidaq_Mngr = (IntPtr)0;
            }

        }


        private void buttonRunStop_Click(object sender, EventArgs e)
        {
            Cursor preCursor = Cursor.Current;
            Cursor.Current = Cursors.WaitCursor;

            if (this.Sidaq_Mngr != null)
            {
                SiDaqStatus sidaq_status = SiDaqGetStatus(this.Sidaq_Mngr);
                SiDaqStatus run_status = SiDaqStatus.SIDAQ_RUN;//next status.
                SiDaqStatus new_status = sidaq_status;
                switch (sidaq_status)
                {
                    case SiDaqStatus.SIDAQ_SYS_ERROR:
                    case SiDaqStatus.SIDAQ_IDLE:
                        if (sidaq_status == SiDaqStatus.SIDAQ_SYS_ERROR &&  GUICurrentStatus != GUIStatus.GUI_IDLE)
                        {
                            break;
                        }
                        this._data_bytes = 0;
                        if (!this.GetFrameLengthParam(out string errMes))
                        {
                            MessageBox.Show(errMes);
                            Cursor.Current = preCursor;
                            return;

                        }
                        if (!this.GetLimitCountParam(out errMes))
                        {
                            MessageBox.Show(errMes);
                            Cursor.Current = preCursor;
                            return;

                        }
                        this.SetTextMessage("data received " + this._data_bytes.ToString() + " bytes", textMessage);
                        new_status = SiDaqApi.SiDaqRun(this.Sidaq_Mngr, this.LengthFunc, HeaderLen, FixedLen, 100 * 1024 * 1024, this.LimitRecvCount, this.LimitFrameCount, 262144, 262144);
                        if (new_status == run_status)
                        {
                            if (this.checkBoxWriteFile.Checked)
                            {
                                this.WriteFileFlg = true;
                                this.WriteFilePath = Path.Combine(this.textBoxFilePath.Text, $"{DateTime.Now.ToString("yyyyMMddHHmmssfff")}.txt");
                            }
                            else
                            {
                                this.WriteFileFlg = false;
                                this.WriteFilePath = string.Empty;
                            }
                            
                            this.AddOpeLog("Run Start", DateTime.Now);
                            this.GUICurrentStatus = GUIStatus.GUI_RUNNING;
                            this.UpdateGUIControlStatus(this.GUICurrentStatus);
                            this.StartTime = DateTime.Now;
                            if(this.recvthread_object != null)
                            {
                                this.recvthread_object.Join(5000);
                                recvthread_object = null;
                            }
                            this.recvthread_object = new Thread(new ThreadStart(recv_thread));
                            this.recvthread_object.Start();
                            SetTextMessage(SiDaqApi.SiDaqErrorMesageStr(this.Sidaq_Mngr), textBoxError);
                        }
                        break;
                    case SiDaqStatus.SIDAQ_RUN:
                        this.AddOpeLog("Run Stopping…", DateTime.Now);
                        this.GUICurrentStatus = GUIStatus.GUI_STOPPING;
                        this.UpdateGUIControlStatus(this.GUICurrentStatus);
                        SiDaqApi.SiDaqStop(this.Sidaq_Mngr);
                        break;
                }


            }
            Cursor.Current = preCursor;
        }

        public void recv_thread()
        {
            int data_len = -1;
            bool remain_flg = false;

            while (true)
            {
                //Get Data loop break check
                if (Sidaq_Mngr == (IntPtr)0)
                {
                    break;
                }
                var run_status = SiDaqApi.SiDaqGetStatus(this.Sidaq_Mngr);
                int recv_count = 0;
                int frame_count = 0;
                if(run_status == SiDaqStatus.SIDAQ_IDLE || run_status == SiDaqStatus.SIDAQ_STOPPING || run_status == SiDaqStatus.SIDAQ_SYS_ERROR)
                {
                    recv_count = SiDaqApi.SiDaqGetDataCount(this.Sidaq_Mngr);
                    frame_count = SiDaqApi.SiDaqGetFrameDataCount(this.Sidaq_Mngr);
                    if (recv_count == 0 && frame_count == 0)
                    {
                        for (int i = 0; i < 10; i++)
                        {
                            recv_count = SiDaqApi.SiDaqGetDataCount(this.Sidaq_Mngr);
                            frame_count = SiDaqApi.SiDaqGetFrameDataCount(this.Sidaq_Mngr);
                            if (recv_count == 0 && frame_count == 0)
                            {
                                remain_flg = false;
                                Thread.Sleep(100);
                            }
                            else
                            {
                                remain_flg = true;
                                break;
                            }
                        }

                        if (remain_flg == false)
                        {
                            break;
                        }
                    }

                }

                //Get Frame Data loop
                byte[] data = SiDaqApi.SiDaqGetDataCopy(this.Sidaq_Mngr, ref data_len, 100);
                if (data != null)
                {
                    if (data_len > 0)
                    {
                        this._data_bytes += (ulong)data_len;
                        if (this.WriteFileFlg)
                        {
                            SiDaqApi.SiDaqPauseRecvData(this.Sidaq_Mngr);
                            this.WriteFile(this.WriteFilePath, data);
                            SiDaqApi.SiDaqResumeRecvData(this.Sidaq_Mngr);
                        }
                    }
                    else if (data_len < 0)
                    {
                        Console.WriteLine("recvthread invalid data len : {0} with data ptr", data_len);
                    }
                    data = null;
                }
                else if (data_len < 0)
                {
                    Console.WriteLine("recvthread negative data len: {0} ", data_len);
                }
                Thread.Sleep(0);
            }

        }

        public void WriteFile(string filepath, byte[] data)
        {
            try
            {
                using (StreamWriter writer = new StreamWriter(filepath, true))
                {
                    var writedata = BitConverter.ToString(data);
                    writer.WriteLine(writedata);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.ToString());
            }
        }

        delegate void SetMessageDelegate(string message, TextBox textBox);//for Text Messages from thread.

        void _setTextMessage(string message, TextBox textBox)
        {
            textBox.Text = message;
        }

        void SetTextMessage(string message, TextBox textBox)
        {
            if(InvokeRequired)
            {
                this.Invoke(new SetMessageDelegate( this._setTextMessage),  message, textBox);

                return;
            }
            this._setTextMessage(message, textBox);
        }

        void _setTextBoxStatus(string message, TextBox textBox)
        {
            this.textBoxStatus.Text = message;
            this.textBoxStatus.Text = SiDaqApi.SiDaqStatusTextStr(this.Sidaq_Mngr);
            this.textBoxError.Text = SiDaqApi.SiDaqErrorMesageStr(this.Sidaq_Mngr);
        }


        void SetTextBoxStatus(string message)
        {
            if (InvokeRequired)
            {
                this.Invoke(new SetMessageDelegate(this._setTextBoxStatus), message, textBoxStatus);
                return;
            }
            this._setTextBoxStatus(message, textBoxStatus);
        }

        private void comboBoxRunMode_SelectionChangeCommitted(object sender, EventArgs e)
        {
            FrameLenFunction selected = (FrameLenFunction)comboBoxRunMode.SelectedValue;
            this.FrameControlInitialize(selected);
            
        }

        private void buttonBrowse_Click(object sender, EventArgs e)
        {
            using (var dialog = new FolderBrowserDialog())
            {
                if (Directory.Exists(this.textBoxFilePath.Text))
                {
                    dialog.SelectedPath = this.textBoxFilePath.Text;
                }

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    this.textBoxFilePath.Text = dialog.SelectedPath;
                }
            }
        }

        private void buttonRbcpRead_Click(object sender, EventArgs e)
        {
            string retString = string.Empty;
            if (this.Sidaq_Mngr == (IntPtr)0)
            {
                retString = "Please Connect";
            }
            else
            {
                var ret = SiDaqApi.SiDaqRbcpReadBytesArray(this.Sidaq_Mngr, 0xffffff00, 64);
                if(ret.Length != 0)
                {
                    retString = CreateSiTCPInfo(ret);
                }
                else
                {
                    retString = "RBCP Read Failed.";
                }
            }

            this.AddOpeLog(retString, DateTime.Now);
            
        }

        private string CreateSiTCPInfo(byte[] ret_data)
        {
            string newline = Environment.NewLine;
            string ret = $"SiTCP Info" + newline;

            if (ret_data.Length < 64) return ret;
            const UInt32 ADDR_RESERVED = 0xffffff00;
            const UInt32 ADDR_SITCP_CONTROL = ADDR_RESERVED + 0x10;
            const UInt32 ADDR_MAC = ADDR_RESERVED + 0x12;
            const UInt32 ADDR_IP = ADDR_RESERVED + 0x18;
            const UInt32 ADDR_TCP_DATA_PORT = ADDR_RESERVED + 0x1c;
            const UInt32 ADDR_TCP_COMMAND_PORT = ADDR_RESERVED + 0x1e;
            const UInt32 ADDR_TCP_MAX_SEGMENT_SIZE = ADDR_RESERVED + 0x20;
            const UInt32 ADDR_UDP_PORT = ADDR_RESERVED + 0x22;
            const UInt32 ADDR_TCP_KEEP_ALIVE_BNE = ADDR_RESERVED + 0x24;
            const UInt32 ADDR_TCP_KEEP_ALIVE_BE = ADDR_RESERVED + 0x26;
            const UInt32 ADDR_TCP_TIMEOUT_CONNECTING = ADDR_RESERVED + 0x28;
            const UInt32 ADDR_TCP_TIMEOUT_DISCONNECT = ADDR_RESERVED + 0x2a;
            const UInt32 ADDR_TCP_RECONNECT_INTERVAL = ADDR_RESERVED + 0x2c;
            const UInt32 ADDR_TCP_RETRANSMITION_TIMER = ADDR_RESERVED + 0x2e;

            // IP Address
            UInt32 begin = ADDR_IP - ADDR_RESERVED;
            ret += $"IP Address : {ret_data[begin + 0]}.{ret_data[begin + 1]}.{ret_data[begin + 2]}.{ret_data[begin + 3]}" + newline;

            //TCP Data Port
            begin = ADDR_TCP_DATA_PORT - ADDR_RESERVED;
            ret += $"TCP Port : {(ret_data[begin + 0] << 8) + ret_data[begin + 1]}" + newline;

            // UDP Port
            begin = ADDR_UDP_PORT - ADDR_RESERVED;
            ret += $"UDP Port : {(ret_data[begin + 0] << 8) + ret_data[begin + 1]}" + newline;

            // MAC Address
            begin = ADDR_MAC - ADDR_RESERVED;
            ret += $"MAC Address : {BitConverter.ToString(ret_data, (int)begin, 6)}" + newline;

            // TCP Command Port
            begin = ADDR_TCP_COMMAND_PORT - ADDR_RESERVED;
            ret += $"TCP Command Port : {(ret_data[begin + 0] << 8) + ret_data[begin + 1]}" + newline;

            // TCP Max Segment Size
            begin = ADDR_TCP_MAX_SEGMENT_SIZE - ADDR_RESERVED;
            ret += $"TCP Max Segment Size : {(ret_data[begin + 0] << 8) + ret_data[begin + 1]}" + newline;


            // TCP Keep Alive Buffer Not Empty
            begin = ADDR_TCP_KEEP_ALIVE_BNE - ADDR_RESERVED;
            ret += $"TCP Keep Alive (Buffer Not Empty) : {(ret_data[begin + 0] << 8) + ret_data[begin + 1]}msec" + newline;

            // TCP Keep Alive Buffer Empty
            begin = ADDR_TCP_KEEP_ALIVE_BE - ADDR_RESERVED;
            ret += $"TCP Keep Alive (Buffer Empty) : {(ret_data[begin + 0] << 8) + ret_data[begin + 1]}msec" + newline;

            // TCP Timeout Connecting
            begin = ADDR_TCP_TIMEOUT_CONNECTING - ADDR_RESERVED;
            ret += $"TCP Timeout Connecting : {(ret_data[begin + 0] << 8) + ret_data[begin + 1]}msec" + newline;

            // TCP Timeout Disconnect
            begin = ADDR_TCP_TIMEOUT_DISCONNECT - ADDR_RESERVED;
            ret += $"TCP Timeout Disconnect : {((ret_data[begin + 0] << 8) + ret_data[begin + 1]) * 256}msec" + newline;

            // TCP Reconnect Interval
            begin = ADDR_TCP_RECONNECT_INTERVAL - ADDR_RESERVED;
            ret += $"TCP Recconect Interval : {(ret_data[begin + 0] << 8) + ret_data[begin + 1]}msec" + newline;

            // TCP Retransmition timer
            begin = ADDR_TCP_RETRANSMITION_TIMER - ADDR_RESERVED;
            ret += $"TCP Retransmition timer : {(ret_data[begin + 0] << 8) + ret_data[begin + 1]}msec" + newline;

            // TCP KeepAlive Enabled
            begin = ADDR_SITCP_CONTROL - ADDR_RESERVED;
            ret += $"TCP KeepAlive Enabled : {(ret_data[begin + 0] != 0)}" + newline;

            return ret;
        }

        private void buttonRbcpWrite_Click(object sender, EventArgs e)
        {
            
            string retString = string.Empty;
            if (this.Sidaq_Mngr == (IntPtr)0)
            {
                retString = "Please Connect";
            }
            else
            {
                var resetByte = new byte[] { 0x80 };
                var ret = SiDaqApi.SiDaqRbcpWriteBytes(this.Sidaq_Mngr, resetByte, 0xffffff10, (uint)resetByte.Length);
                if (ret <  0)
                {
                    
                    retString = "RBCP Write Failed.";
                }
                else
                {
                    retString = "RBCP Write Success. SiTCP Reset.";
                }
            }

            this.AddOpeLog(retString, DateTime.Now);
        }

        private void buttonReset_Click(object sender, EventArgs e)
        {
            this.AddOpeLog("Reset Queue Data…", DateTime.Now);
            this.buttonReset.Enabled = false;
            SiDaqApi.SiDaqResetQueue(this.Sidaq_Mngr);
            this.AddOpeLog("Reset Queue Data Finish", DateTime.Now);
        }
    }

    public enum FrameLenFunction
    {
        NULL,
        Header,
        Fixed,
    }

    public enum GUIStatus
    {
        GUI_NO_CONNECTION,
        GUI_IDLE,
        GUI_RUNNING,
        GUI_STOPPING,
    }
}


