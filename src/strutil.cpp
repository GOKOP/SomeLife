#include "strutil.hpp"
#include <charconv>

namespace strutil {
	std::optional<int> stoi(std::string_view str) {
		int value;
		auto result = std::from_chars(str.begin(), str.end(), value);
		if(result.ec != std::errc() || result.ptr != str.end()) return {};
		else return value;
	}

	std::optional<float> stof(std::string_view str) {
		float value;
		auto result = std::from_chars(str.begin(), str.end(), value);
		if(result.ec != std::errc() || result.ptr != str.end()) return {};
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
