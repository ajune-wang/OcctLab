//////////////////////////////////////////////////////////////////////////
// ��¼�������ݵ�һ����
#ifndef _KSTG_BLOCK_H_
#define _KSTG_BLOCK_H_

#include <vector>
#include <string>
#include <map>
#include "QxLibDef.h"

// Code Value������
typedef unsigned short vcode_t;

class CArchive;
class TopoDS_Shape;
class QxStgCodeValue;
class QxStgBlock;

typedef std::vector<QxStgCodeValue *>		QxStgCodeValueArray;
typedef std::map<vcode_t,QxStgCodeValue *>	QxStgCodeValueMap;



//�Ƿ�unsigned���͵����룬ռ�ݵ�4bite
#define KSTG_BITS_MASK_UNSIGNED	(0x1 << 3)
//code�Ƿ�short���ͣ���Ҫʹ��2�ֽڣ������룬ʹ��7bit
#define KSTG_BITS_MASK_SHORT_CODE	(0x1 << 6)	
//�Ƿ����������,ռ�ݵ�8bit
#define KSTG_BITS_MASK_ARRAY		(0x1 << 7)
// �����С��bit����,5,6��bit
#define KSTG_BITS_MASK_ARRAY_NUM	0x30



// ���Ͷ���
#define KSTG_TYPE_NULL		0    //δ֪������
#define KSTG_TYPE_CHAR		0x01
#define KSTG_TYPE_BYTE		(KSTG_TYPE_CHAR | KSTG_BITS_MASK_UNSIGNED)
#define KSTG_TYPE_SHORT		0x02
#define KSTG_TYPE_USHORT		(KSTG_TYPE_SHORT | KSTG_BITS_MASK_UNSIGNED)
#define KSTG_TYPE_INT			0x03
#define KSTG_TYPE_UINT		(KSTG_TYPE_INT | KSTG_BITS_MASK_UNSIGNED)
#define KSTG_TYPE_INT64		0x04
#define KSTG_TYPE_UINT64		(KSTG_TYPE_INT64 | KSTG_BITS_MASK_UNSIGNED)
#define KSTG_TYPE_BOOL		0x05
#define KSTG_TYPE_FLOAT		0x06
#define KSTG_TYPE_DOUBLE		0x07
#define KSTG_TYPE_BLOCK		0x08 //�����һ����������ͣ���¼һ��QxStgBlock����

#define KSTG_INVALID_CODE		0xFFFF //����ֵ����Ϊ��Чֵ������ʹ��

#define KSTG_IS_VALID_CODE(code) (code != KSTG_INVALID_CODE)

//�ж��Ƿ��޷�����
#define KSTG_IS_UNSIGNED(b)	((b & KSTG_BITS_MASK_UNSIGNED) != 0)
//�ж��Ƿ�����
#define KSTG_IS_ARRAY(b)		((b & KSTG_BITS_MASK_ARRAY) != 0)
//�ж�code�Ƿ�ʹ��2byte
#define KSTG_IS_SHORT_CODE(b)  ((b & KSTG_BITS_MASK_SHORT_CODE) != 0)

// ��¼һ����code��ֵ����ʽ���£�
// typeattr(1BYTE) code(1BYTE) [array num(1-4BYTE) or blocksize] data(�䳤����)
// ���typeattr: 1-4bit�����������ͣ�����3bit�����Ƿ��޷�������
//                 5-6bit��¼��¼���������block��С��byte����0-3��Ӧ1-4��BYTE
//                 7bit:��ʾcode�Ƿ��ֽڣ�1��codeʹ��2���ֽ�
//                 8bit:�����Ƿ�����
// �����������С��ʹ�ñ䳤���飨1-4��BYTE����¼��
// data�Ĵ�С��������¼�����������typeattr���������С��
class QXLIB_API QxStgCodeValue{
public:
	QxStgCodeValue();
	QxStgCodeValue(vcode_t code,int ival);
	QxStgCodeValue(vcode_t code,unsigned int ival);
	QxStgCodeValue(vcode_t code,double dval);
	QxStgCodeValue(vcode_t code,double da[3]);
	QxStgCodeValue(vcode_t code,bool bval);
	QxStgCodeValue(vcode_t code,const char *pszBuf);
	QxStgCodeValue(vcode_t code,BYTE* pbuf,int n,bool bCopy=false);
	QxStgCodeValue(vcode_t code,QxStgBlock *pBlock,bool bFree=true);
	~QxStgCodeValue();

