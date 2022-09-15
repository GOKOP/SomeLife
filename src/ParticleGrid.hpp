#pragma once

#include <vector>
#include <SFML/System.hpp>
#include "Particle.hpp"

class ParticleGrid {
	using Cell = std::vector<Particle>;

	sf::Vector2f grid_size;
	sf::Vector2f cell_size;

	std::vector<Cell> cells;

public:

	ParticleGrid(sf::Vector2i window_size, int cell_resolution);

	const Cell& get_cell_at(sf::Vector2f pos) const;
	std::vector<Cell> get_cells_in(sf::FloatRect area) const;
	int get_total_particle_count() const;

	void insert(const Particle& particle);
	void remove(const Particle& particle);

	// iterator stuff

	struct Iterator {
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = Particle;
		using pointer = const Particle*;
		using reference = const Particle&;

		using part_iter_t = std::vector<value_type>::const_iterator;
		using cell_iter_t = std::vector<Cell>::const_iterator;

		Iterator(const std::vector<Cell>& cell_vec, cell_iter_t cell_iter, part_iter_t particle_iter);

		reference operator*() const;
		pointer operator->();
		Iterator& operator++();
		Iterator operator++(int);
		friend bool operator==(const Iterator& a, const Iterator& b);
		friend bool operator!=(const Iterator& a, const Iterator& b);

	private:
		const std::vector<Cell>& cell_vec;
		cell_iter_t cell_iter;
		part_iter_t particle_iter;
	};

	Iterator begin() const;
	Iterator end() const;
};
