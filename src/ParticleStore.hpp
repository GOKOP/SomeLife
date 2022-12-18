#pragma once

#include <vector>
#include <span>
#include "opencl-utils.hpp"
#include "Particle.hpp"

// Note that this isn't an equivalent to the ParticleStore struct in the OpenCL C code
// init_buffers_with_particles should be called between `add_particle()` and use

class ParticleStore {
	std::vector<Particle> particles; // for reference on the CPU side

public:
	// these are just public because they're needed as read-write to pass them to the GPU
	cl::Buffer positions;
	cl::Buffer velocities;
	cl::Buffer colors;

	inline const std::vector<Particle>& get_particles() const { return particles; }
	inline void add_particle(const Particle& particle) { particles.push_back(particle); }
	inline void overwrite_vector(const std::vector<Particle> new_vec) { particles = new_vec; }

	void init_buffers_with_particles(const cl::Context& context, const cl::CommandQueue& queue);
	void update_particles_from_buffers(const cl::CommandQueue& queue);
};
