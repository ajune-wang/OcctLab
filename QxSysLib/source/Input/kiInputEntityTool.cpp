#include "StdAfx.h"
#include "kiCommand.h"
#include "kcModel.h"
#include "kcEntity.h"
#include "kiInputPicker.h"
#include "QxFacePointPicker.h"
#include "kiInputEntityTool.h"

#pragma warning(suppress : 4996)

//Ϊ�����ת����������ʹ�õ�����
static struct stEntShapeType{
	int		_enttype;
	TopAbs_ShapeEnum  _shapetype;
}s_aTypePair[7] = {
 	{ KCT_ENT_POINT, TopAbs_VERTEX },
	{ KCT_ENT_EDGE, TopAbs_EDGE },
	{ KCT_ENT_WIRE,  TopAbs_WIRE },
	{ KCT_ENT_FACE,  TopAbs_FACE },
	{ KCT_ENT_SHELL, TopAbs_SHELL },
	{ KCT_ENT_SOLID, TopAbs_SOLID },
	{ KCT_ENT_COMPOUND, TopAbs_COMPOUND }
};

//////////////////////////////////////////////////////////////////////////
QxShapePickInfo::QxShapePickInfo()
{
	_bValid = false;
	_t = _u = _v = 0.0;
}

QxShapePickInfo::QxShapePickInfo(const QxShapePickInfo& other)
{
	*this = other;
}

QxShapePickInfo&		QxShapePickInfo::operator=(const QxShapePickInfo& other)
{
	_bValid = other._bValid;
	_t = other._t;
	_u = other._u; _v = other._v;
	_p = other._p;
	return *this;
}



kiInputEntityTool::kiInputEntityTool(kiCommand *pCommand,const char *pszPrompt,kiOptionSet *pOptionSet)
	:kiInputTool(pCommand,pszPrompt,pOptionSet),m_selSet(pCommand->GetModel())
{
	ASSERT(pCommand != NULL && pszPrompt != NULL);
	m_hiColor = Quantity_NOC_BLUE1;
	m_nTypeMask = 0;
	m_nSelLimit = 0;
	m_bSingleSelect = false;
	m_bNeedPickInfo = false;
	m_bNaturalMode = true;//Ĭ��Ϊ��Ȼ����ѡ�񣬲�֧���Ӷ���ѡ����Ҫ����
	m_bTypeSwitch = false;//Ĭ��ȫ������
	m_nPickedPointTypeMask = 0;

	m_nShapeTypes = 0;
	m_nOptionIdx = -1;
	pTypeOptEnum_ = NULL;
}

kiInputEntityTool::~kiInputEntityTool(void)
{
}

//���ö�Ӧѡ��
void  kiInputEntityTool::SetOption(int typeMask,bool bSingleSel)
{
	//��Ҫ�ڳ�ʼ��״̬���趨
	if(GetInputState() != KINPUT_INITIAL){
		ASSERT(FALSE);
		return;
	}
	m_nTypeMask = typeMask;
	m_bSingleSelect = bSingleSel;
	//
	DoSplitShapeTypes();
}

void  kiInputEntityTool::SetOption(int typeMask,int num)
{
	//��Ҫ�ڳ�ʼ��״̬���趨
	if(GetInputState() != KINPUT_INITIAL){
		ASSERT(FALSE);
		return;
	}

	m_nTypeMask = typeMask;
	m_nSelLimit = num;
	if(num == 1){
		m_bSingleSelect = true;
	}
	//
	DoSplitShapeTypes();
}

//����ѡ���������
void  kiInputEntityTool::DoSplitShapeTypes()
{
	m_nShapeTypes = 0;
	if(m_nTypeMask == 0)
		return;

	int ix = 0;
	for(ix = 0;ix < 7;ix ++){
		if(m_nTypeMask & s_aTypePair[ix]._enttype){
			m_aShapeTypes[m_nShapeTypes] = s_aTypePair[ix]._shapetype;
			m_nShapeTypes ++;
		}
	}
}

