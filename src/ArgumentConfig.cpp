#include "ArgumentConfig.hpp"

#include <vector>
#include <charconv>

#include "strutil.hpp"

std::string_view ArgumentConfig::get_recipe_path() const {
	return recipe_path;
}

std::string_view ArgumentConfig::get_recording_path() const {
	return recording_path;
}

ArgumentConfig::RecordingState ArgumentConfig::get_recording_state() const {
	return recording_state;
}

int ArgumentConfig::get_framerate() const {
	return framerate;
}

std::string_view ArgumentConfig::get_errors() const {
	return errors;
}

ArgumentConfig::ArgumentConfig(int argc, const char** argv):
	recording_state(RecordingState::None),
	recipe_path(""),
	recording_path(""),
	framerate(-1),
	errors("")
{
	std::vector<std::string_view> args;
	for(int i=0; i<argc; ++i) {
		args.push_back(std::string_view(argv[i]));
	}

	auto recipe_result = read_option(args, "recipe");
	auto record_result = read_option(args, "record");
	auto replay_result = read_option(args, "replay");
	auto framerate_result = read_option(args, "framerate");

	// checking for conflicts

	if(recipe_result.has_value() && replay_result.has_value()) {
		errors += "Options `--recipe` and `--replay` cannot be combined\n";
	}

	if(record_result.has_value() && replay_result.has_value()) {
		errors += "Options `--record` and `--replay` cannot be combined\n";
	}

	if(!replay_result.has_value() && !recipe_result.has_value()) {
		errors += "Either `--recipe` or `--replay` option is required\n";
	}

	// applying values

	if(recipe_result.has_value()) recipe_path = recipe_result.value();

	if(record_result.has_value()) {
		recording_state = RecordingState::Recording;
		recording_path = record_result.value();
	}

	if(replay_result.has_value()) {
		recording_state = RecordingState::Replaying;
		recording_path = replay_result.value();
	}

	if(framerate_result.has_value()) {
		auto framerate_str = framerate_result.value();
		auto maybe_framerate = strutil::stoi_positive(framerate_str);
		if(maybe_framerate.has_value()) framerate = maybe_framerate.value();
		// this generates a warning on GCC with -std=c++20 and -O2
		//else errors += "`" + std::string(framerate_str) + "` is not a positive integer number\n";
		else {
			errors += "`";
			errors += framerate_str;
			errors += "is not a positive integer number\n";
		}
	}

	int option_number = 0;
	option_number += recipe_result.has_value() ? 1 : 0;
	option_number += record_result.has_value() ? 1 : 0;
	option_number += replay_result.has_value() ? 1 : 0;
	option_number += framerate_result.has_value() ? 1 : 0;

	int expected_args = (option_number * 2) + 1;

	if(argc != expected_args) {
		int redundant_args = argc - expected_args;
		errors += std::to_string(redundant_args) + " undesired arguments.\n";
	}
}

std::optional<std::string_view> ArgumentConfig::read_option(
		const std::vector<std::string_view>& args,
		std::string_view option_name)
{
	auto option_iter = args.begin();

	for(; option_iter != args.end(); ++option_iter) {
		auto arg = *option_iter;
		if(arg.substr(0, 2) != "--") continue;
		arg.remove_prefix(2);
		if(arg == option_name) break;
	}

	if(option_iter == args.end()) return {};

	auto next_iter = option_iter + 1;

	if(next_iter == args.end()) {
		errors += "Option `--" + std::string(option_name) + "` requires and argument.\n";
		return {};
	}
	
	return *next_iter;
}
