#pragma once

#include "opencl-proxy.hpp"

#pragma pack(push, 4)
struct Rule {
	cl_uchar3 particle1_color;
	cl_uchar3 particle2_color;
	cl_float first_cut;
	cl_float second_cut;
	cl_float peak;
};
#pragma pack(pop)
