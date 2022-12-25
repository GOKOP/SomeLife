#include <storage.hcl>

// This file deals with reorganizing particle data so that it can be understood as a grid
// Particles are sorted according to which grid cell would they belong to
// And a separate array stores where each cell begins
// This is done on the GPU mainly to avoid copying data back and forth

// sorting

int particle_cell_index(ParticleStore* store, int idx, float2 cell_size, int grid_width) {
	int cell_x = store->positions[idx].x / cell_size.x;
	int cell_y = store->positions[idx].y / cell_size.y;
	return grid_width * cell_y + cell_x;
}

int particle_cell_index_const(ConstParticleStore* store, int idx, float2 cell_size, int grid_width) {
	int cell_x = store->positions[idx].x / cell_size.x;
	int cell_y = store->positions[idx].y / cell_size.y;
	return grid_width * cell_y + cell_x;
}

bool greater_by_grid_cell(ParticleStore* store, int idx1, int idx2, float2 cell_size, int grid_width) {
	return particle_cell_index(store, idx1, cell_size, grid_width) >
		particle_cell_index(store, idx2, cell_size, grid_width);
}

bool smaller_by_grid_cell(ParticleStore* store, int idx1, int idx2, float2 cell_size, int grid_width) {
	return particle_cell_index(store, idx1, cell_size, grid_width) <
		particle_cell_index(store, idx2, cell_size, grid_width);
}

void swap_particles(ParticleStore* store, int idx1, int idx2) {
	float2 pos = store->positions[idx1];
	float2 vel = store->velocities[idx1];
	uchar3 col = store->colors[idx1];

	store->positions[idx1] = store->positions[idx2];
	store->velocities[idx1] = store->velocities[idx2];
	store->colors[idx1] = store->colors[idx2];

	store->positions[idx2] = pos;
	store->velocities[idx2] = vel;
	store->colors[idx2] = col;
}

// bitonic sort (one step, called repeatedly from c++)
__kernel void sort_by_grid_cells_step (
		__global float2* positions,
		__global float2* velocities,
		__global uchar3* colors,
		int buffer_size,
		float2 cell_size,
		int grid_width,
		int stage, 
		int step)
{
	ParticleStore store = { positions, velocities, colors, buffer_size };
	bool sort_increasing = true;

	int i = get_global_id(0);
	int pair_distance = 1 << (stage - step);
	int block_width = 2 * pair_distance;
	
	int left_idx = (i % pair_distance) + (i / pair_distance) * block_width;
	int right_idx = left_idx + pair_distance;

	int same_direction_block_width = 1 << stage;

	if((i / same_direction_block_width) % 2 == 1) sort_increasing = !sort_increasing;
	bool do_swap = 
		(sort_increasing &&
		greater_by_grid_cell(&store, left_idx, right_idx, cell_size, grid_width)) ||
		(!sort_increasing &&
		smaller_by_grid_cell(&store, left_idx, right_idx, cell_size, grid_width));

	if(do_swap) swap_particles(&store, left_idx, right_idx);
}

// generate cell positions
// this doesn't really benefit from parallelism, I just wanna do it in GPU memory
// Should be called with small global work size; only one thread matters anyway

__kernel void regenerate_grid(
		__constant float2* positions,
		__constant float2* velocities,
		__constant uchar3* colors,
		int particle_count,
		__global int* cell_positions,
		int cell_count,
		float2 cell_size,
		int grid_width)
{
	if(get_global_id(0) > 0) return;

	ConstParticleStore store = { positions, velocities, colors, particle_count };

	int prev_cell_idx = -1;
	for(int i=0; i<store.number; ++i) {
		int cell_idx = particle_cell_index_const(&store, i, cell_size, grid_width);

		if(cell_idx != prev_cell_idx) {
			for(int j = prev_cell_idx; j <= cell_idx; ++j) {
				cell_positions[j] = i;
			}
		}

		prev_cell_idx = cell_idx;
	}

	for(int i=prev_cell_idx+1; i < cell_count; ++i) {
		cell_positions[i] = cell_count;
	}
}
