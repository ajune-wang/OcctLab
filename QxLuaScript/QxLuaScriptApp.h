// QxLuaScript.h : QxLuaScript DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQxLuaScriptApp
// �йش���ʵ�ֵ���Ϣ������� QxLuaScript.cpp
//

class CQxLuaScriptApp : public CWinApp
{
public:
	CQxLuaScriptApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
