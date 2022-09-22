#include "Simulation.hpp"
#include <random>
#include <cmath>
#include <omp.h>

float lerp(float x, float y, float where) {
	return where * (y - x) + x;
}

void float_bandaid(float& val) {
	if(std::isnan(val) || std::isinf(val)) val = 0;
}

Simulation::Simulation(const Recipe& recipe, int threads):
	particles({0, 0}, 1)
{
	if(threads != 0) omp_set_num_threads(threads);

	for(const auto& step : recipe.get_steps()) {
		if(std::holds_alternative<Recipe::Window>(step)) {
			auto window = std::get<Recipe::Window>(step);
			board_size.x = window.width;
			board_size.y = window.height;
			particles = ParticleGrid({window.width, window.height}, 30);
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

	particles.init_new_with_old();
}

const ParticleGrid& Simulation::get_particles() const {
	return particles;
}

const sf::Vector2i Simulation::get_board_size() const {
	return board_size;
}

void Simulation::add_particle(const Particle& particle) {
	if(particle.position.x > 0 && particle.position.y > 0 &&
	   particle.position.x < board_size.x && particle.position.y < board_size.y) {
		particles.insert(particle);
	}
}

void Simulation::add_random_particles(int amount, sf::Color color) {
	std::random_device random_dev;
	auto seed = random_dev();

	std::default_random_engine eng(seed);
	auto x_dist = std::uniform_real_distribution<float>(0, board_size.x);
	auto y_dist = std::uniform_real_distribution<float>(0, board_size.y);

	for(int i=0; i<amount; ++i) {
		add_particle(Particle({x_dist(eng), y_dist(eng)}, {0, 0}, color));
	}
}

void Simulation::add_rule(const Rule& rule) {
	rules.push_back(rule);
}

float Simulation::calculate_force(const Rule& rule, float distance) {
	float large_value = 1;

	if(distance > rule.second_cut) return 0;

	if(distance < rule.first_cut) {
		float val = (rule.first_cut / distance) - 1 + rule.peak;
		if(val > large_value || std::isnan(val) || std::isinf(val)) return large_value;
		else return val;
	}

	if(rule.second_cut == rule.first_cut) return 0;
	return lerp(rule.peak, 0, distance / (rule.second_cut - rule.first_cut));
}

void Simulation::execute_rule(const Rule& rule, Particle& particle1, const Particle& particle2) {
	float distance_x = particle1.position.x - particle2.position.x;
	float distance_y = particle1.position.y - particle2.position.y;
	float distance = std::sqrt(distance_x*distance_x + distance_y*distance_y);
	if(distance == 0) return;

	float normalized_x = distance_x / distance;
	float normalized_y = distance_y / distance;

	float force = calculate_force(rule, distance);
	float force_x = force * normalized_x;
	float force_y = force * normalized_y;

	particle1.velocity.x += force_x;
	particle1.velocity.y += force_y;
}

sf::Vector2f Simulation::apply_friction(sf::Vector2f velocity) {
	velocity.x *= (1.f - friction);
	velocity.y *= (1.f - friction);
	return velocity;
}

void Simulation::perform_movement(Particle& particle) {
	particle.velocity = apply_friction(particle.velocity);

	float new_x = particle.position.x + particle.velocity.x;
	float new_y = particle.position.y + particle.velocity.y;

	if(new_x < 0 || new_x >= board_size.x) particle.velocity.x *= -0.5;
	else particle.position.x = new_x;

	if(new_y < 0 || new_y >= board_size.y) particle.velocity.y *= -0.5;
	else particle.position.y = new_y;
}

void Simulation::fix_particle(Particle& particle) {
	float_bandaid(particle.position.x);
	float_bandaid(particle.position.y);
	float_bandaid(particle.velocity.x);
	float_bandaid(particle.velocity.y);
}

void Simulation::update() {
	const auto& old_particles = particles.get_particles();
	auto& new_particles = particles.get_mut_new_particles();

	#pragma omp parallel for
	for(int i=0; i<new_particles.size(); ++i) {
		auto& particle1 = new_particles[i];
		particle1 = old_particles[i];

		for(const auto& rule : rules) {
			if(rule.particle1_color != particle1.color) continue;

			auto relevant_area = sf::FloatRect(
					particle1.position.x - rule.second_cut,
					particle1.position.y - rule.second_cut,
					rule.second_cut * 2,
					rule.second_cut * 2);

			auto ranges = particles.get_ranges_in(relevant_area);
			for(const auto range : ranges) {
				for(int j = range.first; j < range.second; ++j) {
					const auto& particle2 = old_particles[j];
					if(particle1 == particle2) continue;
					if(rule.particle2_color != particle2.color) continue;
					execute_rule(rule, particle1, particle2);
				}
			}
		}

		perform_movement(particle1);
	}

	particles.swap_vecs();
	particles.sort();
}
