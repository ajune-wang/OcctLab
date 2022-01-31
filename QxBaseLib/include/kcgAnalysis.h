#pragma once

#include <vector>
#include <map>
#include <string>

#include "QxLibDef.h"

class kcxCurveTypeInfo;
class kcxSurfTypeInfo;
typedef std::map<std::string,kcxCurveTypeInfo *> stdCurveTypeInfoMap;
typedef std::map<std::string,kcxSurfTypeInfo *> stdSurfTypeInfoMap;

// һ���������ļ�¼
class QXLIB_API kcxCrossRec{
public:
	kcxCrossRec();
	kcxCrossRec(double dF,double dL,double dMin,double dMax);

	void						Reset();

public:
	double					dFirst_,dLast_;
	double					dMin_,dMax_;
};

//ͳ�����ߵ�������Ϣ
class QXLIB_API kcxCurveTypeInfo{
public:
	kcxCurveTypeInfo();
	kcxCurveTypeInfo(const std::string &szName);

	void						Reset();

	void						Collect(const Handle(Geom_Curve) &aCurve,const TopoDS_Edge &aEdge);
	std::string				ToString();

public:
	std::string				sTypeName_;//��������
	int						nTotalNum_;//�ܸ���
	int						nbPeriodic_;//���ڸ���
	int						nbCrossEnd_;//�������߿����Ĵ���
	std::vector<kcxCrossRec>	aCrossRecVec_;
};

//ͳ�����������
class QXLIB_API kcxSurfTypeInfo{
public:
	kcxSurfTypeInfo();
	kcxSurfTypeInfo(const std::string &szName);

	void						Reset();

	void						Collect(const Handle(Geom_Surface) &aSurf,const TopoDS_Face &aFace);

	std::string				ToString();

public:
	std::string				strTypeName_;//��������
	int						nCount_;//����
	int						nbUPeriodic_,nbVPeriodic_,nbUVPeriodic_;//��������ĸ���
	int						nbUCrossSeam_,nbVCrossSeam_;//�ü��������ڱ߽�ĸ���
	std::vector<kcxCrossRec>	uCrossRecVec_,vCrossRecVec_;
};

class QXLIB_API kcgAnalysis
{
public:
	kcgAnalysis(void);
	~kcgAnalysis(void);

	void						CollectCurveTypeInfo(const TopoDS_Shape &aShape);
	void						CollectSurfTypeInfo(const TopoDS_Shape &aShape);

	void						Clear();

	bool						WriteToFile(const char *pszFile);

protected:
	void						CollectEdgeCurveTypeInfo(const TopoDS_Edge &aEdge);
	void						GetCurveTypeName(const Handle(Geom_Curve) &aCurve,std::string &sTypeName,std::string &sTypeFixName);

	kcxCurveTypeInfo*			GetCurveTypeInfo(const std::string &sName);

protected:
	void						CollectSurfTypeInfo(const Handle(Geom_Surface) &aSurf,const TopoDS_Face &aFace);
	void						GetSurfTypeName(const Handle(Geom_Surface) &aSurf,std::string &sTypeName,std::string &sTypeFixName);

	kcxSurfTypeInfo*			GetSurfTypeInfo(const std::string &sName);

protected:
	stdCurveTypeInfoMap		aCurveTypeInfoMap_;
	stdSurfTypeInfoMap		aSurfTypeInfoMap_;
};


