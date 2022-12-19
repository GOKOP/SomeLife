#pragma once

#include <vector>
#include <span>
#include "opencl-utils.hpp"
#include "Particle.hpp"

// Note that this isn't an equivalent to the ParticleStore struct in the OpenCL C code
// Note that `init_buffers_with_particles()` should be called between `add_particle()` and use

class ParticleStore {
	// for reference on the CPU side
	std::vector<cl_float2> positions_cpu;
	std::vector<cl_float2> velocities_cpu;
	std::vector<cl_uchar3> colors_cpu;

public:
	// these are just public because they're needed as read-write to pass them to the GPU
	cl::Buffer positions;
	cl::Buffer velocities;
	cl::Buffer colors;

	inline cl_float2 get_position(int idx) const { return positions_cpu[idx]; }
	inline cl_float2 get_velocity(int idx) const { return velocities_cpu[idx]; }
	inline cl_uchar3 get_color(int idx) const { return colors_cpu[idx]; }
	inline std::size_t get_particle_count() const { return positions_cpu.size(); }

	void add_particle(const Particle& particle);
	void overwrite_data_from_other(const ParticleStore& other);

	void init_buffers_with_particles(const cl::Context& context, const cl::CommandQueue& queue);
	void update_particles_from_buffers(const cl::CommandQueue& queue);
};
