#pragma once
#include "mimalloc.h"
#include <assert.h>
template<typename T = std::byte>
struct allocator {
	using value_type = T;
	constexpr allocator() noexcept = default;
	explicit constexpr allocator(const char*) noexcept {}
	template<typename U>
	constexpr allocator(allocator<U>) noexcept {}
	[[nodiscard]] auto allocate(std::size_t n) const noexcept {
		if (alignof(T) <= 16) {
			return static_cast<T*>(mi_malloc(sizeof(T) * n));
		} else {
			return static_cast<T*>(mi_aligned_alloc(alignof(T), sizeof(T) * n));
		}
	}
	[[nodiscard]] auto allocate(std::size_t n, size_t alignment, size_t) const noexcept {
		assert(alignment >= alignof(T));
		if (alignof(T) <= 16) {
			return static_cast<T*>(mi_malloc(sizeof(T) * n));
		} else {
			return static_cast<T*>(mi_aligned_alloc(alignof(T), sizeof(T) * n));
		}
	}
	void deallocate(T* p, size_t) const noexcept {
		mi_free(p);
	}
	void deallocate(void* p, size_t) const noexcept {
		mi_free(p);
	}
	template<typename R>
	[[nodiscard]] constexpr auto operator==(allocator<R>) const noexcept -> bool {
		return std::is_same_v<T, R>;
	}
};