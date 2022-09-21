#include "ParticleGrid.hpp"
#include <cmath>

ParticleGrid::ParticleGrid(sf::Vector2i window_size, int cell_resolution):
	grid_size(cell_resolution, cell_resolution),
	cell_size(window_size.x / cell_resolution, window_size.y / cell_resolution),
	cell_positions((grid_size.x * grid_size.y), 0),
	compare(grid_size.x, cell_size)
{}

const std::vector<Particle>& ParticleGrid::get_particles() const {
	return particles;
}

void ParticleGrid::insert(const Particle& particle) {
	auto it = particles.begin();
	for(it; it != particles.end(); ++it) {
		if(compare(particle, *it)) break;
	}

	particles.insert(it, particle);

	int cell_x = floor(particle.position.x / cell_size.x);
	int cell_y = floor(particle.position.y / cell_size.y);
	int cell_n = cell_y * grid_size.x + cell_x;

	for(int i=0; i < cell_positions.size(); ++i) {
		if(i > cell_n) ++cell_positions[i];
	}
}

std::vector<std::pair<std::size_t, std::size_t>> ParticleGrid::get_ranges_in(
		sf::FloatRect area
) const {
	float y_start = floor(area.top / cell_size.y) * cell_size.y;
	int y_cells = ceil((area.height + area.top - y_start) / cell_size.y);
	float y_end = y_start + y_cells * cell_size.y;

	float x_start = floor(area.left / cell_size.x) * cell_size.x;
	float x_cells = ceil((area.width + area.left - x_start) / cell_size.x);
	float x_end = x_start + x_cells * cell_size.x;
	
	std::vector<std::pair<std::size_t, std::size_t>> res(y_cells, {0, 0});

	float previous_y = -1;
	int range_i = 0;


	for(int i=0; i<cell_positions.size(); ++i) {
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
	if(range_i >= 0 && range_i < res.size() && res[range_i].second == 0) {
		res[range_i].second = particles.size();
	}

	return res;
}

void ParticleGrid::remove(const Particle& particle) {
	auto it = std::find(particles.begin(), particles.end(), particle);
	particles.erase(it);

	int cell_x = floor(particle.position.x / cell_size.x);
	int cell_y = floor(particle.position.y / cell_size.y);
	int cell_n = cell_y * grid_size.x + cell_x;

	for(int i=0; i < cell_positions.size(); ++i) {
		if(i > cell_n) --cell_positions[i];
	}
}

void ParticleGrid::sort() {
	std::sort(particles.begin(), particles.end(), compare);
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
