#include "filesystem_std.h"
#include "lua_meta.h"
namespace lua {
string path_to_string(std::filesystem::path const& path) {
	auto str = to_string(path);
#ifdef _WIN32
	for (auto& i : str) {
		if (i == '/') i = '\\';
	}
#endif
	return str;
}
vector<string> all_files(char const* dir) {
	std::filesystem::path path{dir};
	vector<string> strs;
	for (auto& p : std::filesystem::directory_iterator(path)) {
		strs.emplace_back(path_to_string(p.path()));
	}
	return strs;
}

bool is_dir(char const* dir) {
	std::filesystem::path path{dir};
	return std::filesystem::is_directory(path);
}
bool is_file(char const* dir) {
	std::filesystem::path path{dir};
	return std::filesystem::is_regular_file(path);
}
bool exists(char const* dir) {
	std::filesystem::path path{dir};
	return std::filesystem::exists(path);
}
string basename(char const* dir) {
	std::filesystem::path path{dir};
	return to_string(path.filename().replace_extension());
}
string filename(char const* dir) {
	std::filesystem::path path{dir};
	return to_string(path.filename());
}
string absolute(char const* dir) {
	std::filesystem::path path{dir};
	return path_to_string(std::filesystem::absolute(path));
}
string relative(char const* dir) {
	std::filesystem::path path{dir};
	return path_to_string(std::filesystem::relative(path));
}
string extension(char const* dir) {
	std::filesystem::path path{dir};
	if (path.has_extension()) {
		return to_string(path.extension());
	} else {
		return "";
	}
}
string join(char const* a, char const* b){
	auto str = std::filesystem::path{a} / std::filesystem::path{b};
	return path_to_string(str);
}

static const luaL_Reg funcs[]{
	LUA_REGIST_FUNC(all_files, all_files),
	LUA_REGIST_FUNC(is_dir, is_dir),
	LUA_REGIST_FUNC(is_file, is_file),
	LUA_REGIST_FUNC(exists, exists),
	LUA_REGIST_FUNC(basename, basename),
	LUA_REGIST_FUNC(filename, filename),
	LUA_REGIST_FUNC(absolute, absolute),
	LUA_REGIST_FUNC(relative, relative),
	LUA_REGIST_FUNC(extension, extension),
	LUA_REGIST_FUNC(join, join),
	{nullptr, nullptr}};
LUA_LIBC_EXTERN int luaopen_lua_path(lua_State* L) {
	luaL_newlib(L, funcs);
	return 1;
}
}// namespace lua