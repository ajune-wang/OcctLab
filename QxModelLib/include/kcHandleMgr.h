//////////////////////////////////////////////////////////////////////////
// �������һ��model��һ����һ��model�����ж����handle����Ψһ�ġ�
//
#ifndef _KC_HANDLE_MGR_H_
#define _KC_HANDLE_MGR_H_

#include <vector>
#include <map>
#include "QxLibDef.h"

// ��1��ʼ��handle��0����ʾ�Ƿ�handle��
#ifndef  KC_IS_VALID_HANDLE
#define  KC_IS_VALID_HANDLE(h) (h != 0)
#endif

class CArchive;
class kcHandleMap;
typedef std::map<int,kcHandleMap *>		stdHandleMapMap;

// ����ΨһHandle�Ĺ������䡢�ͷŵȵ�
class QXLIB_API kcHandleMap{
public:
	kcHandleMap();
	~kcHandleMap();

	bool					Initialize(int capacity = 256);
	void					Clear();

	// ����һ�����õ�Handle.
	int					AllocHandle();
    // �ͷ�һ�����õ�Handle
	void					FreeHandle(int handle);
	// ���һ��handle�Ѿ�ʹ����
	bool					MaskHandle(int handle);

	bool					Read(CArchive& ar);
	bool					Write(CArchive& ar);

protected:
	int					MapMaxHandle();

protected:
	BYTE					*_pHandleMap;//ÿ��bit��ǵ�ǰhandle�Ƿ���á�
	int					_nMapSize;
	int					_nMaxHandle;//�Ѿ���������ľ��ֵ��
	int					_nIdleHandle;//���о������
};

class QXLIB_API kcHandleMgr
{
public:
	kcHandleMgr(void);
	~kcHandleMgr(void);

	bool					Initialize();
	void					Clear();

	//�������ͣ�������ͷ�Handle
	int					AllocHandle(int entType);
	void					FreeHandle(int entType,int handle);
	// ���һ��handle�Ѿ�ʹ����
	bool					MaskHandle(int entType,int handle);

	bool					Read(CArchive& ar);
	bool					Write(CArchive& ar);

protected:
	void					InitHandleMapMap();
	kcHandleMap*			FindHandleMap(int entType);

protected:
	kcHandleMap			_layerHandleMap;//ͼ��
	kcHandleMap			_pointHandleMap;
	kcHandleMap			_curveHandleMap;
	kcHandleMap			_wireHandleMap;
	kcHandleMap			_faceHandleMap;
	kcHandleMap			_shellHandleMap;
	kcHandleMap			_solidHandleMap;
	kcHandleMap			_compHandleMap;
	kcHandleMap			_blockHandleMap;
	kcHandleMap			aDiamDimHandleMap_;
	kcHandleMap			aRadDimHandleMap_;
	kcHandleMap			aLenDimHandleMap_;
	kcHandleMap			aAngDimHandleMap_;
	kcHandleMap			_basePlaneHandleMap;

	stdHandleMapMap		_handleMapMap;
	std::vector<int>		_handleMapArray;
};

#endif