	bool				IsValid() const;
	vcode_t				GetCode() const { return code_; }
	bool				IsUnsigned() const;
	bool				IsBlock() const;
	bool				IsArray() const;

	bool				Set(vcode_t code,int ival);
	bool				Set(vcode_t code,unsigned int ival);
	bool				Set(vcode_t code,double dval);
	bool				Set(vcode_t code,double da[3]);
	bool				Set(vcode_t code,bool bval);
	bool				Set(vcode_t code,const char *pszBuf);
	bool				Set(vcode_t code,BYTE* pbuf,int n,bool bCopy=false);
	bool				Set(vcode_t code,QxStgBlock *pBlock,bool bFree=true);

	void				Clear();

	// �����������ݵĴ�С
	int				GetFullSize();

public:
	bool				GetValue(int& iv) const;
	bool				GetValue(unsigned int& uiv) const;
	bool				GetValue(double& dv) const;
	bool				GetValue(double da[3]) const;
	bool				GetValue(bool& bv) const;
	bool				GetValue(std::string& str) const;
	bool				GetValue(BYTE *& pbuf,int &size) const;
	QxStgBlock*		GetBlock() const;

public:
	// ���ض�ȡ���ֽ�����ʧ�ܷ���0
	int				ReadData(BYTE *pBuf,bool bCopy=false);
	// ����д����ֽ�����ʧ�ܷ���0
	int				WriteData(BYTE *pBuf);

protected:
	void				Reset();
	void				SetCT(vcode_t code,BYTE type,bool bAry=false);

	int				CalcHeaderSize();
	void				CalcDataSize();
	void				CalcFullSize();

	BYTE				BuildTypeAttr();

	void				WriteValueToBuf(BYTE *pBuf);
	bool				ReadValueFromBuf(BYTE *pBuf,bool bCopy=false);

protected:
	BYTE				type_;//����,��4bit
	BYTE				attrib_;//���ԣ���4bit
	vcode_t			code_;//���
	bool				bArray_;//�Ƿ�����
	int				nArrayNum_;//�������
	int				nFullSize_;//�������ݵĴ�С
	int				nDataSize_;//���ݵĴ�С��byte��
	BYTE				*pValueBuf_;//�������ݵ����飬���������͡�code����������

	//��¼����ֵ
	union{
		char						cval;
		BYTE						ucval;
		short					sval;
		unsigned short			usval;
		int						ival;
		unsigned int				uival;
		int64_t					ival64;
		uint16_t					uival64;
		bool						bval;
		float					fval;
		double					dval;
		BYTE						*ary;//��¼�������������Ǹ��ݾ������ͱ䶯��
	}value_;
	bool				bCopyData_; //�Ƿ񿽱������ݣ�������������Ҫ�ͷ�

	//��¼һ��block������������ʵ��block�ڲ�����block��Ƕ�װ���
	QxStgBlock		*pBlock_;
	bool				bFreeBlock_;//�Ƿ��ͷŸö���
};

///////////////////////////////////////////////////////////////////////////////
//

//�������
#define KSTG_BLOCK_TYPE_UNKNOWN	0x0		//δ֪����

#define KSTG_IS_VALID_BLOCK_TYPE(t) (t != KSTG_BLOCK_TYPE_UNKNOWN)

///////////////////////////////////////////////////////////////////////////////
// ����ʵ�ֵĿ�

#define KSTG_CODE_MAP_SIZE 32

// ��Ľṹ��
// type(short) version(short) attrib(short) valueNum(1BYTE) datasize(1-4BYTE) һϵ��CodeValue����
// valueNum:��CodeValue�ĸ���,���ǵ�code��BYTE���ͣ����255������˿���ʹ��1byte
// attrib: 0-1bit�Ǽ�¼datasize��Ҫʹ�õ�byte��
//
class QXLIB_API QxStgBlockBase{
public:
	QxStgBlockBase();
	virtual ~QxStgBlockBase();

	virtual bool					IsValid() const;
	short						GetType() const;
	short						GetVersion() const;

	virtual void					Clear();

	int							GetCodeValueCount() const;
	QxStgCodeValue*				GetCodeValueByIndex(int idx) const;

	//��ȡ������Ĵ�С,�����Ƕ��ʹ��
	int							GetBlockSize();
	
protected:
	//
	void							Reset();
	// 
	bool							DoAddCodeValue(QxStgCodeValue *pCV);

