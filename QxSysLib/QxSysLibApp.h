// QxSysLib.h : QxSysLib DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQxSysLibApp
// �йش���ʵ�ֵ���Ϣ������� QxSysLib.cpp
//

class CQxSysLibApp : public CWinApp
{
public:
	CQxSysLibApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
