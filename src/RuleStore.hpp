#pragma once

#include "opencl-utils.hpp"
#include "Rule.hpp"

// see comment in ParticleStore.hpp

class RuleStore {
	std::vector<Rule> rules;
	cl::Buffer colors1;
	cl::Buffer colors2;
	cl::Buffer first_cuts;
	cl::Buffer second_cuts;
	cl::Buffer peaks;

public:

	inline void add_rule(const Rule& rule) { rules.push_back(rule); }
	void init_buffers_with_rules(const cl::Context& context, const cl::CommandQueue& queue);

	inline const cl::Buffer& get_colors1() const { return colors1; }
	inline const cl::Buffer& get_colors2() const { return colors2; }
	inline const cl::Buffer& get_first_cuts() const { return first_cuts; }
	inline const cl::Buffer& get_second_cuts() const { return second_cuts; }
	inline const cl::Buffer& get_peaks() const { return peaks; }

	inline std::size_t get_rule_count() const { return rules.size(); }
};
