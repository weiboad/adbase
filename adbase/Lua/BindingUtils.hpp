#ifndef ADBASE_LUA_BINDINGUTILS_HPP_
#define ADBASE_LUA_BINDINGUTILS_HPP_

#include <adbase/Logging.hpp>
extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}
#include <memory>
#include <sstream>
#include "Adaptor.hpp"

namespace adbase {
namespace lua {

// {{{ class LuaAutoBlock
class LuaAutoBlock {
    public:
        LuaAutoBlock(lua_State *);
        ~LuaAutoBlock();

    private:
        LuaAutoBlock(const LuaAutoBlock &src) = delete;
        const LuaAutoBlock &operator=(const LuaAutoBlock &src) = delete;

        lua_State *_state;
        int _topStack;
};

// }}}
// {{{ LuaBindingUserdataInfo

template <typename TC>
struct LuaBindingUserdataInfo {
	typedef TC valueType;
	typedef std::shared_ptr<valueType> pointerType;
	typedef std::weak_ptr<valueType> userdataType;
	typedef userdataType *userdataPtrType;

	static const char *getLuaMetatableName() {
		return typeid(valueType).name();
	}
};

// }}}
// {{{ fn
namespace fn {
    int getPcallHmsg(lua_State *L);
    int luaStackdump(lua_State *L);
    bool execFile(lua_State *L, const char *filePath);
    bool execCode(lua_State *L, const char *codes);
	void printTraceback(lua_State *L, const std::string &msg);
	void printStack(lua_State *L);
	std::string luaStackdumpToString(lua_State *L);
}
// }}}
// {{{ LuaBindingPlacementNewAndDelete

template<typename TC>
struct LuaBindingPlacementNewAndDelete {
	typedef TC valueType;

	// {{{ static void pushMetatable()

	static void pushMetatable(lua_State *L) {
		const char *className = LuaBindingUserdataInfo<valueType>::getLuaMetatableName();
		luaL_getmetatable(L, className);
		if (lua_istable(L, -1)) {
			return;
		}

		lua_pop(L, 1);
		luaL_newmetatable(L, className);
		lua_pushcfunction(L, luaGc);
		lua_setfield(L, -2, "__gc");
	}
	// }}}
	// {{{ static int luaGc()

  	static int luaGc(lua_State *L) {
        if (0 == lua_gettop(L)) {
			LOG_ERROR << "userdata __gc is called without self";
            return 0;
        }

		// metatable表触发
		if (0 == lua_isuserdata(L, 1)) {
			lua_remove(L, 1);
			return 0;
		}

        auto obj = static_cast<valueType *>(luaL_checkudata(L, 1, LuaBindingUserdataInfo<valueType>::getLuaMetatableName()));
        if (nullptr == obj) {
			LOG_ERROR << "try to convert userdata to " << LuaBindingUserdataInfo<valueType>::getLuaMetatableName()
					  << " but failed";
            return 0;
        }

        // 执行析构，由lua负责释放内存
        obj->~valueType();
        return 0;
	}
	// }}}
	// {{{ static valueType *create()

	template <typename... TParams>
	static valueType *create(lua_State *L, TParams &&... params) {
		valueType *obj = new (lua_newuserdata(L, sizeof(valueType))) valueType(std::forward<TParams>(params)...);
		pushMetatable(L);
		lua_setmetatable(L, -2);
		return obj;
	}

	// }}}
};

// }}}
}
}

#endif