	// �������������ݴ�С����������ͷ
	int							CalcBlockDataSize();
	// �����ͷ�Ĵ�С
	int							CalcBlockHeaderSize();

protected:
	bool							CodeExisted(vcode_t code) const;

protected:
	short						type_; //������
	short						version_; //�汾����һ������˵��Ӧ��������
	short						attrib_;//���ԣ���2bit��¼datasizeʹ�õ�byte��,34bit��¼cv�ĸ���
	int							nBlockSize_;//�������С����λbyte
	int							nDataSize_; //���ݿ��С��������codevalue��ֵ������������3��
	BYTE							*pDataBuf_; //���ݿ飬��СΪnDataSize_
	unsigned int					nEndMagic_; //��ĩβ��ʶ

protected:
	QxStgCodeValueArray			aCodeValueArray_;//
	QxStgCodeValueMap				aCodeValueMap_;//
};

class QXLIB_API QxStgBlock : public QxStgBlockBase{
public:
	QxStgBlock();
	QxStgBlock(short type);
	~QxStgBlock();


public://д�벿�ֽӿں���
	bool							Initialize(short type);
	
	bool							AddCodeValue(QxStgCodeValue *pCodeValue);
	bool							AddCodeValue(vcode_t code,int ival);
	bool							AddCodeValue(vcode_t code,unsigned int ival);
	bool							AddCodeValue(vcode_t code,double dval);
	bool							AddCodeValue(vcode_t code,double da[3]);
	bool							AddCodeValue(vcode_t code,bool bval);
	bool							AddCodeValue(vcode_t code,BYTE *pb,int size,bool bcopy=true);
	bool							AddCodeValue(vcode_t code,const char *pszBuf);
	// ����һ���ӿ飬�ӿ�ָ����¼���ڲ���bFreeBlk:true,�����ڲ������ͷŸö���
	bool							AddCodeValue(vcode_t code,QxStgBlock *pBlk,bool bFreeBlk=true);

	bool							Write(CArchive &ar);
	//д�뵽�ڴ��У�����д��byte��Ŀ
	int							WriteToBuf(BYTE *pBuf);

protected://д�벿�ָ�������
	//
	bool							BuildDataBuffer(BYTE *pBuf);
	// ����ͷ����,����buf����Чbyte����
	int							BuildHeaderBuf(BYTE *pBuf);				

public: //��ȡ���ֺ���
	// �����ж�ȡ
	bool							Read(CArchive &ar);
	// �����͵Ķ�ȡ���������g��ƥ�䣬����ʧ��
	bool							Read(CArchive &ar,short type);
	// ���ڴ��ж�ȡ�����ض�ȡ���ֽ�����ʧ�ܣ�����0
	int							ReadFromBuf(BYTE *pBuf,bool bCopy);

	//����code��ȡֵ��
	QxStgCodeValue*				GetCodeValueByCode(vcode_t code);
	bool							GetValueByCode(vcode_t code,int &ival);
	bool							GetValueByCode(vcode_t code,unsigned int &ival);
	bool							GetValueByCode(vcode_t code,double &dval);
	bool							GetValueByCode(vcode_t code,double da[3]);
	bool							GetValueByCode(vcode_t code,bool &bval);
	bool							GetValueByCode(vcode_t code,BYTE *&pb,int &size);
	bool							GetValueByCode(vcode_t code,std::string& str);
	QxStgBlock*					GetSubBlock(vcode_t code); //��ȡ��Ƕ���ӿ�
	
protected://��ȡ��������
	bool							ReadHeaderData(CArchive &ar);
	// ��ȡͷ�����ݣ����ض�ȡ����
	int							ReadHeaderData(BYTE *pBuf);
	// ��ȡ���е�CV
	bool							ReadCodeValueArray(BYTE *pBuf,bool bCopy);

protected:
	int							nReadCVNum_; //��ȡ��CV����ĸ���
};

//////////////////////////////////////////////////////////////////////////
// һ��shape��ص�����
// ����һ��shape�����ݣ�����BRep�ṹ�����������ַ�������ѹ����
// ���ս��������һ�����У���ṹ��ԭʼBRep���ݳ��� + ѹ�����BRep����
class QXLIB_API kstgShapeData{
public:
	kstgShapeData();
	~kstgShapeData();

	// ��������
	// nSize:���ݿ�Ĵ�С
	// pData:���е�����,ͷһ��int��¼ѹ��ǰ�����ݴ�С��������ѹ���������
	BOOL							Build(const TopoDS_Shape& aShape,BYTE *&pData,int &nSize);
	// ����
	TopoDS_Shape					Parse(BYTE *pData,int nSize);
};


#endif