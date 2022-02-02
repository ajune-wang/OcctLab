#include "stdafx.h"
#include <IntAna_IntConicQuad.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include "kvGrid.h"
#include "kcModel.h"
#include "kcHandleMgr.h"
#include "QxStgBlock.h"
#include "kcBasePlane.h"


kcBasePlane::kcBasePlane(const Handle(V3d_View)& aView)
{
	_hView = aView;
	_pGrid = NULL;
	_bActive = false;//Ĭ�ϷǼ���

	_szName = "";
}


kcBasePlane::~kcBasePlane(void)
{
	Destroy();
}

//����һ������ϵ����
int  kcBasePlane::Create(kvCoordSystem& cs,kcModel *pModel,const char *pszName)
{
	ASSERT(pModel);
	_cs = cs;
	_pModel = pModel;

	//��ʼ������
	if(!InitGrid())
		return 0;
	
	kcHandleMgr *pHandleMgr = pModel->GetHandleMgr();
	_nHandle = pHandleMgr->AllocHandle(KCT_BASE_PLANE);

	if(pszName){
		_szName = pszName;
	}else{
		char szBuf[64];
		sprintf_s(szBuf,64,"��׼�� %d",_nHandle);
		_szName = szBuf;
	}

	return 1;
}

bool  kcBasePlane::InitGrid()
{
	ASSERT(_pModel);
	Handle(AIS_InteractiveContext) aCtx = _pModel->GetAISContext();
	if(aCtx.IsNull()){
		ASSERT(FALSE);
		return false;
	}
	_pGrid = new kvGrid(aCtx,this);
	if(!_pGrid)
		return false;

	_pGrid->SetWidth(200.0);
	_pGrid->SetHeight(200.0);
	_pGrid->SetCellSize(5.0);
	_pGrid->SetVisible(false);//Ĭ���ǲ��ɼ���

	return true;
}

void  kcBasePlane::Destroy()
{
	if(_pGrid){
		delete _pGrid;
		_pGrid = NULL;
	}
}

int  kcBasePlane::GetCoordSystem(kvCoordSystem& cs)
{
	cs = _cs;
	return 1;
}

const kVector3& kcBasePlane::GetZDirection() const
{
	return _cs.Z();
}

//�ж�һ���Ƿ��ٻ�׼����
bool kcBasePlane::IsOnPlane(const gp_Pnt &p)
{
	kPoint3 kp(p.X(), p.Y(), p.Z());
	_cs.WCSToUCS(kp);
	if (fabs(kp.z()) < K_DBL_EPS)
		return true;
	return false;
}

gp_Pln kcBasePlane::GetPlane()
{
	gp_Pln aPln;
	gp_Ax2 ax2;
	_cs.Get(ax2);
	aPln.SetPosition(gp_Ax3(ax2));

	return aPln;
}

// ��Ļ��������ת���ռ�����.
// x,y:��������,vp:�ռ�� vdir:�ռ�����.
//
bool kcBasePlane::PixelToModel(int x,int y,kPoint3& vp,kVector3& vdir)
{
	ASSERT(!_hView.IsNull());
	double vx,vy,vz,dx,dy,dz;
	_hView->ConvertWithProj(x,y,vx,vy,vz,dx,dy,dz);
	vp.set(vx,vy,vz);
	vdir.set(dx,dy,dz);
	vdir.normalize();

	return true;
}

// �ռ�㵽��Ļ�����ת��
// wp:�ռ��,wcs����ϵ��.x,y:���ص���Ļ����
//
bool kcBasePlane::ModelToPixel(const kPoint3& wp,int& x,int& y)
{
	ASSERT(!_hView.IsNull());
	_hView->Convert(wp.x(),wp.y(),wp.z(),x,y);
	return true;
}

// ��Ļ�������굽����ƽ���.
// x,y:��������,wp:����ƽ���ϵĵ�,wcs����ϵ��.
//
bool kcBasePlane::PixelToWorkPlane(int x,int y,kPoint3& wp)
{
	ASSERT(!_hView.IsNull());
	kPoint3 vp;
	kVector3 vdir,vNorm;
	PixelToModel(x,y,vp,vdir);

	//�͹���ƽ����
	const kPoint3& org = _cs.Org();
	vNorm = _cs.Z();
	
	return LinPlnIntersection(vp,vdir,org,vNorm,wp);
}

