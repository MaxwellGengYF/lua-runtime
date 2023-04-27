local lib = import("lib")
local my_is_host
local function find_process_path(process)
	local cut
	local is_win = my_is_host("windows")
	if is_win then
		cut = ";"
	else
		cut = ":"
	end
	local path_str = os.getenv("PATH")
	if path_str then
		local paths = lib.string_split(path_str, cut)
		for i, pth in ipairs(paths) do
			if os.isfile(path.join(pth, process)) then
				return pth
			end
		end
	end
	return nil
end

local function sort_key(map, func)
	local keys = {}
	for k, v in pairs(map) do
		table.insert(keys, k)
	end
	table.sort(keys)
	for i, v in ipairs(keys) do
		func(v, map[v])
	end
end

local function find_llvm()
	local clang_name = "clang"
	if my_is_host("linux") and os.isfile("/usr/bin/llvm-ar") then
		return "/usr"
	elseif my_is_host("macosx") then
		import("lib.detect.find_tool")
		local bindir = find_path("llvm-ar", "/usr/local/Cellar/llvm/*/bin")
		if bindir then
			return path.directory(bindir)
		end
	else
		clang_name = "clang.exe"
	end
	local clang_bin = find_process_path(clang_name)
	if clang_bin then
		return lib.string_replace(path.directory(clang_bin), "\\", "/")
	end
	return nil
end
function main(...)
	-- workaround xmake
	if type(os.is_host) == "function" then
		my_is_host = os.is_host
	elseif type(os.host) == "function" then
		my_is_host = function(p)
			return os.host() == p
		end
	else
		my_is_host = is_host
	end
	local args = {}
	for i, v in ipairs({...}) do
		local kv = lib.string_split(v, "=")
		if table.getn(kv) == 2 then
			args[kv[1]] = kv[2]
		end
	end
	local sb = lib.StringBuilder()
	sb:add("lua_toolchain = {\n")
	local toolchain = args["toolchain"]
	local sdk_path
	if toolchain then
		args["toolchain"] = nil
		if toolchain == "llvm" then
			sdk_path = find_llvm()
		end
	else
		sdk_path = find_llvm()
		if sdk_path then
			toolchain = "llvm"
		elseif my_is_host("windows") then
			toolchain = "msvc"
		else
			toolchain = "gcc"
		end
	end
	sb:add("\ttoolchain = \""):add(toolchain):add("\",\n")
	if sdk_path then
		sb:add("\tsdk = \""):add(sdk_path):add("\",\n")
	end
	sb:add("}\n")
	sb:write_to(path.join(os.scriptdir(), "options.lua"))
	sb:dispose()
end
