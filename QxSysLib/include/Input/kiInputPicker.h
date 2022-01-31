//////////////////////////////////////////////////////////////////////////
//
#ifndef _KI_INPUT_PICKER_H_
#define _KI_INPUT_PICKER_H_

#include "kPoint.h"
#include "QxLibDef.h"

class kcDocContext;

class QXLIB_API kiCurvePicker{
public:
	kiCurvePicker(kcDocContext *pDocCtx);

	bool						Init(const TopoDS_Edge& aEdge);
	bool						Init(const TopoDS_Wire& aWire);

	//���Ƿ����������ϡ�itol�������ؼ���ľ��ȡ�
	bool						Pick(int x,int y,int itol,kPoint3& pp);
	bool						Pick(int x,int y,int itol,double& t,kPoint3& pp);
	bool						Pick(int x,int y,int itol,double& t,kPoint3& pp,TopoDS_Edge& aPickEdge);

protected:
	kcDocContext				*pDocContext_;
	TopoDS_Shape				_aShape;
};


#endif