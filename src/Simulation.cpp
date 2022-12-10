#include "Simulation.hpp"
#include <random>
#include <cmath>
#include <fstream>

Simulation::Simulation(const Recipe& recipe, bool cpu_is_big_endian):
	cpu_is_big_endian(cpu_is_big_endian),
	runner("simulation.cl", "update_particle")
{
	cl_float friction = 0;

	for(const auto& step : recipe.get_steps()) {
		if(std::holds_alternative<Recipe::Window>(step)) {
			auto window = std::get<Recipe::Window>(step);
			board_size.x = window.width;
			board_size.y = window.height;
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

	runner.init_data(particles, rules, {{board_size.x, board_size.y}}, friction);
}

const std::vector<Particle>& Simulation::get_particles() const {
	return particles;
}

const sf::Vector2i Simulation::get_board_size() const {
	return board_size;
}

void Simulation::add_particle(const Particle& particle) {
	if(particle.position.x > 0 && particle.position.y > 0 &&
	   particle.position.x < board_size.x && particle.position.y < board_size.y) {
		particles.push_back(particle);
	}
}

void Simulation::add_random_particles(int amount, sf::Color color) {
	std::random_device random_dev;
	auto seed = random_dev();

	std::default_random_engine eng(seed);
	auto x_dist = std::uniform_real_distribution<float>(0, board_size.x);
	auto y_dist = std::uniform_real_distribution<float>(0, board_size.y);

	cl_uchar3 cl_color {{color.r, color.g, color.b}};

	for(int i=0; i<amount; ++i) {
		add_particle(Particle{{{x_dist(eng), y_dist(eng)}}, {{0, 0}}, cl_color});
	}
}

void Simulation::add_rule(const Rule& rule) {
	rules.push_back(rule);
}

void Simulation::update() {
	runner.run_and_fetch(particles);
}

void Simulation::init_recording(std::ofstream& out) const {
	std::size_t particle_count = particles.size();
	out.write(reinterpret_cast<const char*>(&board_size.x), sizeof(sf::Int32));
	out.write(reinterpret_cast<const char*>(&board_size.y), sizeof(sf::Int32));
	out.write(reinterpret_cast<const char*>(&particle_count), sizeof(sf::Int32));
}

void Simulation::record(std::ofstream& out) const {
	for(const auto& particle : particles) {
		if(cpu_is_big_endian) {
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
