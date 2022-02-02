#include "StdAfx.h"
#include <strstream>
#include <sstream>
#include <TopoDS_Shape.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <FSD_File.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Storage_Root.hxx>
#include "zlib.h"
#include "QxBaseDef.h"
#include "QxStgBlock.h"

//��ĩβ�ı�ʶ
#define KSTG_BLOCK_END_MAGIC   0xFF00BCEF   //BloCk Edge Flag

//////////////////////////////////////////////////////////////////////////////
//
// �������ͣ���ȡ���Ͷ���Ĵ�С��byte����
static int uxGetTypeSize(BYTE type)
{
	int n = 0;
	BYTE bty = type & 0x07; //��������
	switch(bty){
	case KSTG_TYPE_CHAR: n = sizeof(char); break;
	case KSTG_TYPE_SHORT: n = sizeof(short); break;
	case KSTG_TYPE_INT:	n = sizeof(int); break;
	case KSTG_TYPE_INT64: n = sizeof(int64_t); break;
	case KSTG_TYPE_FLOAT: n = sizeof(float); break;
	case KSTG_TYPE_DOUBLE: n = sizeof(double); break;
	case KSTG_TYPE_BOOL: n = sizeof(bool); break;
	default:
		break;
	}
	return n;
}

// �����������ʹ�õ�byte����
static int uxCalcBytesOfArrayNum(int n)
{
	int nbytes = 0;
	if(n <= 0xFF){
		nbytes = 1;
	}else if(n <= 0xFFFF){
		nbytes = 2;
	}else if(n <= 0xFFFFFF){
		nbytes = 3;
	}else{//
		nbytes = 4;
	}
	return nbytes;
}

static bool uxIsValidType(BYTE type)
{
	if(type == KSTG_TYPE_BLOCK) return true;
	BYTE btyp = type & 0x07;
	return (btyp != KSTG_TYPE_NULL);
}

// д�����������ʹ�ñ��ֽ���������ʹ���ֽ���
static int uxWriteArrayNum(BYTE *pBuf,int nArrayNum)
{
	int nbAryNum = uxCalcBytesOfArrayNum(nArrayNum);
	if(nbAryNum == 1){
		*pBuf = nArrayNum;
		return 1;
	}
	
	//����д��
	int ix = 0, nz = nArrayNum;
	for(ix = 0;ix < nbAryNum;ix ++){
		*pBuf = nz & 0xFF;
		nz = nz >> 8;
		pBuf ++;
	}
	return nbAryNum;
}

// ��ȡ���������ʹ�ñ��ֽڸ�����¼��nbByte��ʹ���ֽڸ���
// ���أ��������ֵ
static int uxReadArrayNum(BYTE *pBuf,int nbByte)
{
	int n = 0;
	if(nbByte == 1){
		n = pBuf[0];
	}else{
		int ix = 0,fact = 1;
		for(ix = 0;ix < nbByte;ix ++){
			n += pBuf[ix] * fact;
			fact = fact << 8;
		}
	}
	return n;
}

///////////////////////////////////////////////////////////////////////////////
//
QxStgCodeValue::QxStgCodeValue()
{
	Reset();
}

QxStgCodeValue::QxStgCodeValue(vcode_t code,int ival)
{
	ASSERT(KSTG_IS_VALID_CODE(code));
	Reset();
	bool bRc = Set(code,ival);
	ASSERT(bRc);
}

QxStgCodeValue::QxStgCodeValue(vcode_t code,unsigned int ival)
{
	ASSERT(KSTG_IS_VALID_CODE(code));
	Reset();
	bool bRc = Set(code,ival);
	ASSERT(bRc);
}

QxStgCodeValue::QxStgCodeValue(vcode_t code,double dval)
{
	ASSERT(KSTG_IS_VALID_CODE(code));
	Reset();
	bool bRc = Set(code,dval);
	ASSERT(bRc);
}

QxStgCodeValue::QxStgCodeValue(vcode_t code,double da[3])
{
	ASSERT(KSTG_IS_VALID_CODE(code));
	Reset();
	bool bRc = Set(code,da);
	ASSERT(bRc);
}

QxStgCodeValue::QxStgCodeValue(vcode_t code,bool bval)
{
	ASSERT(KSTG_IS_VALID_CODE(code));
	Reset();
	bool bRc = Set(code,bval);
	ASSERT(bRc);
}

QxStgCodeValue::QxStgCodeValue(vcode_t code,const char *pszBuf)
{
	ASSERT(KSTG_IS_VALID_CODE(code));
	ASSERT(pszBuf);
	Reset();
	bool bRc = Set(code,pszBuf);
	ASSERT(bRc);
}

QxStgCodeValue::QxStgCodeValue(vcode_t code,BYTE* pbuf,int n,bool bCopy)
{
	ASSERT(KSTG_IS_VALID_CODE(code));
	ASSERT(pbuf && (n > 0));
	Reset();
	bool bRc = Set(code,pbuf,n,bCopy);
	ASSERT(bRc);
}

QxStgCodeValue::QxStgCodeValue(vcode_t code,QxStgBlock *pBlock,bool bFree)
{
	ASSERT(KSTG_IS_VALID_CODE(code));
	ASSERT(pBlock);
	Reset();
	bool bRc = Set(code,pBlock,bFree);
	ASSERT(bRc);
}

QxStgCodeValue::~QxStgCodeValue()
{
	Clear();
}

bool QxStgCodeValue::IsValid() const
{
	if(!uxIsValidType(type_) || !KSTG_IS_VALID_CODE(code_))
		return false;

	// ������ļ��
	if(bArray_ && (!value_.ary || nArrayNum_ <= 0))
		return false;

	// ��ļ��
	if(IsBlock() && !pBlock_)
		return false;

	return true;
}

