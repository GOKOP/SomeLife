#include "Particle.hpp"
#include <iostream>

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

std::ostream& operator<<(std::ostream& out, const Particle& particle) {
	out.write(reinterpret_cast<const char*>(&particle.position.x), sizeof(float));
	out.write(reinterpret_cast<const char*>(&particle.position.y), sizeof(float));
	out.write(reinterpret_cast<const char*>(&particle.velocity.x), sizeof(float));
	out.write(reinterpret_cast<const char*>(&particle.velocity.y), sizeof(float));

	out.write(reinterpret_cast<const char*>(&particle.color.r), sizeof(sf::Uint8));
	out.write(reinterpret_cast<const char*>(&particle.color.g), sizeof(sf::Uint8));
	out.write(reinterpret_cast<const char*>(&particle.color.b), sizeof(sf::Uint8));

	return out;
}
