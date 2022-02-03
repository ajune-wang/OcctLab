#include "StdAfx.h"
#include "kcEntity.h"
#include "kcLayer.h"
#include "kcBasePlane.h"
#include "QxStgBlock.h"
#include "QxStgModelDef.h"
#include "kcHandleMgr.h"

////////////////////////////////////////////////////////
//
kcHandleMap::kcHandleMap()
{
	_pHandleMap = NULL;
	_nMapSize = 0;
	_nMaxHandle = 0;
	_nIdleHandle = 0;
}

kcHandleMap::~kcHandleMap()
{
	Clear();
}

bool  kcHandleMap::Initialize(int capacity)
{
	//����ɵ�
	if(_pHandleMap){
		Clear();
	}

	_nMapSize = capacity;
	if(capacity <= 0){
		_nMapSize = 256;
	}

	int ix;
	_pHandleMap = new BYTE[_nMapSize];
	for(ix = 0;ix < _nMapSize;ix ++)
		_pHandleMap[ix] = 0;
	
	_pHandleMap[0] = 1;//0����Ч��Handle�����ܷ���
	_nIdleHandle = _nMapSize * 8 - 1;//ȥ��0

	return true;
}

void  kcHandleMap::Clear()
{
	if(_pHandleMap){
		delete [] _pHandleMap;
		_pHandleMap = NULL;
	}

	_nMapSize = 0;
	_nMaxHandle = 0;
	_nIdleHandle = 0;
}

// ��n��byte��˵��handle��Ӧ��ΧΪ[0,n*8-1]
// ���磺��mapsizeΪ1��handleΪ0-255��256��.
//
int  kcHandleMap::MapMaxHandle()
{
	ASSERT(_nMapSize > 0);
	return (_nMapSize * 8 - 1);
}

// ����һ�����õ�Handle.
// ������ԣ����������䣬���û���ˣ��ٿ�ǰ���Ƿ��п��У�����д�
// �����з��䡣
//
int  kcHandleMap::AllocHandle()
{
	int nHandle = 0;
	int ix = 0;
	BYTE bv = 0;

	//���û�п���handle���ٷ���
	if(_nIdleHandle <= 0){
		//�������ˣ���Ҫ�ٴη����ڴ档
		BYTE *pBuf = new BYTE[_nMapSize * 2];
		if(pBuf){
			for(ix = 0;ix < _nMapSize;ix ++)
				pBuf[ix] = _pHandleMap[ix];
			for(ix = 0;ix <_nMapSize;ix ++)
				pBuf[_nMapSize + ix] = 0;

			delete[] _pHandleMap;
			_pHandleMap = pBuf;
			_nIdleHandle = _nMapSize << 8;
			_nMapSize *= 2;
		}
	}

	if(_nIdleHandle > 0){
		if(_nMaxHandle < MapMaxHandle()){
			_nMaxHandle ++;//�ȼ�
			nHandle = _nMaxHandle;
			_nIdleHandle --;
			
			//���
			ix = _nMaxHandle >> 3;// 
			bv = 0x01 << (_nMaxHandle % 8);
			_pHandleMap[ix] |= bv;
		}else{
			//Ѱ���ͷŵ�handle��
			for(ix = 0;ix < _nMapSize;ix ++){
				if(_pHandleMap[ix] != 0xFF)
					break;
			}
			nHandle = ix * 8;

			bv = 0x01;
			for(ix = 0;ix < 8;ix ++){
				if((_pHandleMap[ix] & bv) == 0){
					_pHandleMap[ix] |= bv;
					break;
				}
				bv = bv << 1;
			}
			nHandle += ix;
		}
	}

	return nHandle;
}

// �ͷ�һ�����õ�Handle
void  kcHandleMap::FreeHandle(int handle)
{
	if(!KC_IS_VALID_HANDLE(handle) || handle > _nMaxHandle)
		return;

	int ix = handle >> 3;
	BYTE bv = 0x01 << (handle % 8);

	_pHandleMap[ix] &= ~bv;

	_nIdleHandle ++;
	if(_nMaxHandle == handle)
		_nMaxHandle --;
}

// ���һ��handle�Ѿ�ʹ����,Ҫ��֤�ڴ��Ѿ������㹻�ˡ�
// �ڲ������Զ������ڴ�.
bool  kcHandleMap::MaskHandle(int nHandle)
{
	//��Щ��Ҫ��֤
	ASSERT(_nMapSize > 0 && nHandle >0 && nHandle <= MapMaxHandle());

	int ix = nHandle >> 3;// 
	BYTE bv = 0x01 << (nHandle % 8);
	ASSERT(ix < _nMapSize);
	if((_pHandleMap[ix] & bv) == 0){//��δ���
		_pHandleMap[ix] |= bv;

		if(_nMaxHandle < nHandle)	_nMaxHandle = nHandle;
		_nIdleHandle --;
	}else{
		ASSERT(FALSE);
	}

	return true;
}

