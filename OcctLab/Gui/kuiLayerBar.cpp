// GUI\kuiLayerBar.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "kuiLayerBar.h"
#include ".\kuilayerbar.h"


// kuiLayerBar

IMPLEMENT_DYNAMIC(kuiLayerBar, CPinDockBar)
kuiLayerBar::kuiLayerBar()
{
}

kuiLayerBar::~kuiLayerBar()
{
}


BEGIN_MESSAGE_MAP(kuiLayerBar, CPinDockBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// kuiLayerBar ��Ϣ�������

void kuiLayerBar::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
}

int kuiLayerBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPinDockBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}

void kuiLayerBar::OnSize(UINT nType, int cx, int cy)
{
	CPinDockBar::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
}
