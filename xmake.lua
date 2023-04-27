set_xmakever("2.7.8")
add_rules("mode.release", "mode.debug")
-- disable ccache in-case error
set_policy("build.ccache", false)
-- try options.lua
includes("scripts/options.lua")
if lua_toolchain then
	for k, v in pairs(lua_toolchain) do
		set_config(k, v)
	end
end
includes("scripts/xmake_func.lua")

if is_arch("x64", "x86_64", "arm64") then
	if is_mode("debug") then
		set_targetdir("bin/debug")
    else
		set_targetdir("bin/release")
	end
	includes("src")
	includes("ext/mimalloc")
else
	target("_lua_illegal_env")
	set_kind("phony")
	on_load(function(target)
		utils.error("Illegal environment. Please check your compiler, architecture or platform.")
	end)
	target_end()
end
