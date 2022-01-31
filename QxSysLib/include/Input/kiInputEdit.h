//////////////////////////////////////////////////////////////////////////
// �򵥵�����ӿ���
#ifndef _KI_INPUT_EDIT_H_
#define _KI_INPUT_EDIT_H_

#include <string>
#include "QxLibDef.h"

class kiOptionItem;
class kiOptionSet;
class kiInputTool;
class kiInputEdit;

//����һ��kiInputEdit�Ļص��ӿڣ���֪ͨ��Ҫ����Ķ���
class kiInputEditCallBack{
public:
	//��InputEdit�����ı��ı�ʱ
	virtual int				OnEditInputChanged(const char *pszText) = 0;

	//��InputEdit�ı��������ʱ��ͨ���ǰ����˻س���ո����
	virtual int				OnEditInputFinished(const char *pszText) = 0;

	//��һ��ѡ�ѡ��ʱ�����磺ͨ������ַ������ͨ�����
	virtual int				OnEditOptionSelected(int idx,kiOptionItem *pItem) = 0;
};

class QXLIB_API kiInputEdit
{
public:
	kiInputEdit();

	//��Ҫ�ӿڣ�Ϊ���빤����ʾһ����ʾ��Ϣ�����Դ�ѡ���б�
	//InputToolָ�벻��ΪNULL.
	virtual int				RequestInput(const char *pPromptText,
										kiInputEditCallBack *pEditCallBack,
										kiOptionSet *pOptionSet = NULL) = 0;
	//��ʾһ����ʾ��Ϣ.
	virtual void				PromptText(const char *lpszText) = 0;

	//��������
	virtual void				Update() = 0;

	//��ȡ�����ı�
	virtual std::string		GetInputText() const = 0;		

	//���ڱ��浱ǰ��������ͻָ���������
	virtual int				PushRequest() = 0;
	virtual int				PopRequest() = 0;

protected:
	kiInputEditCallBack		*_pInputEditCallBack;//
	kiOptionSet				*m_pOptionSet;
};

#endif