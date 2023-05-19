#pragma once
#include <vector>
#include <span>
#include <string>
#include <lua_memory.h>
#ifdef _MSC_VER
#define LUA_LIBC_EXTERN extern "C" __declspec(dllexport)
#else
#define LUA_LIBC_EXTERN extern "C"
#endif
namespace lua {
template<typename T = std::byte>
struct allocator {
	using value_type = T;
	constexpr allocator() noexcept = default;
	explicit constexpr allocator(const char*) noexcept {}
	template<typename U>
	constexpr allocator(allocator<U>) noexcept {}
	[[nodiscard]] auto allocate(std::size_t n) const noexcept {
		if (alignof(T) <= 16) {
			return static_cast<T*>(lua_allocate(sizeof(T) * n));
		} else {
			return static_cast<T*>(lua_aligned_allocate(alignof(T), sizeof(T) * n));
		}
	}
	[[nodiscard]] auto allocate(std::size_t n, size_t alignment, size_t) const noexcept {
		assert(alignment >= alignof(T));
		if (alignof(T) <= 16) {
			return static_cast<T*>(lua_allocate(sizeof(T) * n));
		} else {
			return static_cast<T*>(lua_aligned_allocate(alignof(T), sizeof(T) * n));
		}
	}
	void deallocate(T* p, size_t) const noexcept {
		lua_free(p);
	}
	void deallocate(void* p, size_t) const noexcept {
		lua_free(p);
	}
	template<typename R>
	[[nodiscard]] constexpr auto operator==(allocator<R>) const noexcept -> bool {
		return std::is_same_v<T, R>;
	}
};

template<typename T>
using vector = std::vector<T, lua::allocator<T>>;
using string = std::basic_string<char, std::char_traits<char>, lua::allocator<char>>;
using string_view = std::basic_string_view<char, std::char_traits<char>>;
using std::span;
}// namespace lua