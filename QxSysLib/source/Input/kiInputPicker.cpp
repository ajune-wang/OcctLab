#include "StdAfx.h"
#include <algorithm>
#include "QxBaseUtils.h"
#include "kcDocContext.h"
#include "kcgIntersect.h"
#include "kcBasePlane.h"
#include "kiInputPicker.h"

//////////////////////////////////////////////////////////////////////////
//
kiCurvePicker::kiCurvePicker(kcDocContext *pDocCtx)
	:pDocContext_(pDocCtx)
{

}

bool	kiCurvePicker::Init(const TopoDS_Edge& aEdge)
{
	if(aEdge.IsNull()) return false;
	_aShape = aEdge;
	return true;
}

bool	kiCurvePicker::Init(const TopoDS_Wire& aWire)
{
	if(aWire.IsNull()) return false;
	_aShape = aWire;
	return true;
}

bool	kiCurvePicker::Pick(int x,int y,int itol,kPoint3& pp)
{
	TopoDS_Edge aPickEdge;
	double t;
	return Pick(x,y,itol,t,pp,aPickEdge);
}

bool	kiCurvePicker::Pick(int x,int y,int itol,double& t,kPoint3& pp)
{
	TopoDS_Edge aPickEdge;
	return Pick(x,y,itol,t,pp,aPickEdge);
}

static bool axIptComp(const kcxCurveIntpt& ipt1,const kcxCurveIntpt& ipt2)
{
	if(ipt1._dist < ipt2._dist)
		return true;
	return false;
}

//���Ƿ����������ϡ�itol�������ؼ���ľ��ȡ�
bool	kiCurvePicker::Pick(int x,int y,int itol,double& t,kPoint3& pp,TopoDS_Edge& aPickEdge)
{
	if(_aShape.IsNull())
		return false;

	if(itol <= 0) itol = 1;
    
	//��Ҫ�ļ���
	double dScale = pDocContext_->GetViewToWindowScale();//
	double dtol = itol * dScale;

	//������������
	kPoint3 vp;
	kVector3 vdir;
	kcBasePlane *pWorkPlane = pDocContext_->GetCurrentBasePlane();
	ASSERT(pWorkPlane);
	pWorkPlane->PixelToModel(x,y,vp,vdir);

	TopoDS_Edge aEdge;
	try{
		gp_Lin lin(KCG_GP_PNT(vp),KCG_GP_DIR(vdir));
		BRepBuilderAPI_MakeEdge me(lin);
		if(me.IsDone())
			aEdge = me.Edge();
	}catch(Standard_Failure){
		return false;
	}
	if(aEdge.IsNull())
		return false;

	int ix,nbIpt = 0;
	kcgCurveCurveInter ccInter;
	if(_aShape.ShapeType() == TopAbs_EDGE)
		nbIpt = ccInter.Intersect(aEdge,TopoDS::Edge(_aShape),dtol);
	else
		nbIpt = ccInter.Intersect(aEdge,TopoDS::Wire(_aShape),dtol);
	if(nbIpt == 0)
		return false;

	std::vector<kcxCurveIntpt> aIntpt;
	kcxCurveIntpt crvipt;
	for(ix = 0;ix < nbIpt;ix ++)
	{
		kcxCCIntpt ccipt = ccInter.InterPoint(ix);
		crvipt = ccipt._ipt2;//����Ŀ�������ϵ���Ϣ
		aIntpt.push_back(crvipt);
	}

	//���վ����С�����ҵ�������С��
	if(aIntpt.size() > 1)
	{
		std::sort(aIntpt.begin(),aIntpt.end(),axIptComp);
	}
	crvipt = aIntpt.at(0);
	t = crvipt._t;
	pp = crvipt._p;
	if(_aShape.ShapeType() == TopAbs_EDGE)
		aPickEdge = TopoDS::Edge(_aShape);
	else
	{
		TopoDS_Iterator ite(_aShape);
		int widx = 0;
		for(;ite.More();ite.Next())
		{
			TopoDS_Edge aE = TopoDS::Edge(ite.Value());
			if(widx == crvipt._widx)
			{
				aPickEdge = aE;
				break;
			}
			widx ++;
		}
	}

	return true;
}

