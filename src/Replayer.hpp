#pragma once

#include <fstream>
#include <vector>
#include <string_view>
#include "opencl-utils.hpp"
#include "Particle.hpp"

class Replayer {
	std::ifstream file_input;
	std::vector<Particle> particles;
	cl_int2 board_size;

public:

	Replayer(std::string_view recording_file);

	bool is_good() const;
	const std::vector<Particle>& get_particles() const;
	cl_int2 get_board_size() const;

	void next_frame();
};
