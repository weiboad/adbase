#ifndef ADBASE_LUA_BINDINGUNWRAPPER_HPP_
#define ADBASE_LUA_BINDINGUNWRAPPER_HPP_

#include <adbase/Logging.hpp>
#include "BindingWrapper.hpp"
#include "Adaptor.hpp"

#define LUA_CHECK_TYPE_AND_RET(name, L, index, ret)                                            \
    if (!lua_is##name(L, index)) {                                                             \
		LOG_ERROR << "parameter " << index << " must be a " << #name << " , got " << luaL_typename(L, index);  \
		fn::printTraceback(L, "");	\
        return ret;                                                                            \
    }

namespace adbase {
namespace lua {
namespace detail {

	// {{{ template UnwraperVarLuaType
	template <typename Tt, typename Ty>
	struct UnwraperVarLuaType;

	template <typename Tt>
	struct UnwraperVarLuaType<Tt, lua_Unsigned> {
		typedef lua_Unsigned valueType;

		static Tt unwraper(lua_State *L, int index) {
			if (lua_gettop(L) < index) {
				return static_cast<Tt>(0);
			}

			LUA_CHECK_TYPE_AND_RET(number, L, index, static_cast<Tt>(0));
			return static_cast<Tt>(lua_tointeger(L, index));
		}
	};

	template <typename Tt>
	struct UnwraperVarLuaType<Tt, lua_CFunction> {
		typedef lua_CFunction valueType;

		static Tt unwraper(lua_State* L, int index) {
			if (lua_gettop(L) < index) {
				return static_cast<Tt>(nullptr);
			}

			return static_cast<Tt>(lua_tocfunction(L, index));
		}
	};

	template <typename Tt>
	struct UnwraperVarLuaType<Tt, lua_Integer> {
		typedef lua_Integer valueType;

		static Tt unwraper(lua_State *L, int index) {
			if (lua_gettop(L) < index) {
				return static_cast<Tt>(0);
			}

			LUA_CHECK_TYPE_AND_RET(number, L, index, static_cast<Tt>(0));
			return static_cast<Tt>(lua_tointeger(L, index));
		}
	};

	template <typename Tt>
	struct UnwraperVarLuaType<Tt, lua_Number> {
		typedef lua_Number valueType;

		static Tt unwraper(lua_State *L, int index) {
			if (lua_gettop(L) < index) {
				return static_cast<Tt>(0);
			}

			LUA_CHECK_TYPE_AND_RET(number, L, index, static_cast<Tt>(0));
			return static_cast<Tt>(lua_tonumber(L, index));
		}
	};

	template<typename Tt, typename Ty>
	struct UnwraperPtrVarLuaType {
		typedef Ty valueType;

		static Tt unwraper(lua_State* L, int index) {
			if (lua_gettop(L) < index) {
				return nullptr;
			}
			if (lua_isnil(L, index)) {
				return nullptr;
			}
			if (!lua_islightuserdata(L, index)) {
				return nullptr;
			}
			return reinterpret_cast<Tt>(lua_topointer(L, index));
		}
	};

	template <typename Tt, typename Ty>
	struct UnwraperVarLuaType {
		typedef Ty valueType;

		static Ty unwraper(lua_State *L, int index) {
			typedef typename std::remove_reference<typename std::remove_cv<typename std::remove_pointer<Tt>::type>::type>::type objT;
            static_assert(std::is_pod<objT>::value, "custom type must be pod type");
			if (lua_gettop(L) < index) {
				return static_cast<Tt>(0); // NULL static_cast from 'null_ptr' to 'unsigned long' is not allowed
			}
			if (lua_isnil(L, index)) {
				return static_cast<Tt>(0); // NULL static_cast from 'null_ptr' to 'unsigned long' is not allowed
			}

			objT *ptr = reinterpret_cast<objT *>(lua_touserdata(L, index));
			if (nullptr == ptr) {
				return static_cast<Tt>(0); // NULL static_cast from 'null_ptr' to 'unsigned long' is not allowed
			}

			return static_cast<Tt>(*ptr);
		}
	};

	// }}}
	// {{{ template UnwraperVar

	template<typename...Ty>
	struct UnwraperVar;

	template <typename... Ty>
	struct UnwraperVar<void, Ty...> {
		template <typename TParam>
		static void unwraper(lua_State *L, int index) {}
	};

	template <typename... Ty>
	struct UnwraperVar<uint8_t, Ty...> : public UnwraperVarLuaType<uint8_t, lua_Unsigned> {};
	template <typename... Ty>
	struct UnwraperVar<uint16_t, Ty...> : public UnwraperVarLuaType<uint16_t, lua_Unsigned> {};
	template <typename... Ty>
	struct UnwraperVar<uint32_t, Ty...> : public UnwraperVarLuaType<uint32_t, lua_Unsigned> {};
	template <typename... Ty>
	struct UnwraperVar<uint64_t, Ty...> : public UnwraperVarLuaType<uint64_t, lua_Unsigned> {};

