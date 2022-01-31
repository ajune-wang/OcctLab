//////////////////////////////////////////////////////////////////////////
//
#ifndef _KCM_SHAPE_ANALYSIS_H_
#define _KCM_SHAPE_ANALYSIS_H_

#include <vector>
#include <string>
#include "kColor.h"
#include "kiCommand.h"
#include "kiOptionSet.h"

class kiInputEntityTool;
class kcDispFrameArrow;
class kcDispSolidArrow;
class kcPreviewLine;
class kcPreviewNurbsPoles;
class kcPreviewSegments;
class kcPreviewMesh;
class kcPreviewCurve;

class kcmShapeAnalysisNormal : public kiCommand
{
public:
	kcmShapeAnalysisNormal(void);
	virtual ~kcmShapeAnalysisNormal(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL		CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

	//ѡ�ѡ����
	virtual void			OnOptionSelected(kiOptionItem *pOptItem);

protected:
	virtual int			OnExecute();
	virtual int			OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	BOOL					CalcNormal();

	void					ClearArrowDisp();

protected:
	kiInputEntityTool		*m_pInputEntity;
	kiOptionSet			m_optionSet;
	bool					m_bTopoNormal;

	std::vector<kcDispSolidArrow *>	m_arrowList;
};

// ��������ķ�ʧ�Ͳü����ķ���
//
class kcmShapeAnalysisFaceOrient : public kiCommand
{
public:
	kcmShapeAnalysisFaceOrient(void);
	virtual ~kcmShapeAnalysisFaceOrient(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int			OnExecute();
	virtual int			OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	BOOL					ShowFaceWiresAndNorm();
	BOOL					ShowWiresAndNorm(const TopoDS_Face& aFace);

	void					ClearArrowDisp();

protected:
	kiInputEntityTool		*m_pInputEntity;
	kiInputEntityTool		*m_pInputFace;

	kiOptionSet			m_optionSet;
	bool					m_bTopoNormal;//���˷�ʧ
	bool					m_bUseWireOrit;//�Ƿ����ϻ��ķ���
	bool					m_bCorrectWireOrder;//�Ƿ��������ķ���

	std::vector<kcDispSolidArrow *>	  m_arrowList;
};

//��ʾ�������ϸ��Ϣ
class kiFacePropInfo : public kiCommand
{
public:
	kiFacePropInfo();
	virtual ~kiFacePropInfo();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	BOOL					ShowFacePropInfo();

protected:
	kiInputEntityTool		*m_pInputFace;

	kiOptionSet			m_optionSet;
	bool					bCumOri_;//�Ƿ��Ǹ�������
};

//��ʾ����Ĳü��������ǻ򲻿���face����.���ǻ򲻿���edge����.
class kiFaceWireOri : public kiCommand
{
public:
	kiFaceWireOri();
	virtual ~kiFaceWireOri();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	BOOL					ShowFaceWireOri();

	void					BuildSurfNormArrow(const TopoDS_Face& aFace);
	void					BuildDispArrow(const TopoDS_Edge& aE,const TopoDS_Face& aF,bool bCumEdgeOri);
	void					ClearArrowDisp();

	kcDispSolidArrow*		BuildDispArrow(const gp_Pnt& pnt,const gp_Vec& dire,double arrlen,const kColor color);

protected:
	kiInputEntityTool		*m_pInputFace;

	kiOptionSet			m_optionSet;
	bool					bCumFaceOri_;//�Ƿ���face����
	bool					bCumEdgeOri_;//�Ƿ���Edge����

	std::vector<kcDispSolidArrow *> sldArrowVec_;//��ͷ��ʾ����
};

//��ʾ���Ƶ�
class kiDisplayNbsPoles : public kiCommand{
public:
	kiDisplayNbsPoles();
	~kiDisplayNbsPoles();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int			OnExecute();
	virtual int			OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

	bool					DoShowPoles();

protected:
	kiInputEntityTool		*pPickEntTool_;
	kcPreviewNurbsPoles	*pPreviewPoles_;
};

//������������
class kiBatAnalyze : public kiCommand{
public:
	kiBatAnalyze();
	virtual ~kiBatAnalyze();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

protected:
	virtual int				OnExecute();
	virtual int				OnEnd(int nCode);

protected:
	bool					GetStepFileList(const CString &szFolder);

protected:
	std::string				strPath_;
	std::vector<std::string>	stepFileList_;//�ļ��б�
};

//����������ʾ
class kcmCurveCurvature : public kiCommand{
public:
	kcmCurveCurvature();
	~kcmCurveCurvature();

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL			CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

