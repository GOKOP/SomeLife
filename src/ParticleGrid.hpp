#pragma once

#include <vector>
#include <utility>
#include <SFML/System.hpp>
#include "Particle.hpp"

/* Ok so it's actually a bit weird grid implementation;
 * all particles are stored in a single vector which is sorted
 * based on what grid cell does a particle belong to.
 * Positions in that vector where the grid cells start are stored in another vector.
 * This is certainly weird but when I was designing it I hoped it would somehow enable me to
 * leverage the GPU to do the work of updating the particles.
 * Now I'm not sure if it does, but I didn't look into it too much yet so I'm leaving it
 * just in case it will turn out useful when I do.
 * It's certainly better performing than my terrible quad tree implementation tho
 */

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

	// one of them represents previous state
	// while the other one is meant to be updated based on it
	std::vector<Particle> particles1;
	std::vector<Particle> particles2;
	bool p1_is_new;

	CompareByGridCell compare;

	std::vector<Particle>& get_mut_particles();

public:

	ParticleGrid(sf::Vector2i window_size, int cell_resolution);

	const std::vector<Particle>& get_particles() const;
	const std::vector<Particle>& get_new_particles() const;
	std::vector<Particle>& get_mut_new_particles();
	std::vector<std::pair<std::size_t, std::size_t>> get_ranges_in(sf::FloatRect area) const;

	void insert(const Particle& particle);
	void remove(const Particle& particle);
	void sort();
	void swap_vecs();
	void init_new_with_old();
};