void kiInputEntityTool::NeedPickInfo(bool bNeeded)
{
	m_bNeedPickInfo = bNeeded;
}

//�趨Ϊѡ����Ȼ����ģʽ������֧�־ֲ�����ѡȡ
void  kiInputEntityTool::SetNaturalMode(bool bEnable)
{
	//��Ҫ�ڳ�ʼ��״̬���趨
	if(GetInputState() != KINPUT_INITIAL){
		ASSERT(FALSE);
		return;
	}
	m_bNaturalMode = bEnable;
}

//�趨Ϊ�����л�ģʽ�����Ƕ������Ϳ���ѡ���л�
void  kiInputEntityTool::SetTypeSwitch(bool bEnable)
{
	//��Ҫ�ڳ�ʼ��״̬���趨
	if(GetInputState() != KINPUT_INITIAL){
		ASSERT(FALSE);
		return;
	}
	m_bTypeSwitch = bEnable;
}

// �趨ȫ�ֵĿɽ�������ѡ����Ӷ���ֻ���ڸö����ڲ�ѡȡ
void  kiInputEntityTool::SetGlobalAISObject(const Handle(AIS_InteractiveObject) &aObj)
{
	hGlobalAISObj_ = aObj;
}

bool  kiInputEntityTool::SetEntityTypes(int typeMask)
{
	//��Ҫ�ڳ�ʼ��״̬���趨
	if(GetInputState() != KINPUT_INITIAL){
		ASSERT(FALSE);
		return false;
	}

	m_nTypeMask = typeMask;
	//
	DoSplitShapeTypes();
	return true;
}

//�趨ʰȡ�ĸ�������
bool  kiInputEntityTool::SetEntityLimit(int nLimit)
{
	//��Ҫ�ڳ�ʼ��״̬���趨
	if(GetInputState() != KINPUT_INITIAL){
		ASSERT(FALSE);
		return false;
	}
	m_nSelLimit = nLimit;
	return true;
}

//��ѡ���ѡ
void  kiInputEntityTool::SetSingleSelect(bool bEnable)
{
	//��Ҫ�ڳ�ʼ��״̬���趨
	if(GetInputState() != KINPUT_INITIAL){
		ASSERT(FALSE);
		return;
	}

	m_bSingleSelect = bEnable;
}

//��Ҫ��������Ϣ��ͨ����Edge��Face����
void  kiInputEntityTool::NeedPickedPoint(int typeMask)
{
	//��Ҫ�ڳ�ʼ��״̬���趨
	if(GetInputState() != KINPUT_INITIAL){
		ASSERT(FALSE);
		return;
	}

	m_nPickedPointTypeMask = typeMask;
}

//����ѡ��ģʽ,��Ҫ����ֲ������ѡ��ģʽ
void  kiInputEntityTool::ActivateSelectionModes()
{
	if(!m_bNaturalMode){
		DoActivateSelectionModes();
	}
}

//ȥ����ѡ��ģʽ,�ָ�����Ȼѡ��״̬
void  kiInputEntityTool::DeactivateSelectionModes()
{
	DoActiveDefaultSelectionMode();
}

int  kiInputEntityTool::OnBegin()
{
	//���ѡ����ȫ��ѡ���еĶ��󣬲���������Ҫ��
	if(IsInputDone()){
		if(m_pCommand){
			m_pCommand->OnInputFinished(this);
		}

		return KSTA_DONE;
	}

	//����ֲ�ѡ��ģʽ
	if(!m_bNaturalMode){
		DoActivateSelectionModes();
	}

	return KSTA_CONTINUE;
}

