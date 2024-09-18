//
// SiDaqMfcSampleDlg.cpp
// ~~~~~~~~~~~~~~  
// 
// Copyright (C) 2024  Bee Beans Technologies Co.,Ltd.
//
// Released under the MIT License.
// (See accompanying file LICENSE)
//

// implementation file

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "SiDaqMfcSample.h"
#include "SiDaqMfcSampleDlg.h"
#include "afxdialogex.h"
#include < atlpath.h >//CPath
#include <time.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CSiDaqMfcSampleDlg dialog



CSiDaqMfcSampleDlg::CSiDaqMfcSampleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIDAQMFCSAMPLE_DIALOG, pParent), m_pSiDaqMngr(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_winthread_ptr = NULL;
	m_update_timer = NULL;
	CurrentGuiStatus = GUI_NO_CONNECTION;
}

void CSiDaqMfcSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_RUN_MODE, m_combobox_run_mode);
}



BEGIN_MESSAGE_MAP(CSiDaqMfcSampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CSiDaqMfcSampleDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CSiDaqMfcSampleDlg::OnBnClickedButtonRunStop)
	ON_COMMAND(ID_APP_EXIT, &CSiDaqMfcSampleDlg::OnAppExit)
	ON_BN_CLICKED(IDC_BUTTON_RBCP_READ, &CSiDaqMfcSampleDlg::OnBnClickedButtonRbcpRead)
	ON_BN_CLICKED(IDC_BUTTON_FILE_BROWSE, &CSiDaqMfcSampleDlg::OnBnClickedButtonFileBrowse)
	ON_BN_CLICKED(IDC_BUTTON_RBCP_WRITE, &CSiDaqMfcSampleDlg::OnBnClickedButtonRbcpWrite)
	ON_BN_CLICKED(IDOK, &CSiDaqMfcSampleDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CSiDaqMfcSampleDlg::OnBnClickedButtonReset)
	ON_CBN_SELCHANGE(IDC_COMBO_RUN_MODE, &CSiDaqMfcSampleDlg::OnCbnSelchangeComboRunMode)
END_MESSAGE_MAP()

// CSiDaqMfcSampleDlg message handlers

