target("lua-lib")
_config_project({
	project_kind = "shared"
})
if is_plat("windows") then
	add_defines("LUA_BUILD_AS_DLL", {
		public = true
	})
end
add_includedirs("lua", {public = true})
add_deps("mimalloc")
add_files("lua/*.c")
target_end()

target("lua_libc")
_config_project({
	project_kind = "shared"
})
add_deps("lua-lib")
add_files("libc/libc.cpp")
target_end()

target("lua_path")
_config_project({
	project_kind = "shared"
})
add_deps("lua-lib")
add_files("libc/file.cpp")
target_end()

local function lua_target(name, kind, file)
	target(name)
	_config_project({
		project_kind = kind
	})
	add_deps("lua-lib")
	add_files("lua/main/" .. file)
	target_end()
end

lua_target("lua", "binary", "lua.c")
lua_target("luac", "binary", "luac.c")