//////////////////////////////////////////////////////////////////////////
// entity����������Ϣ����ʾ���ݡ�
// ��ʾ����ʹ��AIS����ʾ�ͼ�¼��

#ifndef _KC_ENTITY_H_
#define _KC_ENTITY_H_

#include <string>
#include <vector>
#include <list>
#include <map>
#include <AIS_InteractiveObject.hxx>
#include <Quantity_Color.hxx>
#include <gp_Circ.hxx>
#include "kBase.h"
#include "kPoint.h"
#include "kVector.h"
#include "AIS_EntityShape.h"
#include "QxLibDef.h"

#define KC_ENT_NAME_MAX		64

typedef std::vector<TopoDS_Shape>  QxTopShapeArray;
typedef std::list<TopoDS_Shape> QxTopShapeList;

// ����ʹ�ø�word��Ϊ����������,��word��Ϊ�����Ͷ���.
// �����Ҫ�ٶ�����������,�������Ʒ�������.
// ����:0x201 ��curve�����µ�ֱ��.
#define KCT_ENT_POINT			0x01
#define KCT_ENT_EDGE			0x02
#define KCT_ENT_WIRE			0x04 //��ͨwire
#define KCT_ENT_FACE			0x08
#define KCT_ENT_SHELL			0x10
#define KCT_ENT_SOLID			0x20
#define KCT_ENT_COMPOUND		0x40
#define KCT_ENT_BLOCK			0x80
#define KCT_ENT_DIM			0x100 //dim��
#define KCT_ENT_DIM_LENGTH		(KCT_ENT_DIM + 1)
#define KCT_ENT_DIM_DIAMETER	(KCT_ENT_DIM + 2)
#define KCT_ENT_DIM_RADIUS		(KCT_ENT_DIM + 3)
#define KCT_ENT_DIM_ANGLE		(KCT_ENT_DIM + 4)

#define KCT_ENT_ALL				(KCT_ENT_POINT | KCT_ENT_EDGE | KCT_ENT_WIRE | KCT_ENT_FACE | KCT_ENT_SHELL | KCT_ENT_SOLID | KCT_ENT_COMPOUND)

#ifndef IS_POINT_ENTITY
#define IS_POINT_ENTITY(pEnt)	(pEnt->GetType() & KCT_ENT_POINT)
#endif

#ifndef IS_EDGE_ENTITY
#define IS_EDGE_ENTITY(pEnt)   (pEnt->GetType() & KCT_ENT_EDGE)
#endif

#ifndef IS_WIRE_ENTITY
#define IS_WIRE_ENTITY(pEnt)	(pEnt->GetType() & KCT_ENT_WIRE)
#endif

#ifndef IS_FACE_ENTITY
#define IS_FACE_ENTITY(pEnt)	(pEnt->GetType() & KCT_ENT_FACE)
#endif

#ifndef IS_SHELL_ENTITY
#define IS_SHELL_ENTITY(pEnt)	(pEnt->GetType() & KCT_ENT_SHELL)
#endif


#ifndef IS_SOLID_ENTITY
#define IS_SOLID_ENTITY(pEnt)	(pEnt->GetType() & KCT_ENT_SOLID)
#endif

#ifndef IS_COMPOUND_ENTITY
#define IS_COMPOUND_ENTITY(pEnt)	(pEnt->GetType() & KCT_ENT_COMPOUND)
#endif

#ifndef IS_BLOCK_ENTITY
#define IS_BLOCK_ENTITY(pEnt)	(pEnt->GetType() & KCT_ENT_BLOCK)
#endif

class CArchive;
class kcLayer;
class kcModel;
class kiSnapMngr;
class QxStgBlock;

class kcEntity;
	class kcPointEntity;
	class kcEdgeEntity;
	class kcWireEntity;//�����
	class kcFaceEntity;
	class kcShellEntity;
	class kcSolidEntity;
	class kcCompoundEntity;//��ϣ�����������ϡ�
	class kcBlockEntity; //����󣬿��԰�������������͵�kcEntity����
	class kcDimEntity;
		class kcLengthDimEntity;
		class kcDiameterDimEntity;
		class kcRadiusDimEntity;
		class kcAngleDimEntity;
	
