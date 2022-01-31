#include "StdAfx.h"
#include <fstream>
#include <BRepTools_ShapeSet.hxx>
#include "kcgBrepFile.h"

kcgBRepFile::kcgBRepFile(void)
{
}

kcgBRepFile::~kcgBRepFile(void)
{
}

// д������ļ���,����ѡ���Ƿ񱣴�������Ƭ
// ��BRepTools::Writeһ�£�������ָ���Ƿ񱣴�������Ƭ��Ϣ��
BOOL	kcgBRepFile::Write(const TopoDS_Shape& aShape,const char *lpszFile,BOOL bWithTriangles)
{
	std::filebuf fbuf;
	std::ostream aStream(&fbuf);
	if(!fbuf.open(lpszFile,std::ios::out)){
		return FALSE;
	}
	
	BRepTools::Write(aShape,aStream);
	
	return TRUE;
}

// ���ļ��ж�ȡ����
BOOL	kcgBRepFile::Read(TopoDS_Shape& aShape,const char *lpszFile,BOOL bWithTriangles)
{
	std::filebuf fic;
	std::istream in(&fic);
	if (!fic.open(lpszFile, std::ios::in)) 
		return FALSE;

	BRep_Builder B;
	BRepTools::Read(aShape,in,B);
    
	return TRUE;
}