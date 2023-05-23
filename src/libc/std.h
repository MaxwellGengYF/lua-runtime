#pragma once
#include <vector>
#include <span>
#include <string>
#include <variant>
#include <tuple>
#include <array>
#include <lua_memory.h>
#include <unordered_map>
#include <unordered_set>
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

template<typename T, typename alloc = lua::allocator<T>>
using vector = std::vector<T, alloc>;
using string = std::basic_string<char, std::char_traits<char>, lua::allocator<char>>;
using string_view = std::basic_string_view<char, std::char_traits<char>>;
using std::apply;
using std::array;
using std::get;
using std::pair;
using std::span;
using std::tuple;
using std::variant;
using std::visit;
template<typename T>
struct func_ptr;
template<typename R, typename... Args>
struct func_ptr<R (*)(Args...)> {
	using type = R (*)(Args...);
};
template<typename R, typename... Args>
struct func_ptr<R(Args...)> {
	using type = R (*)(Args...);
};
template<typename T>
using func_ptr_t = func_ptr<T>::type;
template<typename K, typename V, typename Hasher = std::hash<K>, typename Equal = std::equal_to<K>, typename alloc = lua::allocator<pair<K const, V>>>
using unordered_map = std::unordered_map<K, V, Hasher, Equal, alloc>;
template<typename K, typename Hasher = std::hash<K>, typename Equal = std::equal_to<K>, typename alloc = lua::allocator<K>>
using unordered_set = std::unordered_set<K, Hasher, Equal, alloc>;
}// namespace lua