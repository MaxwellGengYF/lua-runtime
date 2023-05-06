#include <lua.hpp>
#include <filesystem>
#include "std.h"
namespace lua {
LUA_DEFINE_FUNC(all_files) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	vector<string> strs;
	for (auto& p : std::filesystem::directory_iterator(path)) {
		strs.emplace_back(p.path().string<char, std::char_traits<char>, allocator<char>>());
	}
	lua_createtable(L, strs.size(), 0);
	size_t idx = 1;
	for (auto&& i : strs) {
		lua_pushstring(L, i.c_str());
		lua_rawseti(L, -2, idx);
		++idx;
	}
	return 1;
}
LUA_DEFINE_FUNC(is_dir) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushboolean(L, std::filesystem::is_directory(path));
	return 1;
}
LUA_DEFINE_FUNC(is_file) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushboolean(L, std::filesystem::is_regular_file(path));
	return 1;
}
LUA_DEFINE_FUNC(basename) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, path.filename().replace_extension().string<char, std::char_traits<char>, allocator<char>>().c_str());
	return 1;
}
LUA_DEFINE_FUNC(filename) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, path.filename().string<char, std::char_traits<char>, allocator<char>>().c_str());
	return 1;
}
LUA_DEFINE_FUNC(absolute) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, std::filesystem::absolute(path).string<char, std::char_traits<char>, allocator<char>>().c_str());
	return 1;
}
LUA_DEFINE_FUNC(relative) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, std::filesystem::relative(path).string<char, std::char_traits<char>, allocator<char>>().c_str());
	return 1;
}
LUA_DEFINE_FUNC(extension) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	if (path.has_extension()) {
		lua_pushstring(L, path.extension().string<char, std::char_traits<char>, allocator<char>>().c_str());
	} else {
		lua_pushnil(L);
	}
	return 1;
}
LUA_DEFINE_FUNC(join) {
	auto str = (std::filesystem::path{luaL_checkstring(L, 1)} / std::filesystem::path{luaL_checkstring(L, 2)}).string<char, std::char_traits<char>, allocator<char>>();
	lua_pushstring(L, str.c_str());
	return 1;
}
static const luaL_Reg funcs[]{
	LUA_REGIST_FUNC(all_files),
	LUA_REGIST_FUNC(is_dir),
	LUA_REGIST_FUNC(is_file),
	LUA_REGIST_FUNC(basename),
	LUA_REGIST_FUNC(filename),
	LUA_REGIST_FUNC(absolute),
	LUA_REGIST_FUNC(relative),
	LUA_REGIST_FUNC(extension),
	LUA_REGIST_FUNC(join),
    {nullptr, nullptr}
};
LUA_LIBC_EXTERN int luaopen_lua_path(lua_State* L) {
	luaL_newlib(L, funcs);
	return 1;
}
}// namespace lua