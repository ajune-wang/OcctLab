//����ͳ��
#ifndef _KCM_SHAPE_STATISTIC_H_
#define _KCM_SHAPE_STATISTIC_H_

#include "kiCommand.h"

class kiOptionItem;
class kiOptionSet;

class kcmShapeStatistic : public kiCommand{
public:
	kcmShapeStatistic();
	virtual ~kcmShapeStatistic();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

public:
	//ѡ�ѡ����
	virtual void			OnOptionSelected(kiOptionItem *pOptItem);

protected:
	virtual int			OnExecute();

protected:
	void					DoCollectSurfTypeInfo();
	void					DoCollectCurveTypeInfo();
	void					DoCountSurfInfo();
	//
	void					DoGetOrientStat();

protected:
	// ������Ҫ�����빤��,��Initialize�е��ã�ÿ�������������һ�Ρ�
	virtual	BOOL			CreateInputTools();
	// ���ٴ��������빤��.ÿ�����������һ��.
	virtual BOOL			DestroyInputTools();
	// ��begin�е��ã���Ҫ�ǳ�ʼ�����߶��С�
	virtual BOOL			InitInputToolQueue();

protected:
	kiOptionSet			*_pOptionSet;
};

#endif