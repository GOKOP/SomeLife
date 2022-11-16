#include <optional>
#include <string_view>

namespace strutil {
	std::optional<int> stoi(std::string_view str);
	std::optional<float> stof(std::string_view str);
	std::optional<int> stoi_positive(std::string_view str);
	std::optional<int> stoi_nonegative(std::string_view str);
}
