// QxCurvePlugin.h : QxCurvePlugin DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CQxCurvePluginApp
// �йش���ʵ�ֵ���Ϣ������� QxCurvePlugin.cpp
//

class CQxCurvePluginApp : public CWinApp
{
public:
	CQxCurvePluginApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
