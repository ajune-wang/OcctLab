//////////////////////////////////////////////////////////////////////////
//
#ifndef _KUI_INPUT_POINT_H_
#define _KUI_INPUT_POINT_H_

#include "kPoint.h"
#include "kVector.h"
#include "kiOptionSet.h"
#include "kiInputTool.h"


// ʹ�ñȽ϶��һ����
class kcPreviewText;

class QXLIB_API kiInputPoint : public kiInputTool
{
public:
	kiInputPoint(kiCommand *pCommand,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);
	kiInputPoint(kiInputTool *pParent,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);
	virtual ~kiInputPoint(void);

	//��ʼ��
	BOOL						Init(kPoint3 *pPoint,bool bSpacePoint = true);
	// �Ƿ���ֵʵʱ����
	void						SetRealtimeUpdate(bool bEnable);
	void						SetShowText(bool bEnable);
	// �Ƿ�����ʰȡ�����е�vertex
	void						EnableSubShape(bool bEnable);

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
	// �����ı���ɣ�ͨ��������Խ�����
	virtual int				DoTextInputFinished(const char *pInputText);
	
protected:
	// ���ӵ��ֵ
	void						CalcPoint(kuiMouseInput& mouseInput);

protected:
	kPoint3					*_pPoint;//��������
	kPoint3					_point;//�����
	kVector3					_vdir;//���ռ������ʱ,��������.
	bool						_bSnapPoint;//�Ƿ��Զ���׽��

protected://ѡ��
	bool						_bSpacePoint;//�Ƿ�ռ��
	bool						_bRealtimeUpdate;//�Ƿ�ʵʱ���¹���������ֵ��
	bool						_bShowText;
	bool						bEnableSubShape_;//�Ƿ��ܹ�ʰȡ�����е�Vertex

	kcPreviewText				*_pPreviewText;
};

#endif
