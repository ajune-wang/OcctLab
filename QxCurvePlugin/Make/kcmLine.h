//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_LINE_H_
#define _KCM_LINE_H_

#include <vector>
#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputPointTool;
class kcPreviewLine;
class kiOptionSet;

class kcmLine : public kiCommand
{
public:
	kcmLine(void);
	virtual ~kcmLine(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL				CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int				OnInputFinished(kiInputTool *pTool);

	// ��������ı�ʱ������øûص�����
	virtual  int				OnInputChanged(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL				CreateInputTools();
	virtual BOOL				DestroyInputTools();
	virtual BOOL				InitInputToolQueue();

	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);
	virtual int				OnRButtonUp(kuiMouseInput& mouseInput);

protected:
	//�������߶���
	BOOL						MakeLineEntities();

protected:
	kiOptionSet				m_optionSet;
	bool						_bMultiLine;//����ֱ��
	bool						_bMakeWire;//�������wire
	double					m_dLen;//���ȣ�double�����ʡ�

protected:
	kiInputPointTool			*_pInputStartPoint;
	kiInputPointTool			*_pInputEndPoint;
    kPoint3					_startPoint,
							_endPoint;
	std::vector<kPoint3>		_pointList;

protected:
	int						m_nState;//
	kcPreviewLine				*_pPreviewLine;
	std::vector<kcPreviewLine*>	_prevLineList;
};

#endif