typedef std::list<kcEntity *>  kcEntityList;
typedef std::vector<kcEntity *> kcEntityArray;
typedef std::map<kcEntity *,int> kcEntityMap;

// ��occ�Ķ���һ��
typedef enum tagLineStyle{
	eTOL_SOLID,
	eTOL_DASH,
	eTOL_DOT,
	eTOL_DOTDASH,
	eTOL_USERDEFINED
}eLineStyle;

//��������
QXLIB_API bool  ksIsValidEntityType(int entType);
QXLIB_API int   ksEntityType(const TopAbs_ShapeEnum shapeType);
QXLIB_API TopAbs_ShapeEnum  ksShapeType(int entType);
QXLIB_API int   ksSeparateEntityTypes(int enttypes,int *pTypeArray);//�������͸���

// �����Ĺ�����󣬹��ܰ�����
// 1����¼���κ���������
// 2����¼��ص�����
// 3����¼�͹�����ʾ����
// 4����д�������
// 5�����һЩ���ƻ���չ���ܵ�
//
class QXLIB_API kcEntity
{
public:
	kcEntity(void);
	kcEntity(kcLayer *pLayer);
	virtual ~kcEntity(void);

	//��������
	virtual int				GetType() const { return _entType; }
	DWORD					GetHandle() const { return _nHandle; }
	const char*				GetName() const { return _szName;}
	bool						HasName() const;
	bool						SetNameDesc(const char *pszDesc); //�趨�������������֣������������

	// ��ȡ��Ӧ�ĵͲ����,���ؾֲ�,���ܹ��ⲿ�޸�.
	virtual TopoDS_Shape		GetShape();

	virtual BOOL				IsValid() const { return bValid_; }
	virtual void				SetModified(BOOL bModified);
	virtual BOOL				IsModified() const { return bModified_; }

	//�Ƿ��ע����
	virtual bool				IsDimension() const { return false; }

	// ���úͻ�ȡͼ�㡣
	virtual BOOL				SetLayer(kcLayer *pLayer);
	virtual kcLayer*			GetLayer() const { return _pLayer; }

	virtual bool				SetOwner(kcEntity *pOwner);
	virtual kcEntity*			GetOwner() const;

	virtual BOOL				Destroy();

public://һЩ�����ж�
	//�Ƿ�պ�
	virtual bool				IsClosed() { return false;}

public:
	// �����Զ���׽����Ϣ
	virtual BOOL				CalcSnapPoint(kiSnapMngr& snapMgr) {  return FALSE; }

public:
	// ��ȡ
	virtual BOOL				Read(CArchive& ar);
	// д��
	virtual BOOL				Write(CArchive& ar);
	// ��ar�ж���һ��kcEntity����
	static kcEntity*			ReadEntity(CArchive &ar);

protected:
	// ��ӹ�������
	virtual BOOL				AddCommonAttrib(QxStgBlock& blk);
	//
	virtual BOOL				ParseCommonAttrib(QxStgBlock& blk);
	//
	virtual BOOL				AddShapeData(QxStgBlock& blk,const TopoDS_Shape& aShape);
	virtual TopoDS_Shape		ParseShapeData(QxStgBlock& blk);		

