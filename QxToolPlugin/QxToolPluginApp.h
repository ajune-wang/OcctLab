// QxToolPlugin.h : QxToolPlugin DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQxToolPluginApp
// �йش���ʵ�ֵ���Ϣ������� QxToolPlugin.cpp
//

class CQxToolPluginApp : public CWinApp
{
public:
	CQxToolPluginApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
