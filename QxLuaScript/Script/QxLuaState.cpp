//
#include "stdafx.h"
#include <map>
#include <vector>
#include <string>
#include "QxLuaState.h"

class QxModuleReg;
class QxLuaRegistry;
typedef std::vector<QxModuleReg *> QxModuleRegArray;

//����ע���ģ��
class QxModuleReg {
public:
	QxModuleReg(const char *modname, lua_CFunction openFunc, const char *desc);
	~QxModuleReg();

public:
	std::string			sModuleName_;//ģ������
	lua_CFunction		pOpenFunc_;
	std::string			sDesc_;//ģ�������
};

QxModuleReg::QxModuleReg(const char *modname, lua_CFunction openFunc, const char *desc)
{
	ASSERT(modname && openFunc && desc);
	sModuleName_ = modname;
	pOpenFunc_ = openFunc;
	sDesc_ = desc;
}

QxModuleReg::~QxModuleReg()
{

}

///////////////////////////////////////////////////////////////////////////////
//
class QxLuaRegistry {
public:
	QxLuaRegistry();
	~QxLuaRegistry();

	//ע��һ����չģ��,�ú���������lua������������lua_State���󲻴�������£�����
	//��Ϣ��¼�ڸö����ڣ������ڴ���lua_Stateʱ������ӵ�lua_State������
	bool					RegisterModule(const char *modname, lua_CFunction openf, const char *desc);

	// ����¼��module����ӵ�lua_State�С�
	void					InitExtModules(lua_State *L);

protected:
	void					Clear();

protected:
	QxModuleRegArray		aModuleRegArray_;
};

QxLuaRegistry::QxLuaRegistry()
{
}

QxLuaRegistry::~QxLuaRegistry()
{
	Clear();
}

//ע��һ����չģ��,�ú���������lua������������lua_State���󲻴�������£�����
//��Ϣ��¼�ڸö����ڣ������ڴ���lua_Stateʱ������ӵ�lua_State������
bool QxLuaRegistry::RegisterModule(const char *modname, lua_CFunction openf, const char *desc)
{
	if (!modname || !openf || !desc) {
		ASSERT(FALSE);
		return false;
	}
	
	//�ж�ģ�������Ƿ��Ѿ�������
	std::string sModName = modname;
	QxModuleRegArray::size_type ix, isize = aModuleRegArray_.size();

	for (ix = 0; ix < isize; ix++) {
		//���ﲻ���Ǵ�Сд������������磺Os��os�������׻�����ģ������
		std::string::size_type slen = aModuleRegArray_[ix]->sModuleName_.size();
		if (sModName.size() == slen) {
			//������Դ�Сд���Է�ֹ���ƣ�Sys��sys������ģ��ͬʱ����
			if (_stricmp(modname, aModuleRegArray_[ix]->sModuleName_.c_str()) == 0) {
				TRACE("\n module name %s has existed.", modname);
				ASSERT(FALSE);
				return false;
			}
		}
	}

	QxModuleReg *pModReg = new QxModuleReg(modname, openf, desc);
	aModuleRegArray_.push_back(pModReg);

	return true;
}

// ����¼��module����ӵ�lua_State�С�
void QxLuaRegistry::InitExtModules(lua_State *L)
{
	ASSERT(L);
	if (aModuleRegArray_.empty())
		return;

	//ע����չģ��
	QxModuleRegArray::size_type ix, isize = aModuleRegArray_.size();
	for (ix = 0; ix < isize; ix++) {
		luaL_requiref(L, aModuleRegArray_[ix]->sModuleName_.c_str(),
			aModuleRegArray_[ix]->pOpenFunc_, 1);
		//module table is on top
		lua_pop(L, 1);//pop module object
	}
}

void QxLuaRegistry::Clear()
{
	QxModuleRegArray::size_type ix, isize = aModuleRegArray_.size();
	for (ix = 0; ix < isize; ix++) {
		delete aModuleRegArray_[ix];
	}
	aModuleRegArray_.clear();
}

///////////////////////////////////////////////////////////////////////////////
// ȫ�ֱ���������
static QxLuaRegistry s_luaReqistry;

///////////////////////////////////////////////////////////////////////////////
//

QxLuaState::QxLuaState()
{
	luaState_ = NULL;

	Init();
}

QxLuaState::~QxLuaState()
{
	Close();
}

bool QxLuaState::Init()
{
	if (luaState_ != NULL)
		return true;

	luaState_ = luaL_newstate();
	if (luaState_ == NULL) {
		szError_ = "new state failed.";
		return false;
	}
	//
	luaL_openlibs(luaState_);
	s_luaReqistry.InitExtModules(luaState_);

	return true;
}

void QxLuaState::Close()
{
	if (luaState_ != NULL) {
		lua_close(luaState_);
		luaState_ = NULL;
	}
	szError_.clear();
}

// ����ű�������Ƿ��д���.����ִ�нű�
bool QxLuaState::Compile(const char *pszScript)
{
	ASSERT(luaState_ != NULL);
	if (!pszScript || pszScript[0] == '\0') {
		return false;
	}

	// Loads a Lua chunk without running it. 
	// If there are no errors, lua_load pushes the compiled chunk as a Lua function on top of the stack. 
	// Otherwise, it pushes an error message.
	int rc = luaL_loadstring(luaState_, pszScript);
	if (rc != LUA_OK) {
		DoGetErrorMsg();
		return false;
	}
	lua_pop(luaState_, 1);//pop compiled chunk

	return true;
}

// ִ�нű�
//
bool QxLuaState::Execute(const char *pszScript)
{
	ASSERT(luaState_ != NULL);
	if (!pszScript || pszScript[0] == '\0') {
		ASSERT(FALSE);
		return false;
	}

	//����
	int rc = luaL_loadstring(luaState_, pszScript);
	if (rc != LUA_OK) {
		DoGetErrorMsg();
		return false;
	}

	//ִ��
	rc = lua_pcall(luaState_, 0, LUA_MULTRET, 0);
	if (rc != LUA_OK) {
		DoGetErrorMsg();
		return false;
	}

	return true;
}

// ִ��һ���ű��ļ�
//
bool QxLuaState::ExecuteFile(const std::string &szFile)
{
	ASSERT(luaState_ != NULL);
	if (szFile.empty()) {
		ASSERT(FALSE);
		return false;
	}

	//����
	std::string strEmtpy("");
	int rc = luaL_loadfilex(luaState_, szFile.c_str(), NULL);
	if (rc != LUA_OK) {
		DoGetErrorMsg();
		return false;
	}

	//ִ��
	rc = lua_pcall(luaState_, 0, LUA_MULTRET, 0);
	if (rc != LUA_OK) {
		DoGetErrorMsg();
		return false;
	}

	return true;
}

const char* QxLuaState::GetErrorMsg() const
{
	if (szError_.empty()) return NULL;
	return szError_.c_str();
}

void QxLuaState::DoGetErrorMsg()
{
	//����Ĵ�����Ϣ
	if (lua_isstring(luaState_, -1)) {
		const char *pszMsg = lua_tostring(luaState_, -1);
		if (pszMsg == NULL) {
			szError_ = "Null error";
		}else {
			szError_ = pszMsg;
		}
	}else{
		szError_ = "Unknown error.";
	}
	lua_pop(luaState_, 1);//pop err message
}

//ע��һ���Զ���ģ��
bool QxLuaState::RegisterModule(const char *modname, lua_CFunction openf, const char *desc)
{
	return s_luaReqistry.RegisterModule(modname, openf, desc);
}

