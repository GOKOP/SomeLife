#pragma once

#include <string>
#include <utility>
#include <optional>

class Config {
	const int default_fps = 60;
	const int default_threads = 8;

	int target_fps;
	int threads;
	std::string errors;

	std::pair<std::string, std::string> line_to_keyvalue(const std::string& line);

public:

	Config();
	Config(const std::string& filename);

	int get_target_fps() const;
	int get_threads() const;
	const std::string& get_errors() const;
};