bool QxStgCodeValue::IsUnsigned() const
{
	if(!uxIsValidType(type_) || type_ == KSTG_TYPE_BLOCK)
		return false;
	return KSTG_IS_UNSIGNED(type_);
}

bool QxStgCodeValue::IsBlock() const
{
	return (type_ == KSTG_TYPE_BLOCK);
}

bool QxStgCodeValue::IsArray() const
{
	return bArray_;
}

void QxStgCodeValue::SetCT(vcode_t code,BYTE type,bool bAry)
{
	code_ = code;
	type_ = type;
	bArray_ = bAry;
}

bool QxStgCodeValue::Set(vcode_t code,int ival)
{
	if(!KSTG_IS_VALID_CODE(code)){
		ASSERT(FALSE);
		return false;
	}
	SetCT(code,KSTG_TYPE_INT);
	value_.ival = ival;

	return true;
}

bool QxStgCodeValue::Set(vcode_t code,unsigned int ival)
{
	if(!KSTG_IS_VALID_CODE(code)){
		ASSERT(FALSE);
		return false;
	}
	SetCT(code,KSTG_TYPE_UINT);
	value_.uival = ival;

	return true;
}

bool QxStgCodeValue::Set(vcode_t code,double dval)
{
	if(!KSTG_IS_VALID_CODE(code)){
		ASSERT(FALSE);
		return false;
	}
	SetCT(code,KSTG_TYPE_DOUBLE);
	value_.dval = dval;

	return true;
}

bool QxStgCodeValue::Set(vcode_t code,double da[3])
{
	if(!KSTG_IS_VALID_CODE(code)){
		ASSERT(FALSE);
		return false;
	}
	SetCT(code,KSTG_TYPE_DOUBLE,true);
	nArrayNum_ = 3;
	//����һ��
	double *pAry = new double[3];
	pAry[0] = da[0];pAry[1] = da[1];pAry[2] = da[2];
	value_.ary = (BYTE*)pAry;
	bCopyData_ = true;

	return true;
}

bool QxStgCodeValue::Set(vcode_t code,bool bval)
{
	if(!KSTG_IS_VALID_CODE(code)){
		ASSERT(FALSE);
		return false;
	}
	SetCT(code,KSTG_TYPE_BOOL);
	value_.bval = bval;

	return true;
}

bool QxStgCodeValue::Set(vcode_t code,const char *pszBuf)
{
	if(!KSTG_IS_VALID_CODE(code) || !pszBuf){
		ASSERT(FALSE);
		return false;
	}
	size_t n = strlen(pszBuf);
	if(n == 0)
		return false;

	SetCT(code,KSTG_TYPE_CHAR,true);
	nArrayNum_ = (int)n + 1; //������\0��λ��
	//Ĭ�Ͽ���
	char *pbuf = new char[n + 2];
	strncpy_s(pbuf,n+2,pszBuf,n); //ĩβ���Զ����0
	value_.ary = (BYTE*)pbuf; //����һ��
	bCopyData_ = true;
	
	return true;
}

bool	 QxStgCodeValue::Set(vcode_t code,BYTE* pbuf,int n,bool bCopy)
{
	if(!KSTG_IS_VALID_CODE(code) || !pbuf || (n <= 0)){
		ASSERT(FALSE);
		return false;
	}
	SetCT(code,KSTG_TYPE_BYTE,true);
	//
	nArrayNum_ = n;
	bCopyData_ = bCopy;
	if(bCopy){
		BYTE *pCB = new BYTE[n];
		memcpy(pCB,pbuf,n);
		value_.ary = pCB;
	}else{
		value_.ary = pbuf;
	}
	return true;
}

bool QxStgCodeValue::Set(vcode_t code,QxStgBlock *pBlock,bool bFree)
{
	if(!KSTG_IS_VALID_CODE(code) || !pBlock){
		ASSERT(FALSE);
		return false;
	}
	SetCT(code,KSTG_TYPE_BLOCK);
	//��¼����
	bFreeBlock_ = bFree;
	pBlock_ = pBlock;
	return true;
}

void QxStgCodeValue::Clear()
{
	//�ͷſ���������
	if(bArray_ && value_.ary && bCopyData_){
		//�ָ�����Ӧ������
		if(type_ == KSTG_TYPE_CHAR){
			char *pbuf = (char *)value_.ary;
			delete[] pbuf;
		}else if(type_ == KSTG_TYPE_BYTE){
			BYTE *pbuf = (BYTE *)value_.ary;
			delete[] pbuf;
		}else if(type_ == KSTG_TYPE_DOUBLE){
			double *pbuf = (double *)value_.ary;
			delete[] pbuf;
		}
		value_.ary = NULL;
	}

	//���ͷ�
	if(pBlock_ && bFreeBlock_){
		delete pBlock_;
	}

	Reset();
}

void QxStgCodeValue::Reset()
{
	type_ = KSTG_TYPE_NULL;
	attrib_ = 0;
	code_ = KSTG_INVALID_CODE;
	bArray_ = false;
	nArrayNum_ = 0;
	nFullSize_ = 0;
	nDataSize_ = 0;
	pValueBuf_ = NULL;
	value_.ival64 = 0; //ʹ����������
	bCopyData_ = false;
	pBlock_ = NULL;
	bFreeBlock_ = true;//Ĭ������Ҫ��
}

bool QxStgCodeValue::GetValue(int& iv) const
{
	if(!IsValid() || (type_ != KSTG_TYPE_INT)){
		ASSERT(FALSE);
		return false;
	}
	iv = value_.ival;
	return true;
}

bool QxStgCodeValue::GetValue(unsigned int& uiv) const
{
	if(!IsValid() || (type_ != KSTG_TYPE_UINT)){
		ASSERT(FALSE);
		return false;
	}
	uiv = value_.uival;
	return true;
}

