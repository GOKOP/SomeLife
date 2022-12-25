#pragma once

#include <vector>
#include <string>
#include <utility>
#include "Recipe.hpp"
#include "ParticleStore.hpp"
#include "RuleStore.hpp"

class Simulation {
	const std::string opencl_file_name = "res/opencl/simulation.cl";
	const std::string opencl_kernel_name = "update_particle";
	const int grid_resolution = 30;

	cl_int2 board_size;
	cl_float friction;

	cl::Context context;
	cl::CommandQueue command_queue;
	cl::Kernel kernel;
	cl::NDRange global_work_size;

	RuleStore rule_store;
	ParticleStore store1;
	ParticleStore store2;
	bool store1_is_new = true;

	inline ParticleStore& new_store() { return store1_is_new ? store1 : store2; };
	inline ParticleStore& old_store() { return store1_is_new ? store2 : store1; };
	inline const ParticleStore& new_store() const { return store1_is_new ? store1 : store2; };
	inline const ParticleStore& old_store() const { return store1_is_new ? store2 : store1; };

	void add_particle(const Particle& particle);
	inline void add_rule(const Rule& rule) { rule_store.add_rule(rule); }
	void add_random_particles(int amount, sf::Color color);
	cl::Program read_opencl_program(std::string_view file, const cl::Device& device);

	void init_from_recipe(const Recipe& recipe);
	void init_opencl();

public:
	Simulation(const Recipe& recipe);

	inline const ParticleStore& get_particle_store() const { return new_store(); }
	inline const cl_int2 get_board_size() const { return board_size; }

	void update();
	void init_recording(std::ofstream& out) const;
	void record(std::ofstream& out) const;
};
