//////////////////////////////////////////////////////////////////////////
// ����ӿ���.
#ifndef _KI_COMMAND_H_
#define _KI_COMMAND_H_

#include <string>
#include <list>
#include <map>
#include <AIS_InteractiveContext.hxx>

#include "kBase.h"
#include "kPoint.h"
#include "kVector.h"
#include "QxLibDef.h"
#include "kiInputEdit.h"
#include "kiOptionSet.h"

class kuiMouseInput;
class kuiKeyInput;
class kcModel;
class kcEntity;
class kiInputTool;
class kiInputEdit;
class kcSnapMngr;
class kiSelSet;
class kvCoordSystem;
class kcBasePlane;
class kcDocContext;
class kiInputOptionsTool;
class QxCmdTimer;
typedef std::list<kiInputTool *>	TInputToolList;
typedef std::map<int,UINT_PTR> stdTimerIDMap;

class kiCommandMgr;

//����ִ�е�״̬,��������״̬,�����״̬��
//����������ͨ��״̬����Ҫ���ڱ�ʾ�����ɹ���ʧ��
#define		KSTA_ERROR			-1
#define		KSTA_SUCCESS			0

//����������״̬�������������̻��������̻�����������
//
#define		KSTA_CONTINUE			0 //���̼���ִ��״̬
#define		KSTA_DONE			1 //���״̬
#define		KSTA_CANCEL			2 //ȡ��״̬
#define		KSTA_FAILED			3 //ִ��ʧ��

// ����Ļ��ࡣ
// ��InputEditCallBack�����������InputEdit���н�������������Ӧ
//
class QXLIB_API kiCommand : public kiInputEditCallBack,public kiOptionCallBack
{
public:
	kiCommand(void);
	virtual ~kiCommand(void);

	// ִ��һ�γ�ʼ�����ͷŵĵط���
	virtual  int				Initialize(int nCmdID,kcModel *pModel,kiCommandMgr *pCmdMgr);
	virtual  int				Destroy();

	// ��ʼִ������.
	//ִ�кͽ����ӿ�
	virtual	 int				Execute();
	// ��������ִ��.ͨ�����ⲿ����,command�ڲ���Ҫ���øú���.
	// ����������������������,Ҳ������;�ж�����ʱ������.
	// ��Ҫ������������Դ���ͷŵ�,��֤����ص�ִ��ǰ�ĳ�ʼ״̬.
	//
	virtual  int				End(int nCode);
	// Ӧ������,Ĭ�����Ҽ�������øú���.
	// �������KSTA_DONE,������������,�������ִ��.
	//
	virtual  int				Apply();//ʵ��Ӧ��

	// ��������������,��Ҫ��Apply�������ʹ��.
	// ���������е���Ҽ�ʱ�����û�����빤�ߣ���������ݸú�������ֵ
	// ���в�ͬ����
	//   �������TRUE: �������Ե���Apply��ִ�й��ܣ�����������
	//   �������FALSE���򲻻����Apply��ֱ�ӽ���������˳�.
	// ����ĺ������Ʋ�ǡ����CanApply���ܸ����ʡ�
	virtual  BOOL				CanFinish() { return FALSE; }//�����Ƿ�������

	// �����Ƿ�������״̬
	virtual  BOOL				IsRunning() const { return m_bRunning; }	

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int				OnInputFinished(kiInputTool *pTool) { return KSTA_CONTINUE;}
	// ��������ı�ʱ������øûص�����
	virtual  int				OnInputChanged(kiInputTool *pTool) { return KSTA_CONTINUE;}

	// ��ѡ��ı��ĵ��ú���
	virtual void				OnOptionChanged(kiInputTool *pTool,kiOptionItem *pOptionItem) {} //ѡ��ı���
	virtual void				OnOptionClicked(kiInputTool *pTool,kiOptionItem *pOptionItem) {} //ѡ����

	// ������ǰ�����빤��
	virtual  int				EndCurrentInputTool();

public://InputEditCallBack
	//��InputEdit�����ı��ı�ʱ
	virtual int				OnEditInputChanged(const char *pszText);

	//��InputEdit�ı��������ʱ
	virtual int				OnEditInputFinished(const char *pszText);

	//��һ��ѡ�ѡ��ʱ�����磺ͨ������ַ������ͨ�����
	virtual int				OnEditOptionSelected(int idx,kiOptionItem *pItem);

public://IOptionCallBack
	//ѡ��ı���
	virtual void				OnOptionChanged(kiOptionItem *pOptItem);
	//ѡ�ѡ����
	virtual void				OnOptionSelected(kiOptionItem *pOptItem);

protected:
	// ����ѡ��button�����ʱ������ø���Ӧ����
	// nID: ��ѡ��button��Ӧ��IDֵ
	virtual void				OnOptionButtonClicked(int nID);
	
protected:
	//����ĳ�ʼ�����ͷŴ���,ÿ����������������Եĳ�ʼ�������ٲ�����
	virtual int				OnInitialize();
	virtual int				OnDestroy();

	// ִ�к���
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

	// �����Ӧ�ô�����
	virtual int				OnApply();

	// �����ȷ��������
	virtual int				OnOK();

protected:
	// ���nId��֤�����ڲ�Ψһ����
	bool						SetTimer(int nId,UINT uElapse);
	bool						KillTimer(int nId);
	bool						PauseTimer(int nId);
	bool						ResumeTimer(int nId);
	bool						HasTimer(int nId);

	// ��ʱ��������
	virtual int				OnTimer(int nId,DWORD dwTime);

	stdTimerIDMap				aTimerIdMap_;

protected:
	int						HandlerTimer(UINT_PTR nIDEvent,DWORD dwPassTime);
	friend class QxCmdTimer;