BOOL CSiDaqMfcSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_combobox_run_mode.AddString(L"NULL");
	m_combobox_run_mode.AddString(L"Header");
	m_combobox_run_mode.AddString(L"Fixed");
	m_combobox_run_mode.SetCurSel(0);
	CurrentGuiStatus = GUI_NO_CONNECTION;
	UpdateGUIControlStatus(CurrentGuiStatus);
	((CEdit*)GetDlgItem(IDC_IP_ADDRESS))->SetWindowTextW(CStringW("localhost"));
	((CEdit*)GetDlgItem(IDC_TCP_PORT))->SetWindowTextW(CStringW("24242"));
	((CEdit*)GetDlgItem(IDC_UDP_PORT))->SetWindowTextW(CStringW("4660"));
	((CEdit*)GetDlgItem(IDC_HEADER_LEN))->SetWindowTextW(CStringW("2"));
	((CEdit*)GetDlgItem(IDC_FRAME_LEN))->SetWindowTextW(CStringW("10000"));
	((CEdit*)GetDlgItem(IDC_RECV_LIMIT_COUNT))->SetWindowTextW(CStringW("5000"));
	((CEdit*)GetDlgItem(IDC_FRAME_LIMIT_COUNT))->SetWindowTextW(CStringW("10000"));
	CString curDir; 
	wchar_t path[_MAX_PATH], drive[_MAX_PATH], dir[_MAX_PATH], fname[_MAX_PATH], ext[_MAX_PATH];

	if (::GetModuleFileName(NULL, path, _MAX_PATH) != 0)
	{
		::_wsplitpath_s(path, drive, dir, fname, ext);
		curDir = CString(drive) + CString(dir);
		((CEdit*)GetDlgItem(IDC_STATIC_FILEPATH))->SetWindowTextW(curDir);
		curDir.ReleaseBuffer();
	}
	AddOpelog("System Start");
	((CButton*)GetDlgItem(IDC_RECV_LIMIT_CHECK))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_FRAME_LIMIT_CHECK))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_WRITE_FILE_CHECK))->SetCheck(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSiDaqMfcSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSiDaqMfcSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSiDaqMfcSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CSiDaqMfcSampleDlg::GetFrameLength(unsigned char* recv_data_ptr, UINT data_len, UINT header_len) {
	if (data_len < header_len) {
		return -1;
	}
	if (header_len == 0) {
		return data_len;
	}

	//Big Endian
	int header = 0;
	for (UINT i = 0; i < header_len; i++)
	{
		header += (int)(recv_data_ptr[i] << (header_len - i - 1) * 8);
	}

	if (header == 0) {
		return -1;
	}
	return header;

}

void CSiDaqMfcSampleDlg::UpdateGUIControlStatus(GUIStatus guistatus) {
	int index = 0;
	switch (guistatus)
	{
	case GUI_NO_CONNECTION:
		GetDlgItem(IDC_IP_ADDRESS)->EnableWindow(TRUE);
		GetDlgItem(IDC_TCP_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_UDP_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowTextW(CStringW("Connect"));
		GetDlgItem(IDC_BUTTON_RUN)->SetWindowTextW(CStringW("Run"));
		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_RUN_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_HEADER_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECV_LIMIT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LIMIT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECV_LIMIT_COUNT)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LIMIT_COUNT)->EnableWindow(FALSE);
		GetDlgItem(IDC_WRITE_FILE_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FILE_BROWSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RBCP_READ)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RBCP_WRITE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);
		break;
	case GUI_IDLE:
		GetDlgItem(IDC_IP_ADDRESS)->EnableWindow(FALSE);
		GetDlgItem(IDC_TCP_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_UDP_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowTextW(CStringW("Disconnect"));
		GetDlgItem(IDC_BUTTON_RUN)->SetWindowTextW(CStringW("Run"));
		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_RUN_MODE)->EnableWindow(TRUE);
		index = ((CComboBox*)GetDlgItem(IDC_COMBO_RUN_MODE))->GetCurSel();
		FrameControlInitialize(index);
		GetDlgItem(IDC_RECV_LIMIT_CHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_FRAME_LIMIT_CHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_RECV_LIMIT_COUNT)->EnableWindow(TRUE);
		GetDlgItem(IDC_FRAME_LIMIT_COUNT)->EnableWindow(TRUE);
		GetDlgItem(IDC_WRITE_FILE_CHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FILE_BROWSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RBCP_READ)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RBCP_WRITE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);
		break;
	case GUI_RUNNING:
		GetDlgItem(IDC_IP_ADDRESS)->EnableWindow(FALSE);
		GetDlgItem(IDC_TCP_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_UDP_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowTextW(CStringW("Disconnect"));
		GetDlgItem(IDC_BUTTON_RUN)->SetWindowTextW(CStringW("Stop"));
		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_RUN_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_HEADER_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECV_LIMIT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LIMIT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECV_LIMIT_COUNT)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LIMIT_COUNT)->EnableWindow(FALSE);
		GetDlgItem(IDC_WRITE_FILE_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FILE_BROWSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RBCP_READ)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RBCP_WRITE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(FALSE);
		break;
	case GUI_STOPPING:
		GetDlgItem(IDC_IP_ADDRESS)->EnableWindow(FALSE);
		GetDlgItem(IDC_TCP_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_UDP_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowTextW(CStringW("Disconnect"));
		GetDlgItem(IDC_BUTTON_RUN)->SetWindowTextW(CStringW("Stop"));
		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_RUN_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_HEADER_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECV_LIMIT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LIMIT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECV_LIMIT_COUNT)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LIMIT_COUNT)->EnableWindow(FALSE);
		GetDlgItem(IDC_WRITE_FILE_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FILE_BROWSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RBCP_READ)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RBCP_WRITE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE);
		break;
	default:
		break;
	}
}

