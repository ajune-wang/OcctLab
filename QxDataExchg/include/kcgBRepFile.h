//////////////////////////////////////////////////////////////////////////
// �����󱣴浽brep�ļ��У����brep�ļ��ж�ȡ����
#ifndef _KCG_BREP_FILE_H_
#define _KCG_BREP_FILE_H_

class kcgBRepFile
{
public:
	kcgBRepFile(void);
	~kcgBRepFile(void);

	// д������ļ���,����ѡ���Ƿ񱣴�������Ƭ
	BOOL					Write(const TopoDS_Shape& aShape,const char *lpszFile,BOOL bWithTriangles);

	// ���ļ��ж�ȡ����
	BOOL					Read(TopoDS_Shape& aShape,const char *lpszFile,BOOL bWithTriangles);
};

#endif