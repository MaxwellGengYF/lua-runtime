target("lua-lib")
_config_project({
	project_kind = "shared"
})
if is_plat("windows") then
	add_defines("LUA_BUILD_AS_DLL", {
		public = true
	})
end
add_includedirs("./", {public = true})
add_deps("mimalloc")
add_files("*.c")
target_end()

local function lua_target(name, kind, file)
	target(name)
	_config_project({
		project_kind = kind
	})
	add_deps("lua-lib")
	add_files("main/" .. file)
	target_end()
end

lua_target("lua", "binary", "lua.c")
lua_target("luac", "binary", "luac.c")