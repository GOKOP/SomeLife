#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <utility>
#include <condition_variable>
#include "ParticleGrid.hpp"
#include "Recipe.hpp"

class Simulation {
	float friction;
	sf::Vector2i board_size;
	std::vector<Rule> rules;
	ParticleGrid particles;

	void add_particle(const Particle& particle);
	void add_random_particles(int amount, sf::Color color);
	void add_rule(const Rule& rule);

	float calculate_force(const Rule& rule, float distance);
	sf::Vector2f apply_friction(sf::Vector2f velocity);
	void execute_rule(const Rule& rule, Particle& particle1, const Particle& particle2);
	void perform_movement(Particle& particle);
	void fix_particle(Particle& particle);

public:
	Simulation(const Recipe& recipe, int threads);

	const ParticleGrid& get_particles() const;
	const sf::Vector2i get_board_size() const;

	void update();
	void init_recording(std::ofstream& out) const;
	void record(std::ofstream& out) const;
};
