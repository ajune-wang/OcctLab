#pragma once
#include "afxcmn.h"

#include <vector>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include "resource.h"
// CShapeStructDialog �Ի���

class CShapeStructDialog : public CDialog
{
	DECLARE_DYNAMIC(CShapeStructDialog)

public:
	CShapeStructDialog(const TopoDS_Shape& aShape,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShapeStructDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_SHAPE_STRUCT };

public:
	//�Ƿ�ʹ�ø�����ķ���
	void							UseParentOri(bool bEnable) { bUseParentOri_ = bEnable;}

protected:
	bool							bUseParentOri_;//�����Ӷ���ʱ���Ƿ�ʹ�ø�����ķ���

protected:
	// ��ʾһ���������Ϣ
	HTREEITEM					ShowShape(const TopoDS_Shape& aShape,HTREEITEM hParent);

	//�������ʾ����
	HTREEITEM					ShowCompound(const TopoDS_Shape& aShape,HTREEITEM hParent);
	HTREEITEM					ShowCompSolid(const TopoDS_Shape& aShape,HTREEITEM hParent);
	HTREEITEM					ShowSolid(const TopoDS_Shape& aShape,HTREEITEM hParent);
	HTREEITEM					ShowShell(const TopoDS_Shape& aShape,HTREEITEM hParent);
	HTREEITEM					ShowFace(const TopoDS_Shape& aShape,HTREEITEM hParent);
	HTREEITEM					ShowWire(const TopoDS_Wire& aWire,HTREEITEM hParent);
	//��ʾ�ü�����wire
	HTREEITEM					ShowWire(const TopoDS_Wire& aWire,const TopoDS_Face& aFace,HTREEITEM hParent);
	HTREEITEM					ShowEdge(const TopoDS_Shape& aShape,HTREEITEM hParent);
	//��ʾ�ü����е�edge
	HTREEITEM					ShowEdge(const TopoDS_Shape& aShape,const TopoDS_Face& aFace,HTREEITEM hParent);
	HTREEITEM					ShowVertex(const TopoDS_Shape& aShape,HTREEITEM hParent);

	//��ʾshape�Ļ�����Ϣ������������λ�á��ڲ�ָ��ȡ�
	HTREEITEM					ShowShapeBaseProp(const TopoDS_Shape& aShape,HTREEITEM hParent);

	//��ʾface��geom���ԡ���Ҫ��surface�����ԡ�
	HTREEITEM					ShowFaceGeomProp(const TopoDS_Shape& aShape,HTREEITEM hParent);

	//��ʾsurface������
	HTREEITEM					ShowSurface(const Handle(Geom_Surface)& aSurf,HTREEITEM hParent);
	HTREEITEM					ShowBSplineSurface(const Handle(Geom_BSplineSurface)& aSurf,HTREEITEM hParent);
	HTREEITEM					ShowRectTrimmedSurface(const Handle(Geom_RectangularTrimmedSurface) &aSurf,HTREEITEM hParent);
	HTREEITEM					ShowElementCommonProp(const Handle(Geom_ElementarySurface)& aSurf,HTREEITEM hParent);
	HTREEITEM					ShowPlane(const Handle(Geom_Plane)& aSurf,HTREEITEM hParent);
	HTREEITEM					ShowCylinder(const Handle(Geom_CylindricalSurface)& aSurf,HTREEITEM hParent);
	HTREEITEM					ShowLinearExtSurf(const Handle(Geom_SurfaceOfLinearExtrusion)& aSurf,HTREEITEM hParent);
	HTREEITEM					ShowRevolutionSurf(const Handle(Geom_SurfaceOfRevolution)& aSurf,HTREEITEM hParent);

	void							ShowSurfaceBaseProp(const Handle(Geom_Surface)& aSurf,HTREEITEM hParent);
	
	//��ʾface�Ĳü������ԡ�
	HTREEITEM					ShowFaceLoopProp(const TopoDS_Shape& aShape,HTREEITEM hParent);

	// ��ʾ�ʷ�������Ϣ
	HTREEITEM					ShowFaceMeshProp(const TopoDS_Shape& aShape,HTREEITEM hParent);


	//��ʾEdge��geom���ԡ�
	HTREEITEM					ShowEdgeGeomProp(const TopoDS_Shape& aShape,HTREEITEM hParent);


	//��ʾcurve������
	HTREEITEM					ShowCurve(const Handle(Geom_Curve)& aCur,HTREEITEM hParent);
	HTREEITEM					ShowSplineCurve(const Handle(Geom_BSplineCurve)& aCrv,HTREEITEM hParent);
	HTREEITEM					ShowSpline2DCurve(const Handle(Geom2d_BSplineCurve)& aCrv,HTREEITEM hParent);

	HTREEITEM					Show2DCurve(const Handle(Geom2d_Curve)& aCur,HTREEITEM hParent);

protected:
	// ��ʾ������Ϣ
	HTREEITEM					ShowShareInfo(const TopoDS_Shape& aShape,HTREEITEM hParent);

	// 
	void							ShowVertexShareInfo(HTREEITEM hParent);
	void							ShowEdgeShareInfo(HTREEITEM hParent);

	void							InsertKnots(const TColStd_Array1OfReal& aKnots,int npl,HTREEITEM hParent);
	void							InsertMults(const TColStd_Array1OfInteger& aMults,int npl,HTREEITEM hParent);

	//
	void							MapShape(const TopoDS_Shape& aShape,TopTools_DataMapOfShapeInteger& aMap,TopAbs_ShapeEnum stype);

protected:
	// ��ʾͳ����Ϣ
	HTREEITEM					ShowShapeStatics(const TopoDS_Shape& aShape,HTREEITEM hParent);

	HTREEITEM					ShowAllEdge(const TopoDS_Shape& aShape,HTREEITEM hParent);

	HTREEITEM					ShowFaceAllLoop(const TopoDS_Shape& aShape,HTREEITEM hParent);

	HTREEITEM					ShowWireOrderedVtx(const TopoDS_Wire &aW,HTREEITEM hParent);

protected:
	TopoDS_Shape				m_aShape;//Ҫ��ʾ�Ķ���

	TopTools_IndexedMapOfShape	m_aFaceIdxMap;
	TopTools_IndexedMapOfShape	m_aEdgeIdxMap;
	TopTools_IndexedMapOfShape m_aVertexIdxMap;//��¼���й�����󣬻�ȡ��Ψһ���

	TopTools_DataMapOfShapeInteger	m_aVertexCntMap;
	TopTools_DataMapOfShapeInteger	m_aEdgeCntMap;//key��shape��value��int��¼shape������Ĵ���

	TopTools_IndexedDataMapOfShapeListOfShape  m_aVEMap;
	TopTools_IndexedDataMapOfShapeListOfShape  m_aEFMap;	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CTreeCtrl m_ssTree;
	virtual BOOL OnInitDialog();
};
