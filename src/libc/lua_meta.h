#pragma once
#include "std.h"
#include <lua.hpp>

namespace lua {
#define LUA_DEFINE_FUNC(F) int libc_##F(lua_State* L)
#define LUA_REGIST_FUNC(F) \
	{ #F, libc_##F }
template<typename X>
struct RunLua;

template<typename T>
struct is_tuple : std::false_type {};
template<typename... A>
struct is_tuple<std::tuple<A...>> : std::true_type {
};
template<typename T>
static constexpr bool is_tuple_v = is_tuple<T>::value;
template<typename T>
static constexpr bool _always_false_v = false;
template<typename R, typename... Args>
struct RunLua<R(Args...)> {
	template<typename Arg>
	static Arg get_arg(lua_State* L, int index) {
		using PureArg = std::remove_cvref_t<Arg>;
		if constexpr (std::is_same_v<PureArg, char const*> || std::is_same_v<PureArg, string> || std::is_same_v<PureArg, string_view>) {
			return PureArg{luaL_checkstring(L, index)};
		} else if constexpr (std::is_integral_v<PureArg>) {
			return static_cast<PureArg>(luaL_checkinteger(L, index));
		} else if constexpr (std::is_pointer_v<PureArg>) {
			return reinterpret_cast<PureArg>(luaL_checkinteger(L, index));
		} else if constexpr (std::is_floating_point_v<PureArg>) {
			return static_cast<PureArg>(luaL_checknumber(L, index));
		} else {
			static_assert(_always_false_v<PureArg>, "Argument type not supported");
		}
	};
	template<typename Ret>
	static void process_ret(lua_State* L, Ret&& ret, int index) {
		using PureRet = std::remove_cvref_t<Ret>;
		if constexpr (std::is_same_v<PureRet, char const*>) {
			lua_pushstring(L, ret);
		} else if constexpr (std::is_same_v<PureRet, string>) {
			lua_pushstring(L, ret.c_str());
		} else if constexpr (std::is_same_v<PureRet, bool>) {
			lua_pushboolean(L, ret);
		} else if constexpr (std::is_integral_v<PureRet>) {
			lua_pushinteger(L, ret);
		} else if constexpr (std::is_floating_point_v<PureRet>) {
			lua_pushnumber(L, ret);
		} else if constexpr (std::is_pointer_v<PureRet>) {
			lua_pushinteger(L, reinterpret_cast<size_t>(ret));
		} else {
			static_assert(_always_false_v<PureRet>, "Return type not supported");
		}
	};
};
template<typename T, typename Func>
struct FuncImpl;

#define LUA_DEFINE_TEMPLATE_FUNC(lua_name, func_name)                                                           \
	struct _unused_type##lua_name {};                                                                           \
	template<typename R, typename... Args>                                                                      \
	struct FuncImpl<R(Args...), _unused_type##lua_name> {                                                       \
		static int run(lua_State* L) {                                                                          \
			using TempType = RunLua<R(Args...)>;                                                                \
			int index = 0;                                                                                      \
			auto get_arg_func = [&]<typename A>() {                                                             \
				++index;                                                                                        \
				return TempType::get_arg<A>(L, index);                                                          \
			};                                                                                                  \
			auto call_func = [&] -> decltype(auto) {                                                            \
				return std::apply(func_name, std::tuple<Args...>{get_arg_func.template operator()<Args>()...}); \
			};                                                                                                  \
			using RetType = decltype(call_func());                                                              \
			if constexpr (std::is_void_v<RetType>) {                                                            \
				call_func();                                                                                    \
				return 0;                                                                                       \
			} else if constexpr (is_tuple_v<RetType>) {                                                         \
				int count = 0;                                                                                  \
				std::apply(                                                                                     \
					[&]<typename... Rets>(Rets&&... ret) {                                                      \
					++count;                                                                                    \
					auto d = {(TempType::process_ret(L, std::forward<Rets>(ret), count), count)...};            \
					},                                                                                          \
					call_func());                                                                               \
				return count;                                                                                   \
			} else {                                                                                            \
				TempType::process_ret(L, call_func(), 1);                                                       \
				return 1;                                                                                       \
			}                                                                                                   \
		}                                                                                                       \
	};                                                                                                          \
	using lua_name##_template = FuncImpl<decltype(func_name), _unused_type##lua_name>;

#define LUA_REGIST_TEMPLATE_FUNC(lua_name) \
	luaL_Reg { #lua_name, lua_name##_template::run }

}// namespace lua