	//ѡ��ı���
	virtual void			OnOptionChanged(kiOptionItem *pOptItem);

protected:
	virtual int			OnExecute();
	virtual int			OnEnd(int nCode);
	
protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

	bool					DoShowCurvature();
	bool					DoBuildDispData();

protected:
	kiInputEntityTool		*pPickEntTool_;
	kcPreviewSegments		*pPreviewSegs_;

	kiOptionSet			aOptionSet_;

	int					nbSamp_;//���������
	double				dScale_;//���ű���

private:
	std::vector<TopoDS_Shape>	aShapeAry_;
};

//�ʷ�������ʾ
class kcmMeshDisplay : public kiCommand{
public:
	kcmMeshDisplay();
	~kcmMeshDisplay();

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

protected:
	virtual int			OnExecute();
	virtual int			OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	bool					DoDispMesh();

protected:
	kiInputEntityTool		*pPickEntTool_;
	kcPreviewMesh			*pPreivewMesh_;

	kiOptionSet			aOptionSet_;	
};

//���淽����ʾ
class kcmFaceOrientAnalysis : public kiCommand{
public:
	kcmFaceOrientAnalysis();
	~kcmFaceOrientAnalysis();

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

	// ��������ı�ʱ������øûص�����
	virtual  int			OnInputChanged(kiInputTool *pTool);

	//ѡ�ѡ����
	virtual void			OnOptionSelected(kiOptionItem *pOptItem);

protected:
	virtual int			OnExecute();
	virtual int			OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	//
	void					ShowUVOrient();

protected:
	kiInputEntityTool		*pPickFaceTool_;
	kiOptionSet			aOptionSet_;

	kcPreviewCurve		*pUIsoPreviewCurve_;
	kcPreviewCurve		*pVIsoPreviewCurve_;
	kcDispSolidArrow		*pUIsoCrvArrow_;
	kcDispSolidArrow		*pVIsoCrvArrow_;
};

class kcmFaceNormalAnalysis : public kiCommand
{
public:
	kcmFaceNormalAnalysis(void);
	virtual ~kcmFaceNormalAnalysis(void);

	// ��������������,��Ҫ��Apply�������ʹ��.
	virtual  BOOL		CanFinish();//�����Ƿ�������

	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
	//
	virtual  int			OnInputFinished(kiInputTool *pTool);

	//ѡ�ѡ����
	virtual void			OnOptionSelected(kiOptionItem *pOptItem);

protected:
	virtual int			OnExecute();
	virtual int			OnEnd(int nCode);

protected:
	// ������Ҫ�����빤��
	virtual	BOOL			CreateInputTools();
	virtual BOOL			DestroyInputTools();
	virtual BOOL			InitInputToolQueue();

protected:
	BOOL					DisplayNormal();

protected:
	kiInputEntityTool		*m_pInputEntity;
	kiOptionSet			m_optionSet;
	bool					m_bTopoNormal;

	kcDispSolidArrow		*pNormalArrow_;//Ԥ����ͷ
};

////����������ʾ
//class kcmCurveCurvature : public kiCommand{
//public:
//	kcmCurveCurvature();
//	~kcmCurveCurvature();
//
//	// ��������������,��Ҫ��Apply�������ʹ��.
//	virtual  BOOL			CanFinish();//�����Ƿ�������
//
//	// �����빤�߻�ȡ���,����ʱ,��ص���Ӧ����ĸú���.
//	// ���ڼ���ִ��������л����빤��.Ҳ���ܷ���KSTA_DONE,��������ִ�н���.
//	//
//	virtual  int			OnInputFinished(kiInputTool *pTool);
//
//protected:
//	virtual int			OnExecute();
//	virtual int			OnEnd(int nCode);
//
//protected:
//	// ������Ҫ�����빤��
//	virtual	BOOL			CreateInputTools();
//	virtual BOOL			DestroyInputTools();
//	virtual BOOL			InitInputToolQueue();
//
//	bool					DoShowCurvature();
//
//protected:
//	kiInputEntityTool		*pPickEntTool_;
//	kcPreviewSegments		*pPreviewSegs_;
//
//	kiOptionSet			aOptionSet_;
//
//	int					nbSamp_;//���������
//	double				dScale_;//���ű���
//};

#endif