//���������л�ѡ��
BOOL  kiInputEntityTool::CreateTypeSwitchOption()
{
	if(!pTypeOptEnum_){	
		pTypeOptEnum_ = new kiOptionEnum("ʰȡ����",'T',m_nOptionIdx);
		int ix,entyp = 0,ic = 0;
		for(ix = 0;ix < m_nShapeTypes;ix ++){
			switch(m_aShapeTypes[ix]){
			case TopAbs_VERTEX:
				pTypeOptEnum_->AddEnumItem("Vertex",'V');
				break;
			case TopAbs_EDGE:
				pTypeOptEnum_->AddEnumItem("Edge",'E');
				break;
			case TopAbs_WIRE:
				pTypeOptEnum_->AddEnumItem("Wire",'W');
				break;
			case TopAbs_FACE:
				pTypeOptEnum_->AddEnumItem("Face",'F');
				break;
			case TopAbs_SOLID:
				pTypeOptEnum_->AddEnumItem("Solid",'S');
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}

		pTypeOptEnum_->SetOptionCallBack(this);
	}
	
	// ��ӵ���Ӧ��ѡ����
	kiOptionSet *pOptSet = GetInOptionSet();
	if(pOptSet){
		// ���뵽��һ��
		pOptSet->InsertOptionItem(0,pTypeOptEnum_);
	}else{
		//���뵽tool�Լ���ѡ�����
		GetToolOptionSet().InsertOptionItem(0,pTypeOptEnum_);
	}

	return TRUE;
}

// �����趨��ѡ��ģʽ,����ͬʱ�Ƕ��ֶ����ѡ��
// ע�⣺���ַ�ʽ�Ĵ���
void  kiInputEntityTool::DoActivateSelectionModes()
{
	ASSERT(!m_bNaturalMode);
	if(m_nTypeMask == 0)
		return;

	Handle(AIS_InteractiveContext) hAISCtx = GetAISContext();
	if(hAISCtx.IsNull()){
		ASSERT(FALSE);
		return;
	}

	if(m_nShapeTypes > 0){
		if(!m_bTypeSwitch){//�������л�����������
			int ix = 0;
			//AIS��ͬʱ�������ѡ��mode�ǿ��Եġ�
			hAISCtx->Deactivate();//ȥ����ɵ�
			for(ix = 0;ix < m_nShapeTypes;ix ++){
				hAISCtx->Activate(AIS_Shape::SelectionMode(m_aShapeTypes[ix]),false);
			}
		}else{//�����л�ģʽ
			CreateTypeSwitchOption();
			//TODO:�����ȼ򵥴�������д���ģ�Ӧ����ӵ�����Ĺ��ܼ���.
			m_nOptionIdx = 0;

			//����Ĭ��ģʽ
			hAISCtx->Deactivate();//ȥ����ɵ�
			hAISCtx->Activate(AIS_Shape::SelectionMode(m_aShapeTypes[m_nOptionIdx]),true);
		}
	}
}

//����Ĭ�ϵ�ѡ��ģʽ��������Ȼ���˶���ѡ��ģʽ����֧�־ֲ�����
//
void  kiInputEntityTool::DoActiveDefaultSelectionMode()
{
	if(!m_bNaturalMode){
		Handle(AIS_InteractiveContext) hAISCtx = GetAISContext();
		if(!hAISCtx.IsNull()){
			hAISCtx->Deactivate();
			hAISCtx->Activate(0,true);//Ĭ�Ϸ�ʽ
		}
	}
}

int  kiInputEntityTool::OnEnd()
{
	//ѡ�񼯲�����գ�������ܻ�Ҫʹ�á�
	//���ⲿ����ѡ���Ƿ���Ҫ��ա�
	DoActiveDefaultSelectionMode();	
	//
	// ��Ҫȥ����ӵ�����ѡ��
	if(pTypeOptEnum_){
		kiOptionSet *pOptSet = GetInOptionSet();
		if(pOptSet){
			pOptSet->RemoveOptionItem(pTypeOptEnum_);
		}else{
			GetToolOptionSet().RemoveOptionItem(pTypeOptEnum_);
		}
		delete pTypeOptEnum_;
		pTypeOptEnum_ = NULL;
	}
	
	return KSTA_DONE;
}

//���ȫ��ѡ���еĶ���
void  kiInputEntityTool::AddInitSelected(bool bAutoReturn)
{
	int entyp = 0;
	kiSelSet *pGlobSelSet = m_pCommand->GetGlobalSelSet();
	if(pGlobSelSet && !pGlobSelSet->IsEmpty()){
		if(m_bSingleSelect){//��ѡһ��
			if(pGlobSelSet->GetSelCount() == 1){
				kiSelEntity se = pGlobSelSet->GetFirstSelected();
				entyp = se._pEntity->GetType();
				if(entyp & m_nTypeMask){
					m_selSet.AddSelected(se);
				}
			}
		}else{
			//��ѡ
			int nSel = 0;
			for(pGlobSelSet->InitSelected();pGlobSelSet->MoreSelected();pGlobSelSet->NextSelected()){
				kiSelEntity se = pGlobSelSet->CurSelected();
				entyp = se._pEntity->GetType();
				if(entyp & m_nTypeMask){
					m_selSet.AddSelected(se);
					nSel ++;
					if(m_nSelLimit > 0 && nSel >= m_nSelLimit)
						break;
				}
			}
		}

		if(bAutoReturn){
			pGlobSelSet->UnHilight(FALSE);
			pGlobSelSet->Clear();

			// ��ʾ
			m_selSet.Hilight(m_hiColor,TRUE);
		}

		if(bAutoReturn){
			if((m_bSingleSelect && m_selSet.GetSelCount() == 1) ||
				(!m_bSingleSelect && m_nSelLimit > 0 && m_selSet.GetSelCount() >= m_nSelLimit))
			{
				SetInputState(KINPUT_DONE);
				// ���ﲻ֪ͨ���ⲿ��Ҫ�ж������Ƿ����
				//if(m_pCommand){
				//	m_pCommand->OnInputFinished(this);
				//}
			}
		}
	}
}

// ��ȡѡ����,ע�⣺���־ֲ�����
//
bool  kiInputEntityTool::GetSelected(kuiMouseInput& mouseInput)
{
	kcModel *pModel = GetModel();
	Handle(AIS_InteractiveContext) hAISCtx = GetAISContext();
	if(!pModel || hAISCtx.IsNull())
		return false;

	kcEntity *pEntity = NULL;
	int ityp = 0,nbSel = 0;
	Handle(AIS_InteractiveObject) hObj;
	Handle(AIS_Shape) aAIShape;
	Handle(SelectMgr_EntityOwner) aOwner;
	TopoDS_Shape aS,aSLoc;
	
	//ֻ��ѡ��һ��,���ѡ�񼯣�����ͻ᲻�Ͻ�ѡ��
	//������ӵ�ѡ����
	if(m_bSingleSelect){
		ClearSel();
	}

	//ʰȡ����
	//hAISCtx->Select(Standard_False);
	hAISCtx->SelectDetected();

	//��������ȡѡ�����
	hAISCtx->InitSelected();
	while(hAISCtx->MoreSelected()){
		// �ڷֽ�״̬�£����ﷵ�ص���������shape��Ӧ��ActiveObject��
		hObj = hAISCtx->SelectedInteractive();
		if(hObj.IsNull() || !hAISCtx->HasSelectedShape()){
			hAISCtx->NextSelected();
			continue;
		}

		// �Ӷ���ѡ��ģʽ�£����������ѡ���������Ҫ�����ж�
		if (!m_bNaturalMode && !hGlobalAISObj_.IsNull() &&
			hGlobalAISObj_ != hObj)
		{
			hAISCtx->NextSelected();
			continue;
		}
			
		aAIShape = Handle(AIS_Shape)::DownCast(hObj);
		aS = aAIShape->Shape();
		// �ڷֽ�ģʽ�£���¼��ͨ�����Ӷ���
		aSLoc = hAISCtx->SelectedShape();
		// 
		aOwner = hAISCtx->SelectedOwner();//���ǻ�ȡ�ö���
		//�ж��Ƿ�ֲ�����
		bool bLocShape = false;
		TopAbs_ShapeEnum aSType;
		if(aS.ShapeType() != aSLoc.ShapeType()){//�ֲ�����
			aSType = aSLoc.ShapeType();
			bLocShape = true;
			ASSERT(aOwner->ComesFromDecomposition());
		}else{
			aSType = aS.ShapeType();
		}
		
		ityp = ksEntityType(aSType);
		if(ityp & m_nTypeMask){//��������
			pEntity = pModel->GetEntity(hObj);
			//��ѡ�����
			kiSelEntity se(pEntity,hObj);
			se.SetEntityOwner(aOwner);
			if(bLocShape && !aSLoc.IsNull()){
				//�ֲ�����
				se.SetLocalShape(aSLoc);
			}
			m_selSet.AddSelected(se);//��ӵ�ѡ��
			nbSel ++;
			//��ѡ����
			if(m_bSingleSelect && nbSel == 1){
				// �����Ҫʰȡ����Ϣ�����м���
				if (m_bNeedPickInfo) {
					GetPickInfo(mouseInput);
				}

				SetInputState(KINPUT_DONE);
				break;
			}
			//���Ƹ�������
			if(m_nSelLimit > 0 && nbSel == m_nSelLimit){
				SetInputState(KINPUT_DONE);
				break;
			}
		}
		 
		hAISCtx->NextSelected();
	}

	return true;
}

// ��ȡedge��wire�ȵ���㴦����Ϣ,Ŀǰ��֧�ֵ�ѡ�����
BOOL kiInputEntityTool::GetPickInfo(kuiMouseInput& mouseInput)
{
	m_aPickInfo._bValid = false;
	if (m_selSet.IsEmpty())
		return FALSE;

	kiSelEntity se = GetFirstSelect();
	TopoDS_Shape aS = se.SelectShape();
	if (!aS.IsNull()) {
		// edge
		if (aS.ShapeType() == TopAbs_EDGE){
			TopoDS_Edge aEdge = TopoDS::Edge(aS);
			kiCurvePicker picker(GetDocContext());

			picker.Init(aEdge);
			if (picker.Pick(mouseInput.m_x, mouseInput.m_y, 4, m_aPickInfo._t, m_aPickInfo._p)){
				m_aPickInfo._bValid = true;
				return TRUE;
			}
		}else if(aS.ShapeType() == TopAbs_FACE){
			double pt[3], uv[2];
			TopoDS_Face aFace = TopoDS::Face(aS);
			QxFacePointPicker facePicker(aFace, this->GetCurrV3dView());

			if(facePicker.PickPoint(mouseInput.m_x, mouseInput.m_y, pt, uv)){
				m_aPickInfo._bValid = true;
				m_aPickInfo._p.set(pt);
				m_aPickInfo._u = uv[0];
				m_aPickInfo._v = uv[1];

				return TRUE;
			}
		}
	}
	
	return FALSE;
}

//�����Ϣ
void	kiInputEntityTool::OnLButtonDown(kuiMouseInput& mouseInput)
{
	
}

// ѡ��ѡ�еĶ���һ��һ��ֻ��ѡ��һ������ѡͨ��selset��ʵ�֡�
//
int  kiInputEntityTool::OnLButtonUp(kuiMouseInput& mouseInput)
{
	Handle(AIS_InteractiveContext) hAISCtx = GetAISContext();
	if(hAISCtx.IsNull())
		return KSTA_CONTINUE;

	// ����
	m_selSet.UnHilight(FALSE);
	// ��ȡѡ����
	GetSelected(mouseInput);
	// ��ʾ
	m_selSet.Hilight(m_hiColor,TRUE);

	if(IsInputDone()){
		if(m_pCommand){
			m_pCommand->OnInputFinished(this);
		}

		return KSTA_DONE;
	}else{
		if(m_pCommand){
			m_pCommand->OnInputChanged(this);
		}
	}

	return KSTA_CONTINUE;
}

// ���ո��س��������
int  kiInputEntityTool::OnEditInputFinished(const char *pszText)
{
	if(m_selSet.GetSelCount() > 0){
		SetInputState(KINPUT_DONE);
		if(m_pCommand){
			m_pCommand->OnInputFinished(this);
		}
		return KSTA_DONE;
	}

	return KSTA_CONTINUE;
}

int  kiInputEntityTool::OnMouseMove(kuiMouseInput& mouseInput)
{
	Handle(AIS_InteractiveContext) hAISCtx = GetAISContext();
	Handle(V3d_View) aView = GetCurrV3dView();
	//�������MoveTo����������ѡ�������
	if(!hAISCtx.IsNull() && !aView.IsNull()){
		hAISCtx->MoveTo(mouseInput.m_x,mouseInput.m_y,aView,Standard_True);
	}

	return KSTA_CONTINUE;
}

int  kiInputEntityTool::OnRButtonUp(kuiMouseInput& mouseInput)
{
	if(m_selSet.GetSelCount() > 0){
		SetInputState(KINPUT_DONE);
		if(m_pCommand){
			m_pCommand->OnInputFinished(this);
		}
		return KSTA_DONE;
	}else{
		SetInputState(KINPUT_CANCEL);
		if(m_pCommand){
			m_pCommand->OnInputFinished(this);
		}
		return KSTA_CANCEL;
	}
}

// ��ѡ��ı��ĵ��ú���
void  kiInputEntityTool::OnOptionChanged(kiOptionItem *pOptionItem)
{
	if(pOptionItem == NULL || pOptionItem->GetType() != KOPT_INPUT_ENUM)
		return;

	if(!m_bTypeSwitch)
		return;

	kiOptionEnum *pEnumItem = (kiOptionEnum *)pOptionItem;
	int preVal = pEnumItem->GetPrevEnumValue();
	//ע�⣺m_nOptionIdx����ѡ���л����Զ����µ�
	//�л�ģʽ
	Handle(AIS_InteractiveContext) hAISCtx = GetAISContext();
	if(!hAISCtx.IsNull()){
		if(preVal != m_nOptionIdx){//ȥ�����ģʽ
			hAISCtx->Deactivate(AIS_Shape::SelectionMode(m_aShapeTypes[preVal]));
		}

		//������ģʽ
		hAISCtx->Activate(AIS_Shape::SelectionMode(m_aShapeTypes[m_nOptionIdx]),false);
	}
}

//���ѡ����
void	kiInputEntityTool::ClearSel()
{
	if(!m_selSet.IsEmpty()){
		m_selSet.UnHilight(TRUE);
		m_selSet.Clear();
	}
}

kiSelSet*  kiInputEntityTool::GetSelSet()
{
	return &m_selSet;
}

int  kiInputEntityTool::GetSelCount()
{
	return m_selSet.GetSelCount();
}

kiSelEntity  kiInputEntityTool::GetFirstSelect()
{
	ASSERT(m_selSet.GetSelCount() > 0);
	kiSelEntity se;
	if(m_selSet.IsEmpty())
		return se;
	m_selSet.InitSelected();
	se = m_selSet.CurSelected();
	return se;
}

TopoDS_Shape kiInputEntityTool::SelectedShape(int ix)
{
	ASSERT(m_selSet.GetSelCount() > 0);
	TopoDS_Shape aS;
	int ic = 0;
	for (m_selSet.InitSelected(); m_selSet.MoreSelected(); m_selSet.NextSelected(), ic++) {
		if (ic == ix) {
			aS = m_selSet.CurSelected().SelectShape();
			break;
		}
	}
	return aS;
}

bool kiInputEntityTool::GetShapePickInfo(QxShapePickInfo& info)
{
	if (!m_aPickInfo._bValid)
		return false;

	info = m_aPickInfo;
	return true;
}