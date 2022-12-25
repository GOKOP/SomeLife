#include "Simulation.hpp"
#include <random>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

Simulation::Simulation(const Recipe& recipe):
	store1({0, 0}, 1), store2({0, 0}, 1)
{
	init_from_recipe(recipe);
	init_opencl();
}

void Simulation::init_from_recipe(const Recipe& recipe) {
	friction = 0;

	for(const auto& step : recipe.get_steps()) {
		if(std::holds_alternative<Recipe::Window>(step)) {
			auto window = std::get<Recipe::Window>(step);
			board_size.x = window.width;
			board_size.y = window.height;

			store1 = ParticleStore(board_size, grid_resolution);
			store2 = ParticleStore(board_size, grid_resolution);
		}
		else if(std::holds_alternative<Recipe::Friction>(step)) {
			auto friction_struct = std::get<Recipe::Friction>(step);
			friction = friction_struct.value;
		}
		else if(std::holds_alternative<Recipe::Particles>(step)) {
			auto parts = std::get<Recipe::Particles>(step);
			add_random_particles(parts.amount, parts.color);
		}
		else if(std::holds_alternative<Rule>(step)) {
			auto rule = std::get<Rule>(step);
			add_rule(rule);
		}
	}
}

cl::Program Simulation::read_opencl_program(std::string_view file, const cl::Device& device) {
	std::ifstream input(file.data());
	std::stringstream ss;
	ss << input.rdbuf();
	std::string source = ss.str();
	return clutils::log_create_program(context, device, source);
}

void Simulation::init_opencl() {
	old_store().overwrite_data_from_other(new_store());

	auto device = clutils::log_get_default_device();
	context = clutils::log_create_context(device);
	command_queue = clutils::log_create_command_queue(context);

	auto simulation_program = read_opencl_program("res/opencl/simulation.cl", device);
	auto particle_grid_program = read_opencl_program("res/opencl/particle-grid.cl", device);
	kernel = clutils::log_create_kernel(simulation_program, opencl_kernel_name);

	old_store().init_opencl(context, command_queue, particle_grid_program);
	new_store().init_opencl(context, command_queue, particle_grid_program);
	rule_store.init_buffers_with_rules(context, command_queue);

	clutils::log_kernel_setarg(kernel, 6, static_cast<cl_int>(new_store().size()));
	clutils::log_kernel_setarg(kernel, 7, rule_store.get_first_cuts());
	clutils::log_kernel_setarg(kernel, 8, rule_store.get_second_cuts());
	clutils::log_kernel_setarg(kernel, 9, rule_store.get_peaks());
	clutils::log_kernel_setarg(kernel, 10, rule_store.get_colors1());
	clutils::log_kernel_setarg(kernel, 11, rule_store.get_colors2());
	clutils::log_kernel_setarg(kernel, 12, static_cast<cl_int>(rule_store.get_rule_count()));
	clutils::log_kernel_setarg(kernel, 13, board_size);
	clutils::log_kernel_setarg(kernel, 14, friction);


	// make the global work size a multiple of preferred local work size
	// this prevents OpenCL from doing something silly
	// like choosing local work size of 1 for odd particle count
	
	std::size_t preferred_size = clutils::log_get_kernel_workgroup_info<std::size_t>(kernel, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE);
	if(new_store().size() % preferred_size == 0) global_work_size = new_store().size();
	else global_work_size = new_store().size() + preferred_size - new_store().size() % preferred_size;
}

void Simulation::add_particle(const Particle& particle) {
	if(particle.position.x > 0 && particle.position.y > 0 &&
	   particle.position.x < board_size.x && particle.position.y < board_size.y) {
		new_store().add_particle(particle);
	}
}

void Simulation::add_random_particles(int amount, sf::Color color) {
	std::random_device random_dev;
	auto seed = random_dev();

	std::default_random_engine eng(seed);
	auto x_dist = std::uniform_real_distribution<float>(0, board_size.x);
	auto y_dist = std::uniform_real_distribution<float>(0, board_size.y);

	for(int i=0; i<amount; ++i) {
		add_particle(Particle{{x_dist(eng), y_dist(eng)}, {0, 0}, {color.r, color.g, color.b}});
	}
}

void Simulation::update() {
	store1_is_new = !store1_is_new;

	clutils::log_kernel_setarg(kernel, 0, old_store().positions);
	clutils::log_kernel_setarg(kernel, 1, old_store().velocities);
	clutils::log_kernel_setarg(kernel, 2, old_store().colors);
	clutils::log_kernel_setarg(kernel, 3, new_store().positions);
	clutils::log_kernel_setarg(kernel, 4, new_store().velocities);
	clutils::log_kernel_setarg(kernel, 5, new_store().colors);

	clutils::log_enqueue_ndrange_kernel(command_queue, kernel, global_work_size);
	new_store().regenerate_grid(command_queue);
	new_store().update_particles_from_buffers(command_queue);
}

void Simulation::init_recording(std::ofstream& out) const {
	std::size_t particle_count = new_store().size();
	out.write(reinterpret_cast<const char*>(&board_size.x), sizeof(cl_int));
	out.write(reinterpret_cast<const char*>(&board_size.y), sizeof(cl_int));
	out.write(reinterpret_cast<const char*>(&particle_count), sizeof(cl_int));
}

void Simulation::record(std::ofstream& out) const {
	for(auto particle : new_store()) {
		if(std::endian::native == std::endian::big) {
			// convert to little endian (not tested)
			auto* ptr = reinterpret_cast<const char*>(&particle);
			for(int i = sizeof(Particle) - 1; i >= 0; --i) {
				out.write(ptr + i, 1);
			}
		} else {
			out.write(reinterpret_cast<const char*>(&particle), sizeof(Particle));
		}
	}
}
