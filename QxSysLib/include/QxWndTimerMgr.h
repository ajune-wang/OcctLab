//
#ifndef _QX_WND_TIMER_MGR_H_
#define _QX_WND_TIMER_MGR_H_

#include <map>
#include "QxLibDef.h"
#include "QxSysInterface.h"

class QxTimerState;
typedef std::map<UINT_PTR,QxTimerState *> QxTimerStateMap;

///////////////////////////////////////////////////////////
// ����MainFrame�Ķ�ʱ��

class QXLIB_API QxMainWndTimerMgr : public IQxTimerManager{
public:
	QxMainWndTimerMgr(HWND hMainWnd);
	~QxMainWndTimerMgr();

	// �趨һ����ʱ��,����ID�����ID���ں����
	virtual UINT_PTR		SetTimer(IQxTimerHandler *pTimerHandler,UINT uElapse);
	// 
	virtual bool			KillTimer(UINT_PTR nIDEvent);

	// �Ƿ��Ѿ����ڶ�ʱ��
	virtual bool			HasTimer(UINT_PTR nIDEvent);
		
	// ��ͣ��ʱ��
	virtual bool			PauseTimer(UINT_PTR nIDEvent);
	// �ָ���ʱ��
	virtual bool			ResumeTimer(UINT_PTR nIDEvent);

	//��ʱ������
	virtual int			OnTimer(UINT_PTR nIDEvent,DWORD dwTime);

protected:
	void					RemoveTimerState(UINT_PTR nIDEvent);
	QxTimerState*			GetTimerState(UINT_PTR nIDEvent);

protected:
	HWND					hMainWnd_; //��ʱ�������Ĵ���
	UINT_PTR				uNextID_;  //��һ�����õ�IDֵ
	QxTimerStateMap		aTimerStateMap_;
};

#endif