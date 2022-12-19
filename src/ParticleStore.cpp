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


// iterator

auto ParticleStore::begin() const -> ConstIterator {
	return ConstIterator(positions_cpu.begin(), velocities_cpu.begin(), colors_cpu.begin());
}

auto ParticleStore::end() const -> ConstIterator {
	return ConstIterator(positions_cpu.end(), velocities_cpu.end(), colors_cpu.end());
}

auto ParticleStore::ConstIterator::operator*() -> Particle {
	return { *pos_iter, *vel_iter, *color_iter };
}

auto ParticleStore::ConstIterator::operator++() -> ConstIterator& {
	++pos_iter;
	++vel_iter;
	++color_iter;
	return *this;
}

auto ParticleStore::ConstIterator::operator--() -> ConstIterator& {
	--pos_iter;
	--vel_iter;
	--color_iter;
	return *this;
}

auto ParticleStore::ConstIterator::operator++(int) -> ConstIterator {
	auto temp = *this;
	++(*this);
	return temp;
}

auto ParticleStore::ConstIterator::operator--(int) -> ConstIterator {
	auto temp = *this;
	--(*this);
	return temp;
}

auto ParticleStore::ConstIterator::operator+=(int i) -> ConstIterator& {
	pos_iter += i;
	vel_iter += i;
	color_iter += i;
	return *this;
}

auto ParticleStore::ConstIterator::operator-=(int i) -> ConstIterator& {
	pos_iter -= i;
	vel_iter -= i;
	color_iter -= i;
	return *this;
}

auto ParticleStore::ConstIterator::operator+(int i) -> ConstIterator {
	auto copied = *this;
	return copied += i;
}

auto ParticleStore::ConstIterator::operator-(int i) -> ConstIterator {
	auto copied = *this;
	return copied -= i;
}

bool operator==(const ParticleStore::ConstIterator& lhs, const ParticleStore::ConstIterator& rhs) {
	return lhs.pos_iter == rhs.pos_iter && lhs.vel_iter == rhs.vel_iter && lhs.color_iter == rhs.color_iter;
}

bool operator!=(const ParticleStore::ConstIterator& lhs, const ParticleStore::ConstIterator& rhs) {
	return !(lhs == rhs);
}
