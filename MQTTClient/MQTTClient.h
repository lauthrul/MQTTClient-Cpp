
// MQTTClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMQTTClientApp: 
// �йش����ʵ�֣������ MQTTClient.cpp
//

class CMQTTClientApp : public CWinApp
{
public:
	CMQTTClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMQTTClientApp theApp;