#pragma once

#include <vector>
#include <span>
#include "opencl-utils.hpp"
#include "Particle.hpp"

// Note that this isn't an equivalent to the ParticleStore struct in the OpenCL C code
// Note that `init_buffers_with_particles()` should be called between `add_particle()` and use

// Particles are arranged in a grid, which is realized by sorting them according to their grid cell
// and keeping track of where do the grid cells start in a separate vector.
// To avoid copying data back and forth, they're sorted on the GPU, using the bitonic sort algorithm
// which requires input data length to be a power of two. For this reason, data here must be padded

class ParticleStore {
	cl_float2 cell_size;
	cl_int2 grid_size;
	cl::Kernel sort_kernel;
	cl::Kernel grid_kernel;
	
	// for reference on the CPU side
	std::vector<cl_float2> positions_cpu;
	std::vector<cl_float2> velocities_cpu;
	std::vector<cl_uchar3> colors_cpu;

	std::size_t real_size; // without padding

	void pad_particles_to_pow2();
	void init_kernels(const cl::Program program);
	void init_buffers_with_particles(const cl::Context& context, const cl::CommandQueue& queue);
	void sort_gpu_particles(const cl::CommandQueue& queue);

public:
	// these are just public because they're needed as read-write to pass them to the GPU
	cl::Buffer positions;
	cl::Buffer velocities;
	cl::Buffer colors;
	cl::Buffer cell_positions;

	// dummy constructor because init data isn't available at Simulation construction
	inline ParticleStore() {}; 
	ParticleStore(cl_int2 window_size, int grid_resolution);

	inline std::size_t size() const { return real_size; }
	inline std::size_t padded_size() const { return positions_cpu.size(); }

	void add_particle(const Particle& particle);
	void overwrite_data_from_other(const ParticleStore& other);

	void init_opencl(const cl::Context& context, const cl::CommandQueue& queue, const cl::Program& program);
	void regenerate_grid(const cl::CommandQueue& queue);
	void update_particles_from_buffers(const cl::CommandQueue& queue);


	// Iterator over the CPU-side data;
	// note that because of the use of a proxy object, the iterator doesn't work entirely
	// and wouldn't pass any iterator concept assert (-> doesn't work for example).
	// Pointer and reference are just copies and the same as value_type
	// Assuming 64-bit pointers this is actually slightly cheaper than creating some referential 
	// type (and a single reference obviously won't work because there's nothing to refer to)

	class ConstIterator {
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = Particle;
		using pointer = Particle;
		using reference = Particle;

		using f2vec_iter = std::vector<cl_float2>::const_iterator;
		using c3vec_iter = std::vector<cl_uchar3>::const_iterator;

		f2vec_iter pos_iter;
		f2vec_iter vel_iter;
		c3vec_iter color_iter;

	public:

		inline ConstIterator(f2vec_iter pos_iter, f2vec_iter vel_iter, c3vec_iter color_iter):
			pos_iter(pos_iter), vel_iter(vel_iter), color_iter(color_iter) {}

		reference operator*();
		//pointer operator->(); // can't work because there's no actual pointer

		ConstIterator& operator++();
		ConstIterator& operator--();

		ConstIterator operator++(int);
		ConstIterator operator--(int);

		ConstIterator& operator+=(int i);
		ConstIterator& operator-=(int i);
		
		ConstIterator operator+(int i);
		ConstIterator operator-(int i);

		bool friend operator==(const ConstIterator& lhs, const ConstIterator& rhs);
		bool friend operator!=(const ConstIterator& lhs, const ConstIterator& rhs);
	};

	ConstIterator begin() const;
	ConstIterator end() const;
};
