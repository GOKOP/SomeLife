#include <chrono>
#include <iostream>
#include "Display.hpp"
#include "Simulation.hpp"

const int target_framerate = 60;
const int threads = 8;

template<class R, class P>
int as_milliseconds(std::chrono::duration<R, P> dur) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
}

int main(int argc, char* argv[]) {
	if(argc != 2) {
		std::cout << "Usage:\n";
		std::cout << argv[0] << " path-to-recipe-file\n";
		return -1;
	}

	auto recipe = Recipe(argv[1]);
	if(!recipe.get_errors().empty()) {
		std::cout << "Error loading \"" << argv[1] << "\":\n";
		std::cout << recipe.get_errors();
		return -1;
	}

	Simulation simulation(recipe, threads);
	Display display(
			simulation.get_board_size().x,
			simulation.get_board_size().y,
			"Life?",
			target_framerate);

	auto last_frame_time = std::chrono::steady_clock::now();

	while(display.window_is_open()) {
		display.handle_events();

		auto current_frame_time = std::chrono::steady_clock::now();
		auto delta_time = current_frame_time - last_frame_time;
		last_frame_time = current_frame_time;

		int framerate = 0;
		int delta_ms = as_milliseconds(delta_time);
		if(delta_ms != 0) framerate = 1000 / delta_ms;

		simulation.update();
		display.draw_window(simulation.get_particles(), framerate);
	}
}
