#include "stdafx.h"
#include <TCollection_AsciiString.hxx>
#include "kcStepMbStringMgr.h"

//��¼һ���ַ���
kcAscExtStringPair::kcAscExtStringPair()
{}

kcAscExtStringPair::kcAscExtStringPair(const std::string &aStr)
{
	if(!aStr.empty()){
		aStr_ = aStr;
	}
}

bool  kcAscExtStringPair::IsValid() const
{
	return (aStr_.empty() ? false : true);
}

bool  kcAscExtStringPair::IsSame(const TCollection_ExtendedString &extStr)
{
	if(aStr_.empty()) return false;
	//
	if(aExtStr_.IsEmpty()){
		TCollection_AsciiString astr(aStr_.c_str());
		TCollection_ExtendedString es(astr);
		aExtStr_ = es;
	}

	return ((aExtStr_ == extStr) ? true : false);
}

/////////////////////////////////////////////////////////////////
//
kcStepMbStringMgr::kcStepMbStringMgr(void)
{}

kcStepMbStringMgr::~kcStepMbStringMgr(void)
{
	Clear();
}

// ��STEP�ļ��У���ȡ���ж��ֽڵ��ַ���
//
void  kcStepMbStringMgr::Read(const char *pszStepFile)
{
	FILE *fp = NULL;
	fopen_s(&fp,pszStepFile,"r");
	if(fp == NULL){
		TRACE("\n #- kcStepMbStringMgr::Read (%s) failed.",pszStepFile);
		return;
	}

	int maxSize = 4000;
	char *pszLine = new char[maxSize];
	int slen = 0,wclen = 0;

	//���ж�ȡ�ͽ���
	while(fgets(pszLine,maxSize,fp)){
		slen = strnlen_s(pszLine,maxSize); //���Ȳ���0��pszLine������\n
		//����Ϊ�գ���ʹ���У�Ҳ�����һ��\n
		if(slen == 0)
			continue;

		//�ж��Ƿ���������˶��ֽ��ַ�
		wclen = ::MultiByteToWideChar(CP_ACP,0,pszLine,-1,NULL,0);//���Ⱥ�0
		if(wclen == slen + 1){//ascii �ַ��������ô���
			continue;
		}

		// �ҳ��ַ���
		int icx = 0,icxStart = -1;
		while(icx < slen){
			if(pszLine[icx] == '\''){
				if(icxStart < 0){
					icxStart = icx + 1;//�ַ�����'��ʼ
				}else{
					//�ҵ�һ���ַ���,��¼
					if(icxStart < icx){//�ж�''���ַ��������Σ��������κܶ�
						int kx;
						std::string szText;
						
						szText.reserve(icx - icxStart + 1);
						for(kx = icxStart;kx < icx;kx ++){
							szText += pszLine[kx];
						}

						//�ж��Ƿ�������ֽ��ַ�
						wclen = ::MultiByteToWideChar(CP_ACP,0,szText.c_str(),-1,NULL,0);//���Ⱥ�0
						if(wclen != (int)szText.size() + 1){//�����˶��ֽ��ַ�
							TRACE("\n #---- text : %s",szText.c_str());
							Add(szText);
						}
					}
					//
					icxStart = -1;//���¿�ʼ
				}
			}

			icx ++;
		}//end while
	}

	fclose(fp);
}

bool  kcStepMbStringMgr::Add(const std::string &ss)
{
	stdAscExtStringMap::iterator ite = aStringMap_.find(ss);
	if(ite != aStringMap_.end()){
		return true;
	}

	kcAscExtStringPair *pStrPair = new kcAscExtStringPair(ss);
	aStringMap_.insert(stdAscExtStringMap::value_type(ss,pStrPair));
	aStrPairVec_.push_back(pStrPair);

	return true;
}

void  kcStepMbStringMgr::Clear()
{
	stdAscExtStringPairVector::size_type ix,isize = aStrPairVec_.size();
	for(ix = 0;ix < isize;ix ++){
		delete aStrPairVec_[ix];
	}
	
	aStringMap_.clear();
	aStrPairVec_.clear();
}

//����һ��ExtString�ַ���
bool  kcStepMbStringMgr::DecodeExtString(const TCollection_ExtendedString &aExtStr,std::string &szStr)
{
	szStr.clear();
	if(aExtStr.IsEmpty())
		return true;

	//��ascii�ַ�����ֱ�ӻ�ȡ
	if(aExtStr.IsAscii()){
		int ix,n = aExtStr.Length();
		for(ix = 1;ix <= n;ix ++){
			Standard_ExtCharacter ec = aExtStr.Value(ix);
			szStr += (char)(ec & 0xff);
		}
		return true;
	}

	//��asc�������Ǳ�����ĵ���utf8�����뵽��unicode
	//ͨ������ȡ
	return Find(aExtStr,szStr);
}

bool  kcStepMbStringMgr::Find(const TCollection_ExtendedString &aExtStr,std::string &aStr)
{
	kcAscExtStringPair *pPair = NULL;
	stdAscExtStringPairVector::size_type ix,isize = aStrPairVec_.size();
	for(ix = 0;ix < isize;ix ++){
		pPair = aStrPairVec_.at(ix);

		if(pPair->IsSame(aExtStr)){
			aStr = pPair->aStr_;
			return true;
		}
	}
	return false;
}

