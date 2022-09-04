#pragma once

#include <SFML/Graphics.hpp>
#include "QuadTree.hpp"

#define POINT_RADIUS 2


class Display {
	sf::RenderWindow window;
	float point_radius;

	void draw_point(sf::Vector2f pos, sf::Color color);

public:
	Display(int width, int height, std::string title, int framerate);

	const sf::RenderWindow& get_window() const;
	bool window_is_open() const;

	void draw_window(const QuadTree& particles);
	void handle_events();
};
