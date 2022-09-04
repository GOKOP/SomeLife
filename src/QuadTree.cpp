#include "QuadTree.hpp"

QuadTree::QuadTree(int level, sf::FloatRect bounds, std::size_t max_elems, int max_level, QuadTree* parent):
	max_elems(max_elems),
	max_level(max_level),
	parent(parent),
	level(level),
	bounds(bounds),
	children{nullptr, nullptr, nullptr, nullptr}
{}

QuadTree::~QuadTree() {
	clear();
}

QuadTree QuadTree::create_with_same_params() const {
	return QuadTree(level, bounds, max_elems, max_level, parent);
}

void QuadTree::move_root_to(QuadTree& new_root) {
	if(parent != nullptr) return; // not root

	new_root.clear();
	new_root.max_elems = max_elems;
	new_root.max_level = max_level;
	new_root.parent = nullptr;
	new_root.level = level;
	new_root.bounds = bounds;
	new_root.elems = elems;
	new_root.children = children;

	for(auto node : new_root.children) {
		if(node != nullptr) node->parent = &new_root;
	}

	children = { nullptr, nullptr, nullptr, nullptr };
}

void QuadTree::clear() {
	elems.clear();

	for(auto& node : children) {
		if(node == nullptr) continue;
		node->clear();
		delete node;
		node = nullptr;
	}
}

void QuadTree::split() {
	if(children[0] != nullptr) return;

	float width = bounds.width / 2;
	float height = bounds.height / 2;
	float x = bounds.left;
	float y = bounds.top;

	children[0] = new QuadTree(level+1, {x + width, y, width, height }, max_elems, max_level, this);
	children[1] = new QuadTree(level+1, {x + width, y + height, width, height }, max_elems, max_level, this);
	children[2] = new QuadTree(level+1, {x, y + height, width, height }, max_elems, max_level, this);
	children[3] = new QuadTree(level+1, {x, y, width, height }, max_elems, max_level, this);
		
	for(const auto& elem : elems) {
		children[get_quadrant(elem.position)]->insert(elem);
	}

	elems.clear();
}

int QuadTree::get_quadrant(sf::Vector2f point) const {
	// 3 0
	// 2 1

	float v_middle = bounds.left + bounds.width / 2;
	float h_middle = bounds.top + bounds.height / 2;

	if(point.x < v_middle && point.y < h_middle) return 3;
	if(point.x < v_middle && point.y >= h_middle) return 2;
	if(point.x >= v_middle && point.y < h_middle) return 0;
	if(point.x >= v_middle && point.y >= h_middle) return 1;
	return -1; // should never happen
}

const std::vector<Particle>& QuadTree::get_elems() const {
	return elems;
}

void QuadTree::insert(const Particle& particle) {
	if((elems.size() < max_elems || level == max_level) && children[0] == nullptr) {
		elems.push_back(particle);
		return;
	}

	split();
	children[get_quadrant(particle.position)]->insert(particle);
}

void QuadTree::flatten() {
	if(children[0] == nullptr) return;

	for(auto& node : children) {
		node->flatten();
		elems.insert(elems.end(), node->begin(), node->end());
		delete node;
		node = nullptr;
	}
}

void QuadTree::remove(const Particle& particle) {
	if(children[0] == nullptr) {
		auto ref = std::find(elems.begin(), elems.end(), particle);
		if(ref != elems.end()) elems.erase(ref);
		return;
	}

	children[get_quadrant(particle.position)]->remove(particle);

	auto elem_count = get_total_elem_count();

	if(elem_count <= max_elems) flatten();
}

std::size_t QuadTree::get_total_elem_count() const {
	auto result = elems.size();
	for(const auto node : children) {
		if(node == nullptr) continue;
		result += node->get_total_elem_count();
	}
	return result;
}

const QuadTree& QuadTree::get_leaf_at(sf::Vector2f point) const {
	if(children[0] == nullptr) return *this;
	return children[get_quadrant(point)]->get_leaf_at(point);
}

std::vector<const QuadTree*> QuadTree::get_leaves_in(sf::FloatRect area) const {
	if(!bounds.intersects(area)) return {};

	std::vector<const QuadTree*> result = { this };
	for(const auto node : children) {
		if(node == nullptr) continue;
		auto node_leaves = node->get_leaves_in(area);
		result.insert(result.end(), node_leaves.begin(), node_leaves.end());
	}

	return result;
}

auto QuadTree::begin() const -> Iterator {
	const QuadTree* node = this;
	while(node->children[0] != nullptr) {
		node = node->children[0];
	}

	auto iter = Iterator(node, node->elems.cbegin(), level);
	if(node->elems.empty()) return ++iter;
	return iter;
}

auto QuadTree::end() const -> Iterator {
	return Iterator(nullptr, Iterator::vec_iter_t(), level);
}

QuadTree::Iterator::Iterator(const QuadTree* node_ptr, vec_iter_t vec_iter, int top_level):
	node_ptr(node_ptr),
	vec_iter(vec_iter),
	top_level(top_level)
{}

auto QuadTree::Iterator::operator*() const -> reference {
	return *vec_iter;
}

auto QuadTree::Iterator::operator->() -> pointer {
	return &(*vec_iter);
}

auto QuadTree::Iterator::get_node_index() -> int {
	if(node_ptr->parent == nullptr) return -1;

	int i;
	for(i = 0; i < 4; ++i) {
		if(node_ptr->parent->children[i] == node_ptr) break;
	}

	return i;
}

auto QuadTree::Iterator::operator++() -> Iterator& {
	if(node_ptr == nullptr) return *this;

	if(vec_iter != node_ptr->elems.cend()) ++vec_iter;

	if(vec_iter == node_ptr->elems.cend()) {

		// traverse the tree up until at the first node that's not a fourth child
		int own_index = get_node_index();
		while(node_ptr != nullptr && own_index == 3 && node_ptr->level > top_level) {
			node_ptr = node_ptr->parent;
			own_index = get_node_index();
		}

		// if that node is the root node then iteration is over
		if(own_index == -1 || node_ptr == nullptr || node_ptr->level <= top_level) {
			node_ptr = nullptr;
			return *this;
		}

		// enter the next child tree of parent and go to the leftmost element
		node_ptr = node_ptr->parent->children[own_index + 1];
		while(node_ptr->children[0] != nullptr) {
			node_ptr = node_ptr->children[0];
		}

		// set the vector iterator and if the node is empty, repeat to find a non-empty one
		// or the end of the tree
		vec_iter = node_ptr->elems.cbegin();
		if(node_ptr->elems.empty()) return ++(*this);
	}

	return *this;
}

auto QuadTree::Iterator::operator++(int) -> Iterator {
	auto tmp = *this;
	++(*this);
	return tmp;
}

auto operator==(const QuadTree::Iterator& a, const QuadTree::Iterator& b) -> bool {
	if(a.node_ptr == nullptr && b.node_ptr == nullptr) return true;
	if(a.node_ptr == b.node_ptr && a.vec_iter == b.vec_iter) return true;
	return false;
}

auto operator !=(const QuadTree::Iterator& a, const QuadTree::Iterator& b) -> bool {
	return !(a == b);
}
