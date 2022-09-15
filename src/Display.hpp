#pragma once

#include <SFML/Graphics.hpp>
#include "ParticleGrid.hpp"

#define POINT_RADIUS 2


class Display {
	sf::RenderWindow window;
	sf::Font font;
	float point_radius;

	void draw_point(sf::Vector2f pos, sf::Color color);
	void print_framerate(int framerate);

public:
	Display(int width, int height, std::string title, int framerate);

	const sf::RenderWindow& get_window() const;
	bool window_is_open() const;

	void draw_window(const ParticleGrid& particles, int framerate);
	void handle_events();
};
