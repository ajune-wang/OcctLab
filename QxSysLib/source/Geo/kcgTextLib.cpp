#include "StdAfx.h"
#include <BRepBndLib.hxx>
#include <Bnd_SeqOfBox.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include "kBase.h"
#include "QxBRepLib.h"
#include "QxSurfLib.h"
#include "kcgTextLib.h"

#pragma warning(disable : 4996)

kcGlyphOutline::kcGlyphOutline(HDC hDC,LPLOGFONT lf)
{
	ASSERT(hDC && lf);
	m_pFont = NULL;
	m_hOldFont = NULL;
	m_hDC = NULL;
	m_pLogFont = NULL;
	
	if(hDC && lf)
	{
		m_pFont = new CFont;
		if(m_pFont && m_pFont->CreateFontIndirect(lf))
		{
			m_hDC = hDC;
			m_pLogFont = lf;

			//ѡ��
			m_hOldFont = ::SelectObject(hDC,m_pFont->GetSafeHandle());
		}
	}
}

kcGlyphOutline::~kcGlyphOutline()
{
	if(m_hDC && m_pFont && m_hOldFont)
	{
		::SelectObject(m_hDC,m_hOldFont);

		m_pFont->DeleteObject();
		delete m_pFont;
	}
}

BOOL	kcGlyphOutline::IsValid() const
{
	if(!m_hDC) return FALSE;
	return TRUE;
}

// ����һ����϶���
//
bool	kcGlyphOutline::GetStringOutLine(LPCTSTR lpszStr,TopTools_SequenceOfShape& seqOfShape)
{
	ASSERT(lpszStr);
	ASSERT(m_hDC);
	if(!lpszStr || !m_hDC) 
		return false;

	int ix,nC = 0;
	WCHAR wBuf[256];

	int nBufSize = ::MultiByteToWideChar(CP_ACP,0,lpszStr,-1,NULL,0);
	if(nBufSize <= 0)//�����˽�β��0��
		return false;

	WCHAR *pwcBuf = NULL;
	if(nBufSize < 256)
		pwcBuf = wBuf;
	else
		pwcBuf = new WCHAR[nBufSize + 1];

	::MultiByteToWideChar(CP_ACP,0,lpszStr,-1,pwcBuf,nBufSize + 1);

	ix = 0;
	while(pwcBuf[ix])
	{
		TopoDS_Shape aS = GetCharOutLine(pwcBuf[ix]);
		if(!aS.IsNull())
		{
			seqOfShape.Append(aS);
			nC ++;
		}

		ix ++;
	}

	if(nBufSize >= 256)
		delete[] pwcBuf;

	return (nC > 0 ? true : false);
}

// ��������ַ���ָ��ͳ���,Ӣ����ĸͨ��1��byte,����2��byte
//
TopoDS_Compound	kcGlyphOutline::GetCharOutLine(char c)
{
	TopoDS_Compound aC;

	ASSERT(m_hDC);
	if(!m_hDC) 
		return aC;

	//�Ƚ��ַ���ת��Unicode
	WCHAR wcBuf[8];
	char buf[2];
	buf[0] = c;buf[1] = '\0';

	MultiByteToWideChar( CP_ACP, 0, buf,
		-1, wcBuf,   
		sizeof(wcBuf)/sizeof(wcBuf[0]));

	aC = GetCharOutLine(wcBuf[0]);

	return aC;
}

