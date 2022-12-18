#pragma once

#include <vector>
#include <string>
#include <utility>
#include "Recipe.hpp"
#include "ParticleStore.hpp"
#include "RuleStore.hpp"

class Simulation {
	const std::string opencl_file_name = "simulation.cl";
	const std::string opencl_kernel_name = "update_particle";

	bool cpu_is_big_endian; // for recording
	cl_int2 board_size;
	cl_float friction;

	cl::Context context;
	cl::CommandQueue command_queue;
	cl::Program program;
	cl::Kernel kernel;

	RuleStore rule_store;
	ParticleStore store1;
	ParticleStore store2;
	bool store1_is_new;

	inline ParticleStore& get_new_store() { return store1_is_new ? store1 : store2; };
	inline const ParticleStore& get_new_store() const { return store1_is_new ? store1 : store2; };
	inline ParticleStore& get_old_store() { return store1_is_new ? store2 : store1; };
	inline const ParticleStore& get_old_store() const { return store1_is_new ? store2 : store1; };

	void add_particle(const Particle& particle);
	inline void add_rule(const Rule& rule) { rule_store.add_rule(rule); }
	void add_random_particles(int amount, sf::Color color);

	void init_from_recipe(const Recipe& recipe);
	void init_opencl();

public:
	Simulation(const Recipe& recipe, bool cpu_is_big_endian);

	inline const std::vector<Particle>& get_particles() const { return get_new_store().get_particles(); }
	inline const cl_int2 get_board_size() const { return board_size; }

	void update();
	void init_recording(std::ofstream& out) const;
	void record(std::ofstream& out) const;
};