	// ����������д�������Լ�������
	virtual bool				DoWriteOtherData(CArchive& ar) { return TRUE; }
	virtual bool				DoReadOtherData(CArchive& ar) { return TRUE; }
	// �����������������м�¼������ֵ
	virtual bool				DoAddOtherCodeValue(QxStgBlock& blk) { return TRUE; }
	virtual bool				DoGetOtherCodeValue(QxStgBlock& blk) { return TRUE; }
	// ��������������Լ�������ֵ
	virtual BOOL				DoAddAttrib(QxStgBlock& blk) { return TRUE; }
	virtual BOOL				DoParseAttrib(QxStgBlock& blk) { return TRUE; }

public:
	//��������
	virtual kcEntity*			Clone() {  return NULL; }

protected:
	//������������
	virtual bool				CloneCommonAttribs(kcEntity *pEntity);

public:
	// ��ʾ
	virtual BOOL				Display(bool bShow,BOOL bUpdateView = TRUE);
	virtual BOOL				IsDisplayed() const;
	virtual BOOL				HasDispalyObj() const;
	const Handle(AIS_InteractiveObject)&	DisplayObj() const { return hAISObject_; }

protected:
	// ������ʾ����
	// Ϊ�¶���������ʾ����
	virtual	BOOL				BuildDisplayObj();
	// ������ʾ���󣬿����оɵ���ʾ����
	virtual BOOL				UpdateDisplayObj(BOOL bUpdateView);
	virtual BOOL				DestroyDisplayObj();
	// �������ʾ����
	virtual BOOL				DoDisplay(BOOL bUpdateView);

protected:
	// ���ݼ�¼�����ݺ����ԣ�����һ��ais��ʾ����
	// ��ʾ�����¼��m_aisObj��
	virtual BOOL				DoBuildDisplayObj();
	// ������ʾ��������
	virtual void				DoSetDisplayAttribs();
	// �����ض�Face������
	virtual void				DoSetFaceDisplayAttribs() {}

protected:
	void						Reset();
	void						SetFaceColorAttribs();

protected:
	int						_entType;//����
	std::string				_sTypeName;//���磺"Point���ߵ�"
	int						_nHandle;//���,����ģ��Ψһ,��Чֵ��1��ʼ.
	char						_szName[KC_ENT_NAME_MAX];//����
	std::string				sNameDesc_; //�����е��������֣����������������
	TopoDS_Shape				aShape_;//OCCT shape����Ϣ
	kcModel					*_pModel;//����model��
	kcLayer					*_pLayer;//����ͼ��
	kcEntity					*pOwner_; //��������Ŀǰ��Ҫ����block�ж���
	bool						bVisible_; //�����Ƿ�ɼ�

public://���Բ���
	virtual bool				SetVisible(bool bVisible,BOOL bUpdate);
	virtual bool				IsVisible();
	virtual void				SetColor(double r,double g,double b,BOOL bUpdate);
	virtual void				SetColor(int icolor,BOOL bUpdate);
	virtual void				GetColor(double& r,double& g,double& b);
	virtual Quantity_Color	GetColor() const;
	virtual bool				SetLineStyle(eLineStyle linStyle,BOOL bUpdate);
	virtual eLineStyle		GetLineStyle() const { return m_eLineStyle; }
	virtual	bool				SetLineWidth(double width,BOOL bUpdate);
	virtual double			GetLineWidth() const { return m_dLineWidth; }
	virtual bool				SetTransparency(double transp,BOOL bUpdate);
	virtual double			GetTransparency() const { return m_dTransparency; }
	virtual bool				SetDisplayModel(int model,BOOL bUpdate);
	virtual int				GetDisplayModel() const { return m_nDisplayModel; }

protected:
	void						ResetAttribs();

protected://����
	kVector3					m_color;//��ʾ��ɫ
	eLineStyle				m_eLineStyle;
	double					m_dLineWidth;//�߿�
	double					m_dTransparency;//͸����,0-1֮��.
	int						m_nDisplayModel;//��ʾģʽ

protected:
	Handle(AIS_InteractiveContext)		_hAISContext;
	Handle(AIS_InteractiveObject)		hAISObject_;//��¼��ʾ����
	Handle(AIS_EntityShape)				hAISEntObj_;//EntityShape��ʾ����

	friend class kcLayer;

protected:
	//
	void						SetValid(BOOL bValid);

private:
	BOOL						bValid_;//�Ƿ�����Ч����
	BOOL						bModified_;//�����Ƿ��޸���
};