// ��ȡһ���ַ�����������
// �����Ѿ�װ��.
TopoDS_Compound		kcGlyphOutline::GetCharOutLine(WCHAR wc)
{
	TopoDS_Compound aC;
	if(wc == 0)
		return aC;

	GLYPHMETRICS gm;
	MAT2 mat;

	memset(&mat, 0, sizeof(mat));
	mat.eM11.value = 1;
	mat.eM12.value = 0;
	mat.eM21.value = 0;
	mat.eM22.value = 1;

	//��ȡ��Ӧ��������Ϣ
	DWORD dwSize = ::GetGlyphOutlineW(m_hDC,wc,GGO_NATIVE,&gm,0,NULL,&mat);
	if(dwSize == 0 || dwSize == GDI_ERROR)
	{
		DWORD err = GetLastError();
		ASSERT(FALSE);
		return aC;
	}

	//��������
	BRep_Builder BB;
	BB.MakeCompound(aC);

	//���λ�ȡÿ������
	TopoDS_Edge aE;

	BYTE *pbOutline = new BYTE[dwSize];
	DWORD dwDataSize = ::GetGlyphOutlineW(m_hDC,wc,GGO_NATIVE,&gm,dwSize,pbOutline,&mat);

	//�Ի�ȡ��Ϣ���н���
	const TTPOLYGONHEADER *pHeader = (const TTPOLYGONHEADER *)pbOutline;
	int nTolSize = dwDataSize;
	int iC = 0;
	while(nTolSize > sizeof(TTPOLYGONHEADER))
	{
		int nCurSize = pHeader->cb;

		nTolSize -= nCurSize;
		if(nTolSize < 0)
		{
			ASSERT(FALSE);
			break;
		}

		//���������ֵ��
		if(pHeader->dwType != TT_POLYGON_TYPE)
		{
			ASSERT(FALSE);
			break;
		}

		TopoDS_Shape aW;
		BRepBuilderAPI_MakeWire mw;
		int nbEdge = 0;

		//KTRACE("\n new counter %d.--",iC);
		m_pntArray.clear();

		//�������ߣ�����wire
		try
		{
			const POINTFX * p = (const POINTFX *) & pHeader->pfxStart;//��ȡ��ʼ��

			AddPoint(FixedToDouble(p[0].x), FixedToDouble(p[0].y));
			const TTPOLYCURVE *pCurve = (const TTPOLYCURVE *)&pHeader[1];//pHeader�����POLYCURVE�ṹ/
			while((BYTE *)pCurve < (BYTE *)(((LPSTR)pHeader) + nCurSize))
			{
				int i;
				p = (const POINTFX *)pCurve->apfx;

				//�����CURVE.
				switch(pCurve->wType)
				{
				case TT_PRIM_LINE://polyline
					{
						//KTRACE("\n -- PRIM_LINE--");
						for(i = 0;i < pCurve->cpfx;i ++)
						{
							aE = BuildLine(p[i]);
							if(!aE.IsNull())
							{
								nbEdge ++;
								mw.Add(aE);
							}
						}
					}
					break;
				case TT_PRIM_QSPLINE://
					{
						//KTRACE("\n -- PRIM_QSPLINE --");
						ASSERT(pCurve->cpfx >= 2);
						//KTRACE("\n curve cpfx is %d.",pCurve->cpfx);
						for(i = 0;i < pCurve->cpfx - 1;i ++)
						{
							if(i == pCurve->cpfx - 2)
							{
								aE = BuildQSpline(FixedToDouble(p[i].x),FixedToDouble(p[i].y),
									FixedToDouble(p[i+1].x),FixedToDouble(p[i+1].y));
							}
							else
							{
								double xi = FixedToDouble(p[i].x),xip = FixedToDouble(p[i+1].x);
								double yi = FixedToDouble(p[i].y),yip = FixedToDouble(p[i+1].y);
								aE = BuildQSpline(xi,yi,(xi + xip)/2,(yi + yip)/2);
							}
							if(!aE.IsNull())
							{
								nbEdge ++;
								mw.Add(aE);
							}
						}
					}
					break;
				case TT_PRIM_CSPLINE:
					{
						//KTRACE("\n TT_PRIM_CSPLINE");
						ASSERT((pCurve->cpfx % 3) == 0);
						for(i = 0;i < pCurve->cpfx;i+=3)
						{
							aE = BuildCSpline(FixedToDouble(p[i].x),FixedToDouble(p[i].y),
								FixedToDouble(p[i+1].x),FixedToDouble(p[i+1].y),
								FixedToDouble(p[i+2].x),FixedToDouble(p[i+2].y));

							if(!aE.IsNull())
							{
								nbEdge ++;
								mw.Add(aE);
							}
						}
					}
					break;
				default:
					break;
				}

				//KTRACE("\n curve add %d edge.",nbEdge);

				//��ȡ��һ��CURVE��
				pCurve = (const TTPOLYCURVE *)&pCurve->apfx[pCurve->cpfx];
			}

			const kPoint2& ph = m_pntArray.front();
			const kPoint2& pt = m_pntArray.back();
			if(ph.distance(pt) > KDBL_MIN)
			{
				try
				{
					BRepBuilderAPI_MakeEdge me(gp_Pnt(pt.x(),pt.y(),0),
						gp_Pnt(ph.x(),ph.y(),0));
					if(me.IsDone())
					{
						nbEdge ++;
						aE = me.Edge();
					}
				}
				catch (Standard_Failure){

				}
				if(!aE.IsNull())
				{
					mw.Add(aE);
				}
			}

			if(mw.IsDone())
			{
				aW = mw.Wire();
				if(!aW.IsNull())
					BB.Add(aC,aW);
			}
		}catch(Standard_Failure){

		}

		//KTRACE("\n contour %d add %d edge.",iC,nbEdge);
		iC ++;
		pHeader = (const TTPOLYGONHEADER *)((BYTE *)pHeader + nCurSize);
	}

	//KTRACE("\n %d coutour.----------",iC);

	delete [] pbOutline;

	return aC;
}