bool QxStgCodeValue::GetValue(double& dv) const
{
	if(!IsValid() || (type_ != KSTG_TYPE_DOUBLE)){
		ASSERT(FALSE);
		return false;
	}
	dv = value_.dval;
	return true;
}

bool QxStgCodeValue::GetValue(double da[3]) const
{
	if(!IsValid() || (type_ != KSTG_TYPE_DOUBLE) || 
		!bArray_ || nArrayNum_ != 3 || !value_.ary)
	{
		ASSERT(FALSE);
		return false;
	}
	double *pAry = (double *)value_.ary;
	da[0] = pAry[0];
	da[1] = pAry[1];
	da[2] = pAry[2];

	return true;
}

bool QxStgCodeValue::GetValue(bool& bv) const
{
	if(!IsValid() || (type_ != KSTG_TYPE_BOOL)){
		ASSERT(FALSE);
		return false;
	}
	bv = value_.bval;
	return true;
}

bool QxStgCodeValue::GetValue(std::string& str) const
{
	if(!IsValid() || (type_ != KSTG_TYPE_CHAR) || 
		!bArray_ || nArrayNum_ <= 0 || !value_.ary)
	{
		ASSERT(FALSE);
		return false;
	}
	//nArrayNum_�ĸ���������ĩβ��0,ary���ַ�����\0
	char *pbuf = (char *)value_.ary;
	str = std::string(pbuf);
	return true;
}

// ��������������ڲ�ָ�룬û�п���
bool QxStgCodeValue::GetValue(BYTE *& pbuf,int &size) const
{
	if(!IsValid() || (type_ != KSTG_TYPE_BYTE) || 
		!bArray_ || nArrayNum_ <= 0 || !value_.ary)
	{
		ASSERT(FALSE);
		return false;
	}
	pbuf = (BYTE*)value_.ary;
	size = nArrayNum_;
	return true;
}

QxStgBlock* QxStgCodeValue::GetBlock() const
{
	if(!IsValid() || !IsBlock()){
		ASSERT(FALSE);
		return false;
	}
	if(pBlock_ && !pBlock_->IsValid())
		return NULL;
	return pBlock_;//����ΪNULL
}

// ���code�䳤���棬ʹ��1��2��BYTE
int QxStgCodeValue::CalcHeaderSize()
{
	ASSERT(KSTG_IS_VALID_CODE(code_));
	if(code_ < 0xFF)
		return 2;
	return 3;
}

// ����data���ݵĴ�С
void QxStgCodeValue::CalcDataSize()
{
	if(IsBlock()){ //�����Ƕ�׿飬��ȡǶ�׿顣
		if(pBlock_){
			nDataSize_ = pBlock_->GetBlockSize();
		}else{
			ASSERT(FALSE);
			nDataSize_ = 0;
		}
	}else{
		nDataSize_ = uxGetTypeSize(type_);
		if(bArray_){
			nDataSize_ *= nArrayNum_;
		}
	}
}

// ����CodeValue�иÿ�ռ���ڴ�Ĵ�С
// ������֧���������Ƕ��block
//
void QxStgCodeValue::CalcFullSize()
{
	int nHdr = CalcHeaderSize();

	//�������ݿ��С
	CalcDataSize();

	//���������С
	nFullSize_ = nHdr + nDataSize_;
	if(bArray_){// ��¼����������ֽ���
		nFullSize_ += uxCalcBytesOfArrayNum(nArrayNum_);
	}else if(IsBlock()){//��¼block��С���ֽ���
		nFullSize_ += uxCalcBytesOfArrayNum(nDataSize_);
	}
}

// ���ɵ�һ���ֽڣ�
// ��4bit�����ͣ�56bit��¼�����������Сʹ�õ�byte��
// 8bit:�����Ƿ�Ϊ����
BYTE QxStgCodeValue::BuildTypeAttr()
{
	BYTE bval = type_;
	// code��Ҫ2�ֽڴ洢
	if(code_ > 0xFF){
		bval |= KSTG_BITS_MASK_SHORT_CODE;
	}
	if(bArray_){
		bval |= KSTG_BITS_MASK_ARRAY;
		int n = uxCalcBytesOfArrayNum(nArrayNum_);
		bval |= ((n-1) << 4); //1-4תΪ0-3
	}else if(IsBlock()){//���Сռ�õ��ֽ���
		ASSERT(nDataSize_ > 0);
		int n = uxCalcBytesOfArrayNum(nDataSize_);
		bval |= ((n-1) << 4); //1-4תΪ0-3
	}
	
	return bval;
}

int QxStgCodeValue::GetFullSize()
{
	if(!IsValid()){
		ASSERT(FALSE);
		return 0;
	}
	if(nFullSize_ == 0){
		CalcFullSize();
	}
	return nFullSize_;
}

// ���ڴ���ж�ȡ����
// ��Ҫ֧����Ƕblock�Ķ�ȡ
//
int QxStgCodeValue::ReadData(BYTE *pBuf,bool bCopy)
{
	int n = 0;

	type_ = pBuf[0] & 0x0F;
	attrib_ = pBuf[0] & 0xF0; //��4bit
	//����1��2���ֽ�
	if(KSTG_IS_SHORT_CODE(attrib_)){
		code_ = (pBuf[2] << 8) | pBuf[1];
		n = 3;
	}else{
		code_ = pBuf[1];
		n = 2;
	}
	if(!uxIsValidType(type_) || !KSTG_IS_VALID_CODE(code_)){
		Reset();
		return 0;
	}
	//
	BYTE *pb = pBuf + n;
	int nbByte = 0; //��¼���������������Ҫʹ�õ��ֽ���
	if(IsBlock()){
		//��ȡ���С
		nbByte = (attrib_ & KSTG_BITS_MASK_ARRAY_NUM) >> 4;
		nbByte += 1;//��Ҫ��1������Χ��0-3
		// ������С
		nDataSize_ = uxReadArrayNum(pb,nbByte);
	}else{
		bArray_ = KSTG_IS_ARRAY(attrib_);
		if(bArray_){
			nbByte = (attrib_ & KSTG_BITS_MASK_ARRAY_NUM) >> 4;
			nbByte += 1;//��Ҫ��1������Χ��0-3
			//����
			nArrayNum_ = uxReadArrayNum(pb,nbByte);
		}
		//�������ݿ��С
		CalcDataSize();
	}
	pb += nbByte;

	//��������
	if(IsBlock()){
		pBlock_ = new QxStgBlock;
		// 
		int nbRead = pBlock_->ReadFromBuf(pb,bCopy);
		if(nbRead != nDataSize_){//������
			ASSERT(FALSE);
			return 0;
		}
		bFreeBlock_ = true;//��Ҫ�ͷ�
	}else{
		if(!ReadValueFromBuf(pb,bCopy)){
			Reset();
			return 0;
		}
	}

	return n + nbByte + nDataSize_;
}

