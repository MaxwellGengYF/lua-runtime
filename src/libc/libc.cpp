#include <lua_memory.h>
#include "lua_meta.h"

namespace lua {

int byteof(uint8_t* b) {
	return *b;
}
void setbyte(uint8_t* ptr, int value) {
	*ptr = value;
}
string tostring(void* chr_ptr, size_t size) {
	return string{reinterpret_cast<char const*>(chr_ptr), size};
}
size_t strptr(char const* str) {
	return reinterpret_cast<size_t>(str);
}

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
bool libc_fwrite(char const* file_name, void* ptr, size_t size) {
	auto f = fopen(file_name, "wb");
	if (!f) {
		return false;
	}
	fwrite(ptr, size, 1, f);
	fclose(f);
	return true;
}
struct HostMap {
	unordered_set<string> map;
	HostMap() {
#ifdef LUA_WINDOWS
		map.emplace("windows");
#endif
#ifdef LUA_LINUX
		map.emplace("linux");
#endif
#ifdef LUA_MACOSX
		map.emplace("macosx");
#endif
#ifdef LUA_UNIX
		map.emplace("unix");
#endif
	}
};
bool is_host(char const* host_name) {
	static HostMap host_map;
	return host_map.map.find(host_name) != host_map.map.end();
}

string libc_getenv(char const* c){
	return std::getenv(c);
}

static const luaL_Reg funcs[] = {
	LUA_REGIST_FUNC(malloc, lua_allocate),
	LUA_REGIST_FUNC(free, lua_free),
	LUA_REGIST_FUNC(memcpy, std::memcpy),
	LUA_REGIST_FUNC(memcmp, std::memcmp),
	LUA_REGIST_FUNC(memset, std::memset),
	LUA_REGIST_FUNC(byteof, byteof),
	LUA_REGIST_FUNC(setbyte, setbyte),
	LUA_REGIST_FUNC(tostring, tostring),
	LUA_REGIST_FUNC(strptr, strptr),
	LUA_REGIST_FUNC(fread, libc_fread),
	LUA_REGIST_FUNC(fwrite, libc_fwrite),
	LUA_REGIST_FUNC(system, system),
	LUA_REGIST_FUNC(getenv, libc_getenv),
	LUA_REGIST_FUNC(is_host, is_host),
	{nullptr, nullptr} /* sentinel */
};

//name of this function is not flexible
LUA_LIBC_EXTERN int luaopen_lua_libc(lua_State* L) {
	luaL_newlib(L, funcs);
	return 1;
}
}// namespace lua