//////////////////////////////////////////////////////////////////////////
// ��������
#ifndef _QX_STG_DOC_WRITER_H_
#define _QX_STG_DOC_WRITER_H_

#include "QxLibDef.h"

class kcDocContext;
class kcModel;
class CArchive;

class QXLIB_API QxStgDocWriter
{
public:
	QxStgDocWriter(kcDocContext *pDocCtx);
	~QxStgDocWriter(void);

	//д������
	bool				Save(CArchive &ar);

protected:
	//����ͷ��
	BOOL				WriteHeaderBlock(CArchive& ar);
	//����ȫ�ֿ�
	BOOL				WriteGlobalBlock(CArchive& ar);

	//д���ļ�
	BOOL				WriteModel(CArchive& ar,kcModel *pModel);

protected:
	kcDocContext		*pDocContext_;
};

#endif