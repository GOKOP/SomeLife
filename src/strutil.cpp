#include "strutil.hpp"
#include <charconv>

namespace strutil {
	std::optional<int> stoi(std::string_view str) {
		int value;

		// should really just be str.begin() and str.end()
		// but apparently from_chars will only work with char* on MSVC stdlib
		const char* begin = str.data();
		const char* end = str.data() + str.size();

		auto result = std::from_chars(begin, end, value);
		if(result.ec != std::errc() || result.ptr != end) return {};
		else return value;
	}

	std::optional<float> stof(std::string_view str) {
		float value;

		const char* begin = str.data();
		const char* end = str.data() + str.size();

		auto result = std::from_chars(begin, end, value);
		if(result.ec != std::errc() || result.ptr != end) return {};
		else return value;
	}

	std::optional<int> stoi_positive(std::string_view str) {
		auto result = stoi(str);
		if(!result.has_value()) return {};
		
		int value = result.value();
		if(value <= 0) return {};
		return value;
	}

	std::optional<int> stoi_nonegative(std::string_view str) {
		auto result = stoi(str);
		if(!result.has_value()) return {};
		
		int value = result.value();
		if(value < 0) return {};
		return value;
	}
}
