#ifndef _KCM_ONE_FACE_BASE_H_
#define _KCM_ONE_FACE_BASE_H_

#include <vector>
#include <TopoDS_Face.hxx>
#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;
class kiInputIntTool;

class kcmOneFaceBase : public kiCommand
{
public:
	kcmOneFaceBase(void);
	virtual ~kcmOneFaceBase(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

	virtual int				OnSelOneFace() = 0;//�麯������Ҫʵ��

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	TopoDS_Face				GetFaceFromGlobalSelSet();
	int						AddShapeToModel(const TopoDS_Shape& aShape);

protected:
	kiInputEntityTool		*m_pSelFaceTool;

	TopoDS_Face				m_aFace;//ѡ�е�face
};

class kcmOneFaceCopy : public kcmOneFaceBase
{
public:
	kcmOneFaceCopy();
	virtual ~kcmOneFaceCopy();

protected:
	virtual int				OnSelOneFace();
};

// ��ȡһ��face�Ĳü����Ĳ�������
class kcmOneFaceParLoop : public kcmOneFaceBase
{
public:
	kcmOneFaceParLoop();
	virtual ~kcmOneFaceParLoop();

protected:
	virtual int				OnSelOneFace();
};

//��һ��������в�ֵ����ȡ��ֵ�����
class kcmInterpOneFace : public kcmOneFaceBase
{
public:
	kcmInterpOneFace();
	virtual ~kcmInterpOneFace();

protected:
	virtual int				OnSelOneFace();
};

//����һ��ѡ�е���
class kcmExpOneFaceToStep : public kcmOneFaceBase
{
public:
	kcmExpOneFaceToStep();
	virtual ~kcmExpOneFaceToStep();

protected:
	virtual int				OnSelOneFace();
};

//�������������һЩ����
class kcmPeriodicBSpSurfTry : public kcmOneFaceBase
{
public:
	kcmPeriodicBSpSurfTry(int subcmd);
	virtual ~kcmPeriodicBSpSurfTry();

protected:
	virtual int				OnSelOneFace();

private:
	void						ToNoPeriodicSurf(const Handle(Geom_BSplineSurface) &aBspSurf);
	void						ChangeUVOrigin(const Handle(Geom_BSplineSurface) &aBspSurf,int uvflag);

protected:
	int						subCmd_;//������
};

//��ǰ��n����
class kcmExtractNFace : public kiCommand{
public:
	kcmExtractNFace();
	virtual ~kcmExtractNFace();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL				CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int				OnInputFinished(kiInputTool *pTool);

protected:
	// ������Ҫ�����빤��
	virtual BOOL				CreateInputTools();
	virtual BOOL				DestroyInputTools();
	virtual BOOL				InitInputToolQueue();

protected:
	BOOL						DoExtractFace();

protected:
	kiInputEntityTool			*_pInputEntityTool;
	kiInputIntTool			*_pInputNumTool;
	int						_nFaceNo;//face�����
};


#endif