TopoDS_Edge		kcGlyphOutline::BuildLine(const POINTFX& pfx)
{
	TopoDS_Edge aE;
	double x,y;
	x = FixedToDouble(pfx.x);
	y = FixedToDouble(pfx.y);
	const kPoint2&  p0 = m_pntArray.back();//���һ��Ԫ�ء�
	try
	{
		BRepBuilderAPI_MakeEdge me(gp_Pnt(p0.x(),p0.y(),0),
			gp_Pnt(x,y,0));
		if(me.IsDone())
			aE = me.Edge();
	}
	catch (Standard_Failure){	
	}
	
	AddPoint(x,y);

	return aE;
}

TopoDS_Edge		kcGlyphOutline::BuildQSpline(double x1,double y1,double x2,double y2)
{
	const kPoint2& p0 = m_pntArray.back();
	return BuildCSpline((p0.x() + 2 * x1)/3,(p0.y() + 2*y1)/3,(2*x1+x2)/3,(2*y1+y2)/3,x2,y2);
}

TopoDS_Edge		kcGlyphOutline::BuildCSpline(double x1,double y1,double x2,double y2,double x3,double y3)
{
	TopoDS_Edge aE;
	const kPoint2& p0 = m_pntArray.back();

	TColgp_Array1OfPnt arpnt(1,4);
	arpnt.SetValue(1,gp_Pnt(p0.x(),p0.y(),0.0));
	arpnt.SetValue(2,gp_Pnt(x1,y1,0.0));
	arpnt.SetValue(3,gp_Pnt(x2,y2,0.0));
	arpnt.SetValue(4,gp_Pnt(x3,y3,0.0));

	try
	{
		Handle(Geom_BezierCurve) aCur = new Geom_BezierCurve(arpnt);
		if(!aCur.IsNull())
		{
			BRepBuilderAPI_MakeEdge me(aCur);
			if(me.IsDone())
				aE = me.Edge();
		}
	}
	catch (Standard_Failure){
		
	}

	AddPoint(x1,y1);
	AddPoint(x2,y2);
	AddPoint(x3,y3);

	return aE;
}

void	kcGlyphOutline::AddPoint(double x,double y)
{
	//KTRACE("\n point x=%f,y=%f",x,y);
	kPoint2 p(x,y);
	m_pntArray.push_back(p);
}

//��fixedֵתΪdouble
double	kcGlyphOutline::FixedToDouble(const FIXED& fv)
{
	double val = 0.0;
	val = fv.value + (double)fv.fract/(1 << 16);
	//KTRACE("\n fv.value %d,fv.fract %d,dval %f.",fv.value,fv.fract,val);
	return val;
}

//////////////////////////////////////////////////////////////////////////
//

kcFontText::kcFontText(LOGFONT& lf)
{
	memset(&m_logFont,0,sizeof(LOGFONT));
	SetLogFont(lf);
}

kcFontText::~kcFontText()
{

}

// �޸���������
void	kcFontText::SetLogFont(LOGFONT& lf)
{
	memcpy(&m_logFont,&lf,sizeof(LOGFONT));
}

