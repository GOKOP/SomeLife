#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <string>
#include <SFML/Graphics.hpp>
#include "Rule.hpp"

class Recipe {
public:
	struct Window { int width; int height; };
	struct Friction { float value; };
	struct Particles { sf::Color color; int amount; };

	using Step = std::variant<Window, Friction, Particles, Rule>;

private:
	std::vector<Step> steps;
	std::string errors;

	std::vector<std::string> line_to_words(const std::string& line);
	std::optional<sf::Color> string_to_color(const std::string& str);

	std::optional<Step> load_window(const std::vector<std::string>& words);
	std::optional<Step> load_friction(const std::vector<std::string>& words);
	std::optional<Step> load_particles(const std::vector<std::string>& words);
	std::optional<Step> load_rule(const std::vector<std::string>& words);

public:
	Recipe(const std::string& filename);

	const std::string& load(const std::string& filename);
	const std::string& get_errors() const;
	const std::vector<Step>& get_steps() const;
};
