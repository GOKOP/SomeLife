#include "ParticleStore.hpp"

void ParticleStore::add_particle(const Particle& particle) {
	positions_cpu.push_back(particle.position);
	velocities_cpu.push_back(particle.velocity);
	colors_cpu.push_back(particle.color);
}

void ParticleStore::overwrite_data_from_other(const ParticleStore& other) {
	positions_cpu = other.positions_cpu;
	velocities_cpu = other.velocities_cpu;
	colors_cpu = other.colors_cpu;
}

void ParticleStore::init_buffers_with_particles(const cl::Context& context, const cl::CommandQueue& queue) {
	positions = clutils::log_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float2) * positions_cpu.size());
	velocities = clutils::log_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float2) * positions_cpu.size());
	colors = clutils::log_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_uchar3) * positions_cpu.size());

	clutils::log_copy(queue, positions_cpu, positions);
	clutils::log_copy(queue, velocities_cpu, velocities);
	clutils::log_copy(queue, colors_cpu, colors);
}

void ParticleStore::update_particles_from_buffers(const cl::CommandQueue& queue) {
	clutils::log_copy(queue, positions, positions_cpu);
	clutils::log_copy(queue, velocities, velocities_cpu);
	clutils::log_copy(queue, colors, colors_cpu);
}
