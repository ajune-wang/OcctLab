//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCG_BREPLIB_H_
#define _KCG_BREPLIB_H_

#include <vector>
#include <Geom_Curve.hxx>
#include "QxLibDef.h"
#include "kVector.h"

typedef std::vector<Handle(Geom_Curve)> GeomCurveArray;

class QXLIB_API QxBRepLib
{
public:
	static double								EdgeLength(const TopoDS_Edge& aEdge);//����edge�ĳ���
	static double								EdgeLengthEx(const TopoDS_Edge& aEdge);//����edge�ĳ���
	static double								WireLength(const TopoDS_Wire& aWire);//����edge�ĳ���

	//���������߶�
	static TopoDS_Edge						MakeSegEdge(const gp_Pnt &p1,const gp_Pnt &p2);
	//����������Edge
	static TopoDS_Edge						MakeEdge(const Handle(Geom_Curve) &aCurve);

	//����������Face
	static TopoDS_Face						MakeFace(const Handle(Geom_Surface) &aSurf);

	// shape�Ŀ�������emptycopy��ͬ����ȿ�����
	static  TopoDS_Edge							CopyEdge(const TopoDS_Edge& aEdge);
	static  TopoDS_Wire							CopyWire(const TopoDS_Wire& aWire);
	static  TopoDS_Face							CopyFace(const TopoDS_Face& aFace);
	static  TopoDS_Shape						CopyShape(const TopoDS_Shape& aShape);

	// �ƶ�����
	static  TopoDS_Shape						MoveShape(const TopoDS_Shape& aShape,const kVector3& vec,BOOL bCopy = TRUE);

	// ��edge����wire
	static  TopoDS_Wire							EdgeToWire(const TopoDS_Edge& aEdge);

	// ��ת
	// p:����,dir:��λ������ang����ת�Ƕȣ����ȡ�
	static  TopoDS_Shape						RotateShape(const TopoDS_Shape& aShape,const kPoint3& p,const kVector3& dir,double ang);
	// ����
	static  TopoDS_Shape						ScaleShape(const TopoDS_Shape& aShape,const kPoint3& p,double scale,BOOL bCopy = TRUE);

	//edge��wire�Ƿ�����
	static   BOOL								IsConnected(const TopoDS_Wire& aW,const TopoDS_Edge& aE);

	//��ȡface���⻷
	//ʹ��BRepTools::OuterWire�Ĵ��룬�����޸ġ�
	static  TopoDS_Wire							OuterWire(const TopoDS_Face& aFace);

	//����һϵ��wire,����ƽ��face.wire���ཻ,�����໥����.��Ҫ���ڴ���������
	//����һϵ��ƽ��.
	static  int									BuildPlnFace(TopTools_ListOfShape& aWList,TopTools_ListOfShape& aFList);

	//wire 1�Ƿ����wire2.ע��:wire1��wire2���ཻ.��Ȼ�����ж��Ƿ��ཻ.������Ϊ��Ч��û�д���.
	static  BOOL								IsWire2InWire1(const TopoDS_Wire& aW1,const TopoDS_Wire& aW2);

	//��һ���⻷��һ���ڻ�����face
	static TopoDS_Face							BuildPlaneFace(const TopoDS_Wire& aOW,const TopTools_ListOfShape& aIWList);

	//��һϵ��������edge��wire����һ��wire.

public:
	// ��һϵ�����ߣ�����һ��wire
	static TopoDS_Wire			BuildOneWire(const GeomCurveArray &aCrvAry);

	// ��һϵ�����ߣ�����һϵ��wire
	static TopoDS_Shape			BuildWires(const GeomCurveArray &aCrvAry);

	// ��һϵ�����ߣ�����һ��edge����϶���
	static TopoDS_Compound		BuildEdgeCompound(const GeomCurveArray &aCrvAry);

	// �޸�shape���󣬷����޸���Ķ���
	static TopoDS_Shape			FixShape(const TopoDS_Shape &aS);
};

// edge�ķָ�
class QXLIB_API kcgEdgeSplit{
public:
	//��һ��edge�ָ�Ϊ����
	static   BOOL			SplitEdge(const TopoDS_Edge& aEdge,double t,
									  TopoDS_Edge& aE1,TopoDS_Edge& aE2);

	//��edgeTool�ָ�edge,���ɵ���edge������seq�У�������edge�ĸ���
	static   int			SplitEdgeByEdge(const TopoDS_Edge& aEdge,const TopoDS_Edge& aEdge2,
									  TopTools_SequenceOfShape& seqOfShape);

	// ��wire�ָ�edge
	static	int				SplitEdgeByWire(const TopoDS_Edge& aEdge,
											const TopoDS_Wire& aWire,
											TopTools_SequenceOfShape& seqOfShape);
};

class QXLIB_API kcgWireSplit{
public:
	// ��һ��wire�ָ�Ϊ����wire��edge
	static	BOOL			SplitWire(const TopoDS_Wire& aWire,gp_Pnt pnt,double distol,
									  TopTools_SequenceOfShape& seqOfShape);

	// ʹ��һ��edge�ָ�wire
	static  int				SplitWireByEdge(const TopoDS_Wire& aWire,
											const TopoDS_Edge& aEdge,
											TopTools_SequenceOfShape& seqOfShape);

	// ʹ��һ��wire�ָ���һ��wire
	static	int				SplitWireByWire(const TopoDS_Wire& aWire,
											const TopoDS_Wire& aWire2,
											TopTools_SequenceOfShape& seqOfShape);
};

#endif