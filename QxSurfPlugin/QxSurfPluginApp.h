// QxSurfPlugin.h : QxSurfPlugin DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQxSurfPluginApp
// �йش���ʵ�ֵ���Ϣ������� QxSurfPlugin.cpp
//

class CQxSurfPluginApp : public CWinApp
{
public:
	CQxSurfPluginApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
