#pragma once

#include <fstream>
#include <vector>
#include <string_view>
#include "Particle.hpp"

class Replayer {
	std::ifstream file_input;
	std::vector<Particle> particles;
	sf::Vector2i board_size;

public:

	Replayer(std::string_view recording_file);

	bool is_good() const;
	const std::vector<Particle>& get_particles() const;
	const sf::Vector2i& get_board_size() const;

	void next_frame();
};
