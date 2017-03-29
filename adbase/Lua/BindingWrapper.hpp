#ifndef ADBASE_LUA_BINDINGWRAPPER_HPP_
#define ADBASE_LUA_BINDINGWRAPPER_HPP_

#include "BindingUtils.hpp"
#include "BindingManager.hpp"

namespace adbase {
namespace lua {
namespace detail {

	// {{{ get params index
	
	template <int... index>
	struct IndexSeqList {
		typedef IndexSeqList<index..., sizeof...(index)> nextType;
	};
	
	template <typename... TP>
	struct BuildArgsIndex;

	template <>
	struct BuildArgsIndex<> {
		typedef IndexSeqList<> indexSeqType;
	};

	template <typename TP1, typename... TP>
	struct BuildArgsIndex<TP1, TP...> {
		typedef typename BuildArgsIndex<TP...>::indexSeqType::nextType indexSeqType;
	};

	// }}}
	// {{{ template WraperVarLuaType
 	template <typename Ty>
	struct WraperVarLuaType;

	template<>
	struct WraperVarLuaType<lua_Unsigned> {
		typedef lua_Unsigned valueType;
		template<typename TParam>
		static int wraper(lua_State* L, const TParam &v) {
            if (sizeof(v) >= sizeof(lua_Integer) 
				&& v > static_cast<TParam>(std::numeric_limits<lua_Integer>::max())) {
            	lua_getglobal(L, "tonumber");
                char n[32] = {0};
                sprintf(n, "%llu", static_cast<unsigned long long>(v));
                lua_pushstring(L, n);
                lua_call(L, 1, 1);
            } else {
                lua_pushinteger(L, static_cast<lua_Unsigned>(v));
            }
			return 1;
		}
	};
	
	template <>
	struct WraperVarLuaType<lua_Integer> {
		typedef lua_Integer valueType;

		template <typename TParam>
		static int wraper(lua_State *L, const TParam &v) {
			lua_pushinteger(L, static_cast<lua_Integer>(v));
			return 1;
		}
	};

	template <>
	struct WraperVarLuaType<lua_Number> {
		typedef lua_Number valueType;

		template <typename TParam>
		static int wraper(lua_State *L, const TParam &v) {
			lua_pushnumber(L, static_cast<lua_Number>(v));
			return 1;
		}
	};

 	template <typename Ty>
    struct wraperPtrVarLuaType {
        typedef Ty valueType;

        template <typename TParam>
        static int wraper(lua_State *L, TParam &&v) {
        	static_assert(std::is_pointer<TParam>::value, "parameter must be a pointer");
            lua_pushlightuserdata(L, v);
            return 1;
        }
    };

	template <>
	struct WraperVarLuaType<lua_CFunction> {
		typedef lua_CFunction valueType;

		template <typename TParam>
		static int wraper(lua_State *L, TParam v) {
			lua_pushcfunction(L, static_cast<lua_CFunction>(v));
			return 1;
		}
	};

 	template <typename Ty>
	struct WraperVarLuaType {
		typedef Ty valueType;

		template <typename TParam>
		static int wraper(lua_State *L, TParam &&v) {
			typedef
				typename std::remove_reference<typename std::remove_cv<typename std::remove_pointer<Ty>::type>::type>::type objT;

			// 最好是不要这么用，如果有需要再加对非POD类型的支持吧
			static_assert(std::is_pod<objT>::value, "custom type must be pod type");

			objT *ptr = reinterpret_cast<objT *>(lua_newuserdata(L, sizeof(objT)));
			memcpy(ptr, &v, sizeof(v));

			return 1;
		}
	};


	// }}}
	// {{{ template WraperVar
	template <typename TRet, typename... Ty>
	struct WraperVar;

	template <typename... Ty>
	struct WraperVar<void, Ty...> {
		static int wraper(lua_State *L, const void *v) { 
			return 0; 
		}
	};

	template <typename... Ty>
	struct WraperVar<uint8_t, Ty...> : public WraperVarLuaType<lua_Unsigned> {
	};
	template <typename... Ty>
	struct WraperVar<uint16_t, Ty...> : public WraperVarLuaType<lua_Unsigned> {
	};
	template <typename... Ty>
	struct WraperVar<uint32_t, Ty...> : public WraperVarLuaType<lua_Unsigned> {
	};
	template <typename... Ty>
	struct WraperVar<uint64_t, Ty...> : public WraperVarLuaType<lua_Unsigned> {
	};

