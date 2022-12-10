#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>

struct Particle {
	cl_float2 position;
	cl_float2 velocity;
	cl_uchar3 color;
};
