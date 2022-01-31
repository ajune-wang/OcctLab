//////////////////////////////////////////////////////////////////////////
// �ı���صĹ��ܡ�
//
#ifndef _KCG_TEXTLIB_H_
#define _KCG_TEXTLIB_H_

#include <string>
#include <vector>
#include "kPoint.h"
#include "QxLibDef.h"

class CFont;

// ��ȡ����������,����������Ϣ����ȡ��Ӧ������������Ϣ��
// ������ȡ������Ϣ�������и�ʽ����Ҳ�������������н�һ���Ĳ�����
// ÿ�����ַ��صĶ���һϵ�е�Wire��
//
class QXLIB_API kcGlyphOutline{
public:
	kcGlyphOutline(HDC hDC,LPLOGFONT lf);
	~kcGlyphOutline();

	BOOL						IsValid() const;

	// ����һ����϶���,ÿ��������Compound����
	//
	bool						GetStringOutLine(LPCTSTR lpszStr,TopTools_SequenceOfShape& seqOfShape);

	// ��ȡһ���ַ�����������
	TopoDS_Compound				GetCharOutLine(char c);

	//��ȡһ���ַ�����������
	TopoDS_Compound				GetCharOutLine(WCHAR wc);

	CFont*						GetFont() const { return m_pFont; }

protected:
	TopoDS_Edge					BuildLine(const POINTFX& pfx);
	TopoDS_Edge					BuildQSpline(double x1,double y1,double x2,double y2);
	TopoDS_Edge					BuildCSpline(double x1,double y1,double x2,double y2,double x3,double y3);
	void						AddPoint(double x,double y);

	//��fixedֵתΪdouble
	double						FixedToDouble(const FIXED& fv);

protected:
	HDC							m_hDC;//
	LPLOGFONT					m_pLogFont;
	CFont						*m_pFont;
	HGDIOBJ						m_hOldFont;
	WCHAR						m_wcText;	
	std::vector<kPoint2>		m_pntArray;
};

//////////////////////////////////////////////////////////////////////////
//

class QXLIB_API kcFontText{
public:
	kcFontText(LOGFONT& lf);
	~kcFontText();

	// �޸���������
	void						SetLogFont(LOGFONT& lf);

	// ��ȡ�ַ�����Ӧ������
	// ���룺
	//   hDC:�豸�ֲ� lpszText���ַ��� dHeight�����ָ߶� dGap�����ּ�հ� 
	//   bWire:��ȡ������������
	// ������aList��������б�
	//       �����Ǹ�ʽ���õ����֣���Χ�����½���0,0,0��.
	//
	BOOL						GetText(HDC hDC,LPCTSTR lpszText,double dHeight,double dGap,BOOL bWire,TopTools_ListOfShape& aList);							

protected:
	//
	TopoDS_Compound				CreateFaceFromWire(const TopoDS_Compound& aOutLine);

protected:
	LOGFONT						m_logFont;
};

class kcText{
public:
	kcText(HDC hDC,LPLOGFONT lf);
	~kcText();

	//��Ҫ�ķ�������ȡ�ַ�����ʽ�����������face
	//bool						GetText(LPCTSTR lpszText,)

protected:
	kcGlyphOutline				m_aGlyphOutline;
};

//////////////////////////////////////////////////////////////////////////
//
class QXLIB_API kxCharFontText{
public:
	kxCharFontText(unsigned char b);
	~kxCharFontText();

public:
	unsigned char				m_uChar;
	TopoDS_Compound				m_aOutLine;
	TopoDS_Compound				m_aFace;
	Bnd_Box						m_aBndBox;//��Χ��
	double						m_dWidth;
	double						m_dHeight;
};

typedef std::vector<kxCharFontText *>	TCharFontTextList;

class QXLIB_API kcCharText{
public:
	kcCharText();
	~kcCharText();

	bool						Initialize(HDC hDC,LPLOGFONT lf);
	void						Destroy();
	bool						IsValid() const;

	//��ȡ�����ַ�������
	TopoDS_Compound				GetOutline(char c,double dHeight);
	//��ȡ�����ַ���face
	TopoDS_Compound				GetFace(char c,double dHeight);

	//��ȡ�����ַ���������face
	bool						GetText(const char *pStr,double dHeight,double dGap,bool bFace,TopTools_ListOfShape& aList);

protected:
	//
	TopoDS_Compound				CreateFaceFromWire(const TopoDS_Compound& aOutLine);

protected:
	kxCharFontText				*m_pCharText[256];//��¼ÿ���ַ���Ӧ���ı���Ϣ
};

QXLIB_API kcCharText&		glb_GetArialCharText();
QXLIB_API kcCharText&		glb_GetCourierCharText();

QXLIB_API bool			glb_InitArialCharText(HDC hDC);
QXLIB_API bool			glb_InitCourierCharText(HDC hDC);



#endif