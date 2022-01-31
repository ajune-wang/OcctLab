//////////////////////////////////////////////////////////////////////////
// ����ƽ���ϵ������ʾ
//
#ifndef _KV_GRID_H_
#define _KV_GRID_H_

#include "QxLibDef.h"

// ���������
#define KVT_GRID_LINE		1 //ֱ���͵�����
#define KVT_GRID_DOT		2 //��������

class kcBasePlane;
class kvWorkPlane;
class kiSnapMngr;

class QXLIB_API kvGrid
{
public:
	kvGrid(const Handle(AIS_InteractiveContext)& aCtx,kcBasePlane *pBasePlane);
	~kvGrid(void);

	void				SetType(int type);
	void				SetVisible(bool bVisible);
	void				SetWidth(double width);
	void				SetHeight(double height);
	void				SetCellSize(double size);

	bool				IsVisible() const { return m_bVisible; }

	// ���и���,��Ҫ�Ǹ�����ʾ
	void				Update();

	void				Destroy();

public:
	//�����Զ���׽��
	void				CalcSnapPoint(kiSnapMngr& snapMgr);

protected:
	//
	void				DestroyStructure(Handle(Graphic3d_Structure)& aStru);
	void				ClearStructure(Handle(Graphic3d_Structure)& aStru);

protected:
	int					m_eType;//����
	bool				m_bVisible;//�Ƿ�ɼ�
	Handle(AIS_InteractiveContext)	m_aisContext;
	kcBasePlane			*_pBasePlane;//�����Ļ�׼��
	double				m_dWidth,m_dHeight;//��͸�
	double				m_dCellSize;//����Ĵ�С.

protected:
	Handle(Graphic3d_Structure)		m_hStruFrm;//�����ʾ����
	Handle(Graphic3d_Structure)		m_hStruIns;//�ڲ���ʾ����
};

#endif