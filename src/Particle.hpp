#pragma once

#include <SFML/Graphics.hpp>

struct Particle {
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Color color;

	Particle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color);
};

bool operator==(const Particle& left, const Particle& right);
bool operator!=(const Particle& left, const Particle& right);
std::ostream& operator<<(std::ostream& out, const Particle& particle); // binary output
