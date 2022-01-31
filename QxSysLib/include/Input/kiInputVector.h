//////////////////////////////////////////////////////////////////////////
// ����һ������
#ifndef _KI_INPUT_VECTOR_H_
#define _KI_INPUT_VECTOR_H_

#include "kPoint.h"
#include "kVector.h"
#include "kiInputTool.h"
#include "kiOptionSet.h"


//����ʸ�������͡�
typedef enum _enumInputVectorType{
	eInputWorkPlaneVector, //���빤��ƽ���ڵ�ʸ��
	eInputXVector, //X��ʸ��,��Ե�ǰ����ƽ����˵
	eInputYVector, //Y��ʸ��,��Ե�ǰ����ƽ����˵
	eInputZVector, //Z��ʸ��,��Ե�ǰ����ƽ����˵
	eInputSpaceVector //�ռ��е�ʸ��,����ʸ��
}eInputVectorType;

class kiInputPoint;
class kcPreviewLine;

class QXLIB_API kiInputVector : public kiInputTool
{
public:
	kiInputVector(kiCommand *pCommand,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);//��ѡ��
	kiInputVector(kiInputTool *pParent,const char *pszPrompt,kiOptionSet *pOptionSet = NULL);

	virtual ~kiInputVector(void);

	//��������
	void					SetType(eInputVectorType eType);
	void					FixVectorDir(eInputVectorType eType);//�̶���һ��������
	void					SetOrgPoint(const kPoint3& org);//�趨����ʼ��
	void					SetPrompt(const char *pszPrompt1,const char *pszPrompt2);//����������ʾ�ַ���

	// ��ʼ�����ִ��
	virtual int				Begin();
	// ���������ִ��,nCode������Ľ��.
	virtual int				End();

	// ��tool�Ļص�����,����tool���ý���ʱ,����.
	virtual int				OnSubInputFinished(kiInputTool *pTool);

	// ��ѡ��ı��ĵ��ú���
	virtual void			OnOptionChanged(kiOptionItem *pOptionItem);

public:
	//�����Ϣ
	virtual int				OnLButtonUp(kuiMouseInput& mouseInput); 
	virtual int				OnMouseMove(kuiMouseInput& mouseInput);

public:
	bool					GetPoint(kPoint3& point) const;
	bool					GetVector(kVector3& vec) const;

protected:
	void					Reset();
	void					Destroy();

	bool					CreateInputTool();

	//��������
	bool					CalcVector(kuiMouseInput& mouseInput);

protected:
	eInputVectorType		m_eType;//��������
	bool					m_bFixedDir;//�Ƿ�̶�����
	bool					m_bSetOrgPoint;//�Ƿ��趨����ʼ��.
	std::string				m_strPrompt1,m_strPrompt2;
	int						m_nVecType;
	kPoint3					m_point;
	kVector3				m_vec;//����ĵ������

protected:
	int						m_nState;//״̬
	kiInputPoint			*m_pInputOrgPoint;
	kiInputPoint			*m_pInputVecPoint;
	kPoint3					m_aOrg;
	kPoint3					m_aDirPnt;
	kiOptionSet				m_dirOptionSet;//�������ѡ��
	kiOptionEnum			*m_pOptionType;
	kiOptionDouble			*m_pOptionLength;
	double					m_dLength;
	kcPreviewLine			*m_pPreviewLine;
};

#endif