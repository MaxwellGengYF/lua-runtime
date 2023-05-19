#include <lua_memory.h>
#include <lua.hpp>
#include "std.h"

namespace lua {
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
		if constexpr (std::is_same_v<PureArg, char*> || std::is_same_v<PureArg, char const*> || std::is_same_v<PureArg, string> || std::is_same_v<PureArg, string_view>) {
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
		if constexpr (std::is_same_v<PureRet, char*> || std::is_same_v<PureRet, char const*>) {
			lua_pushstring(L, ret);
		} else if constexpr (std::is_same_v<PureRet, string>) {
			lua_pushstring(L, ret.c_str());
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

#define LUA_DEFINE_TEMPLATE_FUNC(func_name)                                                                     \
	struct _unused_type##func_name {};                                                                          \
	template<typename R, typename... Args>                                                                      \
	struct FuncImpl<R(Args...), _unused_type##func_name> {                                                      \
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
	using func_name##_template = FuncImpl<decltype(func_name), _unused_type##func_name>;

#define LUA_REGIST_TEMPLATE_FUNC(lua_name, func_name) \
	luaL_Reg { #lua_name, func_name##_template::run }

LUA_DEFINE_TEMPLATE_FUNC(lua_allocate)
LUA_DEFINE_TEMPLATE_FUNC(lua_free)
LUA_DEFINE_TEMPLATE_FUNC(memcpy)
LUA_DEFINE_TEMPLATE_FUNC(memcmp)
LUA_DEFINE_TEMPLATE_FUNC(memset)

LUA_DEFINE_FUNC(memcpy) {
	auto dst = luaL_checkinteger(L, 1);
	auto src = luaL_checkinteger(L, 2);
	auto size = luaL_checkinteger(L, 3);
	memcpy(reinterpret_cast<void*>(dst), reinterpret_cast<void*>(src), size);
	return 0;
}
LUA_DEFINE_FUNC(memcmp) {
	auto dst = luaL_checkinteger(L, 1);
	auto src = luaL_checkinteger(L, 2);
	auto size = luaL_checkinteger(L, 3);
	lua_pushinteger(L, memcmp(reinterpret_cast<void const*>(dst), reinterpret_cast<void const*>(src), size));
	return 1;
}
LUA_DEFINE_FUNC(memset) {
	auto ptr = luaL_checkinteger(L, 1);
	int value = luaL_checkinteger(L, 2);
	auto size = luaL_checkinteger(L, 3);
	memset(reinterpret_cast<void*>(ptr), value, size);
	return 0;
}
LUA_DEFINE_FUNC(byteof) {
	auto ptr = reinterpret_cast<uint8_t*>(luaL_checkinteger(L, 1));
	lua_pushinteger(L, ptr[0]);
	return 1;
}
LUA_DEFINE_FUNC(setbyte) {
	auto ptr = reinterpret_cast<uint8_t*>(luaL_checkinteger(L, 1));
	auto value = luaL_checkinteger(L, 2);
	ptr[0] = value;
	return 0;
}
LUA_DEFINE_FUNC(tostring) {
	auto ptr = reinterpret_cast<char*>(luaL_checkinteger(L, 1));
	auto size = luaL_checkinteger(L, 2);
	ptr[size] = 0;
	lua_pushstring(L, ptr);
	return 1;
}
LUA_DEFINE_FUNC(strptr) {
	auto ptr = luaL_checkstring(L, 1);
	lua_pushinteger(L, reinterpret_cast<uint64_t>(ptr));
	return 1;
}
LUA_DEFINE_FUNC(fread) {
	auto file_name = luaL_checkstring(L, 1);
	auto f = fopen(file_name, "rb");
	if (!f) {
		lua_pushnil(L);
		lua_pushnil(L);
		return 2;
	}
	fseek(f, 0, SEEK_END);
	auto length = ftell(f);
	fseek(f, 0, SEEK_SET);
	auto ptr = lua_allocate(length + 1);
	fread(ptr, length, 1, f);
	fclose(f);
	lua_pushinteger(L, reinterpret_cast<uint64_t>(ptr));
	lua_pushinteger(L, length);
	return 2;
}
LUA_DEFINE_FUNC(fwrite) {
	auto file_name = luaL_checkstring(L, 1);
	auto ptr = reinterpret_cast<void*>(luaL_checkinteger(L, 2));
	auto size = static_cast<uint64_t>(luaL_checkinteger(L, 3));
	auto f = fopen(file_name, "wb");
	if (!f) {
		lua_pushnil(L);
		return 1;
	}
	fwrite(ptr, size, 1, f);
	fclose(f);
	lua_pushboolean(L, true);
	return 1;
}
LUA_DEFINE_FUNC(system) {
	auto command = luaL_checkstring(L, 1);
	lua_pushinteger(L, system(command));
	return 1;
}

static const luaL_Reg funcs[] = {
	LUA_REGIST_TEMPLATE_FUNC(malloc, lua_allocate),
	LUA_REGIST_TEMPLATE_FUNC(free, lua_free),
	LUA_REGIST_TEMPLATE_FUNC(memcpy, memcpy),
	LUA_REGIST_TEMPLATE_FUNC(memcmp, memcmp),
	LUA_REGIST_TEMPLATE_FUNC(memset, memset),
	LUA_REGIST_FUNC(byteof),
	LUA_REGIST_FUNC(setbyte),
	LUA_REGIST_FUNC(tostring),
	LUA_REGIST_FUNC(strptr),
	LUA_REGIST_FUNC(fread),
	LUA_REGIST_FUNC(fwrite),
	LUA_REGIST_FUNC(system),
	{nullptr, nullptr} /* sentinel */
};

//name of this function is not flexible
LUA_LIBC_EXTERN int luaopen_lua_libc(lua_State* L) {
	luaL_newlib(L, funcs);
	return 1;
}
}// namespace lua