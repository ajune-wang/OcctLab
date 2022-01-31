//////////////////////////////////////////////////////////////////////////
// ����һ����ֵ�Ĺ��ߣ��򵥵ķ�װ��
//
#ifndef _KI_INPUT_VALUE_TOOL_H_
#define _KI_INPUT_VALUE_TOOL_H_

#include "kiInputTool.h"

class kiCommand;
class kiOptionSet;

//����һ������
class QXLIB_API kiInputIntTool : public kiInputTool{
public:
	kiInputIntTool(kiCommand *pCommand,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);//��ѡ��
	// ����InputTool�еĹ��캯����
	kiInputIntTool(kiInputTool *pParent,const char *pPromptText,kiOptionSet *pOptionSet = NULL);
	virtual ~kiInputIntTool(void);

	bool						Init(int *pVal);
	bool						Init(int *pVal,int nMin,int nMax);

protected:
	//
	virtual int				OnBegin();

public:
	//��edit�����ı���ɺ󣬵��øú���,�������������ı���Ϣ
	virtual int				OnEditInputFinished(const char *pInputText);

protected:
	// ��ȡ��ʾ�ַ���
	virtual void				DoGetPromptText(std::string& str);

protected:
	int						*m_pIntVal;//����ָ��
	int						m_nMin,m_nMax;//���뷶Χ
};

//����һ��������
class QXLIB_API kiInputDoubleTool : public kiInputTool{
public:
	kiInputDoubleTool(kiCommand *pCommand,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);//��ѡ��
	// ����InputTool�еĹ��캯����
	kiInputDoubleTool(kiInputTool *pParent,const char *pPromptText,kiOptionSet *pOptionSet = NULL);
	virtual ~kiInputDoubleTool(void);

	bool						Init(double *pVal);
	bool						Init(double *pVal,double dMin,double dMax);

protected:
	//
	virtual int				OnBegin();

public:
	//��edit�����ı���ɺ󣬵��øú���,�������������ı���Ϣ
	virtual int				OnEditInputFinished(const char *pInputText);

protected:
	// ��ȡ��ʾ�ַ���
	virtual void				DoGetPromptText(std::string& str);

protected:
	double					*m_pDoubleVal;
	double					m_dMin,m_dMax;
};

#endif