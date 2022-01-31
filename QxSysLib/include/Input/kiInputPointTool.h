//////////////////////////////////////////////////////////////////////////
//
#ifndef _KUI_INPUT_POINT_TOOL_H_
#define _KUI_INPUT_POINT_TOOL_H_

#include "kPoint.h"
#include "kVector.h"
#include "kiOptionSet.h"
#include "kiInputTool.h"


class kcPreviewText;

///����һ����Ĺ��ߣ����Ե�����룬�����ֶ�����
//
class QXLIB_API kiInputPointTool : public kiInputTool
{
public:
	kiInputPointTool(kiCommand *pCommand,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);
	kiInputPointTool(kiInputTool *pParent,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);
	virtual ~kiInputPointTool(void);

	//��ʼ��
	BOOL						Initialize(kPoint3 *pPoint,bool bSpacePoint = true);
	// �Ƿ���ֵʵʱ����
	void						SetRealtimeUpdate(bool bEnable);

protected:
	//
	virtual int				OnBegin();
	virtual int				OnEnd();


public://��ȡ���
	bool						IsSnapPoint() const { return _bSnapPoint; }
	kPoint3					GetPoint() const { return _point; }
	kVector3					GetViewDir() const { return _vdir; }

public:
	//�����Ϣ
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput); 
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual int				OnRButtonUp(kuiMouseInput& mouseInput);

protected:
	// ��ȡ��ʾ�ַ���
	virtual void				DoGetPromptText(std::string& str);

public:
	//��edit�����ı���ɺ󣬵��øú���,�������������ı���Ϣ
	virtual int				OnEditInputFinished(const char *pInputText);

protected:
	// ������ֵ
	void						CalcPoint(kuiMouseInput& mouseInput);

protected:
	kPoint3					*_pPoint;//��������
	kPoint3					_point;//�����
	kVector3					_vdir;//���ռ������ʱ,��������.
	bool						_bSnapPoint;//�Ƿ��Զ���׽��

protected://ѡ��
	bool						_bSpacePoint;//�Ƿ�ռ��
	bool						_bRealtimeUpdate;//�Ƿ�ʵʱ���¹���������ֵ��
};

#endif
