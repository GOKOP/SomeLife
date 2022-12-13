#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/cl2.hpp>

#pragma pack(push, 4)
struct Particle {
	cl_float2 position;
	cl_float2 velocity;
	cl_uchar3 color;
};
#pragma pack(pop)