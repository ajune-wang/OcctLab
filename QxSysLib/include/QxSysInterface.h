// ���ڶ���ϵͳ�Ľӿ�
#ifndef _QX_SYS_INTERFACE_H_
#define _QX_SYS_INTERFACE_H_

// һ����ʱ���������ӿ�
class IQxTimerHandler{
public:
	virtual ~IQxTimerHandler() {}

	// ��Ӧ�Ĵ�����
	virtual void			OnStarting() {} //��ʱ��������������û����
	virtual void			OnStarted() {} //��ʱ���Ѿ�����
	virtual void			OnStopped() {}  //��ʱ������ֹ
	virtual void			OnPause() {}
	virtual void			OnResume() {}

	//��Ϣ��Ӧ����,dwPassTime:�Ǿ����ϴζ�ʱ����ȥ��ʱ�䣬��λms
	virtual int			OnTimer(UINT_PTR nIDEvent,DWORD dwPassTime) = 0;
};


// ���ʱ��������
class IQxTimerManager{
public:
	virtual ~IQxTimerManager() {}

	// �趨һ����ʱ��,����ID�����ID���ں����
	virtual UINT_PTR		SetTimer(IQxTimerHandler *pTimerHandler,UINT uElapse) = 0;
	// 
	virtual bool			KillTimer(UINT_PTR nIDEvent) = 0;

	// �Ƿ��Ѿ����ڶ�ʱ��
	virtual bool			HasTimer(UINT_PTR nIDEvent) = 0;
		
	// ��ͣ��ʱ��
	virtual bool			PauseTimer(UINT_PTR nIDEvent) = 0;
	// �ָ���ʱ��
	virtual bool			ResumeTimer(UINT_PTR nIDEvent) = 0;

	//��ʱ������
	virtual int			OnTimer(UINT_PTR nIDEvent,DWORD dwTime) = 0;
};

#endif