	template <typename... Ty>
	struct UnwraperVar<int8_t, Ty...> : public UnwraperVarLuaType<int8_t, lua_Integer> {};
	template <typename... Ty>
	struct UnwraperVar<int16_t, Ty...> : public UnwraperVarLuaType<int16_t, lua_Integer> {};
	template <typename... Ty>
	struct UnwraperVar<int32_t, Ty...> : public UnwraperVarLuaType<int32_t, lua_Integer> {};
	template <typename... Ty>
	struct UnwraperVar<int64_t, Ty...> : public UnwraperVarLuaType<int64_t, lua_Integer> {};

	template <typename... Ty>
	struct UnwraperVar<float, Ty...> : public UnwraperVarLuaType<float, lua_Number> {};
	template <typename... Ty>
	struct UnwraperVar<double, Ty...> : public UnwraperVarLuaType<double, lua_Number> {};

	template <typename... Ty>
	struct UnwraperVar<lua_CFunction, Ty...> : public UnwraperVarLuaType<lua_CFunction, lua_CFunction> {};


  	template <typename... Ty>
    struct UnwraperVar<bool, Ty...> {
        static bool unwraper(lua_State *L, int index) {
        	if (lua_gettop(L) < index) {
				return static_cast<bool>(false);
			}

            return !!lua_toboolean(L, index);
        }
    };

    template <typename... Ty>
    struct UnwraperVar<const char *, Ty...> {
        static const char *unwraper(lua_State *L, int index) {
            if (lua_gettop(L) < index) {
				return static_cast<const char *>(NULL);
			}

            LUA_CHECK_TYPE_AND_RET(string, L, index, NULL);

            return lua_tostring(L, index);
        }
    };

    template <typename... Ty>
    struct UnwraperVar<char *, Ty...> {
        static char *unwraper(lua_State *L, int index) {
            if (lua_gettop(L) < index) {
				return static_cast<char *>(NULL);
			}

            LUA_CHECK_TYPE_AND_RET(string, L, index, NULL);

            return const_cast<char *>(lua_tostring(L, index));
        }
    };

	template <typename... Ty>
	struct UnwraperVar<std::string, Ty...> {
		static std::string unwraper(lua_State *L, int index) {
			std::string ret;
			LUA_CHECK_TYPE_AND_RET(string, L, index, ret);

			size_t len = 0;
			const char *start = lua_tolstring(L, index, &len);
			ret.assign(start, len);
			return ret;
		}
	};

	template <typename TLeft, typename TRight, typename... Ty>
	struct UnwraperVar<std::pair<TLeft, TRight>, Ty...> {
		static std::pair<TLeft, TRight> unwraper(lua_State *L, int index) {
			std::pair<TLeft, TRight> ret;
			LUA_CHECK_TYPE_AND_RET(table, L, index, ret);

			lua_pushvalue(L, index);

			lua_pushinteger(L, 1);
			lua_gettable(L, -2);
			ret.first = UnwraperVar<TLeft>::unwraper(L, -1);

			lua_pushinteger(L, 2);
			lua_gettable(L, -3);
			ret.second = UnwraperVar<TRight>::unwraper(L, -1);

			lua_pop(L, 3);

			return ret;
		}
	};

 	template <typename Ty, typename... Tl>
	struct UnwraperVar<std::vector<Ty>, Tl...> {
		static std::vector<Ty> unwraper(lua_State *L, int index) {
			std::vector<Ty> ret;
			LUA_CHECK_TYPE_AND_RET(table, L, index, ret);

			LUA_GET_TABLE_LEN(size_t len, L, index);
			ret.reserve(len);

			lua_pushvalue(L, index);
			for (size_t i = 1; i <= len; ++i) {
				lua_pushinteger(L, static_cast<lua_Unsigned>(i));
				lua_gettable(L, -2);
				ret.push_back(UnwraperVar<Ty>::unwraper(L, -1));
				lua_pop(L, 1);
			}
			lua_pop(L, 1);

			return ret;
		}
	};

	template <typename Ty, typename... Tl>
	struct UnwraperVar<std::list<Ty>, Tl...> {
		static std::list<Ty> unwraper(lua_State *L, int index) {
			std::list<Ty> ret;
			LUA_CHECK_TYPE_AND_RET(table, L, index, ret);

			LUA_GET_TABLE_LEN(size_t len, L, index);

			lua_pushvalue(L, index);
			for (size_t i = 1; i <= len; ++i) {
				lua_pushinteger(L, static_cast<lua_Unsigned>(i));
				lua_gettable(L, -2);
				ret.push_back(UnwraperVar<Ty>::unwraper(L, -1));
				lua_pop(L, 1);
			}
			lua_pop(L, 1);

			return ret;
		}
	};

