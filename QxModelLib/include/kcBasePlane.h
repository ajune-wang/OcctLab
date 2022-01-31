// ����һ����׼��
//
#ifndef _KC_BASE_PLANE_H_
#define _KC_BASE_PLANE_H_

#include <V3d_View.hxx>
#include "QxLibDef.h"
#include "kvCoordSystem.h"


class kvGrid;
class kcModel;
class CArchive;

#define  KCT_BASE_PLANE			0x20000  //�����Entity�ĳ�ͻ

class QXLIB_API kcBasePlane
{
public:
	kcBasePlane(const Handle(V3d_View)& aView);
	~kcBasePlane(void);

	//����һ������ϵ����
	int						Create(kvCoordSystem& cs,kcModel *pModel,const char *pszName = NULL);
	void						Destroy();

public:
	void						SetActive(bool bActive);//�Ƿ�����Ϊ��Ļ�׼��

	// ��ȡ�ڲ���Grid
	kvGrid*					GetGrid() const { return _pGrid; }
	Handle(V3d_View)			GetV3dView() const { return _hView; }
	kvCoordSystem*			GetCoordSystem() { return &_cs; }
	int						GetCoordSystem(kvCoordSystem& cs);
	const kVector3&			GetZDirection() const;

	void						SetDefaultView();//�趨Ĭ��ʵ�壬Ŀǰ������ͼ��

	int						GetHandle() const { return _nHandle;}
	void						SetName(const char *szName);
	std::string				GetName() const;
	bool						HasName() const;

public:
	// ��Ļ��������ת��WCS�ռ�����.
	// x,y:��������,vp:�ռ�� vdir:�ռ�����.
	//
	bool						PixelToModel(int x,int y,kPoint3& vp,kVector3& vdir);

	// WCS�ռ�㵽��Ļ�����ת��
	// wp:�ռ��,wcs����ϵ��.x,y:���ص���Ļ����
	bool						ModelToPixel(const kPoint3& wp,int& x,int& y);

	// ��Ļ�������굽����ƽ���ϵ�.ͨ���������ߺ�ƽ���󽻵õ�
	// x,y:��������,wp:����ƽ���ϵĵ�,wcs����ϵ��.
	bool						PixelToWorkPlane(int x,int y,kPoint3& wp);

protected:
	bool						InitGrid();

	// ����ֱ�ߺ�ƽ��Ľ���
	bool						LinPlnIntersection(const kPoint3& linpnt,const kVector3& lindir,
									const kPoint3& plnpnt,const kVector3& plnnorm,kPoint3& intpt);

public:
	// ��ȡ
	BOOL						Read(CArchive& ar,kcModel *pModel);
	// д��
	BOOL						Write(CArchive& ar);

protected:
	int						_nHandle;//���
	std::string				_szName;//����
	kvCoordSystem				_cs;//����ϵ
	kcModel					*_pModel;//����ģ��
	Handle(V3d_View)			_hView;//������view
	kvGrid					*_pGrid;//����ƽ������
	bool						_bActive;//�Ƿ�ǰ��׼��
};

#endif