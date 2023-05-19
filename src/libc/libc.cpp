#include <lua_memory.h>
#include "lua_meta.h"

namespace lua {

LUA_DEFINE_TEMPLATE_FUNC(malloc, lua_allocate)
LUA_DEFINE_TEMPLATE_FUNC(free, lua_free)
LUA_DEFINE_TEMPLATE_FUNC(memcpy, memcpy)
LUA_DEFINE_TEMPLATE_FUNC(memcmp, memcmp)
LUA_DEFINE_TEMPLATE_FUNC(memset, memset)
int byteof(uint8_t* b) {
	return *b;
}
LUA_DEFINE_TEMPLATE_FUNC(byteof, byteof)
void setbyte(uint8_t* ptr, int value) {
	*ptr = value;
}
LUA_DEFINE_TEMPLATE_FUNC(setbyte, setbyte)
string tostring(void* chr_ptr, size_t size) {
	return string{reinterpret_cast<char const*>(chr_ptr), size};
}
LUA_DEFINE_TEMPLATE_FUNC(tostring, tostring)
size_t strptr(char const* str) {
	return reinterpret_cast<size_t>(str);
}
LUA_DEFINE_TEMPLATE_FUNC(strptr, strptr)

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
LUA_DEFINE_TEMPLATE_FUNC(system, system)

static const luaL_Reg funcs[] = {
	LUA_REGIST_TEMPLATE_FUNC(malloc),
	LUA_REGIST_TEMPLATE_FUNC(free),
	LUA_REGIST_TEMPLATE_FUNC(memcpy),
	LUA_REGIST_TEMPLATE_FUNC(memcmp),
	LUA_REGIST_TEMPLATE_FUNC(memset),
	LUA_REGIST_TEMPLATE_FUNC(byteof),
	LUA_REGIST_TEMPLATE_FUNC(setbyte),
	LUA_REGIST_TEMPLATE_FUNC(tostring),
	LUA_REGIST_TEMPLATE_FUNC(strptr),
	LUA_REGIST_FUNC(fread),
	LUA_REGIST_FUNC(fwrite),
	LUA_REGIST_TEMPLATE_FUNC(system),
	{nullptr, nullptr} /* sentinel */
};

//name of this function is not flexible
LUA_LIBC_EXTERN int luaopen_lua_libc(lua_State* L) {
	luaL_newlib(L, funcs);
	return 1;
}
}// namespace lua