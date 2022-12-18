#include "ParticleStore.hpp"
#include <ranges>

void ParticleStore::init_buffers_with_particles(const cl::Context& context, const cl::CommandQueue& queue) {
	positions = clutils::log_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float2) * particles.size());
	velocities = clutils::log_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float2) * particles.size());
	colors = clutils::log_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_uchar3) * particles.size());

	std::vector<cl_float2> float2s;
	std::ranges::transform(particles, std::back_inserter(float2s),
		[](const Particle& p) -> cl_float2 { return { p.position.x, p.position.y }; });
	
	clutils::log_copy(queue, float2s, positions);

	std::ranges::transform(particles, float2s.begin(),
		[](const Particle& p) -> cl_float2 { return { p.velocity.x, p.velocity.y }; });

	clutils::log_copy(queue, float2s, velocities);

	std::vector<cl_uchar3> uchar3s;
	std::ranges::transform(particles, std::back_inserter(uchar3s),
		[](const Particle& p) -> cl_uchar3 { return { p.color.r, p.color.g, p.color.b }; });

	clutils::log_copy(queue, uchar3s, colors);
}

void ParticleStore::update_particles_from_buffers(const cl::CommandQueue& queue) {
	std::vector<cl_float2> positions_vec(particles.size());
	std::vector<cl_float2> velocities_vec(particles.size());
	std::vector<cl_uchar3> colors_vec(particles.size());

	auto colors_vec2 = colors_vec;

	clutils::log_copy(queue, positions, positions_vec);
	clutils::log_copy(queue, velocities, velocities_vec);
	clutils::log_copy(queue, colors, colors_vec);

	for(std::size_t i = 0; i < particles.size(); ++i) {
		auto& pos = positions_vec[i];
		auto& vel = velocities_vec[i];
		auto& col = colors_vec[i];
		particles[i] = { { pos.x, pos.y }, { vel.x, vel.y }, { col.x, col.y, col.z } };
	}
}
