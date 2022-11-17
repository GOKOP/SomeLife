#include <chrono>
#include <iostream>
#include "Display.hpp"
#include "Simulation.hpp"
#include "Config.hpp"
#include "ArgumentConfig.hpp"

using namespace std::chrono;

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

	auto recipe = Recipe(arg_config.get_recipe_path());
	if(!recipe.get_errors().empty()) {
		std::cout << "Error loading \"" << argv[1] << "\":\n";
		std::cout << recipe.get_errors();
		return 1;
	}

	int target_fps;
	if(arg_config.get_framerate() > 0) target_fps = arg_config.get_framerate();
	else target_fps = config.get_target_fps();

	Simulation simulation(recipe, config.get_threads());
	Display display(
			simulation.get_board_size().x,
			simulation.get_board_size().y,
			"Life?",
			target_fps);

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
