//
// SiDaqMfcSample.h
// ~~~~~~~~~~~~~~  
// 
// Copyright (C) 2024  Bee Beans Technologies Co.,Ltd.
//
// Released under the MIT License.
// (See accompanying file LICENSE)
//

// main header file for the PROJECT_NAME application

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSiDaqMfcSampleApp:
// See SiDaqMfcSample.cpp for the implementation of this class
//

class CSiDaqMfcSampleApp : public CWinApp
{
public:
	CSiDaqMfcSampleApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSiDaqMfcSampleApp theApp;
