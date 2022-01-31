//////////////////////////////////////////////////////////////////////////
//
#ifndef _KI_INPUT_LENGTH_H_
#define _KI_INPUT_LENGTH_H_

#include "kPoint.h"
#include "kVector.h"
#include "kiInputTool.h"

class kcPreviewLine;
class kcPreviewText;

class QXLIB_API kiInputLength : public kiInputTool
{
public:
	// ���������еĹ��캯����
	kiInputLength(kiCommand *pCommand,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);
	// ����InputTool�еĹ��캯����
	kiInputLength(kiInputTool *pParent,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);
	virtual ~kiInputLength(void);

	void						Reset();

	// bRealtimeUpdateΪtrueʱ�����Լ�ʱ��ȡ����ֵ��������Ԥ��
	// �����趨����ʱ��ȡ���ǻ�׼���ϵķ���
	void						Set(double basePnt[3],bool bRealtimeUpdate = false);
	void						Set(const  kPoint3& basePnt,bool bRealtimeUpdate = false);

	// �趨��׼��ͷ��򣬼���÷����ϵĳ��ȣ�����ʾԤ��
	void						Set(double basePnt[3],double baseDir[3],bool bRealtimeUpdate = false);
	void						Set(const kPoint3& aBasePnt,const kVector3& aBaseDir,bool bRealtimeUpdate = false);

	// ��baseDir���з���
	void						ReverseBaseDir();

	// ���س���ֵ�������Ǹ�ֵ.
	double					GetLength() const { return _dLength; };
	void						SetDefault(double dLen) { _dDefaultLength = _dLength = dLen; }
	kPoint3					GetEndPoint() const { return _endPoint; }

protected:
	//
	virtual int				OnBegin();
	virtual int				OnEnd();

protected:
	// ��ȡ��ʾ�ַ���
	virtual void				DoGetPromptText(std::string& str);
	// �����ı���ɣ�ͨ��������Խ�����
	virtual int				DoTextInputFinished(const char *pInputText);

public:
	//�����Ϣ
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput); 
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual int				OnRButtonUp(kuiMouseInput& mouseInput);

protected:
	//
	void						CalcCurrPoint(kuiMouseInput& mouseInput,kPoint3& kpnt);

protected:
	double					_dLength;//����
	double					_dDefaultLength;//
	kPoint3					_endPoint;
	double					_aBasePoint[3];//��׼��
	double					_aBaseDir[3];//��׼�����ش˷���ľ���
	bool						_bBaseDir;//�Ƿ��ػ�׼����
	bool						_bRealtimeUpdate;//�Ƿ�ʵʱ���¹���������ֵ��

protected:
	kcPreviewLine				*_pPreviewLine;
	kcPreviewText				*_pPreviewText;
};

#endif