void CSiDaqMfcSampleDlg::FrameControlInitialize(int index) {
	switch (index)
	{
	case 0://NULL
		GetDlgItem(IDC_HEADER_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LEN)->EnableWindow(FALSE);
		break;
	case 1://Header
		GetDlgItem(IDC_HEADER_LEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_FRAME_LEN)->EnableWindow(FALSE);
		break;
	case 2://Fixed
		GetDlgItem(IDC_HEADER_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LEN)->EnableWindow(TRUE);
		break;
	default:
		GetDlgItem(IDC_HEADER_LEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_FRAME_LEN)->EnableWindow(FALSE);
		break;
	}
}

void CSiDaqMfcSampleDlg::OnBnClickedButtonConnect()
{
	CWaitCursor wait_cursor;
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
	if (m_pSiDaqMngr == NULL ) {

		CString ip_address;
		((CEdit*)GetDlgItem(IDC_IP_ADDRESS))->GetWindowTextW(ip_address);
		UINT nTcpPort = GetDlgItemInt(IDC_TCP_PORT);
		m_pSiDaqMngr = SiDaqInitialize(CT2A(ip_address), nTcpPort, 4660, FALSE);
		if (m_pSiDaqMngr) {
			if (SiDaqGetStatus(m_pSiDaqMngr) != SIDAQ_SYS_ERROR) {
				CurrentGuiStatus = GUI_IDLE;
				m_update_timer = SetTimer(SIDAQ_UPDATE_TIMER_ID, SIDAQ_UPDATE_INTERVAL, NULL);
				AddOpelog("Connection Start");
			}
			else {
				CString status_str(SiDaqStatusText(m_pSiDaqMngr));
				SetDlgItemText(IDC_STATIC_STATUS, status_str);
				CString error_str(SiDaqErrorMessage(m_pSiDaqMngr));
				SetDlgItemText(IDC_STATIC_ERROR, error_str);
				SiDaqTerminate(m_pSiDaqMngr, FALSE);
				CurrentGuiStatus = GUI_NO_CONNECTION;
				m_pSiDaqMngr = NULL;
				AddOpelog("Connection Failed");
			}
		}
	}
	else {

		SiDaqTerminate(m_pSiDaqMngr, FALSE);
		CurrentGuiStatus = GUI_NO_CONNECTION;
		m_pSiDaqMngr = NULL;
		KillTimer(m_update_timer);
		m_update_timer = 0;
		AddOpelog("Connection End");
	}
	UpdateGUIControlStatus(CurrentGuiStatus);
}

void CSiDaqMfcSampleDlg::OnDestroy()
{
	if (m_pSiDaqMngr) {
		SiDaqTerminate(m_pSiDaqMngr, FALSE);
	}

}

void WriteDataToFile(CString file_name, unsigned char* data, int data_len)
{
	FILE* file;
	_tfopen_s(&file, file_name, (CString)"a");
	if (file == NULL) return;

	for (int i = 0; i < data_len; i++) {
		fprintf(file, "%02X ", data[i]);
	}
	fprintf(file, "\r\n");

	fclose(file);
}