bool  kcHandleMap::Read(CArchive& ar)
{
	QxStgBlock blk;

	if(!blk.Read(ar,KSTG_BLOCK_HANDLE_MAP)){
		ASSERT(FALSE);
		return false;
	}

	int nMapSize = 0;
	if(blk.GetValueByCode(0,nMapSize)){
		Initialize(nMapSize);
		return true;
	}else{
		ASSERT(FALSE);
	}

	return false;
}

// �������д���˴�С��������Ҫ��ȡ����ʱ����ͨ��mask��ӻ���
//
bool  kcHandleMap::Write(CArchive& ar)
{
	QxStgBlock blk;

	blk.Initialize(KSTG_BLOCK_HANDLE_MAP);//����Ϊ����֤
	blk.AddCodeValue(0,_nMapSize);
	
	return (blk.Write(ar) ? true : false);
}

////////////////////////////////////////////////////////
//
kcHandleMgr::kcHandleMgr(void)
{
}

kcHandleMgr::~kcHandleMgr(void)
{
	Clear();
}

bool  kcHandleMgr::Initialize()
{
	aLayerHandleMap_.Initialize();
	aEntityHandleMap_.Initialize();
	aBasePlaneHandleMap_.Initialize();

	InitHandleMapMap();
	
	return true;
}

// �����˳��ͱ���Ͷ�ȡ��Ӧ��һ��
void  kcHandleMgr::InitHandleMapMap()
{
	aHandleMapMap_.clear();
	
	//����ע����ͬ��map���Ա��entityͳһ����id
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_POINT,&aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_EDGE,&aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_WIRE,&aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_FACE,&aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_SHELL,&aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_SOLID,&aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_COMPOUND,&aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_BLOCK,&aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_DIM_DIAMETER, &aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_DIM_RADIUS, &aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_DIM_LENGTH, &aEntityHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_DIM_ANGLE, &aEntityHandleMap_));
	//
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_ENT_LAYER,&aLayerHandleMap_));
	aHandleMapMap_.insert(stdHandleMapMap::value_type(KCT_BASE_PLANE,&aBasePlaneHandleMap_));
}

void  kcHandleMgr::Clear()
{
	aEntityHandleMap_.Clear();
	aLayerHandleMap_.Clear();
	aBasePlaneHandleMap_.Clear();
		
	aHandleMapMap_.clear();

	//���³�ʼ��
	Initialize();
}

kcHandleMap*  kcHandleMgr::FindHandleMap(int entType)
{
	stdHandleMapMap::iterator ite = aHandleMapMap_.find(entType);
	if(ite != aHandleMapMap_.end())
		return (*ite).second;
	return NULL;
}

//�������ͣ�������ͷ�Handle
int  kcHandleMgr::AllocHandle(int entType)
{
	int nHandle = 0;
	kcHandleMap *pHandleMap = FindHandleMap(entType);
	ASSERT(pHandleMap);
	if(pHandleMap){
		nHandle = pHandleMap->AllocHandle();
	}

	return nHandle;
}

void  kcHandleMgr::FreeHandle(int entType,int handle)
{
	kcHandleMap *pHandleMap = FindHandleMap(entType);
	ASSERT(pHandleMap);
	if(pHandleMap){
		pHandleMap->FreeHandle(handle);
	}
}

// ���һ��handle�Ѿ�ʹ����
bool  kcHandleMgr::MaskHandle(int entType,int handle)
{
	kcHandleMap *pHandleMap = FindHandleMap(entType);
	ASSERT(pHandleMap);
	if(pHandleMap){
		pHandleMap->MaskHandle(handle);
		return true;
	}
	return false;
}

//ע�⣺�������Ͷ�ȡ����Ҫ��֤˳��
// �ı�˳����ɾmap�����ᵼ���ļ���ʽ�ı�
bool  kcHandleMgr::Read(CArchive& ar)
{
	QxStgBlock blk;
	int nbMap = 0;

	if (!blk.Read(ar, KSTG_BLOCK_HANDLE_MGR)) {
		return false;
	}
	if (!blk.GetValueByCode(0, nbMap) || nbMap != 3) {
		ASSERT(FALSE);
		return false;
	}
	//���ζ�ȡ
	aEntityHandleMap_.Read(ar);
	aLayerHandleMap_.Read(ar);
	aBasePlaneHandleMap_.Read(ar);
	//��Ҫ�ĳ�ʼ��
	InitHandleMapMap();

	return true;
}

bool  kcHandleMgr::Write(CArchive& ar)
{
	//д��ͷ
	QxStgBlock blk;
	int nbMap = 3;

	blk.Initialize(KSTG_BLOCK_HANDLE_MGR);
	blk.AddCodeValue(0, nbMap);
	blk.Write(ar);

	//����д��ÿ��map
	aEntityHandleMap_.Write(ar);
	aLayerHandleMap_.Write(ar);
	aBasePlaneHandleMap_.Write(ar);
	
	return true;
}