bool QxStgCodeValue::ReadValueFromBuf(BYTE *pBuf,bool bCopy)
{
	ASSERT(!IsBlock());
	ASSERT(nDataSize_ > 0);
	if(!bArray_){
		switch(type_){
		case KSTG_TYPE_CHAR:
		case KSTG_TYPE_BYTE:
			memcpy(&value_.cval,pBuf,sizeof(char));
			break;
		case KSTG_TYPE_SHORT:
		case KSTG_TYPE_USHORT:
			memcpy(&value_.sval,pBuf,sizeof(short));
			break;
		case KSTG_TYPE_INT:
		case KSTG_TYPE_UINT:
			memcpy(&value_.ival,pBuf,sizeof(int));
			break;
		case KSTG_TYPE_INT64:
		case KSTG_TYPE_UINT64:
			memcpy(&value_.ival64,pBuf,sizeof(int64_t));
			break;
		case KSTG_TYPE_BOOL:
			memcpy(&value_.bval,pBuf,sizeof(bool));
			break;
		case KSTG_TYPE_FLOAT:
			memcpy(&value_.fval,pBuf,sizeof(float));
			break;
		case KSTG_TYPE_DOUBLE:
			memcpy(&value_.dval,pBuf,sizeof(double));
			break;
		default:
			break;
		}
	}else{
		bCopyData_ = bCopy;
		if(bCopy){
			value_.ary = new BYTE[nDataSize_];
			memcpy(value_.ary,pBuf,nDataSize_);
		}else{
			value_.ary = pBuf;
		}
	}
	return true;
}

// д���ݵ��ڴ���У���Ҫ֧��Ƕ��block��д��
int QxStgCodeValue::WriteData(BYTE *pBuf)
{
	ASSERT(IsValid());
	if(!IsValid()) return 0;

	//����һ�����ݴ�С
	CalcDataSize();

	//һ��д�룬�������ӽ�ʡ�ڴ�
	int n = 0, nbBytes = 0;
	BYTE *pb = NULL;

	pBuf[0] = BuildTypeAttr();
	//�ж�ʹ��1��2��byte�洢
	pBuf[1] = (BYTE)(code_ & 0xFF);
	n = 2;
	if(code_ > 0xFF){
		pBuf[2] = (BYTE)(code_ >> 8);
		n ++;
	}
	//д���������
	pb = pBuf + n;
	if(bArray_){
		nbBytes = uxWriteArrayNum(pb,nArrayNum_);
	}else if(IsBlock()){//д����С
		nbBytes = uxWriteArrayNum(pb,nDataSize_);
	}
	pb += nbBytes;

	//д������
	if(IsBlock()){
		int nbWrite = pBlock_->WriteToBuf(pb);
		if(nbWrite != nDataSize_){
			ASSERT(FALSE);
			return 0;
		}
	}else{
		WriteValueToBuf(pb);
	}
	
	return n +  nbBytes + nDataSize_;//����д����ֽ���
}

// ��������ǿ��д�룬�ⲿ����
void QxStgCodeValue::WriteValueToBuf(BYTE *pBuf)
{
	ASSERT(!IsBlock());
	if(!bArray_){//������ֵ
		switch(type_){
		case KSTG_TYPE_CHAR:
		case KSTG_TYPE_BYTE:
			memcpy(pBuf,&value_.cval,sizeof(char));
			break;
		case KSTG_TYPE_SHORT:
		case KSTG_TYPE_USHORT:
			memcpy(pBuf,&value_.sval,sizeof(short));
			break;
		case KSTG_TYPE_INT:
		case KSTG_TYPE_UINT:
			memcpy(pBuf,&value_.ival,sizeof(int));
			break;
		case KSTG_TYPE_INT64:
		case KSTG_TYPE_UINT64:
			memcpy(pBuf,&value_.ival64,sizeof(int64_t));
			break;
		case KSTG_TYPE_BOOL:
			memcpy(pBuf,&value_.bval,sizeof(bool));
			break;
		case KSTG_TYPE_FLOAT:
			memcpy(pBuf,&value_.fval,sizeof(float));
			break;
		case KSTG_TYPE_DOUBLE:
			memcpy(pBuf,&value_.dval,sizeof(double));
			break;
		default:
			break;
		}
	}else{
		//����
		memcpy(pBuf,value_.ary,nDataSize_);
	}
}

///////////////////////////////////////////////////////////////////////////////
//

QxStgBlockBase::QxStgBlockBase()
{
	Reset();
}

QxStgBlockBase::~QxStgBlockBase()
{
	Clear();
}

void QxStgBlockBase::Clear()
{
	//
	QxStgCodeValueArray::size_type ix,isize = aCodeValueArray_.size();
	for(ix = 0;ix < isize;ix ++){
		delete aCodeValueArray_[ix];
	}
	aCodeValueArray_.clear();
	aCodeValueMap_.clear();

	if(pDataBuf_){
		delete[] pDataBuf_;
	}

	Reset();
}

