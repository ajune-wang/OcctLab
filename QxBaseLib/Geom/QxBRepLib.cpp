#include "StdAfx.h"
#include <vector>
#include <algorithm>
#include <TopExp.hxx>
#include <Precision.hxx>
#include <ShapeAlgo.hxx>
#include <BRepTools.hxx>
#include <Bnd_SeqOfBox.hxx>
#include <BRepBndLib.hxx>

#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GC_MakeSegment.hxx>
#include <BOPAlgo_Tools.hxx>
#include <TopExp_Explorer.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeExtend_Status.hxx>

#include "QxPrecision.h"
#include "QxDbgTool.h"
#include "kcgIntersect.h"
#include "QxBRepLib.h"

double  QxBRepLib::EdgeLength(const TopoDS_Edge& aEdge)//����edge�ĳ���
{
	double len = 0.0,dF,dL;
	Handle(Geom_Curve) aCurve;

	aCurve = BRep_Tool::Curve(aEdge,dF,dL);
	if(!aCurve.IsNull()){
		GeomAdaptor_Curve AC(aCurve);
		len = GCPnts_AbscissaPoint::Length(AC,dF,dL);
	}

	return len;
}

//����edge�ĳ���
double  QxBRepLib::EdgeLengthEx(const TopoDS_Edge& aEdge)
{
	GProp_GProps LProps;
	BRepGProp::LinearProperties(aEdge,LProps);
	return LProps.Mass();
}

//����Wire�ĳ���
double  QxBRepLib::WireLength(const TopoDS_Wire& aWire)
{
	GProp_GProps LProps;
	BRepGProp::LinearProperties(aWire,LProps);
	return LProps.Mass();
}

//���������߶�
TopoDS_Edge  QxBRepLib::MakeSegEdge(const gp_Pnt &p1,const gp_Pnt &p2)
{
	TopoDS_Edge aEdge;
	GC_MakeSegment mkSeg(p1,p2);
	if(mkSeg.IsDone()){
		Handle(Geom_Curve) aSeg = mkSeg.Value();
		aEdge = MakeEdge(aSeg);
	}

	return aEdge;
}

//����������Edge
TopoDS_Edge  QxBRepLib::MakeEdge(const Handle(Geom_Curve) &aCurve)
{
	TopoDS_Edge aEdge;
	if(!aCurve.IsNull()){
		try{
			BRepBuilderAPI_MakeEdge mkEdge(aCurve);
			if(mkEdge.IsDone()){
				aEdge = mkEdge.Edge();
			}
		}catch(Standard_Failure){
		}
	}
	return aEdge;
}

//����������Face
TopoDS_Face  QxBRepLib::MakeFace(const Handle(Geom_Surface) &aSurf)
{
	TopoDS_Face aFace;

	if(!aSurf.IsNull()){
		try{
			BRepBuilderAPI_MakeFace mf(aSurf,Precision::Confusion());
			if(mf.IsDone()){
				aFace = mf.Face();
			}
		}catch(Standard_Failure){
		}
	}

	return aFace;
}

// shape�Ŀ�������emptycopy��ͬ����ȿ�����
TopoDS_Edge  QxBRepLib::CopyEdge(const TopoDS_Edge& aEdge)
{
	TopoDS_Edge aCopyEdge;
	if(aEdge.IsNull())
		return aCopyEdge;

	try
	{
		BRepBuilderAPI_Copy cpy(aEdge);
		if(cpy.IsDone())
		{
			TopoDS_Shape aS = cpy.Shape();
			if(!aS.IsNull() &&
				aS.ShapeType() == TopAbs_EDGE)
			{
				aCopyEdge = TopoDS::Edge(aS);
			}
		}
	}
	catch (Standard_Failure){
		KTRACE("\n copy edge failed.");
	}

	return aCopyEdge;
}

TopoDS_Wire		QxBRepLib::CopyWire(const TopoDS_Wire& aWire)
{
	TopoDS_Wire aCopyWire;
	if(aWire.IsNull())
		return aCopyWire;

	try{
		BRepBuilderAPI_Copy cpy(aWire);
		if(cpy.IsDone())
		{
			TopoDS_Shape aS = cpy.Shape();
			if(!aS.IsNull() &&
				aS.ShapeType() == TopAbs_WIRE)
			{
				aCopyWire = TopoDS::Wire(aS);
			}
		}
	}
	catch(Standard_Failure){
		KTRACE("\n copy wire failed.");
	}

	return aCopyWire;
}

TopoDS_Face		QxBRepLib::CopyFace(const TopoDS_Face& aFace)
{
	TopoDS_Face aCopyFace;
	if(aFace.IsNull())
		return aCopyFace;

	try
	{
		BRepBuilderAPI_Copy cpy(aFace);
		if(cpy.IsDone())
		{
			TopoDS_Shape aS = cpy.Shape();
			if(!aS.IsNull() &&
				aS.ShapeType() == TopAbs_FACE)
			{
				aCopyFace = TopoDS::Face(aS);
			}
		}
	}
	catch (Standard_Failure){
		KTRACE("\n copy face failed.");
	}

	return aCopyFace;
}