void  kcBasePlane::SetActive(bool bActive)//�Ƿ�����Ϊ��Ļ�׼��
{
	_bActive = bActive;
	if(_pGrid){
		_pGrid->SetVisible(bActive ? true : false);
	}

	if(_bActive){
		SetDefaultView();
	}
}

//�趨Ĭ��ʵ�壬Ŀǰ������ͼ��
void  kcBasePlane::SetDefaultView()
{
	//�����趨���þֲ�����ϵ��xyƽ����
	if(_bActive){
		Standard_Boolean bImmUpdate = _hView->SetImmediateUpdate(Standard_False);
		//
		kPoint3 org = _cs.Org();
		kVector3 Y = _cs.Y(),Z = _cs.Z();

		//����˳����ѡ���õģ����ܸı䣬��������ZXƽ����ZΪup������
		_hView->SetTwist(0.0);
		_hView->SetAt(org.x(),org.y(),org.z());
		_hView->SetProj(Z.x(),Z.y(),Z.z());
		_hView->SetUp(Y.x(),Y.y(),Y.z());

		_hView->FitAll();
		_hView->ZFitAll();

		_hView->SetImmediateUpdate(bImmUpdate);
		_hView->Update();
	}
}

void  kcBasePlane::SetName(const char *szName)
{
	ASSERT(szName);
	_szName = szName;
}

std::string  kcBasePlane::GetName() const
{
	return _szName;
}

bool  kcBasePlane::HasName() const
{
	return (_szName.empty() ? false : true);
}

// ��ȡ
BOOL  kcBasePlane::Read(CArchive& ar,kcModel *pModel)
{
	QxStgBlock blk,csblk;
	int nval = 0;

	if(!blk.Read(ar) || blk.GetType() != 0x11BB){
		ASSERT(FALSE);
		return FALSE;
	}

	if(!blk.GetValueByCode(1,_nHandle) || 
		!blk.GetValueByCode(2,_szName) ||
		!blk.GetValueByCode(3,nval))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if(!csblk.Read(ar) || !_cs.ParseStgBlock(csblk)){
		ASSERT(FALSE);
		return FALSE;
	}

	_pModel = pModel;
	_bActive = (nval > 0) ? true : false;
	if(!InitGrid())
		return FALSE;

	//��ʶhandle�Ѿ�ʹ����
	kcHandleMgr *pHandleMgr = pModel->GetHandleMgr();
	pHandleMgr->MaskHandle(KCT_BASE_PLANE,_nHandle);

	return TRUE;
}

// д��
BOOL  kcBasePlane::Write(CArchive& ar)
{
	QxStgBlock blk,csblk;
	int nval = _bActive ? 1 : 0;

	blk.Initialize(0x11BB);
	blk.AddCodeValue(1,_nHandle);
	blk.AddCodeValue(2,_szName.c_str());
	blk.AddCodeValue(3,nval);

	blk.Write(ar);

	_cs.BuildStgBlock(csblk);
	csblk.Write(ar);
	
	return TRUE;
}

// ����ֱ�ߺ�ƽ��Ľ���
bool kcBasePlane::LinPlnIntersection(const kPoint3& linpnt,const kVector3& lindir,
					 				    const kPoint3& plnpnt,const kVector3& plnnorm,kPoint3& intpt)
{
	gp_Lin  lin(gp_Pnt(linpnt.x(),linpnt.y(),linpnt.z()),gp_Dir(lindir.x(),lindir.y(),lindir.z()));
	gp_Pln	pln(gp_Pnt(plnpnt.x(),plnpnt.y(),plnpnt.z()),
				gp_Dir(plnnorm.x(),plnnorm.y(),plnnorm.z()));

	kVector3 ld = lindir,pn = plnnorm;
	ld.normalize();
	pn.normalize();
	double dot = ld * pn;
	if(fabs(dot) < 1e-8)//ֱ�ߺ�ƽ��ƽ����
		return false;

	IntAna_IntConicQuad icq;
	icq.Perform(lin,pln,0.0001);
	if(!icq.IsDone() || icq.NbPoints() < 1)
		return false;

	gp_Pnt pnt = icq.Point(1);
	intpt.set(pnt.X(),pnt.Y(),pnt.Z());

	return true;
}