int QxStgBlockBase::GetCodeValueCount() const
{
	return (int)aCodeValueArray_.size();
}

QxStgCodeValue* QxStgBlockBase::GetCodeValueByIndex(int idx) const
{
	if(idx < 0 || idx >= GetCodeValueCount()){
		ASSERT(FALSE);
		return NULL;
	}
	return aCodeValueArray_[idx];
}

// ��ȡ������Ĵ�С,�����Ƕ��ʹ��
// ͷ����С�����ݴ�С��β����ʶ��С
//
int QxStgBlockBase::GetBlockSize()
{
	if(!IsValid()) return 0;
	//
	if(nBlockSize_ == 0){
		//�ȼ������CV�Ĵ�С
		nDataSize_ = CalcBlockDataSize();
		//�����ļ�ͷ�Ĵ�С
		nBlockSize_ = nDataSize_ + CalcBlockHeaderSize();
		//����ĩβ��ʶ
		nBlockSize_ += sizeof(UINT);
	}
	return nBlockSize_;
}

// �������������ݴ�С����������ͷ
int QxStgBlockBase::CalcBlockDataSize()
{
	ASSERT(IsValid());
	int nDataSize = 0;
	QxStgCodeValueArray::size_type ix,isize = aCodeValueArray_.size();
	for(ix = 0;ix < isize;ix ++){
		nDataSize += aCodeValueArray_[ix]->GetFullSize();
	}
	return nDataSize;
}

// �����ͷ�Ĵ�С
int QxStgBlockBase::CalcBlockHeaderSize()
{
	ASSERT(IsValid());
	ASSERT(nDataSize_ > 0);
	int hdrSize = sizeof(short) * 3; // type/ver/attr
	hdrSize += 1; //�������ʹ��1��BYTE���㹻��
	hdrSize += uxCalcBytesOfArrayNum(nDataSize_);
	return hdrSize;
}

//
void QxStgBlockBase::Reset()
{
	type_ = KSTG_BLOCK_TYPE_UNKNOWN; //δ֪����
	version_ = 0;
	attrib_ = 0;
	nBlockSize_ = 0;
	nDataSize_ = 0;
	pDataBuf_ = NULL;
	nEndMagic_ = KSTG_BLOCK_END_MAGIC;
}

// ����ص���������
bool QxStgBlockBase::IsValid() const
{
	if(!KSTG_IS_VALID_BLOCK_TYPE(type_) ||
		aCodeValueArray_.empty())
		return false;
	return true;
}

short QxStgBlockBase::GetType() const
{
	return type_;
}

short QxStgBlockBase::GetVersion() const
{
	return version_;
}

bool QxStgBlockBase::DoAddCodeValue(QxStgCodeValue *pCV)
{
	ASSERT(pCV && pCV->IsValid());
	vcode_t code = pCV->GetCode();
	ASSERT(!CodeExisted(code));
	aCodeValueArray_.push_back(pCV);
	aCodeValueMap_.insert(QxStgCodeValueMap::value_type(code,pCV));
	return true;
}

