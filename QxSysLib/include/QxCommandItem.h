//////////////////////////////////////////////////////////////////////////
//
#ifndef _QX_COMMAND_ITEM_H_
#define _QX_COMMAND_ITEM_H_

#include <string>
#include "QxLibDef.h"

#define KUI_MENU_BEFORE_SEP		0x01 //�˵�ǰ�ӷָ���
#define KUI_MENU_AFTER_SEP			0x02 //�˵���ӷָ���

class kiCommand;

//�����˵��Ķ��壬����ֱ��ʹ�ò˵����ı�
typedef enum QxAppTopMenuID{
	QX_TOP_MENU_CURVE = 0,  //���߲˵�
	QX_TOP_MENU_SURFACE,
	QX_TOP_MENU_SOLID,
	QX_TOP_MENU_UNKNOWN,  //δ֪
}eAppTopMenuID;

class QXLIB_API QxCommandItem
{
public:
	QxCommandItem(void);
	QxCommandItem(int nLocadID,bool bCached);
	~QxCommandItem(void);

	//���ò˵����ı�
	void						SetMenuText(const char *pszText1,const char *pszText2);
	void						SetMenuText(const char *pszText1,const char *pszText2,
										const char *pszText3);

	void						SetMenuText(eAppTopMenuID topMenuId,const char *pszText2);
	void						SetMenuText(eAppTopMenuID topMenuId,const char *pszText2,
										const char *pszText3);

public:
	kiCommand				*_pCommand;//�������
	int						_nCmdID;//menu��Ӧ��ID.
	int						_nLocalID;//����ڵľֲ�ID
	bool						_bCached;//�����Ƿ񻺴�
	//�˵���Ϣ
	int						_nMenuFlag;//�˵���ʶ.
	int						_nLevel;//�м����˵���
	eAppTopMenuID				eTopMenuID_;  //�����˵�ID��������ã�ʹ�øò˵�������ʹ�������_strMenu[0]
	std::string				_strMenu[3];//��������˵�.
};

#endif