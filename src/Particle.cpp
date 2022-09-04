#include "Particle.hpp"

Particle::Particle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color):
	position(position),
	velocity(velocity),
	color(color)
{}

bool operator==(const Particle& left, const Particle& right) {
	return 
		left.position == right.position &&
		left.velocity == right.velocity &&
		left.color == right.color;
}

bool operator!=(const Particle& left, const Particle& right) {
	return !(left == right);
}
