/////////////////////////////////////////////////////
// ��׼���������ʵ��
//
#pragma once

#include "kiCommand.h"

class kiInputDirTool;
class kiInputDoubleTool;
class kiInputPointTool;
class kiInputEntityTool;

//ƫ�����ɻ�׼��
class kcmOffsetBasePlane : public kiCommand{
public:
	kcmOffsetBasePlane();
	virtual ~kcmOffsetBasePlane();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
	virtual	BOOL			CreateInputTools();
	// ���ٴ��������빤��.ÿ�����������һ��.
	virtual BOOL			DestroyInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL			InitInputToolQueue();

protected:
	BOOL					CreateBasePlane();

protected:
	kiInputDirTool		*_pInputDirTool;
	kiInputDoubleTool		*_pInputOffsetTool;

	kVector3				_offDire;
	double				_dOffset;
};

///�������ɻ�׼��
class kcmThreePntBasePlane : public kiCommand{
public:
	kcmThreePntBasePlane();
	virtual ~kcmThreePntBasePlane();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
	virtual	BOOL			CreateInputTools();
	// ���ٴ��������빤��.ÿ�����������һ��.
	virtual BOOL			DestroyInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL			InitInputToolQueue();

protected:
	BOOL					CreateBasePlane();

protected:
	kiInputPointTool		*pInputPointTool1_;
	kiInputPointTool		*pInputPointTool2_;
	kiInputPointTool		*pInputPointTool3_;

	kPoint3				pnt1_,pnt2_,pnt3_;
};


// ����һ��Face������һ�㣬���ɻ�׼��
class kcmBasePlaneByFace : public kiCommand{
public:
	kcmBasePlaneByFace();
	virtual ~kcmBasePlaneByFace();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	// ִ�к���
	virtual int			OnExecute();

	// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
	virtual	BOOL			CreateInputTools();
	// ���ٴ��������빤��.ÿ�����������һ��.
	virtual BOOL			DestroyInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL			InitInputToolQueue();

protected:
	BOOL					CreateBasePlane();

protected:
	kiInputEntityTool		*pFacePickTool_; //ʰȡƽ��Ĺ���
};
