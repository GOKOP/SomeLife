#include <storage.hcl>

float calculate_force(const RuleStore* rules, int rule_idx, float distance) {
	float large_value = 1;

	if(distance > rules->second_cuts[rule_idx]) return 0;

	if(distance < rules->first_cuts[rule_idx]) {
		float val = (rules->first_cuts[rule_idx] / distance) - 1 + rules->peaks[rule_idx];
		if(val > large_value || isnan(val) || isinf(val)) return large_value;
		else return val;
	}

	if(rules->second_cuts[rule_idx] == rules->first_cuts[rule_idx]) return 0;
	return mix(rules->peaks[rule_idx], 0, distance / (rules->second_cuts[rule_idx] - rules->first_cuts[rule_idx]));
}

void execute_rule(
	RuleStore* rules, 
	ParticleStore* new_particles,
	ConstParticleStore* old_particles,
	int rule_idx, int new_idx, int old_idx
) {
	float distance_x = 
		new_particles->positions[new_idx].x - old_particles->positions[old_idx].x;
	float distance_y = 
		new_particles->positions[new_idx].y - old_particles->positions[old_idx].y;

	float distance = sqrt(distance_x*distance_x + distance_y*distance_y);
	if(distance == 0) return;

	float normalized_x = distance_x / distance;
	float normalized_y = distance_y / distance;

	float force = calculate_force(rules, rule_idx, distance);
	float force_x = force * normalized_x;
	float force_y = force * normalized_y;

	new_particles->velocities[new_idx].x += force_x;
	new_particles->velocities[new_idx].y += force_y;
}

float2 apply_friction(float2 velocity, float friction) {
	velocity.x *= (1.f - friction);
	velocity.y *= (1.f - friction);
	return velocity;
}

void perform_movement(ParticleStore* particles, int idx, int2 board_size, float friction) {
	particles->velocities[idx] = apply_friction(particles->velocities[idx], friction);

	float new_x = particles->positions[idx].x + particles->velocities[idx].x;
	float new_y = particles->positions[idx].y + particles->velocities[idx].y;

	if(new_x < 0 || new_x >= board_size.x) particles->velocities[idx].x *= -0.5;
	else particles->positions[idx].x = new_x;

	if(new_y < 0 || new_y >= board_size.y) particles->velocities[idx].y *= -0.5;
	else particles->positions[idx].y = new_y;
}

void copy_particle(ConstParticleStore* source, ParticleStore* dest, int idx) {
	dest->positions[idx] = source->positions[idx];
	dest->velocities[idx] = source->velocities[idx];
	dest->colors[idx] = source->colors[idx];
}

// can't pass a structure with pointers unfortunately
__kernel void update_particle(
		__constant float2* old_positions,
		__constant float2* old_velocities,
		__constant uchar3* old_colors,
		__global float2* new_positions,
		__global float2* new_velocities,
		__global uchar3* new_colors,
		int particle_count,
		__constant float* rule_first_cuts,
		__constant float* rule_second_cuts,
		__constant float* rule_peaks,
		__constant uchar3* rule_colors1,
		__constant uchar3* rule_colors2,
		int rule_count,
		int2 board_size,
		float friction)
{
    int i = get_global_id(0);
	if(i >= particle_count) i = 0;

	ConstParticleStore old = {old_positions, old_velocities, old_colors, particle_count};
	ParticleStore new = {new_positions, new_velocities, new_colors, particle_count};
	RuleStore rules = {rule_first_cuts, rule_second_cuts, rule_peaks, rule_colors1, rule_colors2, rule_count};

	//copy_particle(&new, &old, i);
	copy_particle(&old, &new, i);

	for(int rule_i = 0; rule_i < rules.number ; ++rule_i) {
		if(any(rules.colors1[rule_i] != old.colors[i])) continue;

		for(int other_i = 0; other_i < old.number; ++other_i) {
			if(i == other_i) continue;
			if(any(rules.colors2[rule_i] != old.colors[other_i])) continue;
			execute_rule(&rules, &new, &old, rule_i, i, other_i);
		}
	}
	perform_movement(&new, i, board_size, friction);
}