	UINT_PTR					GetTimerEventID(int nID);

protected:
	//the aux function used inside the command
	kVector3					GetWorkPlaneZDir();
	void						GetWorkPlaneCoordSystem(kvCoordSystem& cs);

public:
	UINT						GetID() const { return m_nID; }
	void						SetID(UINT nID) {  m_nID = nID; }
	const char*				GetName() const { return m_strName.c_str(); }
	const char*				GetAlias() const { return m_strAlias.c_str(); }

	kcModel*					GetModel() const { return pModel_;}
	kiCommandMgr*				GetCommandMgr() const { return pCmdMgr_; }
	Handle(AIS_InteractiveContext)	GetAISContext() const;
	kcDocContext*				GetDocContext() const;
	kiInputEdit*				GetInputEdit() const;
	const Handle(V3d_View)	GetCurrV3dView() const;
	CView*					GetActiveView() const;
	
	// ��ȡ��ǰ��׼��
	kcBasePlane*				GetCurrentBasePlane() const;

public:
	// ���ȫ��ѡ�񼯡�
	virtual bool				ClearGlobalSelSet();
	virtual bool				IsGlobalSelSetEmpty();
	virtual kiSelSet*			GetGlobalSelSet() const; 

protected:
	//����һ��InputEdit����
	virtual int				RequestEditInput(const char *pszPromptText,kiOptionSet *pOptionSet);
	// ����ǰ��������ѹջ
	virtual int				PushCurrentRequest();
	virtual int				PopRequest();
	//
	void						InitSet(const char *pszName,const char *pszAlias,const char *pszDesc);

protected:
	UINT						m_nID;//ID,ϵͳΨһ,���ⲿ����.
	std::string				m_strName;//��������
	std::string				m_strAlias;//����
	std::string				m_strDesc;//����

	kcSnapMngr				*m_pSnapMgr;//
	
	kiInputEdit				*m_pInputEdit;// ����༭��Ľӿ�ָ��
	BOOL						m_bRunning;//�Ƿ���������
	BOOL						_bInitialized;//�Ƿ��ʼ��
	int						nRequestDepth_;//�������

private:
	kcDocContext				*pDocContext_; //
	kcModel					*pModel_;
	kiCommandMgr				*pCmdMgr_;//���������
	kiSelSet					*pGlobalSelSet_;//ȫ��ѡ��ָ�롣

	//��������Doc��context����,�ɳ�ʼ��ʱ����
	Handle(AIS_InteractiveContext) hAISCtx_;

	kiOptionSet				*pMainButtonOptionSet_;  // ��Ҫ��ť��ѡ��
	kiInputOptionsTool		*pMainButtonOptionTool_; // ������ʾ��Ҫ��ť�����빤��

protected:
	// ���»���
	virtual void				Redraw();

	// ������ǰ����ִ�С��������ʹ�øú�����
	// ����ִ�н�������ֹ��ȡ����Ҫͳһ��cmdMgr��ִ�С�
	// �ú���ִ�к�,��ǰ�������ֹ,�������ڸú������ú�,ִ��
	// �κβ�����.�ú����ڲ���cmdMgr����End����,end������������
	// ��������.
	virtual int				EndCommand(int nCode);

	// ����ִ����ɡ����Խ����ˡ����ý���״̬.
	virtual int				Done();

	// ��ʾ��ʾ��Ϣ��������
	virtual void				PromptMessage(const char *lpszMsg);
	
protected:
	// ���㲶׽��
	virtual BOOL				CalcSnapPoint();

	// �����ʱ��׽��
	virtual bool				AddTempSnapPoint(double x,double y,double z);

	virtual void				ClearTempSnapPoint();

protected:
	// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
	virtual	BOOL				CreateInputTools();
	// ���ٴ��������빤��.ÿ�����������һ��.
	virtual BOOL				DestroyInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL				InitInputToolQueue();
	virtual void				ClearInputToolList();

	// ���һ������ť���빤�ߣ���Ϊ�˵�
	virtual void				AddMainButtonOptionTool(bool withApply=false);
	kiOptionSet*				GetMainButtonOptionSet() const;

	virtual int				AddInputTool(kiInputTool *pTool);

	// ִ��toollist����һ�����빤��.�ٶ�ǰ��������Ѿ��ɹ�.
	virtual int				NavToNextTool();
	// �����ض��Ĺ��߽���ִ��.���ڲ�����������ʱʹ��.
	virtual int				NavToInputTool(kiInputTool *pTool);

protected:
	TInputToolList			m_inputToolQueue;//����tool�б�
	kiInputTool			    *m_pCurInputTool;//��ǰ���빤��.

public:
	//�����Ϣ
	virtual void				OnLButtonDown(kuiMouseInput& mouseInput);
	// ���������������ʱ,���Է���KSTA_DONE,��ʾ�������.
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);
	virtual int				OnLButtonDblClk(kuiMouseInput& mouseInput);
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual void				OnRButtonDown(kuiMouseInput& mouseInput);
	// ���������������ʱ,���Է���KSTA_DONE,��ʾ�������.
	virtual int				OnRButtonUp(kuiMouseInput& mouseInput);

public:
	//������Ϣ
	virtual int				OnKeyDown(kuiKeyInput& keyInput);
	virtual int				OnKeyUp(kuiKeyInput& keyInput);
	virtual int				OnChar(kuiKeyInput& keyInput);

protected:
	//��������
	void						UAddToModel(const std::vector<kcEntity*> &aEntVec);
	//
	void						UAddToModel(kcEntity *pEntity,BOOL bUpdateView=FALSE);
	//
	void						UAddToModel(const TopoDS_Shape &aS,BOOL bUpdateView=FALSE);
};

#endif