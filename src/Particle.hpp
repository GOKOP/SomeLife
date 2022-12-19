#pragma once

#include "opencl-utils.hpp"

struct Particle {
	cl_float2 position;
	cl_float2 velocity;
	cl_uchar3 color;
};
