#define LUA_CORE
// can use other allocator here
#include "lua_memory.h"
#include <mimalloc.h>
void* lua_allocate(size_t size) {
	return mi_malloc(size);
}
void lua_free(void* ptr) {
	mi_free(ptr);
}
void* lua_realloc(void* ptr, size_t new_size) {
	return mi_realloc(ptr, new_size);
}
void* lua_aligned_allocate(size_t align, size_t size){
	return mi_aligned_alloc(align, size);
}