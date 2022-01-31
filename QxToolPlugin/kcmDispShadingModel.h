//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_DISP_SHADING_MODEL_H_
#define _KCM_DISP_SHADING_MODEL_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kiSelSet;

class kcmDispShadingModel : public kiCommand
{
public:
	kcmDispShadingModel(void);
	virtual ~kcmDispShadingModel(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	virtual BOOL			SetModel(kiSelSet *pSelSet) = 0;

protected:
	int						m_nModel;
	kiInputEntityTool		*m_pInputEntityTool;
};

// �����ʾ
class kcmDispWireFrame : public kcmDispShadingModel{
public:
	kcmDispWireFrame();
	virtual ~kcmDispWireFrame();

protected:
	BOOL					SetModel(kiSelSet *pSelSet);
};

// ��Ⱦ��ʾ
class kcmDispShading : public kcmDispShadingModel{
public:
	kcmDispShading();
	virtual ~kcmDispShading();

protected:
	BOOL					SetModel(kiSelSet *pSelSet);
};

#endif