//����һ��shape����ȿ���
//
TopoDS_Shape	QxBRepLib::CopyShape(const TopoDS_Shape& aShape)
{
	TopoDS_Shape aCopyShape;
	if(aShape.IsNull())
		return aCopyShape;

	try{
		BRepBuilderAPI_Copy cpy(aShape);
		if(cpy.IsDone()){
			aCopyShape = cpy.Shape();
		}
	}catch (Standard_Failure){
		KTRACE("\n copy shape failed.");
	}

	return aCopyShape;
}

// �ƶ�����
TopoDS_Shape	QxBRepLib::MoveShape(const TopoDS_Shape& aShape,const kVector3& vec,BOOL bCopy)
{
	TopoDS_Shape aMovShape;
	if(vec.length() < KDBL_MIN)
		aMovShape = aShape;
	else
	{
		gp_Trsf trsf;
		trsf.SetTranslation(gp_Vec(vec[0],vec[1],vec[2]));
		try
		{
			BRepBuilderAPI_Transform trans(aShape,trsf,bCopy ? true : false);
			if(trans.IsDone())
				aMovShape = trans.Shape();

		}catch (Standard_Failure){
		}
	}

	return aMovShape;
}

// ��edge����wire
TopoDS_Wire		QxBRepLib::EdgeToWire(const TopoDS_Edge& aEdge)
{
	TopoDS_Wire aWire;
	if(aEdge.IsNull())
		return aWire;

	try{
		BRepBuilderAPI_MakeWire mw(aEdge);
		if(mw.IsDone())
			aWire = mw.Wire();
	}catch(Standard_Failure){}

	return aWire;
}

// ��ת
// p:����,dir:��λ������ang����ת�Ƕȣ����ȡ�
TopoDS_Shape	QxBRepLib::RotateShape(const TopoDS_Shape& aShape,const kPoint3& p,const kVector3& dir,double ang)
{
	TopoDS_Shape aRotShape;
	gp_Ax1 ax1;
	gp_Trsf trsf;
	ax1.SetLocation(gp_Pnt(p[0],p[1],p[2]));
	ax1.SetDirection(gp_Dir(dir[0],dir[1],dir[2]));
	trsf.SetRotation(ax1,ang);

	try{
		BRepBuilderAPI_Transform trans(aShape,trsf,Standard_True);
		if(trans.IsDone())
			aRotShape = trans.Shape();

	}catch(Standard_Failure){

	}
	
	return aRotShape;
}

// ����
TopoDS_Shape	QxBRepLib::ScaleShape(const TopoDS_Shape& aShape,const kPoint3& p,double scale,BOOL bCopy)
{
	TopoDS_Shape aScaleShape;
	gp_Trsf trsf;
	trsf.SetScale(gp_Pnt(p[0],p[1],p[2]),scale);

	try{
		BRepBuilderAPI_Transform trans(aShape,trsf,bCopy ? true : false);
		if(trans.IsDone())
			aScaleShape = trans.Shape();

	}catch(Standard_Failure){
	}

	return aScaleShape;
}

//edge��wire�Ƿ�����
BOOL			QxBRepLib::IsConnected(const TopoDS_Wire& aW,const TopoDS_Edge& aE)
{
	if(aW.IsNull() || aE.IsNull())
		return FALSE;

	TopoDS_Vertex  v1,v2,v3,v4;
	TopExp::Vertices(aE,v1,v2);
	TopExp::Vertices(aW,v3,v4);
	gp_Pnt p1,p2,p3,p4;
	p1 = BRep_Tool::Pnt(v1);
	p2 = BRep_Tool::Pnt(v2);
	p3 = BRep_Tool::Pnt(v3);
	p4 = BRep_Tool::Pnt(v4);
	double dTol = Precision::Confusion();
	if(p1.Distance(p3) < dTol || p1.Distance(p4) < dTol ||
		p2.Distance(p3) < dTol || p2.Distance(p4) < dTol)
		return TRUE;

	return FALSE;
}

