#include "Config.hpp"
#include <fstream>
#include <stdexcept>

#include "strutil.hpp"

Config::Config():
	target_fps(default_fps),
	threads(default_threads)
{}

Config::Config(const std::string& filename):
	Config()
{
	std::ifstream file(filename);
	if(!file.is_open()) {
		errors += std::string("Could not open file: ") + filename + "\n";
		return;
	}

	std::string line_buffer;
	while(std::getline(file, line_buffer)) {
		if(line_buffer[0] == '#') continue;
		if(line_buffer[0] == '\n') continue;
		if(line_buffer[0] == '\0') continue;

		auto keyval = line_to_keyvalue(line_buffer);

		if(keyval.first.empty() || keyval.second.empty()) {
			errors += std::string("Can't parse: ") + line_buffer;
			continue;
		}

		auto maybe_value = strutil::stoi_nonegative(keyval.second);
		if(!maybe_value.has_value()) {
			errors += std::string("Must be a non-negative integer: \"") + keyval.second + "\"\n";
			continue;
		}
		int value = maybe_value.value();

		if(keyval.first == "target_fps") target_fps = value;
		else if(keyval.first == "threads") threads = value;
		else errors += std::string("Unknown key: \"") + keyval.first + "\"\n";
	}
}

std::pair<std::string, std::string> Config::line_to_keyvalue(const std::string& line) {
	std::string key;
	std::string value;

	bool reading_key = true;
	for(const auto& ch : line) {
		if(ch == ' ') continue;
		if(ch == '=' && reading_key) reading_key = false;
		else if(reading_key) key += ch;
		else value += ch;
	}

	return {key, value};
}

int Config::get_target_fps() const {
	return target_fps;
}

int Config::get_threads() const {
	return threads;
}

const std::string& Config::get_errors() const {
	return errors;
}
