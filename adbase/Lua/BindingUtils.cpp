#include "BindingUtils.hpp"
#include <adbase/Logging.hpp>


namespace adbase {
namespace lua {

// 自动回复栈空间数据
// {{{ LuaAutoBlock::LuaAutoBlock()

LuaAutoBlock::LuaAutoBlock(lua_State *L) : _state(L), _topStack(lua_gettop(_state)) {
}

// }}}
// {{{ LuaAutoBlock::~LuaAutoBlock()

LuaAutoBlock::~LuaAutoBlock() {
    lua_settop(_state, _topStack);
}

// }}}

namespace fn {

// {{{ int getPcallHmsg()

int getPcallHmsg(lua_State *L) {
    if (nullptr == L) return 0;

    lua_getglobal(L, "stackdump");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        lua_pushcfunction(L, luaStackdump);
    }

    return lua_gettop(L);
}

// }}}
// {{{ int luaStackdump()

int luaStackdump(lua_State *L) {
    int top = lua_gettop(L);
    #if LUA_VERSION_NUM >= 502
    luaL_traceback(L, L, nullptr, 2);
    #else
    lua_getglobal(L, "debug");

    lua_getfield(L, -1, "traceback");
    lua_remove(L, -2);

    if (top > 0) {
        lua_pushvalue(L, top);
    } else {
        lua_pushnil(L);
    }
    lua_pushinteger(L, 2);

    if (lua_pcall(L, 2, LUA_MULTRET, 0) != 0) {
        LOG_ERROR << luaL_checkstring(L, -1);
    }
    #endif
    return lua_gettop(L) - top;
}

// }}}
// {{{ bool execFile()

bool execFile(lua_State *L, const char *filePath) {
    LuaAutoBlock autoBlock(L);
    // 获取错误处理函数
    int hmsg = getPcallHmsg(L);
    if (luaL_loadfile(L, filePath) || lua_pcall(L, 0, LUA_MULTRET, hmsg)) {
        LOG_ERROR << luaL_checkstring(L, -1);
        return false;
    }

    return true;
}

// }}}
// {{{ bool execCode()

bool execCode(lua_State *L, const char *codes) {
    LuaAutoBlock autoBlock(L);
    // 获取错误处理函数
    int hmsg = getPcallHmsg(L);
    if (luaL_loadstring(L, codes) || lua_pcall(L, 0, LUA_MULTRET, hmsg)) {
        LOG_ERROR << luaL_checkstring(L, -1);
        return false;
    }

    return true;
}

// }}}
// {{{ void printTraceback()

void printTraceback(lua_State* L, const std::string& msg) {
	LuaAutoBlock autoBlock(L);
	LOG_INFO << "TraceBack:" << msg << "\n" << luaStackdumpToString(L);
}

// }}}
// {{{ std::string luaStackdumpToString()

std::string luaStackdumpToString(lua_State *L) {
	std::stringstream ss;

	int index = 1;
	lua_Debug ar;
	while (lua_getstack(L, index, &ar)) {
		lua_getinfo(L, "Snl", &ar);

		ss << "    ";

		if (ar.short_src) {
			ss << static_cast<const char *>(ar.short_src);
		}

		if (ar.name) {
			ss << "(" << static_cast<const char *>(ar.name) << ")";
		}

		ss << ":" << ar.currentline << std::endl;

		++index;
	}

	return ss.str();
}

// }}}
}

}
}