//ʹ��BRepTools::OuterWire�Ĵ��룬�����޸ġ�
TopoDS_Wire		QxBRepLib::OuterWire(const TopoDS_Face& aFace)
{
	TopoDS_Wire Wres;
	TopExp_Explorer expw (aFace,TopAbs_WIRE);
	BOOL bOverlap = FALSE;//�Ƿ��Χ���໥����

	if (expw.More()) 
	{
		Wres = TopoDS::Wire(expw.Current());
		expw.Next();
		if (expw.More()) 
		{
			Standard_Real UMin, UMax, VMin, VMax;
			Standard_Real umin, umax, vmin, vmax;
			BRepTools::UVBounds(aFace,Wres,UMin,UMax,VMin,VMax);
			while (expw.More()) 
			{
				const TopoDS_Wire& W = TopoDS::Wire(expw.Current());
				BRepTools::UVBounds(aFace,W,umin, umax, vmin, vmax);
				//���¼��жϷ�Χ���ܲ��У����磺һ�������wire��һ��С��wire����С����
				//������ʱ�����ڰ�Χ���㷨�����ܵ������ְ�Χ���ص������´����ѡ��
				//�����㷨�����ںõ������
				if ((umin <= UMin) &&
					(umax >= UMax) &&
					(vmin <= VMin) &&
					(vmax >= VMax)) 
				{//w��Χ�а�Χ�˼�¼�İ�Χ�С�
					Wres = W;
					UMin = umin;
					UMax = umax;
					VMin = vmin;
					VMax = vmax;
				}
				else
				{
					//�ж��Ƿ��ص�
					if(umin > UMax || umax < UMin || vmin > VMax || vmax < VMin)
					{//�������ص�
					}
					else
					{
						//���һ����������һ�������ڣ���d1,d2Ӧ����š�
						double Ud1 = UMax - umax,Ud2 = UMin - umin;
						double Vd1 = VMax - vmax,Vd2 = VMin - vmin;
						if((Ud1 * Ud2 >= 0.0) && (Vd1 * Vd2 > 0.0) ||
							(Ud1 * Ud2 > 0.0) && (Vd1 * Vd2 >= 0.0))
						{
							KTRACE("\n wire overlap [%f,%f]*[%f,%f] ps [%f,%f]*[%f,%f]",
								umin,umax,vmin,vmax,UMin,UMax,VMin,VMax);
							bOverlap = TRUE;
							break;
						}
					}
				}
				expw.Next();
			}
		}
	}
	if(!bOverlap)
	{
		return Wres;
	}

	//�ص�ʹ����һ���㷨
	TopoDS_Wire aOWire;
	try
	{
		aOWire = ShapeAlgo::AlgoContainer()->OuterWire(aFace);//Note:aFace �����쳣???
	}
	catch(Standard_Failure)
	{
		aOWire = Wres;//������ǰ�ġ�
	}

	return aOWire;
}


//����һϵ��wire,����ƽ��face.wire���ཻ,�����໥����.��Ҫ���ڴ���������
//����һϵ��ƽ��.
int		QxBRepLib::BuildPlnFace(TopTools_ListOfShape& aWList,TopTools_ListOfShape& aFList)
{
	int nFs = 0;

	//�ȸ��ݰ�Χ��,�ж��Ƿ��໥����.
	Bnd_SeqOfBox seqBox;
	Bnd_Box bb;
	TopTools_SequenceOfShape seqW,seqAlone;
	TopTools_ListIteratorOfListOfShape its(aWList);
	for(;its.More();its.Next())
	{
		TopoDS_Wire aW = TopoDS::Wire(its.Value());
		BRepBndLib::Add(aW,bb);
		seqBox.Append(bb);
		seqW.Append(aW);
	}

	TopTools_DataMapOfShapeListOfShape mSS;//��¼�˰�����ϵ,key��shape������list�е�shape.
	for(int ix = 1;ix <= seqW.Length();ix ++)
	{
		TopoDS_Wire aW1 = TopoDS::Wire(seqW.Value(ix));
		const Bnd_Box& bb1 = seqBox.Value(ix);

		BOOL bInter = FALSE;
		for(int jx = 1;jx <= seqW.Length();jx ++)
		{
			if(ix == jx)
				continue;

			TopoDS_Wire aW2 = TopoDS::Wire(seqW.Value(jx));
			const Bnd_Box& bb2 = seqBox.Value(jx);

			if(bb1.IsOut(bb2) && bb2.IsOut(bb1))
			{
				continue;
			}
			else
			{
				//�ཻ,�ж����ߵĹ�ϵ.��1��2,����2��1.ע��:��Χ���ཻ��������,��wire������ܲ��ཻ
				if(QxBRepLib::IsWire2InWire1(aW1,aW2))
				{
					if(!mSS.IsBound(aW1))
					{
						TopTools_ListOfShape aList;
						mSS.Bind(aW1,aList);
					}
					BOOL bF = FALSE;
					TopTools_ListIteratorOfListOfShape lit;
					for(lit.Initialize(mSS.Find(aW1));lit.More();lit.Next())
					{
						if(aW2.IsEqual(lit.Value()))
						{
							bF = TRUE;
							break;
						}
					}
					if(!bF)
						mSS.ChangeFind(aW1).Append(aW2);
					bInter = TRUE;
				}
				else if(QxBRepLib::IsWire2InWire1(aW2,aW1))
				{
					if(!mSS.IsBound(aW2))
					{
						TopTools_ListOfShape aList;
						mSS.Bind(aW2,aList);
					}
					BOOL bF = FALSE;
					TopTools_ListIteratorOfListOfShape lit;
					for(lit.Initialize(mSS.Find(aW2));lit.More();lit.Next())
					{
						if(aW1.IsEqual(lit.Value()))
						{
							bF = TRUE;
							break;
						}
					}
					if(!bF)
						mSS.ChangeFind(aW2).Append(aW1);
					bInter = TRUE;
				}
				else
				{
					continue;//��Χ���ཻ,ʵ�ʲ��ཻ
				}
			}
		}

		if(!bInter)//ʲô�����ཻ,������wire
		{
			seqAlone.Append(aW1);
		}
	}
	//���໥������wire,�ж���Ӧ��ϵ,����ƽ��
	TopTools_DataMapIteratorOfDataMapOfShapeListOfShape mapit;
	for(mapit.Initialize(mSS);mapit.More();mapit.Next())
	{
		TopoDS_Wire aOW = TopoDS::Wire(mapit.Key());
		const TopTools_ListOfShape& aList = mapit.Value();

		//����face
		TopoDS_Face aF = QxBRepLib::BuildPlaneFace(aOW,aList);
		if(!aF.IsNull())
		{
			aFList.Append(aF);
			nFs ++;
		}
	}

	//�Զ�����wire����ƽ��.
	for(int ix = 1;ix <= seqAlone.Length();ix ++)
	{
		TopoDS_Wire aW = TopoDS::Wire(seqAlone.Value(ix));
		BRepBuilderAPI_MakeFace MF(aW,Standard_True);
		if(MF.IsDone())
		{
			TopoDS_Face aNF = MF.Face();
			if(!aNF.IsNull())
			{
				aFList.Append(aNF);
				nFs ++;
			}
		}
	}

	return nFs;
}

