#include "stdafx.h"
#include "kiCommand.h"
#include "kiInputOptionsTool.h"


kiInputOptionsTool::kiInputOptionsTool(kiCommand *pCommand,const char *pPromptText,kiOptionSet *pOptionSet)
	:kiInputTool(pCommand,pPromptText,pOptionSet)
{
	bAcceptDefaultValue_ = false;
	bOptionChanged_ = false;
}

kiInputOptionsTool::kiInputOptionsTool(kiInputTool *pParent,const char *pPromptText,kiOptionSet *pOptionSet)
	:kiInputTool(pParent,pPromptText,pOptionSet)
{
	bAcceptDefaultValue_ = false;
	bOptionChanged_ = false;
}

kiInputOptionsTool::~kiInputOptionsTool(void)
{
}

void  kiInputOptionsTool::AcceptDefaultValue(bool bAccept)
{
	bAcceptDefaultValue_ = bAccept;
}

// ���»س���ո��ʱ����Ӧ������
int  kiInputOptionsTool::OnEditInputFinished(const char *pInputText)
{
	//���û��ѡ��ı䣬����ȫ��ʹ��Ĭ��ֵ
	if(bAcceptDefaultValue_){
		SetInputState(KINPUT_DONE);
	}else{
		//ֻ���޸���ѡ�����������ɹ�
		if(bOptionChanged_){
			SetInputState(KINPUT_DONE);
		}
	}
	//����������Ӧ����
	if(m_pCommand){
		m_pCommand->OnInputFinished(this);
	}
	//֪ͨ������
	if(m_pParent){
		m_pParent->OnSubInputFinished(this);
	}

	return kiInputTool::OnEditInputFinished(pInputText);
}

void  kiInputOptionsTool::OnOptionChanged(kiOptionItem *pOptionItem)
{
	bOptionChanged_ = true;

	//֪ͨ����
	if(m_pCommand){
		m_pCommand->OnOptionChanged(this,pOptionItem);
	}
	//
	if(m_pParent){
		m_pParent->OnSubInputFinished(this);
	}

}

//����ѡ�ѡ��ʱ�����øú���
void  kiInputOptionsTool::OnOptionSelected(kiOptionItem *pOptionItem)
{
	if(m_pCommand){
		m_pCommand->OnOptionSelected(pOptionItem);
	}
}

int  kiInputOptionsTool::OnKeyDown(kuiKeyInput& keyInput)
{
	return kiInputTool::OnKeyDown(keyInput);
}

int  kiInputOptionsTool::OnBegin()
{
	bOptionChanged_ = false;

	return kiInputTool::OnBegin();
}