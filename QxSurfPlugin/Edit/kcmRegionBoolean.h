//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_REGION_BOOLEAN_H_
#define _KCM_REGION_BOOLEAN_H_

#include "kiCommand.h"

class kiInputEntityTool;
class kcSolidEntity;
class kcEntity;

class kcmRegionBoolean : public kiCommand
{
public:
	kcmRegionBoolean(void);
	virtual ~kcmRegionBoolean(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	virtual BOOL			DoBoolean();
	virtual TopoDS_Shape	DoBoolean(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2) = 0;

	BOOL					CheckInputShape(kiInputEntityTool *pTool);
	BOOL					GetSolidEntity(kiInputEntityTool *pTool,kcSolidEntity *&pEntity);
	BOOL					GetShapeAndEntity(kiInputEntityTool *pTool,kcEntity *&pEntity,TopoDS_Shape& aShape);

protected:
	kiInputEntityTool		*m_pInputEntity1;
	kiInputEntityTool		*m_pInputEntity2;
};

// ��
class kcmRegionBoolFuse : public kcmRegionBoolean{
public:
	kcmRegionBoolFuse();

protected:
	virtual TopoDS_Shape	DoBoolean(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2);

};

// ��
class kcmRegionBoolCommon : public kcmRegionBoolean{
public:
	kcmRegionBoolCommon();

protected:
	virtual TopoDS_Shape	DoBoolean(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2);

};

// ��
class kcmRegionBoolCut : public kcmRegionBoolean{
public:
	kcmRegionBoolCut();

protected:
	virtual TopoDS_Shape	DoBoolean(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2);

};

#endif