//wire 1�Ƿ����wire2.
BOOL	QxBRepLib::IsWire2InWire1(const TopoDS_Wire& aW1,const TopoDS_Wire& aW2)
{
	BRepBuilderAPI_FindPlane fpln(aW1);
	if(!fpln.Found())
		return FALSE;

	gp_Pln aPln = fpln.Plane()->Pln();
	//����face
	BRepBuilderAPI_MakeFace mf(aPln,aW1);
	if(!mf.IsDone())
		return FALSE;

	TopoDS_Face aNF = mf.Face();

	//�ж�w2��һ�����Ƿ���mf��.
	BOOL bIN = FALSE;
	int nC = 0;
	BRepTopAdaptor_FClass2d fcls(aNF,Precision::PConfusion());

	TopExp_Explorer ex;
	for(ex.Init(aW2,TopAbs_VERTEX);ex.More();ex.Next())
	{
		TopoDS_Vertex aV = TopoDS::Vertex(ex.Current());
		gp_Pnt pnt = BRep_Tool::Pnt(aV);

		GeomAPI_ProjectPointOnSurf pp(pnt,fpln.Plane());
		if(pp.IsDone() && pp.NbPoints() > 0)
		{
			gp_Pnt2d uv;
			double u,v;
			pp.LowerDistanceParameters(u,v);
			uv.SetCoord(u,v);
			//��֤�Ƿ���aW1�ڲ�
			TopAbs_State sta = fcls.Perform(uv);
			if(sta == TopAbs_IN)
			{
				bIN = TRUE;
				break;
			}
		}
		else
		{
			nC ++;
			if(nC > 3)
				break;
		}
	}

	return bIN;
}

//��һ���⻷��һ���ڻ�����face
TopoDS_Face QxBRepLib::BuildPlaneFace(const TopoDS_Wire& aOW,const TopTools_ListOfShape& aIWList)
{

	TopoDS_Face aNF;

	try{
		//��ʹ���⻷����һ��ƽ��
		BRepBuilderAPI_MakeFace MF(aOW,Standard_True);
		if(MF.IsDone())
		{
			aNF = MF.Face();
			if(aNF.IsNull())
				return aNF;
		}

		//�������ζ��ڻ������ж�,��Ҫ��Ҫ��֤���պ�,���һ��ķ�����ȷ
		BRep_Builder B;
		TopTools_ListIteratorOfListOfShape lit;
		for(lit.Initialize(aIWList);lit.More();lit.Next())
		{
			TopoDS_Wire aW = TopoDS::Wire(lit.Value());

			if(aW.IsNull() || !BRep_Tool::IsClosed(aW))
			{
				KTRACE("\n inner wire make null or not closed.");
				continue;
			}

			//�ж��ڻ��ķ���
			TopoDS_Shape aTS = aNF.EmptyCopied();
			TopoDS_Face aTF = TopoDS::Face(aTS);
			aTF.Orientation(TopAbs_FORWARD);
			B.Add(aTF,aW);
			BRepTopAdaptor_FClass2d fcls(aTF,Precision::PConfusion());
			TopAbs_State sta = fcls.PerformInfinitePoint();
			if(sta == TopAbs_OUT)
				aW.Reverse();

			MF.Add(aW);
		}

		//��ȡ���յ�face
		aNF = MF.Face();

	}catch(Standard_Failure){
		return aNF;
	}

	return aNF;
}

