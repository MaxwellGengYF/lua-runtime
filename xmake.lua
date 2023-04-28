set_xmakever("2.7.8")
add_rules("mode.release", "mode.debug")
-- disable ccache in-case error
set_policy("build.ccache", false)

option("bin_dir")
set_default("bin")
set_showmenu(true)
option_end()

option("lua_enable_tests")
set_values(true, false)
set_default(false)
set_showmenu(true)
option_end()

option("_lua_bin_dir")
set_default(false)
set_showmenu(false)
add_deps("bin_dir")
after_check(function(option)
	if path.absolute(os.projectdir()) == path.absolute(os.scriptdir()) then
		local bin_dir = option:dep("bin_dir"):enabled()
		if is_mode("debug") then
			bin_dir = path.join(bin_dir, "debug")
		else
			bin_dir = path.join(bin_dir, "release")
		end
		option:set_value(bin_dir)
	else
		option:set_value(false)
	end
end)
option_end()

-- try options.lua
includes("scripts/options.lua")
if lua_toolchain then
	for k, v in pairs(lua_toolchain) do
		set_config(k, v)
	end
end
includes("scripts/xmake_func.lua")

if is_arch("x64", "x86_64", "arm64") then
	local bin_dir = get_config("_lua_bin_dir")
	if bin_dir then
		set_targetdir(bin_dir)
	end
	includes("ext/mimalloc")
	includes("src")
	if get_config("lua_enable_tests") then		
		includes("src/tests")
	end
else
	target("_lua_illegal_env")
	set_kind("phony")
	on_load(function(target)
		utils.error("Illegal environment. Please check your compiler, architecture or platform.")
	end)
	target_end()
end
