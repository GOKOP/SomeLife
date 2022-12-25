#include "ParticleStore.hpp"
#include <cmath>
#include <ranges>

namespace views = std::views;

ParticleStore::ParticleStore(cl_int2 window_size, int grid_resolution):
	cell_size {
		static_cast<float>(window_size.x) / grid_resolution,
		static_cast<float>(window_size.y) / grid_resolution
	},
	grid_size { grid_resolution, grid_resolution }
{}

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

void ParticleStore::pad_particles_to_pow2() {
	real_size = positions_cpu.size();

	float size_log = std::log2(real_size);
	int truncated_log = std::trunc(size_log);
	int target_pow = truncated_log;
	if(size_log != truncated_log) target_pow = truncated_log + 1;
	std::size_t padded_size = std::pow(2, target_pow);

	cl_float2 past_screen_pos { 
		cell_size.x * grid_size.x + cell_size.x + 100,
		cell_size.y * grid_size.y + cell_size.y + 100
	};

	for([[maybe_unused]]int i : views::iota(real_size, padded_size)) {
		positions_cpu.push_back(past_screen_pos);
		velocities_cpu.push_back({0,0});
		colors_cpu.push_back({0,0,0});
	}
}

void ParticleStore::init_buffers_with_particles(const cl::Context& context, const cl::CommandQueue& queue) {
	positions = clutils::log_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float2) * padded_size());
	velocities = clutils::log_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float2) * padded_size());
	colors = clutils::log_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_uchar3) * padded_size());
	cell_positions = clutils::log_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_int) * grid_size.x * grid_size.y);

	clutils::log_copy(queue, positions_cpu, positions);
	clutils::log_copy(queue, velocities_cpu, velocities);
	clutils::log_copy(queue, colors_cpu, colors);
}

void ParticleStore::update_particles_from_buffers(const cl::CommandQueue& queue) {
	clutils::log_copy(queue, positions, positions_cpu);
	clutils::log_copy(queue, velocities, velocities_cpu);
	clutils::log_copy(queue, colors, colors_cpu);
}

void ParticleStore::init_kernels(const cl::Program program) {
	sort_kernel = clutils::log_create_kernel(program, sort_kernel_name);
	clutils::log_kernel_setarg(sort_kernel, 0, positions);
	clutils::log_kernel_setarg(sort_kernel, 1, velocities);
	clutils::log_kernel_setarg(sort_kernel, 2, colors);
	clutils::log_kernel_setarg(sort_kernel, 3, static_cast<cl_int>(padded_size()));
	clutils::log_kernel_setarg(sort_kernel, 4, cell_size);
	clutils::log_kernel_setarg(sort_kernel, 5, grid_size.x);

	grid_kernel = clutils::log_create_kernel(program, grid_kernel_name);
	clutils::log_kernel_setarg(grid_kernel, 0, positions);
	clutils::log_kernel_setarg(grid_kernel, 1, velocities);
	clutils::log_kernel_setarg(grid_kernel, 2, colors);
	clutils::log_kernel_setarg(grid_kernel, 3, static_cast<cl_int>(size()));
	clutils::log_kernel_setarg(grid_kernel, 4, cell_positions);
	clutils::log_kernel_setarg(grid_kernel, 5, grid_size.x * grid_size.y);
	clutils::log_kernel_setarg(grid_kernel, 6, cell_size);
	clutils::log_kernel_setarg(grid_kernel, 7, grid_size.x);
}

void ParticleStore::sort_gpu_particles(const cl::CommandQueue& queue) {
	int stages = std::log2(padded_size());
	cl_int work_size = padded_size() / 2;

	for(cl_int stage : views::iota(0, stages)) {
		for(cl_int step : views::iota(0, stage + 1)) {
			clutils::log_kernel_setarg(sort_kernel, 6, stage);
			clutils::log_kernel_setarg(sort_kernel, 7, step);
			clutils::log_enqueue_ndrange_kernel(queue, sort_kernel, work_size);
		}
	}
}

void ParticleStore::regenerate_grid(const cl::CommandQueue& queue) {
	sort_gpu_particles(queue);
	clutils::log_enqueue_ndrange_kernel(queue, grid_kernel, 32);
}

void ParticleStore::init_opencl(const cl::Context& context, const cl::CommandQueue& queue, const cl::Program& program) {
	pad_particles_to_pow2();
	init_buffers_with_particles(context, queue);
	init_kernels(program);
	regenerate_grid(queue);
	update_particles_from_buffers(queue);
}

// iterator

auto ParticleStore::begin() const -> ConstIterator {
	return ConstIterator(positions_cpu.begin(), velocities_cpu.begin(), colors_cpu.begin());
}

auto ParticleStore::end() const -> ConstIterator {
	return ConstIterator(
		positions_cpu.begin() + real_size, 
		velocities_cpu.begin() + real_size, 
		colors_cpu.begin() + real_size
	);
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