// ��һϵ�����ߣ�����һ��wire
TopoDS_Wire QxBRepLib::BuildOneWire(const GeomCurveArray &aCrvAry)
{
	TopoDS_Wire aWire;
	if(aCrvAry.empty())
		return aWire;

	TopoDS_Compound aComp = QxBRepLib::BuildEdgeCompound(aCrvAry);
	if(aComp.IsNull())
		return aWire;

	TopoDS_Shape aS,aNW;
	int rc = BOPAlgo_Tools::EdgesToWires(aComp,aS,false);
	if(rc == 0){
		TopTools_IndexedMapOfShape aWMap;
		TopExp::MapShapes(aS,TopAbs_WIRE,aWMap);	
		if(aWMap.Size() != 1)
			return aWire;

		//��Ҫ�޸�һ�£��������������
		aNW = QxBRepLib::FixShape(aWMap(1));
		if(!aNW.IsNull()){
			aWire = TopoDS::Wire(aNW);
		}else{
			aWire = TopoDS::Wire(aWMap(1));
		}
		return aWire;
	}

	return aWire;
}

// ��һϵ�����ߣ�����һϵ��wire
TopoDS_Shape QxBRepLib::BuildWires(const GeomCurveArray &aCrvAry)
{
	TopoDS_Shape aShape;
	if(aCrvAry.empty())
		return aShape;

	TopoDS_Compound aComp = QxBRepLib::BuildEdgeCompound(aCrvAry);
	if(aComp.IsNull())
		return aShape;

	TopoDS_Shape aS,aW,aNW;
	int rc = BOPAlgo_Tools::EdgesToWires(aComp,aS,false);
	if(rc == 0){
		TopoDS_Compound aWComp;
		BRep_Builder aBB;
		TopExp_Explorer aExp;

		aBB.MakeCompound(aWComp);
		for(aExp.Init(aS,TopAbs_WIRE);aExp.More();aExp.Next()){
			aW = aExp.Current();
			//�޸�һ��
			aNW = QxBRepLib::FixShape(aW);
			if(!aNW.IsNull()){
				aBB.Add(aWComp,aNW);
			}else{
				aBB.Add(aWComp,aW);
			}
		}

		return aWComp;
	}
	return aShape;
}

// ��һϵ�����ߣ�����һ��edge����϶���
TopoDS_Compound QxBRepLib::BuildEdgeCompound(const GeomCurveArray &aCrvAry)
{
	BRep_Builder aBB;
	TopoDS_Compound aComp;

	if(aCrvAry.empty())
		return aComp;

	GeomCurveArray::size_type ix,isize = aCrvAry.size();
	TopoDS_Edge aEdge;

	aBB.MakeCompound(aComp);
	for(ix = 0;ix < isize;ix ++){
		aEdge = QxBRepLib::MakeEdge(aCrvAry[ix]);
		if(!aEdge.IsNull()){
			aBB.Add(aComp,aEdge);
		}
	}
	
	return aComp;
}

// �޸�shape���󣬷����޸���Ķ���
TopoDS_Shape QxBRepLib::FixShape(const TopoDS_Shape &aS)
{
	TopoDS_Shape aFS;
	if(aS.IsNull()) return aFS;

	Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape(aS);
	try{
		sfs->Perform();
	}catch(Standard_Failure){
		return aS;	
	}
	//
	aFS = aS;
	if(sfs->Status(ShapeExtend_DONE)){
		aFS = sfs->Shape();
	}
	return aFS;
}

//////////////////////////////////////////////////////////////////////////
//
//��һ��edge�ָ�Ϊ����
BOOL	kcgEdgeSplit::SplitEdge(const TopoDS_Edge& aEdge,double t,
					 		    TopoDS_Edge& aE1,TopoDS_Edge& aE2)
{
	if(aEdge.IsNull())
		return FALSE;

	double df,dl;
	Handle(Geom_Curve) aCur = BRep_Tool::Curve(aEdge,df,dl);
	if(aCur.IsNull())
		return FALSE;

	if(df > dl)
	{
		double tmp = df;dl = df;df = tmp;
	}
	double dtol = 1e-6;
	if(t - dtol < df || t + dtol > dl)
		return FALSE;

	//���������µ�edge
	try
	{
		TopAbs_Orientation ot = aEdge.Orientation();
		aE1 = BRepBuilderAPI_MakeEdge(aCur,df,t);
		aE2 = BRepBuilderAPI_MakeEdge(aCur,t,dl);
		if(aE1.IsNull() || aE2.IsNull())
			return FALSE;

		// ���ַ�����
		aE1.Orientation(ot);
		aE2.Orientation(ot);
		// ��֤edge��˳���edgeһ�¡���������Ĵ���
		if(ot == TopAbs_REVERSED)
		{
			TopoDS_Edge aTE = aE1;
			aE1 = aE2;
			aE2 = aTE;
		}
	}
	catch (Standard_Failure)
	{
		return FALSE;
	}

	return TRUE;
}

