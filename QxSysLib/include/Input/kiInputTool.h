//////////////////////////////////////////////////////////////////////////
// 
#ifndef _KI_INPUT_TOOL_H_
#define _KI_INPUT_TOOL_H_

#include <string>
#include <list>
#include "kBase.h"
#include "kPoint.h"
#include "kVector.h"
#include "QxLibDef.h"
#include "kiOptionSet.h"
#include "kiInputEdit.h"

//����״̬����������ɹ���ȡ����û�����롣
//û������������������������Ҫtool�ڲ�����

#define KINPUT_INITIAL		0  //��ʼ��״̬
#define KINPUT_RUNNING		1  //���빤�����У��ȴ�����
#define KINPUT_DONE		2  //�������
#define KINPUT_CANCEL		3  //����ȡ��

class kiCommand;
class kcModel;
class kuiMouseInput;
class kuiKeyInput;
class kiOptionItem;
class kiOptionSet;
class CXInputEdit;
class kcSnapMngr;
class kcBasePlane;
class kcDocContext;

// InputTool����ֱ��ʹ����kiCommand�У�������������ص���Ӧcommand��OnInputFinished������
// InputTool����ʹ����kiInputTool�У���InputTool���Ա���InputTool����������������ö�Ӧ
//      ��OnSubInputfinished������
// NOTE:
// 1.  InputTool�����Command���������InputTool���԰�����InputTool����InputTool������InputTool
//     �����kiCommand���������
// ���빤����Ҫ��ʾ��Ϣ��ѡ���б��ǿ�ѡ�
//
class QXLIB_API kiInputTool : public kiInputEditCallBack,public kiOptionCallBack
{
public:
	// ���������еĹ��캯����
	kiInputTool(kiCommand *pCommand,const char *pPromptText,kiOptionSet *pOptionSet = NULL);
	// ����InputTool�еĹ��캯����
	kiInputTool(kiInputTool *pParent,const char *pPromptText,kiOptionSet *pOptionSet = NULL);
	virtual ~kiInputTool(void);

	// ��ʼ�����ִ��,����KSTA_CONTINUE�ȡ�
	// ���ΪKSTA_��ʼ�ĺꡣ����ĺ������ؽ����ͬ��
	virtual int				Begin();
	// ���������ִ��,nCode������Ľ��.
	virtual int				End();

	//״̬����
	virtual bool			IsInputRunning() const;
	virtual bool			IsInputDone() const;
	int						GetInputState() const;
	void					SetInputState(int nState);

	// ��tool�Ļص�����,����tool���ý���ʱ,����.
	virtual int				OnSubInputFinished(kiInputTool *pTool);

	Handle(AIS_InteractiveContext)	GetAISContext() const;
	kcDocContext*			GetDocContext() const;

	kiInputEdit*			GetInputEdit() const;

	// ��ȡ�����Ӧ��V3d_View����
	const Handle(V3d_View)	GetCurrV3dView() const;

	// ��ȡ����ģ��
	kcModel*				GetModel() const;

	// ��ȡ��ǰ��׼��
	kcBasePlane*			GetCurrentBasePlane() const;

	kcSnapMngr*				GetSnapManager() const;

protected:// ��������ʹ��
	//
	kiOptionSet&			GetToolOptionSet() { return aToolOptionSet_; }
	// ��ȡ��ǰʹ�õ�set���󣬿����Ǵ����set�������������set
	kiOptionSet*			GetCurOptionSet() const;
	// ��ȡ�Ӳ��������ѡ�
	kiOptionSet*			GetInOptionSet() const;

protected:
	//
	virtual int				OnBegin();
	virtual int				OnEnd();

	// ��ȡ��ʾ�ַ���
	virtual void			DoGetPromptText(std::string& str);
	// �����ı���ɣ�ͨ��������Խ�����
	virtual int				DoTextInputFinished(const char *pInputText);

protected:
	// ��ѡ��ı��ĵ��ú���
	virtual void			OnToolOptionChanged(kiOptionItem *pOptionItem);
	virtual void			OnToolOptionClicked(kiOptionItem *pOptionItem);

public://OptionCallBack
	virtual void			OnOptionChanged(kiOptionItem *pOptionItem);
	virtual void			OnOptionSelected(kiOptionItem *pOptionItem);

public://kiInputEditCallBack�ӿ�ʵ��
	//InputEdit����Ļص�����

