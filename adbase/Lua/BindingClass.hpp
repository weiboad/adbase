#ifndef ADBASE_LUA_BINDINGCLASS_HPP_
#define ADBASE_LUA_BINDINGCLASS_HPP_

#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include "BindingUtils.hpp"
#include "BindingManager.hpp"
#include "BindingNamespace.hpp"

namespace adbase {
namespace lua {

template <typename TC, typename TProxy = TC>
class BindingClass {
    public:
        typedef TC valueType;
        typedef TProxy proxyType;
        typedef BindingClass<valueType, proxyType> selfType;

		typedef BindingNamespace::staticMethod staticMethod;
		typedef typename LuaBindingUserdataInfo<valueType>::userdataType userdataType;
		typedef typename LuaBindingUserdataInfo<proxyType>::pointerType pointerType;
		typedef typename LuaBindingUserdataInfo<proxyType>::userdataPtrType userdataPtrType;
		typedef std::function<int(lua_State *, pointerType)> memberProxyMethodT;

		// {{{ BindingClass()
		
        BindingClass(const char *luaName, const char* nameSpace, lua_State *L) : _luaClassName(luaName), _ownerNs(nameSpace, L), _luaState(L) {
            registerClass();
        }

		// }}}
		// {{{ lua_State* getLuaState()
	
        lua_State *getLuaState() { 
			return _luaState; 
		}

		// }}}
		// {{{ BindingNamespace& getOwnerNamespace()

		BindingNamespace& getOwnerNamespace() {
			return _ownerNs;
		}

		// }}}
		// {{{ int getStaticClassTable()

		int getStaticClassTable() const {
			return _classTable;	
		}

		// }}}
		// {{{ int getMemberTable() 
		
		int getMemberTable() const {
			return _classMemberTable;
		}

		// }}}
		// {{{ selfType& addConst()

		template <typename Ty>
		selfType &addConst(const char* constName, Ty n) {
			lua_State *state = getLuaState();
			int retNum = detail::WraperVar<Ty>::wraper(state, n);
			if (retNum > 0) {
				lua_setfield(state, getStaticClassTable(), constName);	
			}
			return *this; 
		}

		// }}}
		// {{{ selfType& addConst()
		
		selfType &addConst(const char* constName, const char *n, size_t s) {
			lua_State *state = getLuaState();
			lua_pushstring(state, constName);
			lua_pushlstring(state, n, s);
			lua_settable(state, getStaticClassTable());

			return *this;
		}

		// }}}
		// {{{ selfType& addMethod()

		template <typename R, typename... TParam>
		selfType &addMethod(const char *funcName, R (*fn)(TParam... param)) {
			lua_State *state = getLuaState();
			lua_pushstring(state, funcName);
			lua_pushlightuserdata(state, reinterpret_cast<void *>(fn));
			lua_pushcclosure(state, detail::UnwraperStaticFn<R, TParam...>::LuaCFunction, 1);
			lua_settable(state, getStaticClassTable());

			return (*this);
		}

		// }}}
		// {{{ selfType& addMethod()

		template <typename R, typename... TParam>
		selfType &addMethod(const char *funcName, std::function<R(TParam...)> fn) {
			typedef std::function<R(TParam...)> fnT;

			lua_State *state = getLuaState();
			lua_pushstring(state, funcName);

			LuaBindingPlacementNewAndDelete<fnT>::create(state, fn);
			lua_pushcclosure(state, detail::UnwraperFunctorFn<R, TParam...>::LuaCFunction, 1);
			lua_settable(state, getStaticClassTable());

			return (*this);
		}

		// }}}
		// {{{ selfType& addMethod()

  		template <typename R, typename TClass, typename... TParam>
        selfType &addMethod(const char *funcName, R (TClass::*fn)(TParam... param)) {
			static_assert(std::is_convertible<valueType *, TClass *>::value, "class of member method invalid");

			lua_State *state = getLuaState();
			lua_pushstring(state, funcName);

			// 在 lua 栈中分配成员方法的内存
			memberProxyMethodT *fnPtr = LuaBindingPlacementNewAndDelete<memberProxyMethodT>::create(state);

			// 成员方法
			*fnPtr = [fn](lua_State *L, pointerType pobj) {
				return detail::UnwraperMemberFn<R, TClass, TParam...>::LuaCFunction(L, dynamic_cast<TClass *>(pobj.get()), fn);
			};
			lua_pushcclosure(state, memberMethodUnwraper, 1);
			lua_settable(state, getMemberTable());

			return (*this);
        }

		// }}}
		// {{{ selfType& addMethod()

  		template <typename R, typename TClass, typename... TParam>
        selfType &addMethod(const char *funcName, R (TClass::*fn)(TParam... param) const) {
			static_assert(std::is_convertible<valueType *, TClass *>::value, "class of member method invalid");

			lua_State *state = getLuaState();
			lua_pushstring(state, funcName);

			// 在 lua 栈中分配成员方法的内存
			memberProxyMethodT *fnPtr = LuaBindingPlacementNewAndDelete<memberProxyMethodT>::create(state);

			// 成员方法
			*fnPtr = [fn](lua_State *L, pointerType pobj) {
				return detail::UnwraperMemberFn<R, TClass, TParam...>::LuaCFunction(L, dynamic_cast<TClass *>(pobj.get()), fn);
			};
			lua_pushcclosure(state, memberMethodUnwraper, 1);
			lua_settable(state, getMemberTable());

			return (*this);
        }