// ��һ��edge�ָ���һ��edge,��edge������list�У�������edge�ĸ���
//
int		kcgEdgeSplit::SplitEdgeByEdge(const TopoDS_Edge& aEdge,const TopoDS_Edge& aEdge2,
					 				  TopTools_SequenceOfShape& seqOfShape)
{
	int nC = 0;
	std::vector<double>  arpars;//��������б�

	//
	double df,dl;
	Handle(Geom_Curve) aCur = BRep_Tool::Curve(aEdge,df,dl);
	if(aCur.IsNull())
		return 0;

	if(df > dl)
	{
		double tmp = df;dl = df;df = tmp;
	}

	//��
	//�󽻵�
	kcgCurveCurveInter ccInter;
	if(!ccInter.Intersect(aEdge,aEdge2))
		return 0;

	nC = ccInter.NbInterPoints();
	if(nC == 0)
		return 0;

	arpars.push_back(df);
	kcxCCIntpt ipt;
	int ix,iC = 0;
	for(ix = 0;ix < nC;ix ++)
	{
		ipt = ccInter.InterPoint(ix);
		arpars.push_back(ipt._ipt1._t);
	}
	arpars.push_back(dl);

	//����,��С����
	std::sort(arpars.begin(),arpars.end());

	//Ϊÿ�Բ���������һ��edge
	TopAbs_Orientation ot = aEdge.Orientation();
	double t1,t2;
	TopoDS_Edge aNE;
	int isize = (int)arpars.size();
	nC = 0;
	for(ix = 0;ix < isize - 1;ix ++)
	{
		t1 = arpars.at(ix);
		t2 = arpars.at(ix + 1);
		if(fabs(t1 - t2) < K_PAR_TOL)
			continue;
		try
		{
			aNE = BRepBuilderAPI_MakeEdge(aCur,t1,t2);
			if(!aNE.IsNull())
			{
				//ע�ⷽ�򣬱�֤�·ָ�edge��˳���edge�ķ���һ��
				aNE.Orientation(ot);
				if(ot == TopAbs_FORWARD)
					seqOfShape.Append(aNE);
				else
					seqOfShape.Prepend(aNE);
				nC ++;
			}
		}catch (Standard_Failure){
			continue;
		}
	}

	return nC;
}

// ��wire�ָ�edge
// ������wire��edge��edge��.
//
int		kcgEdgeSplit::SplitEdgeByWire(const TopoDS_Edge& aEdge,
									  const TopoDS_Wire& aWire,
									  TopTools_SequenceOfShape& seqOfShape)
{
	int nC = 0;
	std::vector<double>  arpars;//��������б�

	//
	double df,dl;
	Handle(Geom_Curve) aCur = BRep_Tool::Curve(aEdge,df,dl);
	if(aCur.IsNull())
		return 0;

	if(df > dl)
	{
		double tmp = df;dl = df;df = tmp;
	}

	//��
	//���κ�wire��ÿ��edge�󽻵�
	kcgCurveCurveInter ccInter;
	TopExp_Explorer ex;
	kcxCCIntpt ipt;
	int ix,nbIpt;

	arpars.push_back(df);
	for(ex.Init(aWire,TopAbs_EDGE);ex.More();ex.Next())
	{
		TopoDS_Edge aE = TopoDS::Edge(ex.Current());
		if(!ccInter.Intersect(aEdge,aE))
			continue;

		nbIpt = ccInter.NbInterPoints();
		for(ix = 0;ix < nbIpt;ix ++)
		{
			ipt = ccInter.InterPoint(ix);
			arpars.push_back(ipt._ipt1._t);
		}
	}
	arpars.push_back(dl);

	//����,��С����
	std::sort(arpars.begin(),arpars.end());

	//Ϊÿ�Բ���������һ��edge
	TopAbs_Orientation ot = aEdge.Orientation();
	double t1,t2;
	TopoDS_Edge aNE;
	int isize = (int)arpars.size();
	nC = 0;
	for(ix = 0;ix < isize - 1;ix ++)
	{
		t1 = arpars.at(ix);
		t2 = arpars.at(ix + 1);
		if(fabs(t1 - t2) < K_PAR_TOL)
			continue;
		try
		{
			aNE = BRepBuilderAPI_MakeEdge(aCur,t1,t2);
			if(!aNE.IsNull())
			{
				//ע�ⷽ�򣬱�֤�·ָ�edge��˳���edge�ķ���һ��
				aNE.Orientation(ot);
				if(ot == TopAbs_FORWARD)
					seqOfShape.Append(aNE);
				else
					seqOfShape.Prepend(aNE);
				nC ++;
			}
		}catch (Standard_Failure){
			continue;
		}
	}

	return nC;
}

