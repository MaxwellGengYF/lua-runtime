#include <lua.hpp>
#include <mimalloc.h>
#include <iostream>
#include <fstream>
#include <vector>

std::vector<std::byte> compile(const char* str) {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_loadstring(L, str);
	std::vector<std::byte> bytes;
	lua_dump(
		L, [](lua_State* L, const void* p, size_t sz, void* ud) -> int {
			auto& bytes = *reinterpret_cast<std::vector<std::byte>*>(ud);
			auto idx = bytes.size();
			bytes.resize(idx + sz);
			memcpy(bytes.data() + idx, p, sz);
			return 0;
		},
		&bytes, true);
	lua_close(L);
	return bytes;
}

int main() {
	// execyte string directly is also fine
	// luaL_dostring(L, R"(print ("hello world"))");

	auto bytes = compile(R"(print ("hello world"))");
	std::cout << "compiled size: " << bytes.size() << '\n';

	// Create new Lua state and load the lua libraries
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_loadbufferx(L, (const char*)bytes.data(), bytes.size(), "test", nullptr);
	lua_pcall(L, 0, LUA_MULTRET, 0);
	lua_close(L);
	return 0;
}