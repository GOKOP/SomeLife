#include "Display.hpp"

Display::Display(int width, int height, std::string title, int framerate, std::size_t particle_count):
	window(sf::RenderWindow(
				sf::VideoMode(width, height), 
				title, 
				sf::Style::Close,
				sf::ContextSettings(0, 0, 8)
	))
{
	if(framerate > 0) window.setFramerateLimit(framerate);
	font.loadFromFile("res/DejaVuSans.ttf");

	// each particle is drawn as a hexagon (to save on vertices)
	// and thus requires six triangles; each triangle requires three vertices
	// 6 * 3 = 18
	point_vertices.resize(18 * particle_count);
}

const sf::RenderWindow& Display::get_window() const {
	return window;
}

bool Display::window_is_open() const {
	return window.isOpen();
}

std::array<sf::Vertex, POINT_VERTICES> Display::make_point_vertices(sf::Vector2f pos, sf::Color color) {
	const float sqrt_3 = 1.73205;
	float offset_x = POINT_RADIUS / 2.0;
	float offset_y = (POINT_RADIUS * sqrt_3) / 2;

	return {
		sf::Vertex(pos, color),
		sf::Vertex({pos.x - offset_x, pos.y - offset_y}, color),
		sf::Vertex({pos.x + offset_x, pos.y - offset_y}, color),
		sf::Vertex(pos, color),
		sf::Vertex({pos.x + offset_x, pos.y - offset_y}, color),
		sf::Vertex({pos.x + POINT_RADIUS, pos.y}, color),
		sf::Vertex(pos, color),
		sf::Vertex({pos.x + POINT_RADIUS, pos.y}, color),
		sf::Vertex({pos.x + offset_x, pos.y + offset_y}, color),
		sf::Vertex(pos, color),
		sf::Vertex({pos.x + offset_x, pos.y + offset_y}, color),
		sf::Vertex({pos.x - offset_x, pos.y + offset_y}, color),
		sf::Vertex(pos, color),
		sf::Vertex({pos.x - offset_x, pos.y + offset_y}, color),
		sf::Vertex({pos.x - POINT_RADIUS, pos.y}, color),
		sf::Vertex(pos, color),
		sf::Vertex({pos.x - POINT_RADIUS, pos.y}, color),
		sf::Vertex({pos.x - offset_x, pos.y - offset_y}, color)
	};
}

void Display::print_framerate(int framerate) {
	sf::Text text(sf::String(std::to_string(framerate) + " FPS"), font, 15);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(2);
	text.setPosition(5, 5);
	window.draw(text);
}

void Display::draw_window(const std::vector<Particle>& particles, int framerate) {
	window.clear(sf::Color::Black);

	std::size_t vertices_i = 0;

	for(const auto& particle : particles) {
		auto vertices = make_point_vertices(
			{ particle.position.x, particle.position.y },
			{ particle.color.x, particle.color.y, particle.color.z });

		for(const auto& vertex : vertices) {
			point_vertices[vertices_i] = vertex;
			++vertices_i;
		}
	}

	window.draw(point_vertices.data(), point_vertices.size(), sf::Triangles);
	print_framerate(framerate);
	window.display();
}

void Display::draw_window(const ParticleStore& particles, int framerate) {
	window.clear(sf::Color::Black);

	std::size_t vertices_i = 0;

	for(const auto& particle : particles) {
		auto vertices = make_point_vertices(
			{ particle.position.x, particle.position.y },
			{ particle.color.x, particle.color.y, particle.color.z });

		for(const auto& vertex : vertices) {
			point_vertices[vertices_i] = vertex;
			++vertices_i;
		}
	}

	window.draw(point_vertices.data(), point_vertices.size(), sf::Triangles);
	print_framerate(framerate);
	window.display();
}

void Display::handle_events() {
	sf::Event event;
	while(window.pollEvent(event)) {
		if(event.type == sf::Event::Closed) window.close();
	}
}
