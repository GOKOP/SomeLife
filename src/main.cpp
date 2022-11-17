#include <chrono>
#include <iostream>
#include <fstream>
#include "Display.hpp"
#include "Simulation.hpp"
#include "Config.hpp"
#include "ArgumentConfig.hpp"
#include "Replayer.hpp"

using namespace std::chrono;

bool run_simulation(const Config& config, const ArgumentConfig& arg_config, int target_fps) {
	auto recipe = Recipe(arg_config.get_recipe_path());
	if(!recipe.get_errors().empty()) {
		std::cout << "Error loading \"" << arg_config.get_recipe_path() << "\":\n";
		std::cout << recipe.get_errors();
		return false;
	}

	Simulation simulation(recipe, config.get_threads());
	Display display(
			simulation.get_board_size().x,
			simulation.get_board_size().y,
			"Life?",
			target_fps);

	auto record_stream = std::ofstream();
	if(arg_config.get_recording_state() == ArgumentConfig::RecordingState::Recording) {
		record_stream.open(arg_config.get_recording_path().data(), std::ios::binary);
		if(record_stream.good()) simulation.init_recording(record_stream);
		else std::cout << "Failed to open file: " + std::string(arg_config.get_recording_path()) + "; cannot record the simulation.\n";
	}

	auto last_frame_time = steady_clock::now();

	while(display.window_is_open()) {
		display.handle_events();

		auto current_frame_time = steady_clock::now();
		auto delta_time = current_frame_time - last_frame_time;
		last_frame_time = current_frame_time;

		int framerate = 0;
		int delta_us = duration_cast<microseconds>(delta_time).count();
		if(delta_us != 0) framerate = 1000000 / delta_us;

		simulation.update();
		if(record_stream.is_open() && record_stream.good()) simulation.record(record_stream);
		display.draw_window(simulation.get_particles(), framerate);
	}

	return true;
}

bool run_replay(ArgumentConfig arg_config, int target_fps) {
	Replayer replayer(arg_config.get_recording_path());
	if(!replayer.is_good()) {
		std::cout << "Can't open file: " << arg_config.get_recording_path() << "\n";
		return false;
	}

	Display display(
			replayer.get_board_size().x,
			replayer.get_board_size().y,
			"Life?",
			target_fps);

	auto last_frame_time = std::chrono::steady_clock::now();

	while(display.window_is_open()) {
		display.handle_events();

		auto current_frame_time = steady_clock::now();
		auto delta_time = current_frame_time - last_frame_time;
		last_frame_time = current_frame_time;

		int framerate = 0;
		int delta_us = duration_cast<microseconds>(delta_time).count();
		if(delta_us != 0) framerate = 1000000 / delta_us;

		replayer.next_frame();
		display.draw_window(replayer.get_particles(), framerate);
	}

	return true;
}

int main(int argc, const char* argv[]) {
	ArgumentConfig arg_config(argc, argv);
	if(!arg_config.get_errors().empty()) {
		std::cout << "Errors reading command line arguments:\n";
		std::cout << arg_config.get_errors();
		exit(1);
	}

	Config config("res/somelife.conf");
	if(!config.get_errors().empty()) {
		std::cout << "Problems reading config:\n";
		std::cout << config.get_errors() << "\n";
		std::cout << "Config in use:\n";
		std::cout << "target_fps=" << config.get_target_fps() << "\n";
		std::cout << "threads=" << config.get_threads() << "\n\n";
	}

	int target_fps;
	if(arg_config.get_framerate() > 0) target_fps = arg_config.get_framerate();
	else target_fps = config.get_target_fps();

	bool success = true;

	if(arg_config.get_recording_state() == ArgumentConfig::RecordingState::Replaying) {
		success = run_replay(arg_config, target_fps);
	} else {
		success = run_simulation(config, arg_config, target_fps);
	}

	if(success) return EXIT_SUCCESS;
	return EXIT_FAILURE;
}