// һ����
class QXLIB_API kcPointEntity : public kcEntity{
public:
	kcPointEntity();
	kcPointEntity(double point[3]);
	virtual ~kcPointEntity();

	BOOL						SetPoint(double point[3]);
	BOOL						SetPoint(double x,double y,double z);
	BOOL						GetPoint(double point[3]);

protected:
	BOOL						BuildShape();

public:
	//�����Զ���׽����Ϣ
	virtual BOOL				CalcSnapPoint(kiSnapMngr& snapMgr);

protected:
	virtual BOOL				DoAddAttrib(QxStgBlock& blk);
	virtual BOOL				DoParseAttrib(QxStgBlock& blk);

public:
	//��������
	virtual kcEntity*			Clone();

public:
	double					_dPoint[3];
};

// ����curve entity�Ļ��ࡣ
//
class QXLIB_API kcEdgeEntity : public kcEntity
{
public:
	kcEdgeEntity(void);
	virtual ~kcEdgeEntity(void);

	//�Ƿ�պ�
	virtual bool				IsClosed();

	virtual Handle(Geom_Curve)	GetCurve();
	virtual Handle(Geom_Curve)	GetCurve(double& t0,double& t1);
	virtual const TopoDS_Edge	GetEdge() const; 
	BOOL						SetCurve(const Handle(Geom_Curve)& aCurve);
	BOOL						SetEdge(const TopoDS_Edge& aEdge);
	
	//��������
	virtual kcEntity*			Clone();

public:
	//�����Զ���׽����Ϣ
	virtual BOOL				CalcSnapPoint(kiSnapMngr& snapMgr);

protected:
	//
	virtual BOOL				DoAddAttrib(QxStgBlock& blk);
	virtual BOOL				DoParseAttrib(QxStgBlock& blk);
};

// һ�������
class QXLIB_API kcWireEntity : public kcEntity{
public:
	kcWireEntity();
	virtual ~kcWireEntity();

	//�Ƿ�պ�
	virtual bool				IsClosed();

	BOOL						SetWire(const TopoDS_Wire& aWire);
	const TopoDS_Wire			GetWire() const;

public:
	//�����Զ���׽����Ϣ
	virtual BOOL				CalcSnapPoint(kiSnapMngr& snapMgr);

public:
	//��������
	virtual kcEntity*			Clone();
protected:
	virtual BOOL				DoAddAttrib(QxStgBlock& blk);
	virtual BOOL				DoParseAttrib(QxStgBlock& blk);
};

// ����
class QXLIB_API kcFaceEntity : public kcEntity{
public:
	kcFaceEntity();
	virtual ~kcFaceEntity();

	BOOL						SetSurface(const Handle(Geom_Surface)& aSurf);
	BOOL						SetFace(const TopoDS_Face& aFace);
	const TopoDS_Face			GetFace() const;
	const Handle(Geom_Surface) GetSurface();

public:
	//�����Զ���׽����Ϣ
	virtual BOOL				CalcSnapPoint(kiSnapMngr& snapMgr);

protected:
	virtual BOOL				DoAddAttrib(QxStgBlock& blk);
	virtual BOOL				DoParseAttrib(QxStgBlock& blk);

public:
	//��������
	virtual kcEntity*			Clone();
};

// shell
class QXLIB_API kcShellEntity : public kcEntity{
public:
	kcShellEntity();
	virtual ~kcShellEntity();

	BOOL						SetShell(const TopoDS_Shell& aShell);
	const TopoDS_Shell		GetShell() const;

public:
	//�����Զ���׽����Ϣ
	virtual BOOL				CalcSnapPoint(kiSnapMngr& snapMgr);

public:
	//��������
	virtual kcEntity*			Clone();

protected:
	virtual void				DoSetFaceDisplayAttribs();

protected:
	virtual BOOL				DoAddAttrib(QxStgBlock& blk);
	virtual BOOL				DoParseAttrib(QxStgBlock& blk);
};

