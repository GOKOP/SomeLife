#include "ParticleGrid.hpp"

ParticleGrid::ParticleGrid(sf::Vector2i window_size, int cell_resolution):
	grid_size(cell_resolution, cell_resolution),
	cell_size(window_size.x / cell_resolution, window_size.y / cell_resolution),
	cells(grid_size.x * grid_size.y, Cell{})
{}

const ParticleGrid::Cell& ParticleGrid::get_cell_at(sf::Vector2f pos) const {
	int cell_x = pos.x / cell_size.x;
	int cell_y = pos.y / cell_size.y;
	return cells[cell_y * grid_size.x + cell_x];
}

std::vector<ParticleGrid::Cell> ParticleGrid::get_cells_in(sf::FloatRect area) const {
	std::vector<Cell>  result;

	int x = 0;
	int y = 0;
	for(int i=0; i<cells.size(); ++i) {
		sf::FloatRect box(x, y, x + cell_size.x, y + cell_size.y);
		if(box.intersects(area)) result.push_back(cells[i]);

		++x;
		if(x >= grid_size.x) {
			x = 0;
			++y;
		}
	}

	return result;
}

int ParticleGrid::get_total_particle_count() const {
	int count = 0;
	for(const auto& cell : cells) {
		count += cell.size();
	}
	return count;
}

void ParticleGrid::insert(const Particle& particle) {
	int cell_x = particle.position.x / cell_size.x;
	int cell_y = particle.position.y / cell_size.y;
	if(cell_x < 0 || cell_x >= grid_size.x || cell_y < 0 || cell_y >= grid_size.y) return;

	cells[cell_y * grid_size.x + cell_x].push_back(particle);
}

void ParticleGrid::remove(const Particle& particle) {
	int cell_x = particle.position.x / cell_size.x;
	int cell_y = particle.position.y / cell_size.y;
	if(cell_x < 0 || cell_x >= grid_size.x || cell_y < 0 || cell_y >= grid_size.y) return;

	auto& vec = cells[cell_y * grid_size.x + cell_x];
	auto it = std::find(vec.begin(), vec.end(), particle);
	vec.erase(it);
}

ParticleGrid::Iterator ParticleGrid::begin() const {
	return Iterator(cells, cells.cbegin(), cells.cbegin()->cbegin());
}

ParticleGrid::Iterator ParticleGrid::end() const {
	return Iterator(cells, cells.cend(), Iterator::part_iter_t());
}

ParticleGrid::Iterator::Iterator(
		const std::vector<Cell>& cell_vec,
		cell_iter_t cell_iter, 
		part_iter_t particle_iter):
	cell_vec(cell_vec),
	cell_iter(cell_iter),
	particle_iter(particle_iter)
{}

const Particle& ParticleGrid::Iterator::operator*() const {
	return *particle_iter;
}

const Particle* ParticleGrid::Iterator::operator->() {
	return &(*particle_iter);
}

ParticleGrid::Iterator& ParticleGrid::Iterator::operator++() {
	++particle_iter;

	while(particle_iter == cell_iter->cend()) {
		++cell_iter;
		if(cell_iter == cell_vec.cend()) return *this;

		particle_iter = cell_iter->cbegin();
	}

	return *this;
}

ParticleGrid::Iterator ParticleGrid::Iterator::operator++(int) {
	auto temp = *this;
	++(*this);
	return temp;
}

bool operator==(const ParticleGrid::Iterator& a, const ParticleGrid::Iterator& b) {
	if(
			a.cell_vec == b.cell_vec &&
			a.cell_iter == a.cell_vec.cend() &&
			b.cell_iter == b.cell_vec.cend()) {
		return true;
	}

	return a.cell_iter == b.cell_iter && a.particle_iter == b.particle_iter;
}

bool operator!=(const ParticleGrid::Iterator& a, const ParticleGrid::Iterator& b) {
	return !(a == b);
}
