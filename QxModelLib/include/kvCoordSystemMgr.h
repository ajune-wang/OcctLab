//////////////////////////////////////////////////////////////////////////
//
#ifndef _KV_COORD_SYSTEM_MGR_H_
#define _KV_COORD_SYSTEM_MGR_H_

#include <vector>
#include <string>
#include "QxLibDef.h"
#include "kvCoordSystem.h"

class kuiModelTreeInterface;
typedef std::vector<kvCoordSystem *> TCoordSystemArray;

class QXLIB_API kvCoordSystemMgr
{
public:
	kvCoordSystemMgr(void);
	~kvCoordSystemMgr(void);

	// ��ʼ��
	bool					Initialize();
	void					Destroy();
	void					SetModelTreeInterface(kuiModelTreeInterface *pModelTree);
	void					Clear();

	// ���һ������ϵ
	kvCoordSystem*			AddCoordSystem(const kvCoordSystem& cs);
	bool					RemoveCoordSystem(const kvCoordSystem *pCS);
	kvCoordSystem*			FindCoordSystem(const char *pszName);

	// ��ȡ��ǰ����ϵ
	kvCoordSystem*			GetCurrentCS() const;
	
	// ���õ�ǰ����ϵ
	bool					SetCurrentCS(const kvCoordSystem *pCS);

	int						GetCount() const;
	kvCoordSystem*			GetCoordSystem(int idx);

public:
	//��������
	static std::string		GenCoordSystemName();

protected:
	TCoordSystemArray		m_csArray;//����ϵ�б�,���������е�����ϵ.
	DWORD					m_dwCurrCS;//��ǰ����ϵ���±�.
	kuiModelTreeInterface	*m_pModelTree;

protected:
	static DWORD			m_dwNextNameNo;//��һ�����õ��������
};

#endif
