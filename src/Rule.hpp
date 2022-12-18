#pragma once

#include <SFML/Graphics.hpp>

struct Rule {
	sf::Color particle1_color;
	sf::Color particle2_color;
	float first_cut;
	float second_cut;
	float peak;
};
