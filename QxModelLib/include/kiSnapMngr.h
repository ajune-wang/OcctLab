//////////////////////////////////////////////////////////////////////////
//
#ifndef _KI_SNAP_MGR_H_
#define _KI_SNAP_MGR_H_

#include <vector>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Group.hxx>
#include "kPoint.h"
#include "QxLibDef.h"

#define KSNAP_NIL				0 //����׽
#define KSNAP_EXIST_POINT		0x01 //���ڵ�
#define KSNAP_TERM_POINT		0x02 //�˵�
#define KSNAP_MID_POINT			0x04 //�е�
#define KSNAP_CEN_POINT			0x08 //���ĵ㡣
#define KSNAP_TMP_POINT			0x10 //��ʱ��
#define KSNAP_QUA_POINT			0x20 //Բ���ķֵ�
#define KSNAP_GRID_POINT		0x80 //�����
#define KSNAP_ALL				(KSNAP_EXIST_POINT | KSNAP_TERM_POINT | KSNAP_MID_POINT | KSNAP_CEN_POINT | KSNAP_TMP_POINT | KSNAP_GRID_POINT)

class kvWorkPlane;
class CRect;

//��¼һ����׽��¼��
class QXLIB_API kiSnapPoint{
public:
	kiSnapPoint();
	kiSnapPoint(double x,double y,double z);
	kiSnapPoint(const kiSnapPoint& other);

	kiSnapPoint&	operator=(const kiSnapPoint& other);

public:
	kPoint3			_point;
	int				_scx,_scy;//��Ļ����.
};


//����һ���ӿڣ����ڽ��ӿں�ʵ�ַ���
class QXLIB_API kiSnapMngr{
public:
	virtual bool			AddSnapPoint(int snapType,const kiSnapPoint& sp) = 0;
	virtual void			Clear() = 0;

	virtual bool			CalcSnapPoint(const TopoDS_Edge& aEdge) = 0;
	virtual bool			CalcSnapPoint(const TopoDS_Face& aFace) = 0;
	virtual bool			CalcSnapPoint(const TopoDS_Shape& aShape) = 0;

	// ����㲶׽��غ���
	virtual bool			AddGridSnapPoint(const kiSnapPoint& sp) = 0;
	virtual void			ClearGridSnapPoint() = 0;
};

#endif