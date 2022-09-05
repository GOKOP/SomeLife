#include <chrono>
#include <iostream>
#include "Display.hpp"
#include "Simulation.hpp"
#include "Config.hpp"

using namespace std::chrono;

int main(int argc, char* argv[]) {
	if(argc != 2) {
		std::cout << "Usage:\n";
		std::cout << argv[0] << " path-to-recipe-file\n";
		return 1;
	}

	Config config("somelife.conf");
	if(!config.get_errors().empty()) {
		std::cout << "Problems reading config:\n";
		std::cout << config.get_errors() << "\n";
		std::cout << "Config in use:\n";
		std::cout << "target_fps=" << config.get_target_fps() << "\n";
		std::cout << "threads=" << config.get_threads() << "\n\n";
	}

	auto recipe = Recipe(argv[1]);
	if(!recipe.get_errors().empty()) {
		std::cout << "Error loading \"" << argv[1] << "\":\n";
		std::cout << recipe.get_errors();
		return 1;
	}

	Simulation simulation(recipe, config.get_threads());
	Display display(
			simulation.get_board_size().x,
			simulation.get_board_size().y,
			"Life?",
			config.get_target_fps());

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
		display.draw_window(simulation.get_particles(), framerate);
	}
}
