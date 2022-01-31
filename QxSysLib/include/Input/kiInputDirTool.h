///����һ������
#ifndef _KI_INPUT_DIR_TOOL_H_
#define _KI_INPUT_DIR_TOOL_H_

#include "kPoint.h"
#include "kVector.h"
#include "kiInputTool.h"
#include "kiOptionSet.h"

class kiInputPoint;

class QXLIB_API kiInputDirTool : public kiInputTool
{
public:
	// ���������еĹ��캯����
	kiInputDirTool(kiCommand *pCommand,const char *pPromptText,kiOptionSet *pOptionSet = NULL);
	// ����InputTool�еĹ��캯����
	kiInputDirTool(kiInputTool *pParent,const char *pPromptText,kiOptionSet *pOptionSet = NULL);
	virtual ~kiInputDirTool(void);

	// ��ʼ��.bUseWCS:�Ƿ�ʹ��ȫ������ϵ��falseΪʹ�û�׼ƽ��ľֲ�����ϵ
	// bOnlyZDir:�Ƿ�ֻ����Z��
	int						Initialize(kVector3 *pDir,bool bUseWCS,bool bOnlyZDir,bool bUnited);

protected:
	// ��ѡ��ı��ĵ��ú���
	virtual void				OnToolOptionChanged(kiOptionItem *pOptionItem);
	virtual void				OnToolOptionClicked(kiOptionItem *pOptionItem);

public:
	virtual void				OnOptionChanged(kiOptionItem *pOptionItem);
	//��edit�����ı���ɺ󣬵��øú���,�������������ı���Ϣ
	virtual int				OnEditInputFinished(const char *pInputText);

protected:
	//
	virtual int				OnBegin();
	virtual int				OnEnd();

	// ��ȡ��ʾ�ַ���
	virtual void				DoGetPromptText(std::string& str);
	
protected:
	kVector3					*_pDir;
	bool						_bOnlyZDir;//�Ƿ�ֻ��Ҫ��׼Z��
	bool						_bUseWCS;//�Ƿ�ʹ��ȫ������ϵ��false:ʹ�û�׼������ϵ
	bool						_bUnited;
	int						_nStdAxis;//��׼����
};

#endif