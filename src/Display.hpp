#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <ParticleStore.hpp>

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

	void draw_window(const std::vector<Particle>& particles, int framerate);
	void draw_window(const ParticleStore& particles, int framerate);
	void handle_events();

};
