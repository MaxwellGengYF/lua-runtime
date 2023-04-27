target("lua-base")
_config_project({
	project_kind = "object"
})
if is_plat("windows") then
	add_defines("LUA_BUILD_AS_DLL", {
		public = true
	})
end
add_files("*.c")
target_end()

local function lua_target(name, kind, file)
	target(name)
	_config_project({
		project_kind = kind
	})
	add_deps("lua-base")
    if file then
        add_files("main/" .. file)        
    end
	target_end()
end

lua_target("lua", "binary", "lua.c")
lua_target("luac", "binary", "luac.c")

lua_target("lua", "shared")
lua_target("luac", "shared")