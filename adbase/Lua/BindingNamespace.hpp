#ifndef ADBASE_LUA_BINDINGNAMESPACE_HPP_
#define ADBASE_LUA_BINDINGNAMESPACE_HPP_

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}
#include <list>
#include <string>
#include "BindingWrapper.hpp"
#include "BindingUnwrapper.hpp"

namespace adbase {
namespace lua {

template <typename T, typename TP>
class BindingClass;

class BindingNamespace {
	public:
		typedef BindingNamespace selfType;
		typedef int (*staticMethod)(lua_State*);
	
	public:
		BindingNamespace();
		BindingNamespace(const char* _namespace, lua_State* L);
		BindingNamespace(const char* _namespace, BindingNamespace& ns);

		bool open(const char* _namespace, lua_State* L);

		void close();

		int getNamespaceTable();

		BindingNamespace(BindingNamespace & ns);

		BindingNamespace& operator=(BindingNamespace& ns);

		template <typename Ty>
		selfType& addConst(const char* constName, Ty n) {
			lua_State* state = getLuaState();
			int retNum = detail::WraperVar<Ty>::wraper(state, n);
			if (retNum > 0) {
				lua_setfield(state, getNamespaceTable(), constName);	
			}

			return *this;
		}

		selfType& addConst(const char* constName, const char*n, size_t s);

		template <typename R, typename... TParam>
		selfType &addMethod(const char *funcName, R (*fn)(TParam... param)) {
			lua_State *state = getLuaState();
			lua_pushstring(state, funcName);
			lua_pushlightuserdata(state, reinterpret_cast<void *>(fn));
			lua_pushcclosure(state, detail::UnwraperStaticFn<R, TParam...>::LuaCFunction, 1);
			lua_settable(state, getNamespaceTable());

			return (*this);
		}

		template <typename R, typename... TParam>
		selfType &addMethod(const char *funcName, std::function<R(TParam...)> fn) {
			typedef std::function<R(TParam...)> fnT;

			lua_State *state = getLuaState();
			lua_pushstring(state, funcName);

			LuaBindingPlacementNewAndDelete<fnT>::create(state, fn);
			lua_pushcclosure(state, detail::UnwraperFunctorFn<R, TParam...>::LuaCFunction, 1);
			lua_settable(state, getNamespaceTable());

			return (*this);
		}

		lua_State* getLuaState();

		~BindingNamespace();

	private:
		bool findNs();
		void buildNsSet(const char *_namespace);

		lua_State *_luaState;
		std::list<std::string> _ns;
		int _baseStackTop;
		int _thisNs;

		template <typename T, typename TP>
		friend class BindingClass;

};

}
}

#endif
