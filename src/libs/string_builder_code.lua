local libc = require("lua_libc")
local _string_builder = {}

local function _add_capacity(self, s)
	local size = s + self._size
	local capa = self._capacity
	if capa >= size then
		return
	end
	while capa < size do
		capa = capa * 2
	end
	local old_ptr = self._ptr + 1
	local new_ptr = libc.malloc(capa + 1)
	libc.memcpy(new_ptr, old_ptr, self._size)
	libc.free(old_ptr)
	self._ptr = new_ptr - 1
	self._capacity = capa
end
function _string_builder:tostring()
	return libc.tostring(self._ptr + 1, self._size)
end
function _string_builder:reserve(s)
	local capa = self._capacity
	if capa >= s then
		return
	end
	local old_ptr = self._ptr + 1
	local new_ptr = libc.malloc(s + 1)
	libc.memcpy(new_ptr, old_ptr, self._size)
	libc.free(old_ptr)
	self._ptr = new_ptr - 1
	self._capacity = s
end
function _string_builder:equal(str)
	local str_ptr
	local str_size
	if type(str) == "string" then
		str_ptr = libc.strptr(str)
		str_size = #str
	else
		str_ptr = str:caddr()
		str_size = str:size()
	end
	if str_size ~= self.size() then
		return false
	end
	local ptr = self._ptr + self._size + 1
	return libc.memcmp(ptr, str_ptr, str_size) == 0
end
function _string_builder:add(str)
	local str_ptr
	local str_size
	if type(str) == "string" then
		str_ptr = libc.strptr(str)
		str_size = #str
	else
		str_ptr = str:caddr()
		str_size = str:size()
	end
	if str_size == 0 then
		return
	end
	_add_capacity(self, str_size)
	local ptr = self._ptr + self._size + 1
	libc.memcpy(ptr, str_ptr, str_size)
	self._size = self._size + str_size
	return self
end
function _string_builder:subview(offset, size)
	local sf = self
	return {
		_size = math.min(sf._size - offset + 1, size),
		_ptr = sf._ptr + offset,
		size = function(self)
			return self._size
		end,
		caddr = function(self)
			return self._ptr
		end
	}
end
function _string_builder:add_char(c)
	_add_capacity(self, 1)
	self._size = self._size + 1
	libc.setbyte(self._ptr + self._size, c)
	return self
end
function _string_builder:dispose()
	if self._ptr ~= -1 then
		libc.free(self._ptr + 1)
		self._ptr = -1
	end
end
function _string_builder:write_to(path)
	if self._size == 0 then
		return
	end
	libc.fwrite(path, self._ptr + 1, self._size)
end
function _string_builder:get(i)
	return libc.byteof(self._ptr, i)
end
function _string_builder:set(i, v)
	return libc.setbyte(self._ptr + i, v)
end
function _string_builder:erase(i)
	self._size = math.max(self._size - i, 1)
end
function _string_builder:size()
	return self._size
end
function _string_builder:capacity()
	return self._capacity
end
function _string_builder:caddr()
	return self._ptr + 1
end
function _string_builder:clear()
	self._size = 0
end
local function tab_inherit(tab)
	local map = {}
	for k, v in pairs(tab) do
		map[k] = v
	end
	return map
end

local map = {
	StringBuilder = function(str)
		local inst = tab_inherit(_string_builder)
		if str then
			local str_ptr
			local str_size
			if type(str) == "string" then
				str_ptr = libc.strptr(str)
				str_size = #str
			elseif type(str) == "number" then
				inst._size = 0
				inst._capacity = str
				inst._ptr = libc.malloc(str + 1) - 1
				return inst
			else
				str_ptr = str:caddr()
				str_size = str:size()
			end
			local capa = math.max(32, str_size)
			local addr = libc.malloc(capa)
			inst._size = str_size
			inst._capacity = capa
			inst._ptr = addr - 1
			libc.memcpy(addr, str_ptr, str_size)
		else
			inst._size = 0
			inst._capacity = 32
			inst._ptr = libc.malloc(33) - 1
		end
		return inst
	end,
	StringBuilderFromFile = function(str)
		local ptr, size = libc.fread(str)
		local inst = tab_inherit(_string_builder)
		if ptr ~= nil and size ~= nil then
			inst._size = size
			inst._capacity = size
			inst._ptr = ptr - 1
		else
			inst._size = 0
			inst._capacity = 0
			inst._ptr = -1
		end
		return inst
	end,
	string_split = function(str, chr)
		local map = {}
		for part in string.gmatch(str, "([^" .. chr .. "]+)") do
			table.insert(map, part)
		end
		return map
	end,
	string_replace = function(str, from, to)
		local s, _ = str:gsub(from, to)
		return s
	end,
	string_contains = function(str, sub_str)
		return str:match(sub_str) ~= nil
	end
}
return map