UINT recv_thread(LPVOID pParam)
{

	SiDaqThreadParam* sidaq_thread_param = (SiDaqThreadParam*)pParam;
	SiDaqStatus run_status;
	int recv_count;
	int frame_count;
	bool remain_flg = false;
	sidaq_thread_param->receive_bytes = 0;
	sidaq_thread_param->start = clock();
	while (sidaq_thread_param->force_exit_flg) {
		if (sidaq_thread_param->sidaq_mngr_ptr == NULL) {
			break;
		}

		run_status = SiDaqGetStatus(sidaq_thread_param->sidaq_mngr_ptr);
		if (run_status == SIDAQ_IDLE || run_status == SIDAQ_STOPPING || run_status == SIDAQ_SYS_ERROR) {
			recv_count = SiDaqGetDataCount(sidaq_thread_param->sidaq_mngr_ptr);
			frame_count = SiDaqGetFrameDataCount(sidaq_thread_param->sidaq_mngr_ptr);
			if (recv_count == 0 && frame_count == 0) {
				for (int i = 0; i < 10; i++) {
					recv_count = SiDaqGetDataCount(sidaq_thread_param->sidaq_mngr_ptr);
					frame_count = SiDaqGetFrameDataCount(sidaq_thread_param->sidaq_mngr_ptr);
					if (recv_count == 0 && frame_count == 0) {
						remain_flg = false;
						::Sleep(100);
					}
					else {
						remain_flg = true;
						break;
					}
				}

				if (remain_flg == false) {
					break;
				}
			}
		}

		int data_len = 0;
		unsigned char* data_ptr = SiDaqGetData(sidaq_thread_param->sidaq_mngr_ptr, &data_len, 10);

		if (data_ptr) {
			if (sidaq_thread_param->write_file_flg) {
				SiDaqPauseRecvData(sidaq_thread_param->sidaq_mngr_ptr);
				WriteDataToFile(sidaq_thread_param->write_file_path, data_ptr, data_len);
				SiDaqResumeRecvData(sidaq_thread_param->sidaq_mngr_ptr);
			}
			
			sidaq_thread_param->receive_bytes += data_len;
			SiDaqDeleteData(data_ptr);
			
		}
		::Sleep(0);
	}
	return 0;
}


