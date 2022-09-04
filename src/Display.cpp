#include "Display.hpp"

Display::Display(int width, int height, std::string title, int framerate):
	window(sf::RenderWindow(
				sf::VideoMode(width, height), 
				title, 
				sf::Style::Close,
				sf::ContextSettings(0, 0, 8)
	))
{
	if(framerate > 0) window.setFramerateLimit(framerate);
}

const sf::RenderWindow& Display::get_window() const {
	return window;
}

bool Display::window_is_open() const {
	return window.isOpen();
}

void Display::draw_point(sf::Vector2f pos, sf::Color color) {
	auto circle = sf::CircleShape(POINT_RADIUS, 6);
	circle.setFillColor(color);
	circle.setOrigin(POINT_RADIUS, POINT_RADIUS);
	circle.setPosition(pos);
	window.draw(circle);
}

void Display::draw_window(const QuadTree& particles) {
	window.clear(sf::Color::Black);

	for(const auto& particle : particles) {
		draw_point(particle.position, particle.color);
	}

	window.display();
}

void Display::handle_events() {
	sf::Event event;
	while(window.pollEvent(event)) {
		if(event.type == sf::Event::Closed) window.close();
	}
}
