//////////////////////////////////////////////////////////////////////////
//
#ifndef _KC_SNAP_MGR_H_
#define _KC_SNAP_MGR_H_

#include <vector>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Group.hxx>
#include "kPoint.h"
#include "kiSnapMngr.h"

class CRect;
class kcModel;
class kcBasePlane;

// ��¼һϵ�е�
class QXLIB_API kiSnapPointSet{
public:
	kiSnapPointSet();

	bool				AddSnapPoint(const kiSnapPoint& sp);
	void				Clear();

	int					GetCount() const;
	const kiSnapPoint&	GetSnapPoint(int idx) const;

	bool				CalcScreenPoint(const Handle(V3d_View) &aView);

	void				DoSnap(int x,int y,int scope,std::vector<kiSnapPoint>& ss);

protected:
	std::vector<kiSnapPoint>	_spArray;//��׽���б�
};



// ����һ��������׽����Ϣ.
//
class QXLIB_API kcSnapMngr : public kiSnapMngr
{
public:
	kcSnapMngr(kcModel *pModel);
	~kcSnapMngr(void);

	//��ӵ�
	bool					AddSnapPoint(int snapType,const kiSnapPoint& sp);
	bool					AddSnapPointSet(int snapType,const kiSnapPointSet& ss);
	void					Clear();
	//�����ʱ��
	void					ClearTempPoint();

	// ����㲶׽��غ���
	bool					AddGridSnapPoint(const kiSnapPoint& sp);
	void					ClearGridSnapPoint();

	// �ϲ�����mgr
	bool					Merge(const kcSnapMngr& smgr);

	void					SetView(const Handle(V3d_View) &aView);

public:
	//����һ�����ߵ��Զ���׽��
	bool					CalcSnapPoint(const Handle(Geom_Curve)& aCurve);
	bool					CalcSnapPoint(const Handle(Geom_Curve)& aCurve,double t0,double t1);
	bool					CalcSnapPoint(const TopoDS_Edge& aEdge);
	bool					CalcSnapPoint(const TopoDS_Edge& aEdge,const TopoDS_Face& aFace);

	//����һ��������Զ���׽��
	bool					CalcSnapPoint(const TopoDS_Face& aFace);
	bool					CalcSnapPoint(const TopoDS_Shape& aShape);

public:
	void					SetSnapScope(int scope);
	bool					EnableSnap(int nFlag);
	bool					DisableSnap(int nFlag);
	bool					EnableAll();
	bool					DisableAll();

	// ������Ļ����
	bool					CalcScreenPoint();

	// ���в�׽,x,y����Ļ�������㡣
	void					DoSnap(int x,int y);
	bool					HasSnapPoint() const { return _bSnaped; }
	const kiSnapPoint&		GetSnapPoint();

public:
	void					HideSnapMarker();
	void					ShowSnapMarker();

protected:
	int						_nScope;//�Զ���׽�ķ�Χ��С����Ļ���꣬С�ڸþ�����Ϊ��׽�ɹ���
	int						_nSnapFlag;//��׽��־����ʾ��ǰ����׽�ķ�Χ

public:
	kiSnapPointSet*			GetPointSet(int snapType) ;
	const kiSnapPointSet&	ExistPointSet() const;
	const kiSnapPointSet&	TermPointSet() const;
	const kiSnapPointSet&	MidPointSet() const;
	const kiSnapPointSet&	CenPointSet() const;
	const kiSnapPointSet&	TempPointSet() const;
	const kiSnapPointSet& QuaPointSet() const;

protected:
	kcBasePlane*				GetCurrentBasePlane();

protected:
	kiSnapPoint				_org;//ԭ��
	kiSnapPointSet			_pntSet;//���ڵ��б�
	kiSnapPointSet			_termSet;//�˵��б�
	kiSnapPointSet			_midSet;//�е��б�
	kiSnapPointSet			_cenSet;//�����б�
	kiSnapPointSet			_tmpSet;//��ʱ���б�,���������е���ʱ��.
	kiSnapPointSet			_quaSet;//�ķֵ��б�
	kiSnapPointSet			_gridSet;//�����ϵĲ�׽�㡣

protected:
	bool					_bSnaped;//�Ƿ�׽����
	kiSnapPoint				_curSnapPoint;//��ǰ��׽�㡣

protected:
	Handle(Graphic3d_Structure)	_hSnapPntStru;
	Handle(Graphic3d_Group)		_hSnapPntGrp;

	kcModel						*pModel_;//������ģ�Ͷ���
	Handle(V3d_View)				aV3dView_;//��¼���ڵ���ͼ����������ת��
};

inline const kiSnapPointSet&	kcSnapMngr::ExistPointSet() const
{
	return _pntSet;
}

inline const kiSnapPointSet&	kcSnapMngr::TermPointSet() const
{
	return _termSet;
}

inline const kiSnapPointSet&  kcSnapMngr::MidPointSet() const
{
	return _midSet;
}

inline const kiSnapPointSet&  kcSnapMngr::CenPointSet() const
{
	return _cenSet;
}

inline const kiSnapPointSet&  kcSnapMngr::TempPointSet() const
{
	return _tmpSet;
}

inline const kiSnapPointSet&  kcSnapMngr::QuaPointSet() const
{
	return _quaSet;
}

#endif