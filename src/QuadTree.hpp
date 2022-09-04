#pragma once

#include <vector>
#include <array>
#include <iterator>
#include <SFML/Graphics.hpp>
#include "Particle.hpp"

class QuadTree {
	std::size_t max_elems;
	int max_level;

	QuadTree* parent;
	int level;
	sf::FloatRect bounds;

	// if elems is not empty, children should be {nullptr, nullptr, nullptr, nullptr}
	// if children are allocated, elems should be empty
	std::array<QuadTree*, 4> children;
	std::vector<Particle> elems;

	void split();
	int get_quadrant(sf::Vector2f point) const;
	void flatten();

public:
	QuadTree(int level, sf::FloatRect bounds, std::size_t max_elems=10, int max_level=5, QuadTree* parent=nullptr);
	~QuadTree();

	void clear();
	void insert(const Particle& particle);
	void remove(const Particle& particle);

	const std::vector<Particle>& get_elems() const;
	std::size_t get_total_elem_count() const;
	const QuadTree& get_leaf_at(sf::Vector2f point) const;
	std::vector<const QuadTree*> get_leaves_in(sf::FloatRect area) const;

	QuadTree create_with_same_params() const;
	void move_root_to(QuadTree& new_root);
	int get_node_index() const;


	// iterator stuff

	struct Iterator {
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = Particle;
		using pointer = const Particle*;
		using reference = const Particle&;

		using vec_iter_t = std::vector<value_type>::const_iterator;

		Iterator(const QuadTree* node_ptr, vec_iter_t vec_iter, int top_level);

		reference operator*() const;
		pointer operator->();
		Iterator& operator++();
		Iterator operator++(int);
		friend bool operator==(const Iterator& a, const Iterator& b);
		friend bool operator!=(const Iterator& a, const Iterator& b);

	private:
		int get_node_index();

		const QuadTree* node_ptr;
		vec_iter_t vec_iter;
		int top_level;
	};

	Iterator begin() const;
	Iterator end() const;
};
