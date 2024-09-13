
// SiDaqMfcSampleDlg.h : header file
//
#include "SiDaqApi.h"
#pragma once


struct SiDaqThreadParam {
	BOOL thread_continue;
	SiDaqMngr* sidaq_mngr_ptr;
	long long int receive_bytes;
	clock_t start;
	bool write_file_flg;
	bool force_exit_flg = true;
	CString write_file_path;
	HWND dlg_window;
};

#define SIDAQ_UPDATE_TIMER_ID 1 // set next timer.

#define SIDAQ_UPDATE_INTERVAL 500
#define SIDAQ_FILE_DIVIDE_SIZE  1000*1000*50 //(bytes)

typedef enum _GUIStatus {
	GUI_NO_CONNECTION = 0,
	GUI_IDLE = 1,
	GUI_RUNNING = 2,
	GUI_STOPPING = 3,

}GUIStatus;


// CSiDaqMfcSampleDlg dialog
class CSiDaqMfcSampleDlg : public CDialogEx
{
// Construction
public:
	CSiDaqMfcSampleDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIDAQMFCSAMPLE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	struct SiDaqThreadParam m_thread_param;
	CWinThread* m_winthread_ptr;
// Implementation
protected:
	HICON m_hIcon;
	SiDaqMngr* m_pSiDaqMngr;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam); //CLOSE

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

	UINT_PTR m_update_timer;
	GUIStatus CurrentGuiStatus;
	void UpdateGUIControlStatus(GUIStatus guistatus);
	void AddOpelog(const char*);
	void AddOpelog(CString);
	void FrameControlInitialize(int);

public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonRunStop();
	static int GetFrameLength(unsigned char*, UINT, UINT);
	afx_msg void OnAppExit();

	CComboBox m_combobox_run_mode;
	afx_msg void OnBnClickedButtonRbcpRead();
	afx_msg void OnBnClickedButtonFileBrowse();
	afx_msg void OnBnClickedButtonRbcpWrite();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnCbnSelchangeComboRunMode();
};
