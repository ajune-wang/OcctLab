//////////////////////////////////////////////////////////////////////////
// ʰȡentity����
//
#ifndef _KI_INPUT_ENTITY_TOOL_H_
#define _KI_INPUT_ENTITY_TOOL_H_


#include "kiInputTool.h"
#include "kiSelSet.h"

class kiOptionEnum;

// ����ʰȡ����
// ʹ��7.6.0����ʰȡ���ƣ�����ʰȡ��Ȼ����;ֲ����󣬿���֧�ֶ���ģʽ�µ�ѡȡ
// �������kiInputLocalEntityTool�Ĺ���
//

class QXLIB_API QxShapePickInfo{
public:
	QxShapePickInfo();
	QxShapePickInfo(const QxShapePickInfo& other);

	QxShapePickInfo&	operator=(const QxShapePickInfo& other);

public:
	bool				_bValid;
	double				_t;//ʰȡ����
	double				_u, _v;
	kPoint3				_p;
};

class QXLIB_API kiInputEntityTool : public kiInputTool
{
public:
	kiInputEntityTool(kiCommand *pCommand,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);
	virtual ~kiInputEntityTool(void);

	//Ϊ�˼�ʵ�֣�������Щ���ã������ڹ��ߴ��ڳ�ʼ��״̬���ܹ��趨��
	//һ�����߿�ʼִ�У��ȴ����룬�Ͳ��ܸ�����

	//�趨Ҫѡ��Ķ�������
	//����������kcEntity.h�ж��壬���磺KCT_ENT_FACE | KCT_ENT_SHELL
	void						SetOption(int typeMask,bool bSingleSel = false);
	//�趨ѡ��������ͺͶ����������ѡ��ﵽ����ʱ�����Զ������������Ҽ�����ѡ��
	//numΪ-1,������ѡ��ĸ���
	void						SetOption(int typeMask,int num);
	//�趨Ϊѡ����Ȼ����ģʽ������֧�־ֲ�����ѡȡ
	void						SetNaturalMode(bool bEnable);
	//�趨Ϊ�����л�ģʽ�����Ƕ������Ϳ���ѡ���л�
	void						SetTypeSwitch(bool bEnable);
	
	// �趨ȫ�ֵĿɽ�������ѡ����Ӷ���ֻ���ڸö����ڲ�ѡȡ
	void						SetGlobalAISObject(const Handle(AIS_InteractiveObject) &aObj);

	//�������Ľӿ�
	bool						SetEntityTypes(int typeMask);
	bool						SetEntityLimit(int nLimit);//�趨ʰȡ�ĸ�������
	
	void						SetSingleSelect(bool bEnable);//��ѡ���ѡ
	// �Ƿ������������Ϣ,Ŀǰ��֧�ֵ�ѡ����£�edge�ĵ����
	void						NeedPickInfo(bool bNeeded); 

	//��Ҫ��������Ϣ��ͨ����Edge��Face����
	void						NeedPickedPoint(int typeMask);

	void						ActivateSelectionModes();//����ѡ��ģʽ
	void						DeactivateSelectionModes();//ȥ����ѡ��ģʽ

	//virtual int				Begin();
	//virtual int				End();

	void						SetHilightColor(const Quantity_NameOfColor hicol) { m_hiColor = hicol; }

	//���ȫ��ѡ���еĶ���
	virtual void				AddInitSelected(bool bAutoReturn);

protected:
	//
	virtual int				OnBegin();
	virtual int				OnEnd();

	void						DoActivateSelectionModes();
	void						DoActiveDefaultSelectionMode();

public:
	//��InputEdit�ı��������ʱ
	virtual int				OnEditInputFinished(const char *pszText);

public:
	// ��ѡ��ı��ĵ��ú���
	virtual void				OnOptionChanged(kiOptionItem *pOptionItem);

public:
	//�����Ϣ
	virtual void				OnLButtonDown(kuiMouseInput& mouseInput); 
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput);
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);
	virtual int				OnRButtonUp(kuiMouseInput& mouseInput);

public:
	//���ѡ����
	void						ClearSel();
	kiSelSet*				GetSelSet();
	int						GetSelCount();
	kiSelEntity				GetFirstSelect();
	TopoDS_Shape				SelectedShape(int ix);

	bool						GetShapePickInfo(QxShapePickInfo& info);

protected:
	//��ȡѡ����
	bool						GetSelected(kuiMouseInput& mouseInput);
	//����ѡ���������
	void						DoSplitShapeTypes();
	//���������л�ѡ��
	BOOL						CreateTypeSwitchOption();

	//
	BOOL						GetPickInfo(kuiMouseInput& mouseInput);

protected:
	kiSelSet					m_selSet;
	Quantity_NameOfColor		m_hiColor;
	Handle(AIS_InteractiveObject)	hGlobalAISObj_; // ������ڣ���ֻ��ѡ��ö����е��Ӷ���
	
	int						m_nTypeMask;
	int						m_nSelLimit;
	bool						m_bSingleSelect;//ֻѡ��һ��
	bool						m_bNaturalMode;//�Ƿ���Ȼѡ��ģʽ����ʱ����ѡ��ֲ�����
	bool						m_bNeedPickInfo;//�Ƿ���Ҫʰȡ��Ϣ�����������ȡ�

	int						m_nPickedPointTypeMask;//��Ҫʰȡ����Ϣ������
	QxShapePickInfo			m_aPickInfo; //���߻������ʰȡ��Ϣ

protected:
	bool						m_bTypeSwitch;//�Ƿ��������л�ģʽ
	int						m_nOptionIdx;//ѡ���±�
	TopAbs_ShapeEnum			m_aShapeTypes[8];//��������
	int						m_nShapeTypes;//���͸���
	kiOptionEnum				*pTypeOptEnum_; //������ʾ������͵��л�
};

#endif