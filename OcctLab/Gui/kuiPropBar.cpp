// kuiPropBar.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "kuiPropBar.h"
#include ".\kuipropbar.h"


// kuiPropBar

IMPLEMENT_DYNAMIC(kuiPropBar, CPinDockBar)
kuiPropBar::kuiPropBar()
{
}

kuiPropBar::~kuiPropBar()
{
}


BEGIN_MESSAGE_MAP(kuiPropBar, CPinDockBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// kuiPropBar ��Ϣ�������

void kuiPropBar::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
}

int kuiPropBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPinDockBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}

void kuiPropBar::OnSize(UINT nType, int cx, int cy)
{
	CPinDockBar::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
}
