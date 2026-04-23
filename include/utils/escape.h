#pragma once
#include <string>

template <typename Mapper>
std::string escapeText(const std::string &text, Mapper &&mapper) {
	std::string result;
	result.reserve(text.size());
	for (char c : text) {
		std::string_view escaped = mapper(c);
		if (escaped.empty()) {
			result += c;
		} else {
			result += escaped;
		}
	}
	return result;
}