void CSiDaqMfcSampleDlg::OnBnClickedButtonRunStop()
{
	GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
	UINT frame_limit = 0;
	UINT recv_limit = 0;
	int index = 0;
	UINT set_header_len = 0;
	UINT set_fixed_len = 0;
	int (*set_frame_func)(unsigned char*, UINT, UINT) = NULL;
	

	if (m_pSiDaqMngr) {
		SiDaqStatus sidaq_status = SiDaqGetStatus(m_pSiDaqMngr);
		SiDaqStatus run_status = SIDAQ_RUN;//next status.
		SiDaqStatus new_status = sidaq_status;
		CStringW button_text;
		GetDlgItemTextW(IDC_BUTTON_RUN, button_text);
		switch (sidaq_status) {
		case SIDAQ_SYS_ERROR:
			if (CurrentGuiStatus != GUI_IDLE) {
				break;
			}
			// Start Run
		case SIDAQ_IDLE:
			recv_limit = GetDlgItemInt(IDC_RECV_LIMIT_COUNT);
			if (((CButton*)GetDlgItem(IDC_RECV_LIMIT_CHECK))->GetCheck() == 0) {
				recv_limit = 0;
			}

			frame_limit = GetDlgItemInt(IDC_FRAME_LIMIT_COUNT);
			if (((CButton*)GetDlgItem(IDC_FRAME_LIMIT_CHECK))->GetCheck() == 0) {
				frame_limit = 0;
			}
			index = ((CComboBox*)GetDlgItem(IDC_COMBO_RUN_MODE))->GetCurSel();
			set_header_len = GetDlgItemInt(IDC_HEADER_LEN);

			set_fixed_len = GetDlgItemInt(IDC_FRAME_LEN);

			switch (index)
			{
			case 0://NULL
				set_header_len = 0;
				set_fixed_len = 0;
				set_frame_func = NULL;
				break;
			case 1://Header
				set_fixed_len = 0;
				set_frame_func = &GetFrameLength;
				break;
			case 2://Fixed
				set_frame_func = NULL;
				break;
			default:
				set_header_len = 0;
				set_fixed_len = 0;
				set_frame_func = NULL;
				break;
			}
			if (m_winthread_ptr) {
				m_thread_param.force_exit_flg = false;
				while (true) {
					DWORD result = ::WaitForSingleObject(m_winthread_ptr->m_hThread, INFINITE);
					if (result != WAIT_TIMEOUT) {
						break;
					}
					Sleep(0);
				}
				m_winthread_ptr = NULL;
			}
			new_status = SiDaqRun(m_pSiDaqMngr, set_frame_func, set_header_len, set_fixed_len, 100*1024*1024, recv_limit,frame_limit, DEFAULT_RECV_BUFF_SIZE, DEFAULT_MAX_FRAME_LEN);
			if (new_status == run_status) {
				CurrentGuiStatus = GUI_RUNNING;
				if (m_winthread_ptr) {
					while (true) {
						DWORD result = ::WaitForSingleObject(m_winthread_ptr->m_hThread, INFINITE);
						if (result != WAIT_TIMEOUT) {
							break;
						}
						Sleep(0);
					}
					m_winthread_ptr = NULL;
				}
				m_thread_param.dlg_window = this->m_hWnd;// GetDlgItem(IDC_STATIC_DEVICE_INFO)->m_hWnd;
				m_thread_param.sidaq_mngr_ptr = m_pSiDaqMngr;
				m_thread_param.write_file_flg = ((CButton*)GetDlgItem(IDC_WRITE_FILE_CHECK))->GetCheck();
				m_thread_param.force_exit_flg = true;
				if (m_thread_param.write_file_flg) {
					auto time_t = time(NULL);
					tm currentTime;
					localtime_s(&currentTime, &time_t);
					char datetime[32];
					strftime(datetime, sizeof(datetime), "%Y%m%d%H%M%S", &currentTime);
					CString filename = (CString)datetime + (CString)".txt";
					CString dirname;
					((CEdit*)GetDlgItem(IDC_STATIC_FILEPATH))->GetWindowTextW(dirname);
					CString writefilepath = dirname + CString("\\") + filename;
					m_thread_param.write_file_path = writefilepath;
				}
				m_winthread_ptr = AfxBeginThread(recv_thread, (LPVOID)&m_thread_param);
				UpdateGUIControlStatus(CurrentGuiStatus);
				AddOpelog("Run Start");
			}
			else {
				CString status_str(SiDaqStatusText(m_pSiDaqMngr));
				SetDlgItemText(IDC_STATIC_STATUS, status_str);
				CString error_str(SiDaqErrorMessage(m_pSiDaqMngr));
				SetDlgItemText(IDC_STATIC_ERROR, error_str);
				SiDaqTerminate(m_pSiDaqMngr, FALSE);
				CurrentGuiStatus = GUI_IDLE;
				AddOpelog("Run Failed");
				UpdateGUIControlStatus(CurrentGuiStatus);
			}
			break;
		case SIDAQ_RUN:
			AddOpelog("Run Stopping…");
			SiDaqStop(m_pSiDaqMngr);
			CurrentGuiStatus = GUI_STOPPING;
			UpdateGUIControlStatus(CurrentGuiStatus);
			//Queue count
			CString str_rcvqueue;
			str_rcvqueue.Format(_T("%u"), SiDaqGetDataCount(m_pSiDaqMngr));
			SetDlgItemText(IDC_STATIC_RECEIVE, (LPCTSTR)str_rcvqueue);// ->GetString());

			CString str_evntqueue;
			str_evntqueue.Format(_T("%u"), SiDaqGetFrameDataCount(m_pSiDaqMngr));
			SetDlgItemText(IDC_STATIC_EVENT, (LPCTSTR)str_evntqueue);// ->GetString());
			break;
		}

	}
	//GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(TRUE);
	//GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
}

void CSiDaqMfcSampleDlg::OnAppExit()
{
	if (m_winthread_ptr) {
		m_thread_param.force_exit_flg = false;
		while (true) {
			DWORD result = ::WaitForSingleObject(m_winthread_ptr->m_hThread, INFINITE);
			if (result != WAIT_TIMEOUT) {
				break;
			}
			Sleep(0);
		}
		m_winthread_ptr = NULL;
	}

	if( m_pSiDaqMngr ){
		SiDaqTerminate(m_pSiDaqMngr, FALSE);
		m_pSiDaqMngr = NULL;
	}
}




void CSiDaqMfcSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
	bool remain_flg = false;
	if (m_update_timer >= 0) {

		if (nIDEvent == SIDAQ_UPDATE_TIMER_ID) {
			KillTimer(m_update_timer);
		}
		////Update Status Control.
		CString status_str(SiDaqStatusText(m_pSiDaqMngr));
		SetDlgItemText(IDC_STATIC_STATUS, status_str);
		CString error_str(SiDaqErrorMessage(m_pSiDaqMngr));
		SetDlgItemText(IDC_STATIC_ERROR, error_str);
		SiDaqStatus run_status = SiDaqGetStatus(m_pSiDaqMngr);
		if (run_status == SIDAQ_RUN) {
			CString str_bytes;
			if (m_thread_param.receive_bytes < 1000) {
				str_bytes.Format(_T("data received : %lld Bytes"), m_thread_param.receive_bytes);
			}
			else if (m_thread_param.receive_bytes < 1000 *1000) {
				str_bytes.Format(_T("data received : %.3lf KBytes"), m_thread_param.receive_bytes / (double)1000);
			}
			else if (m_thread_param.receive_bytes < 1000 * 1000 * 1000) {
				str_bytes.Format(_T("data received : %.3lf MBytes"), m_thread_param.receive_bytes / (double)(1000 * 1000));
			}
			else {
				str_bytes.Format(_T("data received : %.3lf GBytes"), m_thread_param.receive_bytes / (double)(1000 * 1000 * 1000));
			}
			SetDlgItemText(IDC_STATIC_DEVICE_INFO, (LPCTSTR)str_bytes);// ->GetString());
			//cal byte rate
			clock_t current = clock();
			auto sec = (double)(current - m_thread_param.start) / CLOCKS_PER_SEC;
			if (sec != 0) {
				auto gbps = (m_thread_param.receive_bytes * 8 / sec) / 1000000;
				CString str_gbps;
				str_gbps.Format(_T("data receive rate : %.3e Mbps"), gbps);
				SetDlgItemText(IDC_STATIC_GBPS_INFO, (LPCTSTR)str_gbps);// ->GetString());
			}
		}
		//Queue count
		CString str_rcvqueue;
		int recv_count = SiDaqGetDataCount(m_pSiDaqMngr);
		str_rcvqueue.Format(_T("%u"), recv_count);
		SetDlgItemText(IDC_STATIC_RECEIVE, (LPCTSTR)str_rcvqueue);// ->GetString());

		CString str_evntqueue;
		int frame_count = SiDaqGetFrameDataCount(m_pSiDaqMngr);
		str_evntqueue.Format(_T("%u"), frame_count);
		SetDlgItemText(IDC_STATIC_EVENT, (LPCTSTR)str_evntqueue);// ->GetString());
		if (run_status == SIDAQ_SYS_ERROR && CurrentGuiStatus == GUI_RUNNING) {
			CurrentGuiStatus = GUI_STOPPING;
			UpdateGUIControlStatus(CurrentGuiStatus);
			AddOpelog("Force Stopping…");
		}
		if (CurrentGuiStatus == GUI_STOPPING) {
			if (recv_count == 0 && frame_count == 0) {
				for (int i = 0; i < 10; i++) {
					recv_count = SiDaqGetDataCount(m_pSiDaqMngr);
					frame_count = SiDaqGetFrameDataCount(m_pSiDaqMngr);
					if (recv_count == 0 && frame_count == 0) {
						remain_flg = false;
						::Sleep(100);
					}
					else {
						remain_flg = true;
						break;
					}
				}

				if (remain_flg == false) {
					CurrentGuiStatus = GUI_IDLE;
					UpdateGUIControlStatus(CurrentGuiStatus);
					AddOpelog("Run Stop");
				}
			}
		}

		if (nIDEvent == SIDAQ_UPDATE_TIMER_ID) {
			m_update_timer = SetTimer(SIDAQ_UPDATE_TIMER_ID, SIDAQ_UPDATE_INTERVAL, NULL);
		}
	}
}



void CSiDaqMfcSampleDlg::AddOpelog(CString logString) {
	CEdit* opeLogEditBox = (CEdit*)GetDlgItem(IDC_OPE_LOG);
	int textLength = opeLogEditBox->GetWindowTextLength();

	auto time_t = time(NULL);
	tm currentTime;
	localtime_s(&currentTime, &time_t);
	char datetime[32];
	strftime(datetime, sizeof(datetime), "%Y/%m/%d %H:%M:%S", &currentTime);
	auto newLine = "\r\n";
	auto devide = " : ";
	auto logs = (CString)datetime + (CString)devide + logString + (CString)newLine;
	opeLogEditBox->SetSel(textLength, textLength);
	opeLogEditBox->ReplaceSel(logs);
}

