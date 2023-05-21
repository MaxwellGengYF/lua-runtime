#include <lua_memory.h>
#include "lua_meta.h"

namespace lua {

LUA_DEFINE_FUNC(malloc, lua_allocate)
LUA_DEFINE_FUNC(free, lua_free)
LUA_DEFINE_FUNC(memcpy, memcpy)
LUA_DEFINE_FUNC(memcmp, memcmp)
LUA_DEFINE_FUNC(memset, memset)
int byteof(uint8_t* b) {
	return *b;
}
LUA_DEFINE_FUNC(byteof, byteof)
void setbyte(uint8_t* ptr, int value) {
	*ptr = value;
}
LUA_DEFINE_FUNC(setbyte, setbyte)
string tostring(void* chr_ptr, size_t size) {
	return string{reinterpret_cast<char const*>(chr_ptr), size};
}
LUA_DEFINE_FUNC(tostring, tostring)
size_t strptr(char const* str) {
	return reinterpret_cast<size_t>(str);
}
LUA_DEFINE_FUNC(strptr, strptr)

pair<void*, size_t> libc_fread(char const* file_name) {
	auto f = fopen(file_name, "rb");
	if (!f) {
		return {nullptr, 0};
	}
	fseek(f, 0, SEEK_END);
	auto length = ftell(f);
	fseek(f, 0, SEEK_SET);
	auto ptr = lua_allocate(length + 1);
	fread(ptr, length, 1, f);
	fclose(f);
	return {ptr, length};
}
LUA_DEFINE_FUNC(fread, libc_fread)
bool libc_fwrite(char const* file_name, void* ptr, size_t size) {
	auto f = fopen(file_name, "wb");
	if (!f) {
		return false;
	}
	fwrite(ptr, size, 1, f);
	fclose(f);
	return true;
}
LUA_DEFINE_FUNC(fwrite, libc_fwrite)
LUA_DEFINE_FUNC(system, system)

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