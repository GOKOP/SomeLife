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
	font.loadFromFile("res/DejaVuSans.ttf");
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

void Display::print_framerate(int framerate) {
	sf::Text text(sf::String(std::to_string(framerate) + " FPS"), font, 15);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(2);
	text.setPosition(5, 5);
	window.draw(text);
}

void Display::draw_window(const ParticleGrid& particles, int framerate) {
	draw_window(particles.get_particles(), framerate);
}

void Display::draw_window(const std::vector<Particle>& particles, int framerate) {
	window.clear(sf::Color::Black);

	for(const auto& particle : particles) {
		draw_point(particle.position, particle.color);
	}

	print_framerate(framerate);

	window.display();
}

void Display::handle_events() {
	sf::Event event;
	while(window.pollEvent(event)) {
		if(event.type == sf::Event::Closed) window.close();
	}
}