//////////////////////////////////////////////////////////////////////////
// ��һ��wire�ָ�Ϊ����
BOOL	kcgWireSplit::SplitWire(const TopoDS_Wire& aWire,gp_Pnt pnt,double distol,
							    TopTools_SequenceOfShape& seqOfShape)
{
	kPoint3 p;
	kcgPointCurveInter pcInter;
	int nc = 0;
	TopTools_ListOfShape llf,llr;
	BRepTools_WireExplorer wex;
	TopoDS_Edge aE1,aE2,aTE;
	TopoDS_Shape aS;
	TopoDS_Vertex vf,vl;
	gp_Pnt vpf,vpl;
	BOOL bFound = FALSE;

	// �Ƿ����ĩ���غϡ�
	TopExp::Vertices(aWire,vf,vl);
	vpf = BRep_Tool::Pnt(vf);
	vpl = BRep_Tool::Pnt(vl);
	if(vpf.Distance(pnt) < distol || vpl.Distance(pnt) < distol)
		return FALSE;
    
	p.set(pnt.X(),pnt.Y(),pnt.Z());
	for(wex.Init(aWire);wex.More();wex.Next())
	{
		TopoDS_Edge aE = TopoDS::Edge(wex.Current());
		if(bFound)
		{
			aTE = QxBRepLib::CopyEdge(aE);
			if(aTE.IsNull())
				return FALSE;
			llr.Append(aTE);
		}
		else
		{
			//�ȼ���Ƿ�ͽڵ��غ�
			TopExp::Vertices(aE,vf,vl);
			vpf = BRep_Tool::Pnt(vf);
			vpl = BRep_Tool::Pnt(vl);
			if(vpf.Distance(pnt) < distol)
			{
				bFound = TRUE;
				aTE = QxBRepLib::CopyEdge(aE);
				if(aTE.IsNull())
					return FALSE;
				llr.Append(aTE);
			}
			else if(vpl.Distance(pnt) < distol)
			{
				bFound = TRUE;
				aTE = QxBRepLib::CopyEdge(aE);
				if(aTE.IsNull())
					return FALSE;
				llf.Append(aTE);
			}
			else
			{
				if(pcInter.Intersect(p,aE))
				{
					kcxCurveIntpt ipt = pcInter.InterPoint(0);
					if(!kcgEdgeSplit::SplitEdge(aE,ipt._t,aE1,aE2))
						return FALSE;
					llf.Append(aE1);
					llr.Append(aE2);
					bFound = TRUE;
				}
				else
				{
					aTE = QxBRepLib::CopyEdge(aE);
					if(aTE.IsNull())
						return FALSE;
					llf.Append(aTE);
				}
			}
		}
	}

	//�黷
	TopoDS_Wire aNW;
	try{
		BRepBuilderAPI_MakeWire mw;
		TopTools_ListIteratorOfListOfShape lite;
		for(lite.Initialize(llf);lite.More();lite.Next())
			mw.Add(TopoDS::Edge(lite.Value()));
		if(mw.IsDone())
			aNW = mw.Wire();
	}catch(Standard_Failure){
		return FALSE;
	}
	if(aNW.IsNull())
		return FALSE;

	seqOfShape.Append(aNW);

	try{
		BRepBuilderAPI_MakeWire mw;
		TopTools_ListIteratorOfListOfShape lite;
		for(lite.Initialize(llr);lite.More();lite.Next())
			mw.Add(TopoDS::Edge(lite.Value()));
		if(mw.IsDone())
			aNW = mw.Wire();
	}catch(Standard_Failure){
		return FALSE;
	}
	if(aNW.IsNull())
		return FALSE;

	seqOfShape.Append(aNW);

	return TRUE;
}