    template <typename... Ty>
	struct WraperVar<lua_CFunction, Ty...> : public WraperVarLuaType<lua_CFunction> {};

	template <typename... Ty>
	struct WraperVar<bool, Ty...> {
		static int wraper(lua_State *L, const bool &v) {
			lua_pushboolean(L, v ? 1 : 0);
			return 1;
		}
	};

	template <typename... Ty>
	struct WraperVar<char *, Ty...> {
		static int wraper(lua_State *L, const char *v) {
			lua_pushstring(L, v);
			return 1;
		}
	};

	template <typename... Ty>
	struct WraperVar<const char *, Ty...> {
		static int wraper(lua_State *L, const char *v) {
			lua_pushstring(L, v);
			return 1;
		}
	};

	template <typename TC, typename... Ty>
	struct WraperVar<std::shared_ptr<TC>, Ty...> {
		static int wraper(lua_State *L, const std::shared_ptr<TC> &v) {
			typedef typename LuaBindingUserdataInfo<TC>::userdataType udT;

			// 无效则push nil
			if (!v) {
				lua_pushnil(L);
				return 1;
			}

			void *buff = lua_newuserdata(L, sizeof(udT));
			new (buff) udT(v);

			const char *className = LuaBindingUserdataInfo<TC>::getLuaMetatableName();
			luaL_getmetatable(L, className);

			lua_setmetatable(L, -2);
			return 1;
		}
	};

	template <typename TLeft, typename TRight, typename... Ty>
	struct WraperVar<std::pair<TLeft, TRight>, Ty...> {
		static int wraper(lua_State *L, const std::pair<TLeft, TRight> &v) {
			lua_createtable(L, 2, 0);

			lua_pushinteger(L, 1);
			WraperVar<TLeft>::wraper(L, v.first);
			lua_settable(L, -3);

			lua_pushinteger(L, 2);
			WraperVar<TRight>::wraper(L, v.second);
			lua_settable(L, -3);

			return 1;
		}
	};

	template <typename Ty, typename... Tl>
	struct WraperVar<std::vector<Ty>, Tl...> {
		static int wraper(lua_State *L, const std::vector<Ty> &v) {
			lua_Unsigned res = 1;
			lua_newtable(L);
			int tb = lua_gettop(L);
			for (const Ty &ele : v) {
				// 目前只支持一个值
				lua_pushunsigned(L, res);
				WraperVar<Ty>::wraper(L, ele);
				lua_settable(L, -3);

				++res;
			}
			lua_settop(L, tb);
			return 1;
		}
	};

	template <typename Ty, typename... Tl>
	struct WraperVar<std::list<Ty>, Tl...> {
		static int wraper(lua_State *L, const std::list<Ty> &v) {
			lua_Unsigned res = 1;
			lua_newtable(L);
			int tb = lua_gettop(L);
			for (const Ty &ele : v) {
				// 目前只支持一个值
				lua_pushunsigned(L, res);
				WraperVar<Ty>::wraper(L, ele);
				lua_settable(L, -3);

				++res;
			}
			lua_settop(L, tb);
			return 1;
		}
	};

	// 枚举、整形、指针支持
	template<typename Ty, typename... Tl>
	struct WraperVar : public std::conditional<
							std::is_enum<Ty>::value || std::is_integral<Ty>::value,
							WraperVarLuaType<typename std::conditional<
								std::is_enum<Ty>::value || std::is_unsigned<Ty>::value, lua_Unsigned, 
								lua_Integer>::type>,
							typename std::conditional<std::is_pointer<Ty>::value,
													 wraperPtrVarLuaType<Ty>,
													 WraperVarLuaType<Ty>>::type
							>::type {};

	template<typename... Ty>
	struct WraperVar<std::string, Ty...> {
		static int wraper(lua_State* L, std::string& v) {
			lua_pushlstring(L, v.c_str(), v.size());
			return 1;
		}
		static int wraper(lua_State* L, const std::string& v) {
			lua_pushlstring(L, v.c_str(), v.size());
			return 1;
		}
	};

	// }}}
}
}
}

#endif
