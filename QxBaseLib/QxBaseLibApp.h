// QxBaseLib.h : QxBaseLib DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQxBaseLibApp
// �йش���ʵ�ֵ���Ϣ������� QxBaseLib.cpp
//

class CQxBaseLibApp : public CWinApp
{
public:
	CQxBaseLibApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
