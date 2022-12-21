#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <ParticleStore.hpp>

#define POINT_RADIUS 2
// points are haxagons and require six triangles; each triangle requires three vertices
// all particles are drawn as a single vertex array to save on draw calls
#define POINT_VERTICES 18

class Display {
	sf::RenderWindow window;
	sf::Font font;
	float point_radius;
	std::vector<sf::Vertex> point_vertices; // put here to only allocate once

	std::array<sf::Vertex, POINT_VERTICES> make_point_vertices(sf::Vector2f pos, sf::Color color);
	void print_framerate(int framerate);

public:
	Display(int width, int height, std::string title, int framerate, std::size_t particle_count);

	const sf::RenderWindow& get_window() const;
	bool window_is_open() const;

	void draw_window(const std::vector<Particle>& particles, int framerate);
	void draw_window(const ParticleStore& particles, int framerate);
	void handle_events();

};
