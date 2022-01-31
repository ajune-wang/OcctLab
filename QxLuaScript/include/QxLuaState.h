#pragma once

#include "lua.hpp"
#include "QxLibDef.h"

//
class QXLIB_API QxLuaState {
public:
	QxLuaState();
	~QxLuaState();

	// ����ű�������Ƿ��д���.����ִ�нű�
	bool				Compile(const char *pszScript);

	//ִ�нű�,�ű����ı��ļ�����ʽ����
	bool				Execute(const char *pszScript);

	//ִ��һ���ļ��еĽű�.szFileΪ�ļ�����
	bool				ExecuteFile(const std::string &szFile);

	const char*			GetErrorMsg() const;

public:
	//ע��һ���Զ���ģ��
	static bool			RegisterModule(const char *modname, lua_CFunction openf, const char *desc);
	
protected:
	bool				Init();
	void				Close();

	void				DoGetErrorMsg();

protected:
	lua_State			*luaState_;
	std::string			szError_; //������Ϣ
};