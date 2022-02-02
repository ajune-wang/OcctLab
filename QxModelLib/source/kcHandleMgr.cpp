#include "StdAfx.h"
#include "kcEntity.h"
#include "kcLayer.h"
#include "kcBasePlane.h"
#include "QxStgBlock.h"
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

	if(!blk.Read(ar) || blk.GetType() != 0x1123){
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

bool  kcHandleMap::Write(CArchive& ar)
{
	QxStgBlock blk;

	blk.Initialize(0x1123);//����Ϊ����֤
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
	_layerHandleMap.Initialize();
	_pointHandleMap.Initialize();
	_curveHandleMap.Initialize();
	_wireHandleMap.Initialize();
	_faceHandleMap.Initialize();
	_shellHandleMap.Initialize();
	_solidHandleMap.Initialize();
	_compHandleMap.Initialize();
	_blockHandleMap.Initialize();
	aDiamDimHandleMap_.Initialize();
	aRadDimHandleMap_.Initialize();
	aLenDimHandleMap_.Initialize();
	aAngDimHandleMap_.Initialize();
	_basePlaneHandleMap.Initialize();

	InitHandleMapMap();
	
	return true;
}

// �����˳��ͱ���Ͷ�ȡ��Ӧ��һ��
void  kcHandleMgr::InitHandleMapMap()
{
	_handleMapMap.clear();
	_handleMapArray.clear();

	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_POINT,&_pointHandleMap));
	_handleMapArray.push_back(KCT_ENT_POINT);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_EDGE,&_curveHandleMap));
	_handleMapArray.push_back(KCT_ENT_EDGE);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_WIRE,&_wireHandleMap));
	_handleMapArray.push_back(KCT_ENT_WIRE);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_FACE,&_faceHandleMap));
	_handleMapArray.push_back(KCT_ENT_FACE);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_SHELL,&_shellHandleMap));
	_handleMapArray.push_back(KCT_ENT_SHELL);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_SOLID,&_solidHandleMap));
	_handleMapArray.push_back(KCT_ENT_SOLID);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_COMPOUND,&_compHandleMap));
	_handleMapArray.push_back(KCT_ENT_COMPOUND);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_BLOCK,&_blockHandleMap));
	_handleMapArray.push_back(KCT_ENT_BLOCK);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_DIM_DIAMETER, &aDiamDimHandleMap_));
	_handleMapArray.push_back(KCT_ENT_DIM_DIAMETER);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_DIM_RADIUS, &aRadDimHandleMap_));
	_handleMapArray.push_back(KCT_ENT_DIM_RADIUS);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_DIM_LENGTH, &aLenDimHandleMap_));
	_handleMapArray.push_back(KCT_ENT_DIM_LENGTH);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_DIM_ANGLE, &aAngDimHandleMap_));
	_handleMapArray.push_back(KCT_ENT_DIM_ANGLE);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_ENT_LAYER,&_layerHandleMap));
	_handleMapArray.push_back(KCT_ENT_LAYER);
	_handleMapMap.insert(stdHandleMapMap::value_type(KCT_BASE_PLANE,&_basePlaneHandleMap));
	_handleMapArray.push_back(KCT_BASE_PLANE);
}

void  kcHandleMgr::Clear()
{
	kcHandleMap *pHandleMap = NULL;
	stdHandleMapMap::iterator ite;
	for(ite = _handleMapMap.begin();ite != _handleMapMap.end();ite ++){
		pHandleMap = (*ite).second;
		pHandleMap->Clear();
	}

	_handleMapArray.clear();
	_handleMapMap.clear();

	//���³�ʼ��
	Initialize();
}

kcHandleMap*  kcHandleMgr::FindHandleMap(int entType)
{
	stdHandleMapMap::iterator ite = _handleMapMap.find(entType);
	if(ite != _handleMapMap.end())
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
	std::vector<int>::size_type ix,isize = _handleMapArray.size();
	kcHandleMap *pHandleMap = NULL;
	
	for(ix = 0;ix < isize;ix ++){
		pHandleMap = FindHandleMap(_handleMapArray[ix]);
		ASSERT(pHandleMap);
		pHandleMap->Read(ar);
	}

	return true;
}

bool  kcHandleMgr::Write(CArchive& ar)
{
	std::vector<int>::size_type ix,isize = _handleMapArray.size();
	kcHandleMap *pHandleMap = NULL;
	
	for(ix = 0;ix < isize;ix ++){
		pHandleMap = FindHandleMap(_handleMapArray[ix]);
		ASSERT(pHandleMap);
		pHandleMap->Write(ar);
	}

	return true;
}