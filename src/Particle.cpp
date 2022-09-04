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

bool operator<(const Particle& left, const Particle& right) {
	if(left.position.x < right.position.x) return true;
	if(left.position.x > right.position.x) return false;

	if(left.position.y < right.position.y) return true;
	if(left.position.y > right.position.y) return false;

	if(left.velocity.x < right.velocity.x) return true;
	if(left.velocity.x > right.velocity.x) return false;

	if(left.velocity.y < right.velocity.y) return true;
	if(left.velocity.y > right.velocity.y) return false;

	if(left.color.r < right.color.r) return true;
	if(left.color.r > right.color.r) return false;

	if(left.color.g < right.color.g) return true;
	if(left.color.g > right.color.g) return false;

	if(left.color.b < right.color.b) return true;
	if(left.color.b > right.color.b) return false;

	if(left.color.a < right.color.a) return true;
	if(left.color.a > right.color.a) return false;

	return false;
}