//��ʵ��
class QXLIB_API kcSolidEntity : public kcEntity{
public:
	kcSolidEntity();
	virtual ~kcSolidEntity();

	BOOL						SetSolid(const TopoDS_Solid& aSolid);
	const TopoDS_Solid		GetSolid() const;

public:
	//�����Զ���׽����Ϣ
	virtual BOOL				CalcSnapPoint(kiSnapMngr& snapMgr);

public:
	//��������
	virtual kcEntity*			Clone();

protected:
	virtual void				DoSetFaceDisplayAttribs();

protected:
	virtual BOOL				DoAddAttrib(QxStgBlock& blk);
	virtual BOOL				DoParseAttrib(QxStgBlock& blk);
};

//��ϣ�����������ϡ�
class QXLIB_API kcCompoundEntity : public kcEntity{
public:
	kcCompoundEntity();
	virtual ~kcCompoundEntity();

	BOOL						SetCompound(const TopoDS_Compound& aComp);
	const TopoDS_Compound		GetCompound() const;

public:
	//�����Զ���׽����Ϣ
	virtual BOOL				CalcSnapPoint(kiSnapMngr& snapMgr);

public:
	//��������
	virtual kcEntity*			Clone();

protected:
	virtual void				DoSetFaceDisplayAttribs();

protected:
	//
	virtual BOOL				DoAddAttrib(QxStgBlock& blk);
	virtual BOOL				DoParseAttrib(QxStgBlock& blk);
}; 

//��ϣ�����������ϡ�
class QXLIB_API kcBlockEntity : public kcEntity{
public:
	kcBlockEntity();
	virtual ~kcBlockEntity();

	// ��ȡ��Ӧ�ĵͲ����,���ؾֲ�,���ܹ��ⲿ�޸�.
	virtual TopoDS_Shape		GetShape();

	// ���һ�����󣬶��󽫼�¼�ڸö����ڲ�
	bool						AddEntity(kcEntity *pEntiey);
	const kcEntityList&		GetEntityList() const;

	const TopoDS_Compound		GetCompound() const;

public:
	//�����Զ���׽����Ϣ
	virtual BOOL				CalcSnapPoint(kiSnapMngr& snapMgr);

public:
	//��������
	virtual kcEntity*			Clone();

protected:
	// ���ݼ�¼�����ݺ����ԣ�����һ��ais��ʾ����
	// ��ʾ�����¼��m_aisObj��
	virtual BOOL				DoBuildDisplayObj();
	// ������ʾ��������
	virtual void				DoSetDisplayAttribs();

protected:
	void						DoSetEntityAttribs(kcEntity *pEnt);

protected:
	// �����������������м�¼������ֵ
	virtual bool				DoAddOtherCodeValue(QxStgBlock& blk);
	virtual bool				DoGetOtherCodeValue(QxStgBlock& blk);
	// ����������д�������Լ�������
	virtual bool				DoWriteOtherData(CArchive& ar);
	virtual bool				DoReadOtherData(CArchive& ar);

protected:
	kcEntityList				aEntList_; //��¼һϵ��Entity����
	kcEntityMap				aEntMap_;
	TopoDS_Compound			aComp_; //������ɵ�shape

protected:
	int						nReadEntNum_;//��ȡ��ent����
}; 

class QXLIB_API kcDimEntity : public kcEntity{
public:
	kcDimEntity();
	virtual ~kcDimEntity();

	//�Ƿ��ע����
	virtual bool		IsDimension() const { return true; }

	void				SetFlyout(double dValue);
	double				GetFlyout() const { return dFlyout_; }

	void				SetArrowLength(double len);
	double				GetArrowLength() const { return dArrowLen_; }

