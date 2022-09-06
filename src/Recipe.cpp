#include "Recipe.hpp"
#include <stdexcept>
#include <fstream>

Recipe::Recipe(const std::string& filename) {
	load(filename);
}

std::vector<std::string> Recipe::line_to_words(const std::string& line) {
	std::vector<std::string> words;

	char prev = 0;
	for(const auto& ch : line) {
		if(ch == '\n' || ch == 0) break;

		if(prev == 0) words.push_back(std::string(1, ch));
		else if(ch != ' ' && prev != ' ') words.back() += ch;
		else if(ch != ' ' && prev == ' ') words.push_back(std::string(1, ch));
		prev = ch;
	}

	return words;
}

std::optional<sf::Color> Recipe::string_to_color(const std::string& str) {
	if(str == "black") return sf::Color::Black;
	if(str == "white") return sf::Color::White;
	if(str == "red") return sf::Color::Red;
	if(str == "green") return sf::Color::Green;
	if(str == "blue") return sf::Color::Blue;
	if(str == "yellow") return sf::Color::Yellow;
	if(str == "magenta") return sf::Color::Magenta;
	if(str == "cyan") return sf::Color::Cyan;
	return std::nullopt;
}

std::optional<int> Recipe::stoi(const std::string& str) {
	try {
		std::size_t index = 0;
		int res = std::stoi(str, &index);
		if(index != str.size()) return std::nullopt;
		return res;
	} catch(std::invalid_argument&) {
		return std::nullopt;
	}
}

std::optional<float> Recipe::stof(const std::string& str) {
	try {
		std::size_t index = 0;
		float res = std::stof(str, &index);
		if(index != str.size()) return std::nullopt;
		return res;
	} catch(std::invalid_argument&) {
		return std::nullopt;
	}
}

std::optional<Recipe::Step> Recipe::load_friction(const std::vector<std::string>& words) {
	if(words.size() != 2) {
		errors += "Invalid number of arguments for `friction` (1 expected)\n";
		return std::nullopt;
	}

	auto maybe_value = stof(words[1]);
	if(!maybe_value.has_value()) {
		errors += std::string("\"") + words[1] + "\" is not a floating point number\n";
		return std::nullopt;
	}

	float value = maybe_value.value();

	return Friction { value };
}

std::optional<Recipe::Step> Recipe::load_particles(const std::vector<std::string>& words) {
	if(words.size() != 3) {
		errors += "Invalid number of arguments for `particles` (2 expected)\n";
		return std::nullopt;
	}

	auto maybe_color = string_to_color(words[1]);
	if(!maybe_color.has_value()) {
		errors += std::string("\"") + words[1] + "\" is not a valid color\n";
		return std::nullopt;
	}
	sf::Color color = maybe_color.value();

	auto maybe_amount = stoi(words[2]);
	if(!maybe_amount.has_value()) {
		errors += std::string("\"") + words[2] + "\" is not a valid integer number\n";
		return std::nullopt;
	}
	int amount = maybe_amount.value();

	return Particles { color, amount };
}

std::optional<Recipe::Step> Recipe::load_rule(const std::vector<std::string>& words) {
	if(words.size() != 6) {
		errors += "Invalid number of arguments for `rule` (5 expected)\n";
		return std::nullopt;
	}

	auto maybe_color = string_to_color(words[1]);
	if(!maybe_color.has_value()) {
		errors += std::string("\"") + words[1] + "\" is not a valid color\n";
		return std::nullopt;
	}
	sf::Color color1 = maybe_color.value();

	maybe_color = string_to_color(words[2]);
	if(!maybe_color.has_value()) {
		errors += std::string("\"") + words[2] + "\" is not a valid color\n";
		return std::nullopt;
	}
	sf::Color color2 = maybe_color.value();

	auto maybe_value = stof(words[3]);
	if(!maybe_value.has_value()) {
		errors += std::string("\"") + words[3] + "\" is not a floating point number\n";
		return std::nullopt;
	}
	float first_cut = maybe_value.value();

	maybe_value = stof(words[4]);
	if(!maybe_value.has_value()) {
		errors += std::string("\"") + words[4] + "\" is not a floating point number\n";
		return std::nullopt;
	}
	float second_cut = maybe_value.value();

	maybe_value = stof(words[5]);
	if(!maybe_value.has_value()) {
		errors += std::string("\"") + words[5] + "\" is not a floating point number\n";
		return std::nullopt;
	}
	float peak = maybe_value.value();

	return Rule { color1, color2, first_cut, second_cut, peak };
}

std::optional<Recipe::Step> Recipe::load_window(const std::vector<std::string>& words) {
	if(words.size() != 3) {
		errors += "Invalid number of arguments for `window` (2 expected)\n";
		return std::nullopt;
	}

	auto maybe_value = stoi(words[1]);
	if(!maybe_value.has_value()) {
		errors += std::string("\"") + words[1] + "\" is not a floating point number\n";
		return std::nullopt;
	}
	int width = maybe_value.value();

	maybe_value = stoi(words[2]);
	if(!maybe_value.has_value()) {
		errors += std::string("\"") + words[2] + "\" is not a floating point number\n";
		return std::nullopt;
	}
	int height = maybe_value.value();

	return Window { width, height };
}

const std::string& Recipe::load(const std::string& filename) {
	errors = "";
	steps.clear();

	std::ifstream file(filename);
	if(!file.is_open()) {
		errors += std::string("Can't open file: ") + filename + "\n";
		return errors;
	}

	bool first = true;

	std::string line_buffer;
	while(std::getline(file, line_buffer)) {
		if(line_buffer[0] == '#') continue;

		auto words = line_to_words(line_buffer);
		if(words.empty()) continue;

		if(first && words[0] != "window") {
			errors += "Recipe must start with a `window` command\n";
			break;
		} else if(!first && words[0] == "window") {
			errors += "Recipe must only have one `window` command\n";
			break;
		}

		first = false;
		std::optional<Step> maybe_step = std::nullopt;
		if(words[0] == "window") maybe_step = load_window(words);
		else if(words[0] == "friction") maybe_step = load_friction(words);
		else if(words[0] == "particles") maybe_step = load_particles(words);
		else if(words[0] == "rule") maybe_step = load_rule(words);
		else {
			errors += std::string("Unknown command \"") + words[0] + "\"";
			continue;
		}

		if(maybe_step.has_value()) steps.push_back(maybe_step.value());
	}

	file.close();
	return errors;
}

const std::string& Recipe::get_errors() const {
	return errors;
}

const std::vector<Recipe::Step>& Recipe::get_steps() const {
	return steps;
}
