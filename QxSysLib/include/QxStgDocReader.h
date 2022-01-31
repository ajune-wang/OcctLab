//////////////////////////////////////////////////////////////////////////
// ��ȡ���������
#ifndef _QX_STG_DOC_READER_H_
#define _QX_STG_DOC_READER_H_

#include "QxLibDef.h"

class kcDocContext;
class kcModel;
class CArchive;

// ����Doc

class QXLIB_API QxStgDocReader
{
public:
	QxStgDocReader(kcDocContext *pDocCtx);
	~QxStgDocReader(void);

	// ��ȡ
	bool				Load(CArchive &ar);

protected:
	//
	BOOL				ReadFileHeader(CArchive& ar);

	//
	BOOL				ReadGlobalBlock(CArchive& ar);

	//
	BOOL				ReadModel(CArchive& ar,kcModel *pModel);

protected:
	kcDocContext		*pDocContext_;
	UINT				dwReadVersion_;//����İ汾��
};

#endif