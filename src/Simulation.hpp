#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "QuadTree.hpp"
#include "Recipe.hpp"

class Simulation {

	struct Params {
		float friction;
		sf::Vector2i board_size;
		std::vector<Rule> rules;
	};

	// for threading; `run()` is supposed to be launched in a separate thread
	// writing to any field should be preceeded by acquiring a lock on `mutex`
	// `doing_work` should be set to true from outside the thread
	struct ParticleUpdater {
		std::vector<Particle> particle_vec1;
		std::vector<Particle> particle_vec2;
		std::vector<Particle>* old_particles;
		std::vector<Particle>* new_particles;

		std::atomic<bool> doing_work;
		std::atomic<bool> program_finished;
		const Params& params;

		std::mutex mutex;
		std::condition_variable waiter;

		ParticleUpdater(const Params& params);

		void swap_vectors();
		float calculate_force(const Rule& rule, float distance);
		sf::Vector2f apply_friction(sf::Vector2f velocity);
		void execute_rule(const Rule& rule, Particle& particle1, const Particle& particle2);
		void perform_movement(Particle& particle);
		void fix_particle(Particle& particle);
		void run(const QuadTree* particles);
	};

	Params params;
	QuadTree particles;
	std::vector<ParticleUpdater*> updaters;

	void add_particle(const Particle& particle);
	void add_random_particles(int amount, sf::Color color);
	void add_rule(const Rule& rule);
	void assign_particles();

public:
	Simulation(const Recipe& recipe, int threads);
	~Simulation();

	const QuadTree& get_particles() const;
	const sf::Vector2i get_board_size() const;

	void update();
};
