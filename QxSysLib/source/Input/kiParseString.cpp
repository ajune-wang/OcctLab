#include "StdAfx.h"
#include <string>
#include "kiParseString.h"

// ��������
//
kiParseInteger::kiParseInteger(const char *pString)
:_pString(pString)
{
	ASSERT(pString);
}

//����,���������ival��
bool	kiParseInteger::Parse(int& ival)
{
	ASSERT(_pString);
	if(!_pString) return false;

	const char *pstr = _pString;
	while(*pstr)
	{
		if(!::isdigit(*pstr) &&
			(*pstr != '+') && (*pstr != '-'))
			return false;

		pstr ++;
	}

	ival = atoi(_pString);

	return true;
}

// ����������
kiParseDouble::kiParseDouble(const char *pString)
:_pString(pString)
{
	ASSERT(pString);
}

//���������������dval��
bool	kiParseDouble::Parse(double& dval)
{
	ASSERT(_pString);
	if(!_pString) return false;

	const char *pstr = _pString;
	while(*pstr)
	{
		if(!::isdigit(*pstr) && (*pstr != '.') &&
		   (*pstr != '+') && (*pstr != '-'))
		   return false;

		pstr ++;
	}

	dval = atof(_pString);
	return true;
}

//������
kiParseStringToPoint3::kiParseStringToPoint3(const char *pString)
:_pString(pString)
{
	ASSERT(pString);
}

//����,���������point��.
bool	kiParseStringToPoint3::Parse(kPoint3& point)
{
	if(_pString == NULL) return false;
	std::string strval = "";
	int num = 0;
	double p[3];
	const char *pstr = _pString;
	while(*pstr)
	{
		//���Կո�.
		if(*pstr == ' ')
		{
			pstr ++;
			continue;
		}
		//�Ƿ�Ϸ��ַ�
		if((*pstr < '0' || *pstr > '9') && (*pstr != ',') &&
		   (*pstr != '-') && (*pstr != '+') && (*pstr != '.'))
		   return false;

		if(*pstr == ',')
		{
			if(num >= 2)
				return false;

			if(strval.empty())
				return false;
			//
			p[num] = atof(strval.c_str());
			num ++;
			strval = "";
		}
		else
			strval += *pstr;

		pstr ++;
	}
	//
	if(num == 0)
		return false;

	if(strval.empty())
		return false;

	p[num] = atof(strval.c_str());
	if(num == 1)
		p[2] = 0.0;

	point.set(p);
	
	return true;
}


kiParseString::kiParseString(void)
{
}

kiParseString::~kiParseString(void)
{
}
