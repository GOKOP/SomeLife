typedef struct {
	float2 position;
	float2 velocity;
	uchar3 color;
} Particle;

typedef struct {
	uchar3 color1;
	uchar3 color2;
	float first_cut;
	float second_cut;
	float peak;
} Rule;

float calculate_force(__global const Rule* rule, float distance) {
	float large_value = 1;

	if(distance > rule->second_cut) return 0;

	if(distance < rule->first_cut) {
		float val = (rule->first_cut / distance) - 1 + rule->peak;
		if(val > large_value || isnan(val) || isinf(val)) return large_value;
		else return val;
	}

	if(rule->second_cut == rule->first_cut) return 0;
	return mix(rule->peak, 0, distance / (rule->second_cut - rule->first_cut));
}

void execute_rule(__global const Rule* rule, __global Particle* p1, __global const Particle* p2) {
	float distance_x = p1->position.x - p2->position.x;
	float distance_y = p1->position.y - p2->position.y;
	float distance = sqrt(distance_x*distance_x + distance_y*distance_y);
	if(distance == 0) return;

	float normalized_x = distance_x / distance;
	float normalized_y = distance_y / distance;

	float force = calculate_force(rule, distance);
	float force_x = force * normalized_x;
	float force_y = force * normalized_y;

	p1->velocity.x += force_x;
	p1->velocity.y += force_y;
}

float2 apply_friction(float2 velocity, float friction) {
	velocity.x *= (1.f - friction);
	velocity.y *= (1.f - friction);
	return velocity;
}

void perform_movement(__global Particle* particle, int2 board_size, float friction) {
	particle->velocity = apply_friction(particle->velocity, friction);

	float new_x = particle->position.x + particle->velocity.x;
	float new_y = particle->position.y + particle->velocity.y;

	if(new_x < 0 || new_x >= board_size.x) particle->velocity.x *= -0.5;
	else particle->position.x = new_x;

	if(new_y < 0 || new_y >= board_size.y) particle->velocity.y *= -0.5;
	else particle->position.y = new_y;
}

bool particles_equal(const __global Particle* p1, const __global Particle* p2) {
	return all(isequal(p1->position, p2->position)) &&
		all(isequal(p1->velocity, p2->velocity)) &&
		all(p1->color == p2->color);
}

__kernel void update_particle(
		__global const Particle* old,
		__global Particle* new,
		int n_particles,
		__global const Rule* rules,
		int n_rules,
		int2 board_size,
		float friction)
{
    int i = get_global_id(0);

	new[i] = old[i];

	for(int rule_i = 0; rule_i < n_rules; ++rule_i) {
		if(any(rules[rule_i].color1 != old[i].color)) continue;

		for(int other_i = 0; other_i < n_particles; ++other_i) {
			if(particles_equal(&old[i], &old[other_i])) continue;
			if(any(rules[rule_i].color2 != old[other_i].color)) continue;
			execute_rule(&rules[rule_i], &new[i], &old[other_i]);
		}
	}
	perform_movement(&new[i], board_size, friction);
}
