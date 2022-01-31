#ifndef _KC_DISPOBJ_H_
#define _KC_DISPOBJ_H_

#include "kPoint.h"
#include "kVector.h"
#include <Graphic3d_StructureManager.hxx>
#include "QxLibDef.h"

//��ʾһ������ļ򵥿�ܣ����ڽ����еĹ�����ʾ
class QXLIB_API kcDispFrmShape
{
public:
	kcDispFrmShape(const Handle_AIS_InteractiveContext& aCtx);
	virtual ~kcDispFrmShape();

	//��������
	void								SetIsoNum(int UIsoNum,int VIsoNum);
	void								SetColor(int color);

	//��Ӷ���
	bool								AddShape(const TopoDS_Shape& aShape);
	void								Destroy();

	bool								Show();
	bool								Hide();

	//�ƶ�
	bool								Move(double dx,double dy,double dz);
	// ��ת
	// p:����,dir:��λ������ang����ת�Ƕȣ����ȡ�
	bool								Rotate(const kPoint3& p,const kVector3& dir,double ang);

	// ����
	bool								Scale(const kPoint3& p,double scale);


protected:
	Handle(Graphic3d_StructureManager)	m_aStruMgr;
	Handle(Prs3d_Presentation)			m_aPrs;

	int								m_nUIsoNum,m_nVIsoNum;
	int								m_color;
};


#endif