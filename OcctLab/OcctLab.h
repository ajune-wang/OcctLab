// OcctTry.h : OcctTry Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
#endif

#include "resource.h"       // ������
#include <Standard_Macro.hxx>
#include <Graphic3d_GraphicDriver.hxx>

// COcctLabApp:
// �йش����ʵ�֣������ OcctTry.cpp
//

class QxPluginMgr;
class kiCommandMgr;
class kcHandleMgr;

class COcctLabApp : public CWinApp
{
public:
	COcctLabApp();

	Handle(Graphic3d_GraphicDriver)		GetGraphicDriver() const { return m_graphicDriver; }

	QxPluginMgr*				GetPluginMgr() const { return m_pPluginMgr; }


// ��д
public:
	virtual BOOL				InitInstance();
	virtual int				ExitInstance();
// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

protected:
	Handle(Graphic3d_GraphicDriver)	   m_graphicDriver;

protected:
	//��Щ���Ǻ��ĵ��޹ص���.
	QxPluginMgr				*m_pPluginMgr;//�������

	//���ز��,ע������.
	BOOL						LoadPlugin();

	//��̬���ز��
	BOOL						SearchAndLoadDynPlugin();
};

extern COcctLabApp theApp;
