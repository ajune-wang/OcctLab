//////////////////////////////////////////////////////////////////////////
// ��ʱ��ʾ�ı���Ϣ�����ڿ�����ʾ��
//
#ifndef _KC_PREVIEW_TEXT_H_
#define _KC_PREVIEW_TEXT_H_

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include "QxLibDef.h"

class kvCoordSystem;
class kvWorkPlane;
class kcDocContext;

class QXLIB_API kcPreviewText
{
public:
	kcPreviewText(const Handle(AIS_InteractiveContext)& aCtx,kcDocContext *pDocCtx);
	~kcPreviewText(void);

	bool									SetAspectText(const Handle(Graphic3d_AspectText3d)& aAspText);
	Handle(Graphic3d_AspectText3d)			GetAspectText() ;

	//������������
	void									Set(const char *pszFont,double height,int color);
	void									SetHeight(double height);
	void									SetColor(double r,double g,double b);

	//��ʾ���塣
	void									Text(const char *pszText,int x,int y,double angle);

	void									Clear();

protected:
	void									Destroy();

protected:
	kcDocContext							*pDocContext_;//
	Handle(Graphic3d_Structure)			m_aStru;//��ʾ����
	Handle(Graphic3d_AspectText3d)			m_aAspText;
	
	double									m_dHeight;
};

#endif