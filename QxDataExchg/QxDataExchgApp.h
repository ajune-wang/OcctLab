// QxDataExchg.h : QxDataExchg DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQxDataExchgApp
// �йش���ʵ�ֵ���Ϣ������� QxDataExchg.cpp
//

class CQxDataExchgApp : public CWinApp
{
public:
	CQxDataExchgApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