BOOL	kcFontText::GetText(HDC hDC,LPCTSTR lpszText,double dHeight,double dGap,BOOL bWire,TopTools_ListOfShape& aList)
{
	ASSERT(hDC && lpszText && dHeight > 0.0);
	if(!hDC || !lpszText || lpszText[0] == '\0' || dHeight < 0.0)
		return FALSE;

	//��ȡÿ���ֵ�����
	kcGlyphOutline glyphOutLine(hDC,&m_logFont);
	if(!glyphOutLine.IsValid())
		return FALSE;

	//תΪ���ַ������д���
	int ix,nC = 0;
	WCHAR wBuf[256];

	int nBufSize = ::MultiByteToWideChar(CP_ACP,0,lpszText,-1,NULL,0);
	if(nBufSize <= 0)//�����˽�β��0��
		return FALSE;

	WCHAR *pwcBuf = NULL;
	if(nBufSize < 256)
		pwcBuf = wBuf;
	else
		pwcBuf = new WCHAR[nBufSize + 1];

	::MultiByteToWideChar(CP_ACP,0,lpszText,-1,pwcBuf,nBufSize + 1);

	//���δ����ת���������и�ʽ����
	TopTools_ListOfShape shapeList;
	Bnd_SeqOfBox seqOfBox;
	Bnd_Box totBox,bbox;
	ix = 0;
	while(pwcBuf[ix])
	{
		TopoDS_Compound aS = glyphOutLine.GetCharOutLine(pwcBuf[ix]);
		//
		if(!aS.IsNull() && !bWire)
		{
			TopoDS_Compound aC = CreateFaceFromWire(aS);
			if(!aC.IsNull())//���ʧ�ܣ���Ȼ���������֡�
				aS = aC;
		}
		shapeList.Append(aS);//����Ϊ��
		if(!aS.IsNull())
		{
			bbox.SetVoid();
			BRepBndLib::Add(aS,bbox);
			totBox.Add(bbox);
			seqOfBox.Append(bbox);
		}
		else
		{
			bbox.SetVoid();
			seqOfBox.Append(bbox);//��֤shapeList�ĸ�����seqOfBox�ĸ���һ�¡�
		}

		ix ++;
	}
	ASSERT(seqOfBox.Length() == shapeList.Extent());

	if(nBufSize >= 256)
		delete[] pwcBuf;

	if(dGap < KDBL_MIN)
		dGap = dHeight * 0.1;

	// �������������
	double x0,y0,z0,x1,y1,z1;
	totBox.Get(x0,y0,z0,x1,y1,z1);
	double dSingMaxWidth = x1 - x0,dTotHeight = y1 - y0;
	double scale = dHeight / dTotHeight;
	double xpos = 0.0,dWidth = 0.0;
	kPoint3 org;
	TopTools_ListIteratorOfListOfShape lls;
	ix = 0;
	for(lls.Initialize(shapeList);lls.More();lls.Next())
	{
		TopoDS_Shape aS = lls.Value();
		if(aS.IsNull())
		{
			ix ++;
			xpos += dSingMaxWidth * scale + dGap;//�����հס�
			continue;
		}
		bbox = seqOfBox.Value(ix + 1);
		bbox.Get(x0,y0,z0,x1,y1,z1);
#ifdef _DEBUG
		KTRACE("\n <%.4f,%.4f,%.4f> - <%.4f,%.4f,%.4f>.",x0,y0,z0,x1,y1,z1);
#endif
		TopoDS_Compound aC = TopoDS::Compound(aS);
		org.set(x0,0.0,0.0);//y=0.0��֤��������һ�������ϲ��䡣
		dWidth = x1 - x0;
		//������
		if(fabs(scale - 1.0) > KDBL_MIN)
		{
			aS = QxBRepLib::ScaleShape(aC,org,scale,FALSE);//ֱ���޸�
			if(aS.IsNull())
			{
				ix ++;
				xpos += dWidth * scale + dGap;//�����հ�
				continue;
			}
			aC = TopoDS::Compound(aS);
		}
		//���ƶ�
		if(ix > 0)
		{
			kVector3 v(xpos - x0,0.0,0.0);
			aS = QxBRepLib::MoveShape(aC,v,FALSE);
			if(aS.IsNull())
			{
				ix ++;
				continue;
			}
			aC = TopoDS::Compound(aS);
		}

		if(ix > 0)
			xpos += dWidth * scale + dGap;
		else
			xpos = x0 + (x1 - x0) * scale + dGap;

		aList.Append(aC);
		ix ++;
	}

	return TRUE;
}

TopoDS_Compound		kcFontText::CreateFaceFromWire(const TopoDS_Compound& aOutLine)
{
	TopoDS_Compound aComp;
	if(aOutLine.IsNull())
		return aComp;

	kcgBuildFaceFromPlnWire buildFace;
	buildFace.Init();
	TopExp_Explorer ex;
	for(ex.Init(aOutLine,TopAbs_WIRE);ex.More();ex.Next())
	{
		TopoDS_Wire aW = TopoDS::Wire(ex.Current());
		buildFace.Add(aW);
	}

	if(!buildFace.Build())
		return aComp;

	//��������
	BRep_Builder BB;
	BB.MakeCompound(aComp);

	int ix,ic = buildFace.NbFaces();
	for(ix = 0;ix < ic;ix ++)
		BB.Add(aComp,buildFace.GetFace(ix));

	return aComp;
}

