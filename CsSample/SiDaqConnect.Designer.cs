namespace SiDaqCsSample
{
    partial class SiDaqConnect
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.buttonConnect = new System.Windows.Forms.Button();
            this.ipAddress = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label_ip_address = new System.Windows.Forms.Label();
            this.tcpPort = new System.Windows.Forms.TextBox();
            this.buttonRunStop = new System.Windows.Forms.Button();
            this.textMessage = new System.Windows.Forms.TextBox();
            this.textBoxError = new System.Windows.Forms.TextBox();
            this.textBoxStatus = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.comboBoxRunMode = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.textBoxCount = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.textBoxFrameCount = new System.Windows.Forms.TextBox();
            this.textBoxGbps = new System.Windows.Forms.TextBox();
            this.udpPort = new System.Windows.Forms.TextBox();
            this.headerFrameLen = new System.Windows.Forms.TextBox();
            this.fixedFrameLen = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.opeLog = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.checkBoxRecvCount = new System.Windows.Forms.CheckBox();
            this.textBoxLimitRecvCout = new System.Windows.Forms.TextBox();
            this.textBoxLimitFrameCount = new System.Windows.Forms.TextBox();
            this.checkBoxFrameCount = new System.Windows.Forms.CheckBox();
            this.checkBoxWriteFile = new System.Windows.Forms.CheckBox();
            this.textBoxFilePath = new System.Windows.Forms.TextBox();
            this.buttonBrowse = new System.Windows.Forms.Button();
            this.buttonRbcpRead = new System.Windows.Forms.Button();
            this.buttonRbcpWrite = new System.Windows.Forms.Button();
            this.buttonReset = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // buttonConnect
            // 
            this.buttonConnect.Location = new System.Drawing.Point(498, 12);
            this.buttonConnect.Name = "buttonConnect";
            this.buttonConnect.Size = new System.Drawing.Size(75, 23);
            this.buttonConnect.TabIndex = 4;
            this.buttonConnect.Text = "Connect";
            this.buttonConnect.UseVisualStyleBackColor = true;
            this.buttonConnect.Click += new System.EventHandler(this.connect_Click);
            // 
            // ipAddress
            // 
            this.ipAddress.Location = new System.Drawing.Point(180, 14);
            this.ipAddress.Name = "ipAddress";
            this.ipAddress.Size = new System.Drawing.Size(119, 19);
            this.ipAddress.TabIndex = 1;
            this.ipAddress.Text = "localhost";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(0, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(100, 23);
            this.label1.TabIndex = 0;
            // 
            // label_ip_address
            // 
            this.label_ip_address.AutoSize = true;
            this.label_ip_address.Location = new System.Drawing.Point(12, 17);
            this.label_ip_address.Name = "label_ip_address";
            this.label_ip_address.Size = new System.Drawing.Size(162, 12);
            this.label_ip_address.TabIndex = 0;
            this.label_ip_address.Text = "IP Address/Tcp Port/Udp Port";
            // 
            // tcpPort
            // 
            this.tcpPort.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.tcpPort.Location = new System.Drawing.Point(305, 14);
            this.tcpPort.MaxLength = 5;
            this.tcpPort.Name = "tcpPort";
            this.tcpPort.Size = new System.Drawing.Size(77, 19);
            this.tcpPort.TabIndex = 2;
            this.tcpPort.Text = "24242";
            // 
            // buttonRunStop
            // 
            this.buttonRunStop.Enabled = false;
            this.buttonRunStop.Location = new System.Drawing.Point(498, 35);
            this.buttonRunStop.Name = "buttonRunStop";
            this.buttonRunStop.Size = new System.Drawing.Size(75, 59);
            this.buttonRunStop.TabIndex = 8;
            this.buttonRunStop.Text = "Run";
            this.buttonRunStop.UseVisualStyleBackColor = true;
            this.buttonRunStop.Click += new System.EventHandler(this.buttonRunStop_Click);
            // 
            // textMessage
            // 
            this.textMessage.Location = new System.Drawing.Point(14, 196);
            this.textMessage.Multiline = true;
            this.textMessage.Name = "textMessage";
            this.textMessage.ReadOnly = true;
            this.textMessage.Size = new System.Drawing.Size(239, 22);
            this.textMessage.TabIndex = 12;
            this.textMessage.TabStop = false;
            // 
            // textBoxError
            // 
            this.textBoxError.Location = new System.Drawing.Point(48, 107);
            this.textBoxError.Name = "textBoxError";
            this.textBoxError.ReadOnly = true;
            this.textBoxError.Size = new System.Drawing.Size(410, 19);
            this.textBoxError.TabIndex = 11;
            this.textBoxError.TabStop = false;
            // 
            // textBoxStatus
            // 
            this.textBoxStatus.Location = new System.Drawing.Point(48, 75);
            this.textBoxStatus.Name = "textBoxStatus";
            this.textBoxStatus.ReadOnly = true;
            this.textBoxStatus.Size = new System.Drawing.Size(181, 19);
            this.textBoxStatus.TabIndex = 9;
            this.textBoxStatus.TabStop = false;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(42, 81);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(0, 12);
            this.label2.TabIndex = 9;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(4, 82);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(38, 12);
            this.label3.TabIndex = 10;
            this.label3.Text = "Status";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(4, 114);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(30, 12);
            this.label5.TabIndex = 12;
            this.label5.Text = "Error";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(277, 78);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(56, 12);
            this.label4.TabIndex = 17;
            this.label4.Text = "Run Mode";
            // 
            // comboBoxRunMode
            // 
            this.comboBoxRunMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxRunMode.FormattingEnabled = true;
            this.comboBoxRunMode.Location = new System.Drawing.Point(339, 73);
            this.comboBoxRunMode.Name = "comboBoxRunMode";
            this.comboBoxRunMode.Size = new System.Drawing.Size(119, 20);
            this.comboBoxRunMode.TabIndex = 7;
            this.comboBoxRunMode.SelectionChangeCommitted += new System.EventHandler(this.comboBoxRunMode_SelectionChangeCommitted);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(4, 145);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(158, 12);
            this.label6.TabIndex = 19;
            this.label6.Text = "Remain Receive Queue Count";
            // 
            // textBoxCount
            // 
            this.textBoxCount.Location = new System.Drawing.Point(166, 142);
            this.textBoxCount.Name = "textBoxCount";
            this.textBoxCount.ReadOnly = true;
            this.textBoxCount.Size = new System.Drawing.Size(87, 19);
            this.textBoxCount.TabIndex = 18;
            this.textBoxCount.TabStop = false;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(4, 164);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(149, 12);
            this.label7.TabIndex = 21;
            this.label7.Text = "Remain Frame Queue Count";
            // 
            // textBoxFrameCount
            // 
            this.textBoxFrameCount.Location = new System.Drawing.Point(166, 161);
            this.textBoxFrameCount.Name = "textBoxFrameCount";
            this.textBoxFrameCount.ReadOnly = true;
            this.textBoxFrameCount.Size = new System.Drawing.Size(87, 19);
            this.textBoxFrameCount.TabIndex = 20;
            this.textBoxFrameCount.TabStop = false;
            // 
            // textBoxGbps
            // 
            this.textBoxGbps.Location = new System.Drawing.Point(259, 196);
            this.textBoxGbps.Multiline = true;
            this.textBoxGbps.Name = "textBoxGbps";
            this.textBoxGbps.ReadOnly = true;
            this.textBoxGbps.Size = new System.Drawing.Size(199, 22);
            this.textBoxGbps.TabIndex = 22;
            this.textBoxGbps.TabStop = false;
            // 
            // udpPort
            // 
            this.udpPort.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.udpPort.Location = new System.Drawing.Point(388, 14);
            this.udpPort.MaxLength = 5;
            this.udpPort.Name = "udpPort";
            this.udpPort.Size = new System.Drawing.Size(70, 19);
            this.udpPort.TabIndex = 3;
            this.udpPort.Text = "4660";
            // 
            // headerFrameLen
            // 
            this.headerFrameLen.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.headerFrameLen.Location = new System.Drawing.Point(166, 47);
            this.headerFrameLen.MaxLength = 5;
            this.headerFrameLen.Name = "headerFrameLen";
            this.headerFrameLen.Size = new System.Drawing.Size(63, 19);
            this.headerFrameLen.TabIndex = 5;
            this.headerFrameLen.Text = "2";
            // 
            // fixedFrameLen
            // 
            this.fixedFrameLen.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.fixedFrameLen.Location = new System.Drawing.Point(388, 47);
            this.fixedFrameLen.MaxLength = 5;
            this.fixedFrameLen.Name = "fixedFrameLen";
            this.fixedFrameLen.Size = new System.Drawing.Size(70, 19);
            this.fixedFrameLen.TabIndex = 6;
            this.fixedFrameLen.Text = "10000";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(12, 50);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(148, 12);
            this.label8.TabIndex = 26;
            this.label8.Text = "Event Frame Header Length";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(277, 50);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(107, 12);
            this.label9.TabIndex = 27;
            this.label9.Text = "Fixed Frame Length";
            // 
            // opeLog
            // 
            this.opeLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.opeLog.Location = new System.Drawing.Point(14, 293);
            this.opeLog.Multiline = true;
            this.opeLog.Name = "opeLog";
            this.opeLog.ReadOnly = true;
            this.opeLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.opeLog.Size = new System.Drawing.Size(559, 153);
            this.opeLog.TabIndex = 28;
            this.opeLog.TabStop = false;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(12, 278);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(76, 12);
            this.label10.TabIndex = 29;
            this.label10.Text = "Operation Log";
            // 
            // checkBoxRecvCount
            // 
            this.checkBoxRecvCount.AutoSize = true;
            this.checkBoxRecvCount.Checked = true;
            this.checkBoxRecvCount.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxRecvCount.Location = new System.Drawing.Point(269, 145);
            this.checkBoxRecvCount.Name = "checkBoxRecvCount";
            this.checkBoxRecvCount.Size = new System.Drawing.Size(53, 16);
            this.checkBoxRecvCount.TabIndex = 10;
            this.checkBoxRecvCount.Text = "Limit ";
            this.checkBoxRecvCount.UseVisualStyleBackColor = true;
            // 
            // textBoxLimitRecvCout
            // 
            this.textBoxLimitRecvCout.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.textBoxLimitRecvCout.Location = new System.Drawing.Point(328, 143);
            this.textBoxLimitRecvCout.MaxLength = 5;
            this.textBoxLimitRecvCout.Name = "textBoxLimitRecvCout";
            this.textBoxLimitRecvCout.Size = new System.Drawing.Size(63, 19);
            this.textBoxLimitRecvCout.TabIndex = 11;
            this.textBoxLimitRecvCout.Text = "5000";
            // 
            // textBoxLimitFrameCount
            // 
            this.textBoxLimitFrameCount.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.textBoxLimitFrameCount.Location = new System.Drawing.Point(328, 165);
            this.textBoxLimitFrameCount.MaxLength = 5;
            this.textBoxLimitFrameCount.Name = "textBoxLimitFrameCount";
            this.textBoxLimitFrameCount.Size = new System.Drawing.Size(63, 19);
            this.textBoxLimitFrameCount.TabIndex = 13;
            this.textBoxLimitFrameCount.Text = "10000";
            // 
            // checkBoxFrameCount
            // 
            this.checkBoxFrameCount.AutoSize = true;
            this.checkBoxFrameCount.Checked = true;
            this.checkBoxFrameCount.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxFrameCount.Location = new System.Drawing.Point(269, 167);
            this.checkBoxFrameCount.Name = "checkBoxFrameCount";
            this.checkBoxFrameCount.Size = new System.Drawing.Size(53, 16);
            this.checkBoxFrameCount.TabIndex = 12;
            this.checkBoxFrameCount.Text = "Limit ";
            this.checkBoxFrameCount.UseVisualStyleBackColor = true;
            // 
            // checkBoxWriteFile
            // 
            this.checkBoxWriteFile.AutoSize = true;
            this.checkBoxWriteFile.Location = new System.Drawing.Point(12, 241);
            this.checkBoxWriteFile.Name = "checkBoxWriteFile";
            this.checkBoxWriteFile.Size = new System.Drawing.Size(72, 16);
            this.checkBoxWriteFile.TabIndex = 14;
            this.checkBoxWriteFile.Text = "Save File";
            this.checkBoxWriteFile.UseVisualStyleBackColor = true;
            // 
            // textBoxFilePath
            // 
            this.textBoxFilePath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxFilePath.Location = new System.Drawing.Point(84, 224);
            this.textBoxFilePath.Multiline = true;
            this.textBoxFilePath.Name = "textBoxFilePath";
            this.textBoxFilePath.ReadOnly = true;
            this.textBoxFilePath.Size = new System.Drawing.Size(388, 49);
            this.textBoxFilePath.TabIndex = 35;
            this.textBoxFilePath.TabStop = false;
            // 
            // buttonBrowse
            // 
            this.buttonBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonBrowse.Location = new System.Drawing.Point(498, 250);
            this.buttonBrowse.Name = "buttonBrowse";
            this.buttonBrowse.Size = new System.Drawing.Size(75, 23);
            this.buttonBrowse.TabIndex = 15;
            this.buttonBrowse.Text = "Browse";
            this.buttonBrowse.UseVisualStyleBackColor = true;
            this.buttonBrowse.Click += new System.EventHandler(this.buttonBrowse_Click);
            // 
            // buttonRbcpRead
            // 
            this.buttonRbcpRead.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonRbcpRead.Location = new System.Drawing.Point(14, 452);
            this.buttonRbcpRead.Name = "buttonRbcpRead";
            this.buttonRbcpRead.Size = new System.Drawing.Size(75, 23);
            this.buttonRbcpRead.TabIndex = 16;
            this.buttonRbcpRead.Text = "RBCP Read";
            this.buttonRbcpRead.UseVisualStyleBackColor = true;
            this.buttonRbcpRead.Click += new System.EventHandler(this.buttonRbcpRead_Click);
            // 
            // buttonRbcpWrite
            // 
            this.buttonRbcpWrite.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonRbcpWrite.Location = new System.Drawing.Point(99, 452);
            this.buttonRbcpWrite.Name = "buttonRbcpWrite";
            this.buttonRbcpWrite.Size = new System.Drawing.Size(207, 23);
            this.buttonRbcpWrite.TabIndex = 17;
            this.buttonRbcpWrite.Text = "RBCP Write(Control Register Reset)";
            this.buttonRbcpWrite.UseVisualStyleBackColor = true;
            this.buttonRbcpWrite.Click += new System.EventHandler(this.buttonRbcpWrite_Click);
            // 
            // buttonReset
            // 
            this.buttonReset.Location = new System.Drawing.Point(498, 105);
            this.buttonReset.Name = "buttonReset";
            this.buttonReset.Size = new System.Drawing.Size(75, 23);
            this.buttonReset.TabIndex = 9;
            this.buttonReset.Text = "Reset";
            this.buttonReset.UseVisualStyleBackColor = true;
            this.buttonReset.Click += new System.EventHandler(this.buttonReset_Click);
            // 
            // SiDaqConnect
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(585, 477);
            this.Controls.Add(this.buttonReset);
            this.Controls.Add(this.buttonRbcpWrite);
            this.Controls.Add(this.buttonRbcpRead);
            this.Controls.Add(this.buttonBrowse);
            this.Controls.Add(this.textBoxFilePath);
            this.Controls.Add(this.checkBoxWriteFile);
            this.Controls.Add(this.textBoxLimitFrameCount);
            this.Controls.Add(this.checkBoxFrameCount);
            this.Controls.Add(this.textBoxLimitRecvCout);
            this.Controls.Add(this.checkBoxRecvCount);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.opeLog);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.fixedFrameLen);
            this.Controls.Add(this.headerFrameLen);
            this.Controls.Add(this.udpPort);
            this.Controls.Add(this.textBoxGbps);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.textBoxFrameCount);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.textBoxCount);
            this.Controls.Add(this.comboBoxRunMode);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBoxStatus);
            this.Controls.Add(this.textBoxError);
            this.Controls.Add(this.textMessage);
            this.Controls.Add(this.buttonRunStop);
            this.Controls.Add(this.tcpPort);
            this.Controls.Add(this.label_ip_address);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.ipAddress);
            this.Controls.Add(this.buttonConnect);
            this.Name = "SiDaqConnect";
            this.Text = "SiTCP Event Receive Sample Application";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.SiDaqConnect_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonConnect;
        private System.Windows.Forms.TextBox ipAddress;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label_ip_address;
        private System.Windows.Forms.TextBox tcpPort;
        private System.Windows.Forms.Button buttonRunStop;
        private System.Windows.Forms.TextBox textMessage;
        private System.Windows.Forms.TextBox textBoxError;
        private System.Windows.Forms.TextBox textBoxStatus;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox comboBoxRunMode;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox textBoxCount;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox textBoxFrameCount;
        private System.Windows.Forms.TextBox textBoxGbps;
        private System.Windows.Forms.TextBox udpPort;
        private System.Windows.Forms.TextBox headerFrameLen;
        private System.Windows.Forms.TextBox fixedFrameLen;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox opeLog;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.CheckBox checkBoxRecvCount;
        private System.Windows.Forms.TextBox textBoxLimitRecvCout;
        private System.Windows.Forms.TextBox textBoxLimitFrameCount;
        private System.Windows.Forms.CheckBox checkBoxFrameCount;
        private System.Windows.Forms.CheckBox checkBoxWriteFile;
        private System.Windows.Forms.TextBox textBoxFilePath;
        private System.Windows.Forms.Button buttonBrowse;
        private System.Windows.Forms.Button buttonRbcpRead;
        private System.Windows.Forms.Button buttonRbcpWrite;
        private System.Windows.Forms.Button buttonReset;
    }
}

