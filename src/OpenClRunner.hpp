#pragma once

#include <string_view>
#include "Particle.hpp"
#include "Rule.hpp"

class OpenClRunner {
	cl::Context context;
	cl::CommandQueue command_queue;
	cl::Program program;
	cl::Kernel kernel;

	cl::Buffer buffer1;
	cl::Buffer buffer2;
	cl::Buffer rule_buffer;
	bool buffer1_is_new;

	cl::NDRange global_ndrange;

	inline cl::Buffer& get_new_buffer() { if(buffer1_is_new) return buffer1; else return buffer2; }
	inline cl::Buffer& get_old_buffer() { if(buffer1_is_new) return buffer2; else return buffer1; }

public:

	OpenClRunner(std::string_view program_file, std::string_view function_name);
	void init_data(const std::vector<Particle>& particles, const std::vector<Rule>& rules, cl_int2 board_size, cl_float friction);
	void run_and_fetch(std::vector<Particle>& result);
};