// ʹ��һ��edge�ָ�wire
int		kcgWireSplit::SplitWireByEdge(const TopoDS_Wire& aWire,
									  const TopoDS_Edge& aEdge,
									  TopTools_SequenceOfShape& seqOfShape)
{
	if(aWire.IsNull() || aEdge.IsNull())
		return 0;

	int  nC = 0;
	TopTools_SequenceOfShape seqSplit;
	TopTools_ListOfShape wlist;
	TopoDS_Wire aW;
	BRepTools_WireExplorer wexp;
	TopoDS_Edge aTE;

	// �����ʷ�wire�е�edge�������б�Ҫ�����ӡ�
	// ��֤edge��˳��
	for(wexp.Init(aWire);wexp.More();wexp.Next())
	{
		TopoDS_Edge aE = TopoDS::Edge(wexp.Current());
		//
		seqSplit.Clear();
		// ʹ��aEdge���зָ���ܻ��ж���edge���ɡ�
		nC = kcgEdgeSplit::SplitEdgeByEdge(aE,aEdge,seqSplit);
		if(nC == 0)//û�зָ��¼�ɵ�
		{
			aTE = QxBRepLib::CopyEdge(aE);
			wlist.Append(aTE);
		}
		else
		{
			//�ָ�ɹ�����ǰ���¼��edge�ͷָ�ɵĵ�һ��edgeƴ�ӳ��µ�wire
			TopoDS_Wire aNW;
			try{
				TopTools_ListIteratorOfListOfShape  ll(wlist);
				BRepBuilderAPI_MakeWire  mw;
				for(;ll.More();ll.Next())
					mw.Add(TopoDS::Edge(ll.Value()));
				wlist.Clear();
				//��ӷָ�ĵ�һ��
				mw.Add(TopoDS::Edge(seqSplit.First()));
				if(mw.IsDone())
					aNW = mw.Wire(); 
			}catch(Standard_Failure){
				return 0;
			}
			if(aNW.IsNull())
				return FALSE;
			seqOfShape.Append(aNW);

			//�м䲿��ֱ�����
			int ix;
			for(ix = 2;ix < nC;ix ++)//ע�⣺�±��1��ʼ��������β��
				seqOfShape.Append(seqSplit.Value(ix));

			wlist.Append(seqSplit.Last());
		}
	}
	//ƴ������
	if(wlist.Extent() > 0)
	{
		TopoDS_Wire aNW;
		try{
			TopTools_ListIteratorOfListOfShape  ll(wlist);
			BRepBuilderAPI_MakeWire  mw;
			for(;ll.More();ll.Next())
				mw.Add(TopoDS::Edge(ll.Value()));
			if(mw.IsDone())
				aNW = mw.Wire(); 
		}catch(Standard_Failure){
			return 0;
		}
		if(aNW.IsNull())
			return FALSE;
		seqOfShape.Append(aNW);
	}

	return seqOfShape.Length();
}

// ʹ��һ��wire�ָ���һ��wire
int	kcgWireSplit::SplitWireByWire(const TopoDS_Wire& aWire,
								  const TopoDS_Wire& aWire2,
								  TopTools_SequenceOfShape& seqOfShape)
{
	if(aWire.IsNull() || aWire2.IsNull())
		return 0;

	int  nC = 0;
	TopTools_ListOfShape wlist;
	TopTools_SequenceOfShape seqSplit;
	BRepTools_WireExplorer wexp;
	TopoDS_Edge aTE;

	//�����ʷ�wire�е�edge�������б�Ҫ�����ӡ�
	for(wexp.Init(aWire);wexp.More();wexp.Next())
	{
		TopoDS_Edge aE = TopoDS::Edge(wexp.Current());
		
		seqSplit.Clear();
		//wire�ָ�edge
		nC = kcgEdgeSplit::SplitEdgeByWire(aE,aWire2,seqSplit);
		if(nC == 0)//û�зָ��¼�ɵ�
		{
			aTE = QxBRepLib::CopyEdge(aE);
			wlist.Append(aTE);
		}
		else
		{
			//�ָ�ɹ�����ǰ���¼��edge�ͷָ�ɵĵ�һ��edgeƴ�ӳ��µ�wire
			TopoDS_Wire aNW;
			try{
				TopTools_ListIteratorOfListOfShape  ll(wlist);
				BRepBuilderAPI_MakeWire  mw;
				for(;ll.More();ll.Next())
					mw.Add(TopoDS::Edge(ll.Value()));
				wlist.Clear();
				//��ӷָ�ĵ�һ��
				mw.Add(TopoDS::Edge(seqSplit.First()));
				if(mw.IsDone())
					aNW = mw.Wire(); 
			}catch(Standard_Failure){
				return 0;
			}
			if(aNW.IsNull())
				return FALSE;
			seqOfShape.Append(aNW);

			//�м䲿��ֱ�����
			int ix;
			for(ix = 2;ix < nC;ix ++)//ע�⣺�±��1��ʼ��������β��
				seqOfShape.Append(seqSplit.Value(ix));

			wlist.Append(seqSplit.Last());
		}
	}

	//ƴ������
	if(wlist.Extent() > 0)
	{
		TopoDS_Wire aNW;
		try{
			TopTools_ListIteratorOfListOfShape  ll(wlist);
			BRepBuilderAPI_MakeWire  mw;
			for(;ll.More();ll.Next())
				mw.Add(TopoDS::Edge(ll.Value()));
			if(mw.IsDone())
				aNW = mw.Wire(); 
		}catch(Standard_Failure){
			return 0;
		}
		if(aNW.IsNull())
			return FALSE;
		seqOfShape.Append(aNW);
	}

	return seqOfShape.Length();
}