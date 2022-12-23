// using array of structures pattern to sidestep the issue of struct alignment mismatch
// between the host and the device
typedef struct {
	__global float2* positions;
	__global float2* velocities;
	__global uchar3* colors;
	int number;
} ParticleStore;

typedef struct {
	__constant float2* positions;
	__constant float2* velocities;
	__constant uchar3* colors;
	int number;
} ConstParticleStore;

typedef struct {
	__constant float* first_cuts;
	__constant float* second_cuts;
	__constant float* peaks;
	__constant uchar3* colors1;
	__constant uchar3* colors2;
	int number;
} RuleStore;
