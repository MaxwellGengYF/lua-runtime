#pragma once
#include <filesystem>
#include "std.h"
namespace lua {
inline string to_string(std::filesystem::path const& path) {
	return path.string<char, std::char_traits<char>, allocator<char>>();
}
}// namespace lua