	//��InputEdit�����ı��ı�ʱ
	virtual int				OnEditInputChanged(const char *pszText);

	//��edit�����ı���ɺ󣬵��øú���,�������������ı���Ϣ
	virtual int				OnEditInputFinished(const char *pInputText);

	//��һ��ѡ�ѡ��ʱ�����磺ͨ������ַ�����
	virtual int				OnEditOptionSelected(int idx,kiOptionItem *pItem);
	
protected:
	// �������
	virtual int				DoNotify();
	// ѡ��ı�ͨ��
	virtual int				DoNotifyOptionChanged(kiOptionItem *pOptionItem);
	virtual int				DoNotifyOptionClicked(kiOptionItem *pOptionItem);

public:
	//�����Ϣ
	virtual void			OnLButtonDown(kuiMouseInput& mouseInput);
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);
	virtual int				OnLButtonDblClk(kuiMouseInput& mouseInput);
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual void			OnRButtonDown(kuiMouseInput& mouseInput);
	virtual int				OnRButtonUp(kuiMouseInput& mouseInput);

protected:
	//���㵱ǰ��
	virtual void			CalcInputPoint(kuiMouseInput& mouseInput,double point[3]);

public:
	//������Ϣ
	virtual int				OnKeyDown(kuiKeyInput& keyInput);
	virtual int				OnKeyUp(kuiKeyInput& keyInput);
	virtual int				OnChar(kuiKeyInput& keyInput);

protected:
	void					DoInitialize();//��Ҫ�ĳ�ʼ��
	void					Reset();

	virtual void			CombineOptionSet(kiOptionSet *pOptSet);
	virtual void			SplitOptionSet(kiOptionSet *pOptSet);

protected:
	//����Ĳ���
	kiCommand				*m_pCommand;//����������
	kiInputTool				*m_pParent;//������tool.
	std::string				m_szPromptText; //�����е�������Ϣ
	
	//�ڲ���ȡ�Ĳ���
	kiInputEdit				*m_pInputEdit;
	kcSnapMngr				*m_pSnapMgr;//ʰȡ
	
	//��ǰ���е��ӹ���,���԰��������빤��.
	kiInputTool				*m_pCurSubTool;

private:
	// ÿ��InputTool���Լ���ѡ����ڲ����ɣ������ҹ��߿��Դ����丸�ࣨCommand or InputTool��
	// ��ѡ�񼯡����գ�����ѡ���ϲ���һ��ѡ������ʾ������toolOptionSet��.
	int						m_nInputState;//������״̬��

	// ���������ѡ����󣬹����ڲ��Դ��ģ����ⲿ�����
	// �������������ѡ����Լ��뵽�����Դ���ѡ�������ϲ��ⲿ�����ѡ�
	// ����ֱ��ʹ���ⲿ�����ѡ�
	// pCurOptionSet_ʼ��ָ��ǰʹ�õ�ѡ�������������֮һ��Ҳ����Ϊ��

	//���ߵ��ܵ�ѡ�,����ʹ�����ѡ�������ѡ����Բ��뵽��ѡ��С�
	kiOptionSet				aToolOptionSet_;
	// �Ƿ��乤���Ƿ����Լ���ѡ��
	bool					bHasOwnOptions_;
	//���캯�������ѡ��
	kiOptionSet				*pInOptionSet_; 
	
	//��ǰ��ѡ��ϣ���һ��ָ�룬����ָ��ͬ�Ķ���
	kiOptionSet				*pCurOptionSet_;
	//����ʱ����
	kiOptionItem			*pCurrentOption_;//��ǰ��item��

	Handle(AIS_InteractiveContext) hAISCtx_;//��������
};

inline bool  kiInputTool::IsInputRunning() const
{
	return (m_nInputState == KINPUT_RUNNING) ? true : false;
}

inline bool	  kiInputTool::IsInputDone() const
{
	return (m_nInputState == KINPUT_DONE) ? true : false;
}


#endif
