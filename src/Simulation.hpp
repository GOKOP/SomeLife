#pragma once

#include <vector>
#include <string>
#include <utility>
#include "Recipe.hpp"
#include "OpenClRunner.hpp"

class Simulation {
	bool cpu_is_big_endian; // for recording
	sf::Vector2i board_size;
	std::vector<Rule> rules;
	std::vector<Particle> particles;
	OpenClRunner runner;

	void add_particle(const Particle& particle);
	void add_random_particles(int amount, sf::Color color);
	void add_rule(const Rule& rule);

public:
	Simulation(const Recipe& recipe, bool cpu_is_big_endian);

	const std::vector<Particle>& get_particles() const;
	const sf::Vector2i get_board_size() const;

	void update();
	void init_recording(std::ofstream& out) const;
	void record(std::ofstream& out) const;
};
