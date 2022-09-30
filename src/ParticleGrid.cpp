#include "ParticleGrid.hpp"
#include <cmath>

ParticleGrid::ParticleGrid(sf::Vector2i window_size, int cell_resolution):
	grid_size(cell_resolution, cell_resolution),
	cell_size(window_size.x / cell_resolution, window_size.y / cell_resolution),
	cell_positions((grid_size.x * grid_size.y), 0),
	p1_is_new(false),
	compare(grid_size.x, cell_size)
{}

const std::vector<Particle>& ParticleGrid::get_particles() const {
	if(p1_is_new) return particles2;
	else return particles1;
}

const std::vector<Particle>& ParticleGrid::get_new_particles() const {
	if(p1_is_new) return particles1;
	else return particles2;
}

std::vector<Particle>& ParticleGrid::get_mut_particles() {
	if(p1_is_new) return particles2;
	else return particles1;
}

std::vector<Particle>& ParticleGrid::get_mut_new_particles() {
	if(p1_is_new) return particles1;
	else return particles2;
}

void ParticleGrid::insert(const Particle& particle) {
	auto& particles = get_mut_particles();

	auto it = particles.begin();
	for(; it != particles.end(); ++it) {
		if(compare(particle, *it)) break;
	}

	particles.insert(it, particle);

	int cell_x = floor(particle.position.x / cell_size.x);
	int cell_y = floor(particle.position.y / cell_size.y);
	std::size_t cell_n = cell_y * grid_size.x + cell_x;

	for(std::size_t i=0; i < cell_positions.size(); ++i) {
		if(i > cell_n) ++cell_positions[i];
	}
}

std::vector<std::pair<std::size_t, std::size_t>> ParticleGrid::get_ranges_in(
		sf::FloatRect area
) const {
	float y_start = floor(area.top / cell_size.y) * cell_size.y;
	std::size_t y_cells = ceil((area.height + area.top - y_start) / cell_size.y);

	float x_start = floor(area.left / cell_size.x) * cell_size.x;
	float x_cells = ceil((area.width + area.left - x_start) / cell_size.x);
	float x_end = x_start + x_cells * cell_size.x;
	
	std::vector<std::pair<std::size_t, std::size_t>> res(y_cells, {0, 0});

	float previous_y = -1;
	std::size_t range_i = 0;

	for(std::size_t i=0; i<cell_positions.size(); ++i) {
		float current_y = (i / grid_size.x) * cell_size.y;
		if(current_y < y_start) continue;

		float current_x = (i % grid_size.x) * cell_size.x;
		if(current_x >= x_start && current_x <= x_end) {
			if(previous_y == -1) res[range_i].first = cell_positions[i];
			else if(previous_y != current_y) {
				res[range_i].second = cell_positions[i];
				++range_i;
				if(range_i < y_cells) res[range_i].first = cell_positions[i];
				else break;
			}
			previous_y = current_y;
		}
	}
	if(range_i < res.size() && res[range_i].second == 0) {
		res[range_i].second = get_particles().size();
	}

	return res;
}

void ParticleGrid::remove(const Particle& particle) {
	auto& particles = get_mut_particles();

	auto it = std::find(particles.begin(), particles.end(), particle);
	particles.erase(it);

	int cell_x = floor(particle.position.x / cell_size.x);
	int cell_y = floor(particle.position.y / cell_size.y);
	std::size_t cell_n = cell_y * grid_size.x + cell_x;

	for(std::size_t i=0; i < cell_positions.size(); ++i) {
		if(i > cell_n) --cell_positions[i];
	}
}

void ParticleGrid::sort() {
	auto& particles = get_mut_particles();
	std::sort(particles.begin(), particles.end(), compare);

	// regenerate cell positions
	int prev_cell_ord = -1;
	for(std::size_t i=0; i<particles.size(); ++i) {
		int cell_x = particles[i].position.x / cell_size.x;
		int cell_y = particles[i].position.y / cell_size.y;
		int cell_ord = grid_size.x * cell_y + cell_x;

		if(cell_ord != prev_cell_ord) {
			for(int j = prev_cell_ord + 1; j <= cell_ord; ++j) {
				cell_positions[j] = i;
			}
		}

		prev_cell_ord = cell_ord;
	}

	for(std::size_t i = prev_cell_ord+1; i < cell_positions.size(); ++i) {
		cell_positions[i] = particles.size();
	}
}

void ParticleGrid::swap_vecs() {
	p1_is_new = !p1_is_new;
}

void ParticleGrid::init_new_with_old() {
	get_mut_new_particles() = get_particles();
}

ParticleGrid::CompareByGridCell::CompareByGridCell(int grid_width, sf::Vector2f cell_size):
	grid_width(grid_width),
	cell_size(cell_size)
{}

bool ParticleGrid::CompareByGridCell::operator()(const Particle& p1, const Particle& p2) {
	int cell1_x = p1.position.x / cell_size.x;
	int cell1_y = p1.position.y / cell_size.y;
	int cell1_ord = grid_width * cell1_y + cell1_x;

	int cell2_x = p2.position.x / cell_size.x;
	int cell2_y = p2.position.y / cell_size.y;
	int cell2_ord = grid_width * cell2_y + cell2_x;

	return cell1_ord < cell2_ord;
}
