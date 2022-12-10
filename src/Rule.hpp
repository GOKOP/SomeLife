#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>

struct Rule {
	cl_uchar3 particle1_color;
	cl_uchar3 particle2_color;
	cl_float first_cut;
	cl_float second_cut;
	cl_float peak;
};
