#pragma once

#include "kiCommand.h"

class kcmInputFaceNoLF : public kiCommand
{
public:
	kcmInputFaceNoLF();
	virtual ~kcmInputFaceNoLF();

	// ��ʼִ������.
	//ִ�кͽ����ӿ�
	virtual	 int				Execute();
	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL				CanFinish();//�����Ƿ�������

protected:
	TopoDS_Compound			MakeWireText(const char *lpszText,LOGFONT& lf,double dHeight,double dGap);
};

//
class kcmTestForDebug : public kiCommand
{
public:
	kcmTestForDebug();
	virtual ~kcmTestForDebug();

	// ��ʼִ������.
	//ִ�кͽ����ӿ�
	virtual	 int				Execute();
	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL				CanFinish();//�����Ƿ�������

protected:
	void						PntInSolidTest();
};