//////////////////////////////////////////////////////////////////////////
//
kcText::kcText(HDC hDC,LPLOGFONT lf)
:m_aGlyphOutline(hDC,lf)
{
}

kcText::~kcText()
{

}

//////////////////////////////////////////////////////////////////////////
//

static unsigned char s_byteTab[] = {
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	0,  '!',  '"',  '#',  '$',  '%',  '&',  '\'',  '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',  
	'0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',  
	'@',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',  
	'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  '[',  '\\',  ']',  '^',  '_',  
	'`',  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',  
	'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  '{',  '|',  '}',  '~',  0,  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  
};


kxCharFontText::kxCharFontText(unsigned char b)
:m_uChar(b)
{
	m_aBndBox.SetVoid();
	m_dWidth = 0.0;
	m_dHeight = 0.0;
}

kxCharFontText::~kxCharFontText()
{

}

//////////////////////////////////////////////////////////////////////////
//
kcCharText::kcCharText()
{
	int ix;
	for(ix = 0;ix < 256;ix ++)
		m_pCharText[ix] = NULL;
}

kcCharText::~kcCharText()
{
	Destroy();
}

bool	kcCharText::Initialize(HDC hDC,LPLOGFONT lf)
{
	int ix;
	unsigned char b;
	kcGlyphOutline outline(hDC,lf);
	if(!outline.IsValid())
		return false;

	for(ix = 0;ix < 256;ix ++)
	{
		b = s_byteTab[ix];
		if(b == 0)
			continue;

		TopoDS_Shape aS = outline.GetCharOutLine((char)b);
		if(aS.IsNull())
			continue;

		// ����Face
		TopoDS_Compound aC = CreateFaceFromWire(TopoDS::Compound(aS));
		if(aC.IsNull())
			continue;

		m_pCharText[ix] = new kxCharFontText(b);
		//��¼
		m_pCharText[ix]->m_aOutLine = TopoDS::Compound(aS);
		m_pCharText[ix]->m_aFace = aC;

		//�����Χ��
		BRepBndLib::Add(aS,m_pCharText[ix]->m_aBndBox);
		//
		double x0,y0,z0,x1,y1,z1;
		m_pCharText[ix]->m_aBndBox.Get(x0,y0,z0,x1,y1,z1);
		m_pCharText[ix]->m_dWidth = x1 - x0;
		m_pCharText[ix]->m_dHeight = y1 - y0;
	}

	return true;
}

TopoDS_Compound		kcCharText::CreateFaceFromWire(const TopoDS_Compound& aOutLine)
{
	TopoDS_Compound aComp;
	if(aOutLine.IsNull())
		return aComp;

	kcgBuildFaceFromPlnWire buildFace;
	buildFace.Init();
	TopExp_Explorer ex;
	for(ex.Init(aOutLine,TopAbs_WIRE);ex.More();ex.Next())
	{
		TopoDS_Wire aW = TopoDS::Wire(ex.Current());
		buildFace.Add(aW);
	}

	if(!buildFace.Build())
		return aComp;

	//��������
	BRep_Builder BB;
	BB.MakeCompound(aComp);

	int ix,ic = buildFace.NbFaces();
	for(ix = 0;ix < ic;ix ++)
		BB.Add(aComp,buildFace.GetFace(ix));

	return aComp;
}

void	kcCharText::Destroy()
{
	int ix;
	for(ix = 0;ix < 256;ix ++)
	{
		if(m_pCharText[ix])
			delete m_pCharText[ix];
	}
}

bool	kcCharText::IsValid() const
{
	int ix;
	for(ix = 0;ix < 256;ix ++)
	{
		if(m_pCharText[ix])
			return true;
	}

	return false;
}

