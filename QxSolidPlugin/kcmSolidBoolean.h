//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SOLID_BOOLEAN_H_
#define _KCM_SOLID_BOOLEAN_H_

#include <vector>
#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;
class kcSolidEntity;
class kcEntity;

class kcmSolidBoolean : public kiCommand
{
public:
	kcmSolidBoolean(void);
	virtual ~kcmSolidBoolean(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL				CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int				OnInputFinished(kiInputTool *pTool);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL				CreateInputTools();
	virtual BOOL				DestroyInputTools();
	virtual BOOL				InitInputToolQueue();

protected:
	virtual BOOL				DoBoolean();
	virtual TopoDS_Shape		DoBoolean(const TopoDS_Shape& aS1,const TopTools_ListOfShape& aLS) = 0;

	BOOL						GetSolidEntity(kiInputEntityTool *pTool,kcSolidEntity *&pEntity);
	BOOL						GetShapeAndEntity(kiInputEntityTool *pTool,kcEntity *&pEntity,TopoDS_Shape& aShape);
	BOOL						GetShapesAndEntities(kiInputEntityTool *pTool,std::vector<kcEntity *>& entList,TopTools_ListOfShape& aLS);

	TopoDS_Shape				MakeHalfSolid(const TopoDS_Face& aFace,bool bRev);

protected:
	kiInputEntityTool			*_pInputEntity1;
	kiInputEntityTool			*_pInputEntity2;

	kiOptionSet				_optionSet;
	bool						_bParallel;//�Ƿ�ʹ�ò���ִ��
	bool						_bAsHalfSpace;//face��shell�Ƿ���Ϊ��ռ�

protected:
	bool						bKeepTools_;//����tools����ɾ��
};

// ��
class kcmSolidBoolFuse : public kcmSolidBoolean{
public:
	kcmSolidBoolFuse();

protected:
	virtual TopoDS_Shape	DoBoolean(const TopoDS_Shape& aS1,const TopTools_ListOfShape& aLS);

};

// ��
class kcmSolidBoolCommon : public kcmSolidBoolean{
public:
	kcmSolidBoolCommon();

protected:
	virtual TopoDS_Shape	DoBoolean(const TopoDS_Shape& aS1,const TopTools_ListOfShape& aLS);

};

// ��
class kcmSolidBoolCut : public kcmSolidBoolean{
public:
	kcmSolidBoolCut();

protected:
	virtual TopoDS_Shape	DoBoolean(const TopoDS_Shape& aS1,const TopTools_ListOfShape& aLS);

};

//�ָ�
class kcmSolidSplit : public kcmSolidBoolean{
public:
	kcmSolidSplit();

protected:
	virtual TopoDS_Shape	DoBoolean(const TopoDS_Shape& aS1,const TopTools_ListOfShape& aLS);
};


#endif