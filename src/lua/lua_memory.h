#pragma once
#include "luaconf.h"
#include <stddef.h>
LUA_API void* lua_allocate(size_t size);
LUA_API void* lua_aligned_allocate(size_t align, size_t size);
LUA_API void lua_free(void* ptr);
LUA_API void* lua_realloc(void* ptr, size_t new_size);