//��ȡ�����ַ�������
TopoDS_Compound	kcCharText::GetOutline(char c,double dHeight)
{
	TopoDS_Compound aComp;
	unsigned char b = (unsigned char)c;
	if(s_byteTab[b] == 0)//�޷�ת��
		return aComp;
	
	kxCharFontText *pText = m_pCharText[b];
	if(fabs(dHeight - pText->m_dHeight) > KDBL_MIN)
	{
		//����
		double scale = dHeight / pText->m_dHeight;
		kPoint3 org;
		TopoDS_Shape aS = QxBRepLib::ScaleShape(m_pCharText[b]->m_aOutLine,org,scale);
		if(!aS.IsNull())
			aComp = TopoDS::Compound(aS);
	}
	else
	{
		TopoDS_Shape aS = QxBRepLib::CopyShape(m_pCharText[b]->m_aOutLine);
		if(!aS.IsNull())
			aComp = TopoDS::Compound(aS);
	}

	return aComp;
}

//��ȡ�����ַ���face
TopoDS_Compound	kcCharText::GetFace(char c,double dHeight)
{
	TopoDS_Compound aComp;
	unsigned char b = (unsigned char)c;
	if(s_byteTab[b] == 0)//�޷�ת��
		return aComp;

	kxCharFontText *pText = m_pCharText[b];
	if(fabs(dHeight - pText->m_dHeight) > KDBL_MIN)
	{
		//����
		double scale = dHeight / pText->m_dHeight;
		kPoint3 org;
		TopoDS_Shape aS = QxBRepLib::ScaleShape(m_pCharText[b]->m_aFace,org,scale);
		if(!aS.IsNull())
			aComp = TopoDS::Compound(aS);
	}
	else
	{
		TopoDS_Shape aS = QxBRepLib::CopyShape(m_pCharText[b]->m_aFace);
		if(!aS.IsNull())
			aComp = TopoDS::Compound(aS);
	}

	return aComp;
}

//��ȡ�����ַ�������
bool	kcCharText::GetText(const char *pStr,
							double dHeight,double dGap,bool bFace,
							TopTools_ListOfShape& aList)
{
	ASSERT(pStr && pStr[0] != '\0');
	if(!pStr || pStr[0] == '\0')
		return false;

	TopoDS_Compound aComp;
	double x = 0.0,y = 0.0;
	double dWidth = m_pCharText['W']->m_dWidth;
	double dScale = dHeight / m_pCharText['W']->m_dHeight;
	const char *pptr = pStr;
	while(*pptr)
	{
		//�ո����⴦��
		if(*pptr == ' ')
		{
			x += dWidth * dScale + dGap;
			pptr ++;
			continue;
		}

		if(bFace)
			aComp = GetFace(*pptr,dHeight);
		else
			aComp = GetOutline(*pptr,dHeight);
		if(aComp.IsNull())
		{
			pptr ++;
			continue;
		}

		kVector3 v(x,y,0.0);
		TopoDS_Shape aS;
		if(x > KDBL_MIN)
		{//�ƶ�
			aS = QxBRepLib::MoveShape(aComp,v,FALSE);
			if(aS.IsNull())
			{
				pptr ++;
				continue;
			}
			aComp = TopoDS::Compound(aS);
		}
		aList.Append(aComp);
		
		if(m_pCharText[*pptr]->m_dWidth > dWidth)
			dWidth = m_pCharText[*pptr]->m_dWidth;
		x +=  dWidth * dScale + dGap;
		pptr ++;
	}

	return true;
}

kcCharText&		glb_GetArialCharText()
{
	static kcCharText s_arialCharText;
	return s_arialCharText;
}

kcCharText&		glb_GetCourierCharText()
{
	static kcCharText s_courierCharText;
	return s_courierCharText;
}

bool	glb_InitArialCharText(HDC hDC)
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
	lf.lfHeight = -21;                      // request a 16-pixel-height font
	lf.lfOutPrecision = OUT_STROKE_PRECIS;
	lf.lfClipPrecision = CLIP_STROKE_PRECIS;
	lf.lfQuality = ANTIALIASED_QUALITY;
	lf.lfPitchAndFamily = 34;
	strcpy(lf.lfFaceName, "Arial");        // request a face name "Arial"

	kcCharText& charText = glb_GetArialCharText();
	if(!charText.IsValid())
	{
		if(!charText.Initialize(hDC,&lf))
			return false;
	}
//DEFAULT_QUALITY

	return true;
}

bool	glb_InitCourierCharText(HDC hDC)
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
	lf.lfHeight = 16;                      // request a 16-pixel-height font
	strcpy(lf.lfFaceName, "Courier New");  // request a face name "Arial"

	kcCharText& charText = glb_GetCourierCharText();
	if(!charText.IsValid())
	{
		if(!charText.Initialize(hDC,&lf))
			return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