void CSiDaqMfcSampleDlg::AddOpelog(const char* logString) {
	CSiDaqMfcSampleDlg::AddOpelog((CString)logString);
}

LRESULT CSiDaqMfcSampleDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Close Message
	if (message == WM_CLOSE)
	{
		OnAppExit();
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

CString CreateSiTCPInfo(unsigned char* ret_data)
{
	CString newline = (CString)"\r\n";
	CString ret = (CString)"SiTCP Info" + newline;

	if (ret_data == NULL) return ret;
	const UINT32 ADDR_RESERVED = 0xffffff00;
	const UINT32 ADDR_SITCP_CONTROL = ADDR_RESERVED + 0x10;
	const UINT32 ADDR_MAC = ADDR_RESERVED + 0x12;
	const UINT32 ADDR_IP = ADDR_RESERVED + 0x18;
	const UINT32 ADDR_TCP_DATA_PORT = ADDR_RESERVED + 0x1c;
	const UINT32 ADDR_TCP_COMMAND_PORT = ADDR_RESERVED + 0x1e;
	const UINT32 ADDR_TCP_MAX_SEGMENT_SIZE = ADDR_RESERVED + 0x20;
	const UINT32 ADDR_UDP_PORT = ADDR_RESERVED + 0x22;
	const UINT32 ADDR_TCP_KEEP_ALIVE_BNE = ADDR_RESERVED + 0x24;
	const UINT32 ADDR_TCP_KEEP_ALIVE_BE = ADDR_RESERVED + 0x26;
	const UINT32 ADDR_TCP_TIMEOUT_CONNECTING = ADDR_RESERVED + 0x28;
	const UINT32 ADDR_TCP_TIMEOUT_DISCONNECT = ADDR_RESERVED + 0x2a;
	const UINT32 ADDR_TCP_RECONNECT_INTERVAL = ADDR_RESERVED + 0x2c;
	const UINT32 ADDR_TCP_RETRANSMITION_TIMER = ADDR_RESERVED + 0x2e;

	// IP Address
	UINT32 begin = ADDR_IP - ADDR_RESERVED;
	CString str_data;
	str_data.Format(_T("IP Address : %d.%d.%d.%d\r\n"), ret_data[begin + 0], ret_data[begin + 1], ret_data[begin + 2], ret_data[begin + 3]);
	ret += str_data;

	//TCP Data Port
	begin = ADDR_TCP_DATA_PORT - ADDR_RESERVED;
	str_data.Format(_T("TCP Port : %d\r\n"), (ret_data[begin + 0] << 8) + ret_data[begin + 1]);
	ret += str_data;

	// UDP Port
	begin = ADDR_UDP_PORT - ADDR_RESERVED;
	str_data.Format(_T("UDP Port : %d\r\n"), (ret_data[begin + 0] << 8) + ret_data[begin + 1]);
	ret += str_data;

	// MAC Address
	begin = ADDR_MAC - ADDR_RESERVED;
	str_data.Format(_T("MAC Address : %02X:%02X:%02X:%02X:%02X:%02X\r\n"), ret_data[begin + 0], ret_data[begin + 1], ret_data[begin + 2], ret_data[begin + 3], ret_data[begin + 4], ret_data[begin + 5]);
	ret += str_data;

	// TCP Command Port
	begin = ADDR_TCP_COMMAND_PORT - ADDR_RESERVED;
	str_data.Format(_T("TCP Command Port : %d\r\n"), (ret_data[begin + 0] << 8) + ret_data[begin + 1]);
	ret += str_data;

	// TCP Max Segment Size
	begin = ADDR_TCP_MAX_SEGMENT_SIZE - ADDR_RESERVED;
	str_data.Format(_T("TCP Max Segment Size : %d\r\n"), (ret_data[begin + 0] << 8) + ret_data[begin + 1]);
	ret += str_data;

	// TCP Keep Alive Buffer Not Empty
	begin = ADDR_TCP_KEEP_ALIVE_BNE - ADDR_RESERVED;
	str_data.Format(_T("TCP Keep Alive (Buffer Not Empty) : %d msec\r\n"), (ret_data[begin + 0] << 8) + ret_data[begin + 1]);
	ret += str_data;

	// TCP Keep Alive Buffer Empty
	begin = ADDR_TCP_KEEP_ALIVE_BE - ADDR_RESERVED;
	str_data.Format(_T("TCP Keep Alive (Buffer Empty) : %d msec\r\n"), (ret_data[begin + 0] << 8) + ret_data[begin + 1]);
	ret += str_data;

	// TCP Timeout Connecting
	begin = ADDR_TCP_TIMEOUT_CONNECTING - ADDR_RESERVED;
	str_data.Format(_T("TCP Timeout Connecting : %d msec\r\n"), (ret_data[begin + 0] << 8) + ret_data[begin + 1]);
	ret += str_data;

	// TCP Timeout Disconnect
	begin = ADDR_TCP_TIMEOUT_DISCONNECT - ADDR_RESERVED;
	str_data.Format(_T("TCP Timeout Disconnect: %d msec\r\n"), ((ret_data[begin + 0] << 8) + ret_data[begin + 1]) * 256);
	ret += str_data;

	// TCP Reconnect Interval
	begin = ADDR_TCP_RECONNECT_INTERVAL - ADDR_RESERVED;
	str_data.Format(_T("TCP Recconect Interval : %d msec\r\n"), (ret_data[begin + 0] << 8) + ret_data[begin + 1]);
	ret += str_data;

	// TCP Retransmition timer
	begin = ADDR_TCP_RETRANSMITION_TIMER - ADDR_RESERVED;
	str_data.Format(_T("TCP Retransmition timer : %d msec\r\n"), (ret_data[begin + 0] << 8) + ret_data[begin + 1]);
	ret += str_data;

	// TCP KeepAlive Enabled
	begin = ADDR_SITCP_CONTROL - ADDR_RESERVED;
	if (ret_data[begin + 0] == 0) {
		ret += "TCP KeepAlive Enabled : disable\r\n";
	}
	else {
		ret += "TCP KeepAlive Enabled : eneble\r\n";
	}

	return ret;
}

void CSiDaqMfcSampleDlg::OnBnClickedButtonRbcpRead()
{
	auto ret = SiDaqRbcpReadBytes(m_pSiDaqMngr, 0xffffff00, 64);
	CString retString = CreateSiTCPInfo(ret);
	AddOpelog(retString);	
}


void CSiDaqMfcSampleDlg::OnBnClickedButtonFileBrowse()
{
	CFolderPickerDialog folderDialog;

	// Show dialog
	if (folderDialog.DoModal() == IDOK)
	{
		((CEdit*)GetDlgItem(IDC_STATIC_FILEPATH))->SetWindowTextW(CStringW(folderDialog.GetPathName()));
	}
}


void CSiDaqMfcSampleDlg::OnBnClickedButtonRbcpWrite()
{
	unsigned char buf[1] = { 0x80 };
	auto ret = SiDaqRbcpWriteBytes(m_pSiDaqMngr, &buf[0], 0xffffff10, 1);
	if (ret < 0) {
		AddOpelog("RBCP Write Failed");
	}
	else {
		AddOpelog("RBCP Write Success. SiTCP Reset");
	}
}


void CSiDaqMfcSampleDlg::OnBnClickedOk()
{
	OnAppExit();
	CDialogEx::OnOK();
}



void CSiDaqMfcSampleDlg::OnBnClickedButtonReset()
{
	AddOpelog("Reset Queue Data…");
	SiDaqResetQueue(m_pSiDaqMngr);
	AddOpelog("Reset Queue Data Finish");
}


void CSiDaqMfcSampleDlg::OnCbnSelchangeComboRunMode()
{
	int index = ((CComboBox*)GetDlgItem(IDC_COMBO_RUN_MODE))->GetCurSel();
	FrameControlInitialize(index);
}