bool QxStgBlockBase::CodeExisted(vcode_t code) const
{
	if(aCodeValueMap_.find(code) != aCodeValueMap_.end())
		return true;
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// ��ȡ���ֵ�ʵ��

QxStgBlock::QxStgBlock()
{
	nReadCVNum_ = 0;
}

QxStgBlock::~QxStgBlock()
{
}

// �����͵Ķ�ȡ���������g��ƥ�䣬����ʧ��
bool QxStgBlock::Read(CArchive &ar,short type)
{
	ASSERT(ar.IsLoading());
	if(!Read(ar) || GetType() != type){
		return false;
	}
	return true;
}

// �����ж�ȡ
bool QxStgBlock::Read(CArchive &ar)
{
	ASSERT(ar.IsLoading());

	//����ɵġ�
	Clear();
	nReadCVNum_ = 0;

	if(!ReadHeaderData(ar)){
		Reset();
		return false;
	}
	
	if(nDataSize_ > 0){
		pDataBuf_ = new BYTE[nDataSize_];

		UINT nrr = ar.Read(pDataBuf_,nDataSize_);
		if(nrr != nDataSize_){
			Clear();
			return false;
		}
	}else{
		ASSERT(FALSE);
		Clear();
		return false;
	}

	//���ν���ÿ���������
	if(!ReadCodeValueArray(pDataBuf_,false)){ //���ÿ�����ֱ��ʹ��buf
		Clear();
		return false;
	}

	//����end magic
	UINT dwFlag = 0;
	ar >> dwFlag;
	if(dwFlag != KSTG_BLOCK_END_MAGIC){
		Clear();
		return false;
	}

	return true;
}

// ���ڴ��ж�ȡ�����ض�ȡ���ֽ�����ʧ�ܣ�����0
int QxStgBlock::ReadFromBuf(BYTE *pBuf,bool bCopy)
{
	//����ɵġ�
	Clear();
	nReadCVNum_ = 0;
	//
	int n = ReadHeaderData(pBuf);
	if(n == 0){
		Reset();
		return 0;
	}

	if(!ReadCodeValueArray(pBuf+n,bCopy)){
		Clear();
		return 0;
	}
	n += nDataSize_;
	//����ĩβ���
	UINT dwFlag = 0;
	memcpy(&dwFlag,pBuf+n,sizeof(UINT));
	if(dwFlag != KSTG_BLOCK_END_MAGIC){
		Clear();
		return false;
	}
	
	return n + sizeof(UINT);
}

bool QxStgBlock::ReadHeaderData(CArchive &ar)
{
	BYTE bbuf[32];
	UINT shortsz = sizeof(short);
	UINT nrr = 0,nsz = 0;
	// 
	nsz = 3 * shortsz + 1;//������CV�ĸ���
	nrr = ar.Read(bbuf,nsz);
	if(nrr != nsz){
		return false;
	}
	//
	BYTE *pb = bbuf;
	memcpy(&type_,pb,sizeof(short));
	pb += sizeof(short);
	memcpy(&version_,pb,sizeof(short));
	pb += sizeof(short);
	memcpy(&attrib_,pb,sizeof(short));
	pb += sizeof(short);
	// CV�ĸ���
	nReadCVNum_ = *pb; 

	//��ȡdatasize���ֽڸ���
	// 0-1bit ��¼datasize��byte����0-3��Ҫת��1-4
	UINT nbsDataSize = (attrib_ & 0x03) + 1;
	// ��ȡdatasize
	nrr = ar.Read(bbuf,nbsDataSize);
	if(nrr != nbsDataSize){
		return false;
	}
	// �ȶ�ȡdatasize��ֵ
	nDataSize_ = uxReadArrayNum(bbuf,nbsDataSize);
	if(nReadCVNum_ <= 0 || nDataSize_ <= 0){
		return false;
	}
	
	return true;
}

// ��ȡͷ�����ݣ����ض�ȡ����
int QxStgBlock::ReadHeaderData(BYTE *pBuf)
{
	UINT shortsz = sizeof(short);
	BYTE *pb = pBuf;
	
	memcpy(&type_,pb,shortsz);
	pb += shortsz;
	memcpy(&version_,pb,shortsz);
	pb += shortsz;
	memcpy(&attrib_,pb,shortsz);
	pb += shortsz;
	nReadCVNum_ = *pb; //
	pb ++;
	
	//��ȡ�ֽڸ���
	// 0-1 ��¼datasize��byte����0-3��Ҫת��1-4
	int nbsDataSize = (attrib_ & 0x03) + 1;
	// ��ȡdatasize
	nDataSize_ = uxReadArrayNum(pb,nbsDataSize);
	if(nReadCVNum_ <= 0 || nDataSize_ <= 0){
		Reset();
		return 0;
	}

	return shortsz * 3 + 1 + nbsDataSize;
}

// ��ȡ���е�CV
bool QxStgBlock::ReadCodeValueArray(BYTE *pBuf,bool bCopy)
{
	int ix, nRest = nDataSize_,nReadBytes = 0;
	BYTE *pb = pBuf;
	QxStgCodeValue *pCV = NULL;

	//���ζ�ȡÿ��ֵ��
	for(ix = 0;ix < nReadCVNum_;ix ++){
		pCV = new QxStgCodeValue;
		nReadBytes = pCV->ReadData(pb,bCopy);
		if(!nReadBytes){
			Clear();
			delete pCV;
			return false;
		}
		DoAddCodeValue(pCV);
		
		pb += nReadBytes;
		nRest -= nReadBytes;
	}
	if(nRest != 0){
		ASSERT(FALSE);
		return false;
	}
	return true;
}

QxStgCodeValue* QxStgBlock::GetCodeValueByCode(vcode_t code)
{
	QxStgCodeValue *pCV = NULL;
	QxStgCodeValueMap::iterator ite = aCodeValueMap_.find(code);
	if(ite != aCodeValueMap_.end()){
		pCV = (*ite).second;
	}
	return pCV;
}

bool QxStgBlock::GetValueByCode(vcode_t code,int &ival)
{
	QxStgCodeValue *pCV = GetCodeValueByCode(code);
	if(!pCV || !pCV->IsValid()){
		return false;
	}
	return pCV->GetValue(ival);
}

bool QxStgBlock::GetValueByCode(vcode_t code,unsigned int &ival)
{
	QxStgCodeValue *pCV = GetCodeValueByCode(code);
	if(!pCV || !pCV->IsValid()){
		return false;
	}
	return pCV->GetValue(ival);
}

bool QxStgBlock::GetValueByCode(vcode_t code,double &dval)
{
	QxStgCodeValue *pCV = GetCodeValueByCode(code);
	if(!pCV || !pCV->IsValid()){
		return false;
	}
	return pCV->GetValue(dval);
}

bool QxStgBlock::GetValueByCode(vcode_t code,double da[3])
{
	QxStgCodeValue *pCV = GetCodeValueByCode(code);
	if(!pCV || !pCV->IsValid()){
		return false;
	}
	return pCV->GetValue(da);
}

bool QxStgBlock::GetValueByCode(vcode_t code,bool &bval)
{
	QxStgCodeValue *pCV = GetCodeValueByCode(code);
	if(!pCV || !pCV->IsValid()){
		return false;
	}
	return pCV->GetValue(bval);
}

bool QxStgBlock::GetValueByCode(vcode_t code,BYTE *&pb,int &size)
{
	QxStgCodeValue *pCV = GetCodeValueByCode(code);
	if(!pCV || !pCV->IsValid()){
		return false;
	}
	return pCV->GetValue(pb,size);
}

bool QxStgBlock::GetValueByCode(vcode_t code,std::string& str)
{
	QxStgCodeValue *pCV = GetCodeValueByCode(code);
	if(!pCV || !pCV->IsValid()){
		return false;
	}
	return pCV->GetValue(str);
}

QxStgBlock* QxStgBlock::GetSubBlock(vcode_t code) //��ȡ��Ƕ���ӿ�
{
	QxStgCodeValue *pCV = GetCodeValueByCode(code);
	if(!pCV || !pCV->IsValid()){
		return NULL;
	}
	return pCV->GetBlock();
}

bool QxStgBlock::GetValueByCode(vcode_t code, gp_Pnt &pnt)
{
	double v[3];
	if (GetValueByCode(code, v)) {
		pnt.SetCoord(v[0], v[1], v[2]);
		return true;
	}
	return false;
}

bool QxStgBlock::GetValueByCode(vcode_t code, gp_Vec &vec)
{
	double v[3];
	if (GetValueByCode(code, v)) {
		vec.SetCoord(v[0], v[1], v[2]);
		return true;
	}
	return false;
}

bool QxStgBlock::GetValueByCode(vcode_t code, gp_Dir &dir)
{
	double v[3];
	if (GetValueByCode(code, v)) {
		dir.SetCoord(v[0], v[1], v[2]);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// д�벿�ֵ�ʵ��

QxStgBlock::QxStgBlock(short type)
{
	nReadCVNum_ = 0;
	Initialize(type);
}

bool QxStgBlock::Initialize(short type)
{
	ASSERT(KSTG_IS_VALID_BLOCK_TYPE(type));
	//����ɵ�
	Clear();

	type_ = type;
	return true;
}

bool QxStgBlock::AddCodeValue(QxStgCodeValue *pCodeValue)
{
	if(!pCodeValue || !pCodeValue->IsValid()){
		ASSERT(FALSE);
		return false;
	}
	ASSERT(!CodeExisted(pCodeValue->GetCode())); //code�����ظ�
	return DoAddCodeValue(pCodeValue);
}

bool QxStgBlock::AddCodeValue(vcode_t code,int ival)
{
	ASSERT(!CodeExisted(code) && KSTG_IS_VALID_CODE(code));
	QxStgCodeValue *pCV = new QxStgCodeValue(code,ival);
	return DoAddCodeValue(pCV);
}

bool QxStgBlock::AddCodeValue(vcode_t code,unsigned int ival)
{
	ASSERT(!CodeExisted(code) && KSTG_IS_VALID_CODE(code));
	QxStgCodeValue *pCV = new QxStgCodeValue(code,ival);
	return DoAddCodeValue(pCV);
}

bool QxStgBlock::AddCodeValue(vcode_t code,double dval)
{
	ASSERT(!CodeExisted(code) && KSTG_IS_VALID_CODE(code));
	QxStgCodeValue *pCV = new QxStgCodeValue(code,dval);
	return DoAddCodeValue(pCV);
}

bool QxStgBlock::AddCodeValue(vcode_t code,double da[3])
{
	ASSERT(!CodeExisted(code) && KSTG_IS_VALID_CODE(code));
	QxStgCodeValue *pCV = new QxStgCodeValue(code,da);
	return DoAddCodeValue(pCV);
}

bool QxStgBlock::AddCodeValue(vcode_t code,bool bval)
{
	ASSERT(!CodeExisted(code) && KSTG_IS_VALID_CODE(code));
	QxStgCodeValue *pCV = new QxStgCodeValue(code,bval);
	return DoAddCodeValue(pCV);
}

bool QxStgBlock::AddCodeValue(vcode_t code,BYTE *pb,int size,bool bcopy)
{
	ASSERT(!CodeExisted(code) && KSTG_IS_VALID_CODE(code));
	ASSERT(pb && (size > 0));
	QxStgCodeValue *pCV = new QxStgCodeValue(code,pb,size,bcopy);
	return DoAddCodeValue(pCV);
}

bool QxStgBlock::AddCodeValue(vcode_t code,const char *pszBuf)
{
	ASSERT(!CodeExisted(code) && KSTG_IS_VALID_CODE(code));
	ASSERT(pszBuf && strlen(pszBuf) > 0);
	QxStgCodeValue *pCV = new QxStgCodeValue(code,pszBuf);
	return DoAddCodeValue(pCV);
}

// ����һ���ӿ飬�ӿ�ָ����¼���ڲ���bFreeBlk:true,�����ڲ������ͷŸö���
bool QxStgBlock::AddCodeValue(vcode_t code,QxStgBlock *pBlk,bool bFreeBlk)
{
	ASSERT(!CodeExisted(code) && KSTG_IS_VALID_CODE(code));
	ASSERT(pBlk && pBlk->IsValid());
	QxStgCodeValue *pCV = new QxStgCodeValue(code,pBlk,bFreeBlk);
	return DoAddCodeValue(pCV);
}

bool QxStgBlock::AddCodeValue(vcode_t code, const gp_Pnt &p)
{
	double v[3];
	v[0] = p.X(); v[1] = p.Y(); v[2] = p.Z();
	return AddCodeValue(code, v);
}

bool QxStgBlock::AddCodeValue(vcode_t code, const gp_Vec &vec)
{
	double v[3];
	v[0] = vec.X(); v[1] = vec.Y(); v[2] = vec.Z();
	return AddCodeValue(code, v);
}

bool QxStgBlock::AddCodeValue(vcode_t code, const gp_Dir &dir)
{
	double v[3];
	v[0] = dir.X(); v[1] = dir.Y(); v[2] = dir.Z();
	return AddCodeValue(code, v);
}

// д������
bool QxStgBlock::Write(CArchive& ar)
{
	ASSERT(ar.IsStoring());
	if(aCodeValueArray_.empty() || type_ == KSTG_BLOCK_TYPE_UNKNOWN){
		ASSERT(FALSE);
		return false;
	}

	// ʹ���ڲ���buf
	if(!BuildDataBuffer(NULL)){
		return false;
	}

	//д�����Ϣ
	BYTE bbuf[32];
	int n = BuildHeaderBuf(bbuf);
	
	//д�����Ϣ
	ar.Write(bbuf,n);

	//д������
	ar.Write(pDataBuf_,nDataSize_);

	//д��ĩβ��ʶ
	ar << nEndMagic_;

	return true;
}

// д�뵽�ڴ��У�����д��byte��Ŀ
int QxStgBlock::WriteToBuf(BYTE *pBuf)
{
	if(aCodeValueArray_.empty() || type_ == KSTG_BLOCK_TYPE_UNKNOWN){
		ASSERT(FALSE);
		return 0;
	}

	//д��ͷ
	int n = BuildHeaderBuf(pBuf);
	
	//д������
	BuildDataBuffer(pBuf+n);
	n += nDataSize_;
	//
	//д��ĩβ��ʶ
	memcpy(pBuf+n,&nEndMagic_,sizeof(UINT));
	
	return n + sizeof(UINT);
}

// ����ͷ����,����buf����Чbyte����
int QxStgBlock::BuildHeaderBuf(BYTE *pBuf)
{
	BYTE *pb = pBuf;
	//
	memcpy(pb,&type_,sizeof(short));
	pb += sizeof(short);
	memcpy(pb,&version_,sizeof(short));
	pb += sizeof(short);
	//д���¼datasize��Ҫ���ֽ���
	int nbsDataSize = uxCalcBytesOfArrayNum(nDataSize_);
	// 0-1 ��¼datasize��byte��1-4��Ҫת��0-3
	short attr = nbsDataSize - 1;
	memcpy(pb,&attr,sizeof(short));
	pb += sizeof(short);
	
	//��д���������,����������ᳬ��255
	*pb = (BYTE)(aCodeValueArray_.size() & 0xFF);
	pb ++;
	//д��datasize
	int nbs = uxWriteArrayNum(pb,nDataSize_);

	return (sizeof(short) * 3 + nbs + 1);
}

// �������������ݵ�buf
bool QxStgBlock::BuildDataBuffer(BYTE *pBuf)
{
	ASSERT(!aCodeValueArray_.empty());
	BYTE *pb = NULL;
	int oldSize = nDataSize_;
		
	// �������ݿ��С
	nDataSize_ = CalcBlockDataSize();
	if(nDataSize_ == 0){
		ASSERT(FALSE);
		return false;
	}
	
	if(pBuf){//ʹ�ô����buf
		pb = pBuf;
	}else{//ʹ���ڲ�buf	
		if(!pDataBuf_){
			pDataBuf_ = new BYTE[nDataSize_];
		}else if(oldSize < nDataSize_){//��Ҫ���·���
			delete[] pDataBuf_;
			pDataBuf_ = new BYTE[nDataSize_];
		}
		pb = pDataBuf_;
	}

	//���ο�������
	QxStgCodeValueArray::size_type ix,isize = aCodeValueArray_.size();
	QxStgCodeValue *pCV = NULL;
	int nWriteBytes = 0,nRest = nDataSize_;
	
	for(ix = 0;ix < isize;ix ++){
		pCV = aCodeValueArray_.at(ix);
		nWriteBytes = pCV->WriteData(pb);
		pb += nWriteBytes;
		nRest -= nWriteBytes;//ʣ��ռ�
	}
	ASSERT(nRest == 0);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// һ��shape��ص�����
kstgShapeData::kstgShapeData()
{
}

kstgShapeData::~kstgShapeData()
{
}

// ��������
BOOL kstgShapeData::Build(const TopoDS_Shape& aShape,BYTE *&pData,int &nSize)
{
	if(aShape.IsNull()){
		ASSERT(FALSE);
		return FALSE;
	}

	// ʹ���ڴ�ostream��д���ڴ棬���Ч��
	std::ostrstream ostrm;
	BRepTools::Write(aShape,ostrm,false,false, //��д���ʷ����ݣ�û�б�Ҫ
		TopTools_FormatVersion_CURRENT);

	//��ȡ����
	char *pStr = ostrm.str();
	uLong ssLen = (uLong)ostrm.pcount();//���ݳ���,������\0
	if(!pStr || ssLen == 0)
		return FALSE;

	//��ȡshape������
	BYTE *pBuf = new BYTE[ssLen];
	memcpy(pBuf,pStr,ssLen);
	
	//compress data
	uLong cpSize = compressBound(ssLen);
	pData = new BYTE[cpSize + 16];//
	if(!pData){
		delete[] pBuf;
		return FALSE;
	}
	
	//ѹ��
	uLong destLen = cpSize + 16;//��ʼ�����С������ѹ�����ݷŵ���
	//ͷһ��int��¼ѹ��ǰ���ݵĴ�С��int32��
	int ierr = compress2(pData + sizeof(int32_t),&destLen,pBuf,ssLen,Z_BEST_SPEED);//destLen����ѹ�������ݵĴ�С
	delete[] pBuf;

	if(ierr != Z_OK){
		delete[] pData;
		pData = NULL;
		return FALSE;
	}

	// record compressed data size,��¼ԭ�������ݳ��ȵ����ͷ�� 
	int rawLen = (int)ssLen;
	memcpy(pData,&rawLen,sizeof(int32_t));
	nSize = sizeof(int32_t) + destLen;//�������ݳ��ȼ�¼������ѹ������ܳ���
	
	return TRUE;
}

// ����
TopoDS_Shape	kstgShapeData::Parse(BYTE *pData,int nSize)
{
	TopoDS_Shape aShape;
	if(!pData || nSize == 0)
		return aShape;

	// first,uncompress data
	int rsize = 0;
	memcpy(&rsize,pData,sizeof(int32_t));//ѹ��ǰ���ݴ�С
	BYTE *pBuf = new BYTE[rsize + 16];
	if(!pBuf)
		return aShape;

	uLong usize = rsize + 16;
	int ierr = uncompress(pBuf,&usize,pData + sizeof(int32_t),nSize - sizeof(int32_t));
	if(ierr != Z_OK || usize != rsize){
		delete[] pBuf;
		return aShape;
	}
	
	BRep_Builder BB;
	std::string sData((const char *)pBuf,usize);//�����ȳ�ʼ��
	std::istringstream iss(sData);
	BRepTools::Read(aShape,iss,BB);
	
	return aShape;
}
