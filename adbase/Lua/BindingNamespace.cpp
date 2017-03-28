#include "BindingNamespace.hpp"

namespace adbase {
namespace lua {
// {{{ BindingNamespace::BindingNamespace()

BindingNamespace::BindingNamespace() : _luaState(nullptr), _baseStackTop(0), _thisNs(0) {
}

// }}}
// {{{ BindingNamespace::BindingNamespace()

BindingNamespace::BindingNamespace(const char* nameSpace, lua_State* L) : _luaState(nullptr), _baseStackTop(0), _thisNs(0) {
	open(nameSpace, L);
}

// }}}
// {{{ BindingNamespace::BindingNamespace()

BindingNamespace::BindingNamespace(const char* nameSpace, BindingNamespace& ns) {
	_luaState = ns._luaState;
	_baseStackTop = ns._baseStackTop;
	_thisNs = ns._thisNs;
	buildNsSet(nameSpace);
	findNs();
	_ns.insert(_ns.end(), ns._ns.begin(), ns._ns.end());
}

// }}}
// {{{ BindingNamespace::~BindingNamespace()

BindingNamespace::~BindingNamespace() {
	close();
}

// }}}
// {{{ BindingNamespace::BindingNamespace()

BindingNamespace& BindingNamespace::operator=(BindingNamespace& ns) {
	_thisNs = ns._thisNs;
	_ns = ns._ns;
	_baseStackTop = ns._baseStackTop;
	_luaState = ns._luaState;
	ns._baseStackTop = 0;

	return (*this);
}

// }}}
// {{{ BindingNamespace::BindingNamespace()

BindingNamespace::BindingNamespace(BindingNamespace& ns) : _luaState(nullptr), _baseStackTop(0), _thisNs(0) {
	(*this) = ns;
}

// }}}
// {{{ void BindingNamespace::buildNsSet()

void BindingNamespace::buildNsSet(const char *_namespace) {
	const char *s = _namespace, *e = _namespace;
	while (*e) {
		if ('.' == *e) {
			_ns.push_back(std::string(s, e));
			s = e + 1;
		}

		++e;
	}
	if (s < e) {
		_ns.push_back(std::string(s, e));
	}
}

// }}}
// {{{ int BindingNamespace::getNamespaceTable()

int BindingNamespace::getNamespaceTable() {
	if (_thisNs) {
		return _thisNs;
	}

#ifdef LUA_RIDX_GLOBALS
	_baseStackTop = _baseStackTop ? _baseStackTop : lua_gettop(_luaState);
	lua_rawgeti(_luaState, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
	return _thisNs = lua_gettop(_luaState);
#else

	return _thisNs = LUA_GLOBALSINDEX;
#endif
}

// }}}
// {{{ bool BindingNamespace::open()

bool BindingNamespace::open(const char* nameSpace, lua_State* L) {
	close();

	if (nullptr == nameSpace) {
		return true;
	}

	_luaState = L;
	_ns.clear();
	buildNsSet(nameSpace);
	_baseStackTop = _baseStackTop ? _baseStackTop : lua_gettop(L);

	return findNs();
}

// }}}
// {{{ void BindingNamespace::close()

void BindingNamespace::close() {
	if (0 != _baseStackTop) {
		lua_settop(_luaState, _baseStackTop);
	}

	_baseStackTop = 0;
	_ns.clear();
}

// }}}
// {{{ bool BindingNamespace::findNs()

bool BindingNamespace::findNs() {
	int curNs = getNamespaceTable();
	
	for (auto &ns : _ns) {
		lua_pushlstring(_luaState, ns.c_str(), ns.size());
		lua_gettable(_luaState, curNs);
		if (lua_isnil(_luaState, -1)) {
			lua_pop(_luaState, 1);
			lua_newtable(_luaState);
			int top = lua_gettop(_luaState);
			lua_pushlstring(_luaState, ns.c_str(), ns.size());
			lua_pushvalue(_luaState, top);
			lua_settable(_luaState, curNs);
		}

		if (0 == lua_istable(_luaState, -1)) {
			return false;
		}
		
		curNs = lua_gettop(_luaState);
	}

	_thisNs = curNs;

	return true;
}

// }}}
// {{{ BindingNamespace::selfType& BindingNamespace::addConst()

BindingNamespace::selfType& BindingNamespace::addConst(const char* constName, const char* n, size_t s) {
	lua_State* state = getLuaState();
	lua_pushstring(state, constName);
	lua_pushlstring(state, n, s);
	lua_settable(state, getNamespaceTable());

	return *this;
}

// }}}
// {{{ lua_State* BindingNamespace::getLuaState()

lua_State* BindingNamespace::getLuaState() {
	return _luaState;
}

// }}}

}
}
