#pragma once

#include <vector>
#include <utility>
#include <SFML/System.hpp>
#include "Particle.hpp"

class ParticleGrid {
	struct CompareByGridCell {
		int grid_width;
		sf::Vector2f cell_size;
		CompareByGridCell(int grid_width, sf::Vector2f cell_size);
		bool operator()(const Particle& p1, const Particle& p2);
	};

	sf::Vector2i grid_size;
	sf::Vector2f cell_size;

	std::vector<std::size_t> cell_positions;
	std::vector<Particle> particles;

	CompareByGridCell compare;

public:

	ParticleGrid(sf::Vector2i window_size, int cell_resolution);

	const std::vector<Particle>& get_particles() const;
	std::vector<std::pair<std::size_t, std::size_t>> get_ranges_in(sf::FloatRect area) const;

	void insert(const Particle& particle);
	void remove(const Particle& particle);
	void sort();
};
