//////////////////////////////////////////////////////////////////////////
// ����Ƕȣ��ԽǶȻ򻡶ȱ�ʾ��
#ifndef _KI_INPUT_ANGLE_H_
#define _KI_INPUT_ANGLE_H_

#include "kiInputTool.h"

class kcPreviewLine;
class kcPreviewArc;
class kcPreviewText;
class kvCoordSystem;

class QXLIB_API kiInputAngle : public kiInputTool
{
public:
	// ���������еĹ��캯����
	kiInputAngle(kiCommand *pCommand,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);
	// ����InputTool�еĹ��캯����
	kiInputAngle(kiInputTool *pParent,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);
	virtual ~kiInputAngle(void);

	bool					Init(kvCoordSystem *pCS,double *pAngle,bool bRadian);

protected:
	//
	virtual int				OnBegin();
	virtual int				OnEnd();

	void					Reset();
	void					Clear();

	double					CalcAngle(kuiMouseInput& mouseInput);

protected:
	// ��ȡ��ʾ�ַ���
	virtual void			DoGetPromptText(std::string& str);
	// �����ı���ɣ�ͨ��������Խ�����
	virtual int				DoTextInputFinished(const char *pInputText);

public:
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual int				OnRButtonUp(kuiMouseInput& mouseInput);

protected:
	kvCoordSystem			*_pLocalCS;//ʹ��ԭ���z��x��Ϊ��ʼ����
	double					*_pAngle;
	bool					_bRadian;//�Ƿ񻡶�

protected:
	kcPreviewLine			*_pPreviewLine;
	kcPreviewLine			*_pPreviewLine2;
	kcPreviewArc			*_pPreviewArc;
	kcPreviewText			*_pPreviewText;
};

#endif