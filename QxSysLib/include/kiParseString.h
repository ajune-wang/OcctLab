//////////////////////////////////////////////////////////////////////////
// ʵ�ֶ�һ���ַ���ֵ�Ľ���
#ifndef _KI_PARSE_STRING_H_
#define _KI_PARSE_STRING_H_

#include "kPoint.h"
#include "QxLibDef.h"

class QXLIB_API kiParseInteger{
public:
	kiParseInteger(const char *pString);

	//����,���������ival��
	bool					Parse(int& ival);

protected:
	const char				*_pString;
};

// ����������
class QXLIB_API kiParseDouble{
public:
	kiParseDouble(const char *pString);

	//���������������dval��
	bool					Parse(double& dval);

protected:
	const char				*_pString;
};

//������
class QXLIB_API kiParseStringToPoint3{
public:
	kiParseStringToPoint3(const char *pString);

	//����,���������point��.
	bool					Parse(kPoint3& point);

protected:
	const char				*_pString;
};


class kiParseString
{
public:
	kiParseString(void);
	~kiParseString(void);
};

#endif