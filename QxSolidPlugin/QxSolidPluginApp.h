// QxSolidPlugin.h : QxSolidPlugin DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQxSolidPluginApp
// �йش���ʵ�ֵ���Ϣ������� QxSolidPlugin.cpp
//

class CQxSolidPluginApp : public CWinApp
{
public:
	CQxSolidPluginApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
