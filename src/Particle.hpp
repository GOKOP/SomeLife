#pragma once

#include "opencl-proxy.hpp"

#pragma pack(push, 4)
struct Particle {
	cl_float2 position;
	cl_float2 velocity;
	cl_uchar3 color;
};
#pragma pack(pop)
