#include <lua_memory.h>
#include <lua.hpp>
#include "std.h"

namespace lua {
LUA_DEFINE_FUNC(malloc) {
	auto size = luaL_checkinteger(L, 1);
	auto ptr = lua_allocate(size);
	lua_pushinteger(L, reinterpret_cast<uint64_t>(ptr));
	return 1;
}
LUA_DEFINE_FUNC(free) {
	lua_free(reinterpret_cast<void*>(luaL_checkinteger(L, 1)));
	return 0;
}
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
	LUA_REGIST_FUNC(malloc),
	LUA_REGIST_FUNC(free),
	LUA_REGIST_FUNC(memcpy),
	LUA_REGIST_FUNC(memcmp),
	LUA_REGIST_FUNC(memset),
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