		// }}}
		// {{{ selfType &setDefaultNew()
		
 		template <typename... TParams>
        selfType &setDefaultNew(const std::string &methodName = "new") {
			typedef std::function<pointerType(TParams && ...)> newFnT;
			lua_State *L = getLuaState();

			newFnT fn = [L](TParams &&... params) { return create<TParams &&...>(L, params...); };

			return addMethod<pointerType, TParams &&...>(methodName.c_str(), fn);
		}

		// }}}
		// {{{ static pointerType create()
  		template <typename... TParams>
        static pointerType create(lua_State *L, TParams &&... params) {
            pointerType obj = pointerType(new proxyType(std::forward<TParams>(params)...));

            // 添加到缓存表，防止被立即析构
            BindingClassManagerInst<proxyType>::instance()->addRef(L, obj);
            return obj;
        }

		// }}}
		// {{{ static const char *getLuaMetatableName()
		
		static const char *getLuaMetatableName() {
			return LuaBindingUserdataInfo<valueType>::getLuaMetatableName(); 
		}

		// }}}

    private:
		// {{{ registerClass()
		
        void registerClass() {
            // 初始化后就不再允许新的类注册了
            assert(false == BindingManager::instance()->isInited());

            BindingClassManagerInst<proxyType>::instance(); 
            lua_State *state = getLuaState();

            {
                lua_newtable(state);
                _classTable = lua_gettop(state);

                lua_newtable(state);
                _classMemberTable = lua_gettop(state);

                luaL_newmetatable(state, getLuaMetatableName());
                _classMetaTable = lua_gettop(state);

				// 注册类到namespace
				// 注意__index留空
				lua_pushstring(state, getLuaName());
				lua_pushvalue(state, _classTable);
				lua_settable(state, _ownerNs.getNamespaceTable());
            
				// table 初始化(静态成员)
				lua_pushvalue(state, _classTable);
				lua_setmetatable(state, _classTable);

                lua_pushliteral(state, "__type");
                lua_pushliteral(state, "native class");
                lua_settable(state, _classTable);

				// memtable 初始化(成员函数及变量)
				lua_pushvalue(state, _classMemberTable);
				lua_setmetatable(state, _classMemberTable);

				lua_pushliteral(state, "__index");
				lua_pushvalue(state, _classTable);
				lua_settable(state, _classMemberTable);

				// memtable 的__type默认设为native object(这里仅仅是为了和class.lua交互，如果不设置的话默认就是
				// native code)
				lua_pushliteral(state, "__type");
				lua_pushliteral(state, "native object");
				lua_settable(state, _classMemberTable);

				//metatable 初始化(userdata映射表)
				lua_pushvalue(state, _classMetaTable);
				lua_setmetatable(state, _classMetaTable);

				// 继承关系链 userdata -> metatable(实例接口表): memtable(成员接口表): table(静态接口表) : ...
				lua_pushliteral(state, "__index");
				lua_pushvalue(state, _classMemberTable);
				lua_settable(state, _classMetaTable);
            }
        }

		// }}}
		// {{{ static int memberMethodUnwraper()

		 static int memberMethodUnwraper(lua_State *L) {
			memberProxyMethodT *fn = reinterpret_cast<memberProxyMethodT *>(lua_touserdata(L, lua_upvalueindex(1)));
			if (nullptr == fn) {
				LOG_ERROR << "lua try to call member method in class "
						  << getLuaMetatableName()
						  << " but fn not set.\n";
				fn::printTraceback(L, "");
				return 0;
			}

			const char *className = getLuaMetatableName();
			userdataPtrType pobj = static_cast<userdataPtrType>(luaL_checkudata(L, 1, className)); // get 'self'
			if (nullptr == pobj) {
				LOG_ERROR << "lua try to call "
						  << className
						  << "'s member method but self not set or type error.\n";
				fn::printTraceback(L, "");
				return 0;
			}

			pointerType objPtr = pobj->lock();
			lua_remove(L, 1);

			if (!objPtr) {
				LOG_ERROR << "lua try to call "
						  << className
						  << "'s member method but this=nullptr.\n";
				fn::printTraceback(L, "");
				return 0;
			}

			return (*fn)(L, objPtr);
		}

		// }}}
		// {{{ const char* getLuaName()

		const char *getLuaName() const { 
			return _luaClassName.c_str(); 
		}

		// }}}

    private:
        std::string _luaClassName;
		BindingNamespace _ownerNs;
        lua_State* _luaState;
        int _classTable = 0;
        int _classMemberTable = 0;
        int _classMetaTable = 0;
};

}
}

# endif