	void				SetFontHeight(double h);
	double				GetFontHeight() const { return dFontHeight_; }

protected:
	// �����������������м�¼������ֵ
	virtual bool		DoAddOtherCodeValue(QxStgBlock& blk);
	virtual bool		DoGetOtherCodeValue(QxStgBlock& blk);
	//
	bool				DoAddCirc(const gp_Circ &aCirc, QxStgBlock& blk);
	bool				DoGetCirc(QxStgBlock& blk, gp_Circ &aCirc);
	//
	bool				DoAddPlane(const gp_Pln &aPln, QxStgBlock& blk);
	bool				DoGetPlane(QxStgBlock& blk, gp_Pln &aPln);
	// ������ʾ��������
	virtual void		DoSetDisplayAttribs();

protected:
	double				dFlyout_;
	double				dArrowLen_;//��ͷ�ĳ���
	double				dFontHeight_; //����߶�
};


// ���ȱ�ע
class QXLIB_API kcLengthDimEntity : public kcDimEntity{
public:
	kcLengthDimEntity();
	~kcLengthDimEntity();

	bool			Initialize(const gp_Pnt &p1, const gp_Pnt &p2, const gp_Pln &pln);

protected:
	// ���ݼ�¼�����ݺ����ԣ�����һ��ais��ʾ����
	// ��ʾ�����¼��m_aisObj��
	virtual BOOL	DoBuildDisplayObj();

protected:
	// �����������������м�¼������ֵ
	virtual bool	DoAddOtherCodeValue(QxStgBlock& blk);
	virtual bool	DoGetOtherCodeValue(QxStgBlock& blk);

protected:
	gp_Pnt			aFirstPnt_;
	gp_Pnt			aSecondPnt_;
	gp_Pln			aPlane_;
};

class QXLIB_API kcDiameterDimEntity : public kcDimEntity{
public:
	kcDiameterDimEntity();
	~kcDiameterDimEntity();

	bool			Initialize(const gp_Circ &aCirc);

protected:
	// ���ݼ�¼�����ݺ����ԣ�����һ��ais��ʾ����
	// ��ʾ�����¼��m_aisObj��
	virtual BOOL	DoBuildDisplayObj();

protected:
	// �����������������м�¼������ֵ
	virtual bool	DoAddOtherCodeValue(QxStgBlock& blk);
	virtual bool	DoGetOtherCodeValue(QxStgBlock& blk);

protected:
	gp_Circ			aCirc_;
};

class QXLIB_API kcRadiusDimEntity : public kcDimEntity{
public:
	kcRadiusDimEntity();
	~kcRadiusDimEntity();

	bool			Initialize(const gp_Circ &aCirc,const gp_Pnt& pnt);

protected:
	// ���ݼ�¼�����ݺ����ԣ�����һ��ais��ʾ����
	// ��ʾ�����¼��m_aisObj��
	virtual BOOL	DoBuildDisplayObj();

protected:
	// �����������������м�¼������ֵ
	virtual bool	DoAddOtherCodeValue(QxStgBlock& blk);
	virtual bool	DoGetOtherCodeValue(QxStgBlock& blk);

protected:
	gp_Circ			aCirc_;
	gp_Pnt			aAnchorPnt_;
};


class QXLIB_API kcAngleDimEntity : public kcDimEntity{
public:
	kcAngleDimEntity();
	~kcAngleDimEntity();

	//
	bool			Initialize(const TopoDS_Edge &aE1, const TopoDS_Edge &aE2);
	bool			Initialize(const gp_Pnt &p1, const gp_Pnt &p2, const gp_Pnt &cp);

protected:
	// ���ݼ�¼�����ݺ����ԣ�����һ��ais��ʾ����
	// ��ʾ�����¼��m_aisObj��
	virtual BOOL	DoBuildDisplayObj();

protected:
	// �����������������м�¼������ֵ
	virtual bool	DoAddOtherCodeValue(QxStgBlock& blk);
	virtual bool	DoGetOtherCodeValue(QxStgBlock& blk);


protected:
	gp_Pnt			aFirstPnt_;
	gp_Pnt			aSecondPnt_;
	gp_Pnt			aCenterPnt_;
};

#endif