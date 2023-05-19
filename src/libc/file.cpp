#include "filesystem_std.h"
#include "lua_meta.h"
namespace lua {
LUA_DEFINE_FUNC(all_files) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	vector<string> strs;
	for (auto& p : std::filesystem::directory_iterator(path)) {
		strs.emplace_back(to_string(p.path()));
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
bool is_dir(char const* dir) {
	std::filesystem::path path{dir};
	return std::filesystem::is_directory(path);
}
LUA_DEFINE_TEMPLATE_FUNC(is_dir, is_dir)
bool is_file(char const* dir) {
	std::filesystem::path path{dir};
	return std::filesystem::is_regular_file(path);
}
LUA_DEFINE_TEMPLATE_FUNC(is_file, is_file)
bool exists(char const* dir) {
	std::filesystem::path path{dir};
	return std::filesystem::exists(path);
}
LUA_DEFINE_TEMPLATE_FUNC(exists, exists)

LUA_DEFINE_FUNC(basename) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, to_string(path.filename().replace_extension()).c_str());
	return 1;
}
LUA_DEFINE_FUNC(filename) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, to_string(path.filename()).c_str());
	return 1;
}
LUA_DEFINE_FUNC(absolute) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, to_string(std::filesystem::absolute(path)).c_str());
	return 1;
}
LUA_DEFINE_FUNC(relative) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	lua_pushstring(L, to_string(std::filesystem::relative(path)).c_str());
	return 1;
}
LUA_DEFINE_FUNC(extension) {
	auto dir = luaL_checkstring(L, 1);
	std::filesystem::path path{dir};
	if (path.has_extension()) {
		lua_pushstring(L, to_string(path.extension()).c_str());
	} else {
		lua_pushnil(L);
	}
	return 1;
}
LUA_DEFINE_FUNC(join) {
	auto str = (std::filesystem::path{luaL_checkstring(L, 1)} / std::filesystem::path{luaL_checkstring(L, 2)});
	lua_pushstring(L, to_string(str).c_str());
	return 1;
}
static const luaL_Reg funcs[]{
	LUA_REGIST_FUNC(all_files),
	LUA_REGIST_TEMPLATE_FUNC(is_dir),
	LUA_REGIST_TEMPLATE_FUNC(is_file),
	LUA_REGIST_TEMPLATE_FUNC(exists),
	LUA_REGIST_FUNC(basename),
	LUA_REGIST_FUNC(filename),
	LUA_REGIST_FUNC(absolute),
	LUA_REGIST_FUNC(relative),
	LUA_REGIST_FUNC(extension),
	LUA_REGIST_FUNC(join),
	{nullptr, nullptr}};
LUA_LIBC_EXTERN int luaopen_lua_path(lua_State* L) {
	luaL_newlib(L, funcs);
	return 1;
}
}// namespace lua