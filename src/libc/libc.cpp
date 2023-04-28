#include <mimalloc.h>
#include <lua.hpp>
#include <filesystem>
#include "allocator.h"
#include <vector>
using string = std::basic_string<char, std::char_traits<char>, allocator<char>>;
template<typename T>
using vector = std::vector<T, allocator<T>>;

#ifdef _MSC_VER
#define LIBC_EXTERN extern "C" __declspec(dllexport)
#else
#define LIBC_EXTERN extern "C"
#endif
#define DEFINE_FUNC(F) int libc_##F(lua_State* L)
DEFINE_FUNC(malloc) {
	auto size = luaL_checkinteger(L, 1);
	auto ptr = mi_malloc(size);
	lua_pushinteger(L, reinterpret_cast<uint64_t>(ptr));
	return 1;
}
DEFINE_FUNC(free) {
	mi_free(reinterpret_cast<void*>(luaL_checkinteger(L, 1)));
	return 0;
}
DEFINE_FUNC(memcpy) {
	auto dst = luaL_checkinteger(L, 1);
	auto src = luaL_checkinteger(L, 2);
	auto size = luaL_checkinteger(L, 3);
	memcpy(reinterpret_cast<void*>(dst), reinterpret_cast<void*>(src), size);
	return 0;
}
DEFINE_FUNC(memcmp) {
	auto dst = luaL_checkinteger(L, 1);
	auto src = luaL_checkinteger(L, 2);
	auto size = luaL_checkinteger(L, 3);
	lua_pushinteger(L, memcmp(reinterpret_cast<void const*>(dst), reinterpret_cast<void const*>(src), size));
	return 1;
}
DEFINE_FUNC(memset) {
	auto ptr = luaL_checkinteger(L, 1);
	int value = luaL_checkinteger(L, 2);
	auto size = luaL_checkinteger(L, 3);
	memset(reinterpret_cast<void*>(ptr), value, size);
	return 0;
}
DEFINE_FUNC(byteof) {
	auto ptr = reinterpret_cast<uint8_t*>(luaL_checkinteger(L, 1));
	lua_pushinteger(L, ptr[0]);
	return 1;
}
DEFINE_FUNC(setbyte) {
	auto ptr = reinterpret_cast<uint8_t*>(luaL_checkinteger(L, 1));
	auto value = luaL_checkinteger(L, 2);
	ptr[0] = value;
	return 0;
}
DEFINE_FUNC(tostring) {
	auto ptr = reinterpret_cast<char*>(luaL_checkinteger(L, 1));
	auto size = luaL_checkinteger(L, 2);
	ptr[size] = 0;
	lua_pushstring(L, ptr);
	return 1;
}
DEFINE_FUNC(strptr) {
	auto ptr = luaL_checkstring(L, 1);
	lua_pushinteger(L, reinterpret_cast<uint64_t>(ptr));
	return 1;
}
DEFINE_FUNC(fread) {
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
	auto ptr = mi_malloc(length + 1);
	fread(ptr, length, 1, f);
	fclose(f);
	lua_pushinteger(L, reinterpret_cast<uint64_t>(ptr));
	lua_pushinteger(L, length);
	return 2;
}
DEFINE_FUNC(fwrite) {
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
DEFINE_FUNC(system) {
	auto command = luaL_checkstring(L, 1);
	lua_pushinteger(L, system(command));
	return 1;
}
DEFINE_FUNC(all_files) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	vector<string> strs;
	for (auto& p : std::filesystem::directory_iterator(path)) {
		strs.emplace_back(p.path().string<char, std::char_traits<char>, allocator<char>>());
	}
	lua_createtable(L, strs.size(), 0);
	size_t idx = 1;
	for(auto&& i : strs){
		lua_pushstring(L, i.c_str());
		lua_rawseti(L, -2, idx);
		++idx;
	}
	return 1;
}
DEFINE_FUNC(is_dir) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushboolean(L, std::filesystem::is_directory(path));
	return 1;
}
DEFINE_FUNC(is_file) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushboolean(L, std::filesystem::is_regular_file(path));
	return 1;
}
DEFINE_FUNC(basename) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, path.filename().replace_extension().string<char, std::char_traits<char>, allocator<char>>().c_str());
	return 1;
}
DEFINE_FUNC(filename) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, path.filename().string<char, std::char_traits<char>, allocator<char>>().c_str());
	return 1;
}
DEFINE_FUNC(absolute) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, std::filesystem::absolute(path).string<char, std::char_traits<char>, allocator<char>>().c_str());
	return 1;
}
DEFINE_FUNC(relative) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, std::filesystem::relative(path).string<char, std::char_traits<char>, allocator<char>>().c_str());
	return 1;
}
DEFINE_FUNC(extension) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	if (path.has_extension()) {
		lua_pushstring(L, path.extension().string<char, std::char_traits<char>, allocator<char>>().c_str());
	} else {
		lua_pushnil(L);
	}
	return 1;
}
#define REGIST_FUNC(F) \
	{ #F, libc_##F }
static const luaL_Reg mylib[] = {
	REGIST_FUNC(malloc),
	REGIST_FUNC(free),
	REGIST_FUNC(memcpy),
	REGIST_FUNC(memcmp),
	REGIST_FUNC(memset),
	REGIST_FUNC(byteof),
	REGIST_FUNC(setbyte),
	REGIST_FUNC(tostring),
	REGIST_FUNC(strptr),
	REGIST_FUNC(fread),
	REGIST_FUNC(fwrite),
	REGIST_FUNC(system),
	REGIST_FUNC(all_files),
	REGIST_FUNC(is_dir),
	REGIST_FUNC(is_file),
	REGIST_FUNC(basename),
	REGIST_FUNC(filename),
	REGIST_FUNC(absolute),
	REGIST_FUNC(relative),
	REGIST_FUNC(extension),
	{nullptr, nullptr} /* sentinel */
};

#undef REGIST_FUNC
#undef DEFINE_FUNC

//name of this function is not flexible
LIBC_EXTERN int luaopen_lua_libc(lua_State* L) {
	luaL_newlib(L, mylib);
	return 1;
}