	template <typename Ty, typename... Tl>
	struct UnwraperVar<Ty, Tl...> : public std::conditional<
		  							std::is_enum<Ty>::value || std::is_integral<Ty>::value,
		  							UnwraperVarLuaType<Ty, typename std::conditional<
										std::is_enum<Ty>::value || std::is_unsigned<Ty>::value,
		  								lua_Unsigned, 
										lua_Integer
									>::type>,   // >枚举类型
		  							typename std::conditional<
										std::is_pointer<Ty>::value, 
										UnwraperPtrVarLuaType<Ty, Ty>, // >指针类型
		  								UnwraperVarLuaType<Ty, Ty> // POD类型
		  							>::type>::type {};


	// }}}
	// {{{ template UnwraperStaticFnBase

	template <typename Tr>
    struct UnwraperStaticFnBase;

    template <>
    struct UnwraperStaticFnBase<void> {
        template <typename Tfn, class TupleT, int... N>
        static int runFn(lua_State *L, Tfn fn, IndexSeqList<N...>) {
            fn(UnwraperVar<typename std::tuple_element<N, TupleT>::type>::unwraper(L, N + 1)...);
            return 0;
        }
    };

    template <typename Tr>
    struct UnwraperStaticFnBase {
    template <typename Tfn, class TupleT, int... N>
        static int runFn(lua_State *L, Tfn fn, IndexSeqList<N...>) {
            return WraperVar<typename std::remove_cv<typename std::remove_reference<Tr>::type>::type>::wraper(
                L, fn(UnwraperVar<typename std::tuple_element<N, TupleT>::type>::unwraper(L, N + 1)...)
			);
        }
    };

	// }}}
	// {{{ template UnwraperStaticFn
	
	template <typename Tr, typename... TParam>
	struct UnwraperStaticFn : public UnwraperStaticFnBase<Tr> {
		typedef UnwraperStaticFnBase<Tr> baseType;
		typedef Tr (*valueType)(TParam...);

		// 动态参数个数
		static int LuaCFunction(lua_State *L) {
			valueType fn = reinterpret_cast<valueType>(lua_touserdata(L, lua_upvalueindex(1)));
			if (nullptr == fn) {
				// 找不到函数
				return 0;
			}

			return baseType::template runFn<
				valueType, std::tuple<typename std::remove_cv<typename std::remove_reference<TParam>::type>::type...>>(
				L, fn, typename BuildArgsIndex<TParam...>::indexSeqType());
		}
	};

	// }}}
	// {{{ template UnwraperFunctorFn

	template <typename Tr, typename... TParam>
    struct UnwraperFunctorFn : public UnwraperStaticFnBase<Tr> {
        typedef UnwraperStaticFnBase<Tr> baseType;
        typedef std::function<Tr(TParam...)> valueType;

		// 动态参数个数
		static int LuaCFunction(lua_State *L) {
			valueType *fn = reinterpret_cast<valueType *>(lua_touserdata(L, lua_upvalueindex(1)));
			if (NULL == fn) {
				// 找不到函数
				return 0;
			}

			return baseType::template runFn<
				valueType, std::tuple<typename std::remove_cv<typename std::remove_reference<TParam>::type>::type...> >(
						L, *fn, typename BuildArgsIndex<TParam...>::indexSeqType());
		}
	};

	// }}}
	// {{{ template UnwraperMemberFn

	template <typename Tr, typename TClass, typename... TParam>
	struct UnwraperMemberFn : public UnwraperStaticFnBase<Tr> {
		typedef UnwraperStaticFnBase<Tr> baseType;
		// 动态参数个数 - 成员函数
		static int LuaCFunction(lua_State *L, TClass *obj, Tr (TClass::*fn)(TParam...)) {
			auto fnWraper = [obj, fn](TParam &&... args) { return (obj->*fn)(std::forward<TParam>(args)...); };

			return baseType::template runFn<
				decltype(fnWraper), std::tuple<typename std::remove_cv<typename std::remove_reference<TParam>::type>::type...> >(
						L, fnWraper, typename BuildArgsIndex<TParam...>::indexSeqType());
		}

		static int LuaCFunction(lua_State *L, TClass *obj, Tr (TClass::*fn)(TParam...) const) {
			auto fnWraper = [obj, fn](TParam &&... args) { return (obj->*fn)(std::forward<TParam>(args)...); };

			return baseType::template runFn<
				decltype(fnWraper), std::tuple<typename std::remove_cv<typename std::remove_reference<TParam>::type>::type...> >(
						L, fnWraper, typename BuildArgsIndex<TParam...>::indexSeqType());
		}
	};

	// }}}
}
}
}
#endif
