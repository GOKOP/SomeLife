#pragma once

#ifndef  CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 300
#endif

// Todo: remove when OpenCL 3 lands in vcpkg
#if __has_include(<CL/opencl.hpp>)
	#include <CL/opencl.hpp>
#else
	#include <CL/cl2.hpp>
#endif

#include <string_view>
#include <vector>
#include <span>

// These are there mostly to log every return code if it indicates an error
// They're hyper-tailored for use cases in this program so they're not very portable
// eg. functions only take arguments passed to underlying OpenCL functions
// which I happen to use

namespace clutils {
	void log_error(std::string_view where, cl_int code);

	cl::Device log_get_default_device();
	cl::Context log_create_context(const cl::Device& device);
	cl::CommandQueue log_create_command_queue(const cl::Context& context);
	cl::Program log_create_program(const cl::Context& context, const cl::Device& device, std::string_view source);
	cl::Kernel log_create_kernel(const cl::Program& program, std::string_view function_name);
	cl::Buffer log_create_buffer(const cl::Context& context, cl_mem_flags flags, cl::size_type size);
	void log_enqueue_ndrange_kernel(const cl::CommandQueue& queue, cl::Kernel& kernel, const cl::NDRange& global);

	template<typename T>
	T log_get_kernel_workgroup_info(const cl::Kernel& kernel, const cl::Device& device, cl_kernel_work_group_info name) {
		T result {};
		cl_int ret = kernel.getWorkGroupInfo(device, name, &result);
		if(ret != CL_SUCCESS) log_error("Kernel::getWorkGroupInfo()", ret);
		return result;
	}

	template<typename Container>
	void log_copy(const cl::CommandQueue& queue, const Container& source, cl::Buffer& dest) {
		cl_int ret = cl::copy(queue, source.begin(), source.end(), dest);
		if(ret != CL_SUCCESS) log_error("copy()", ret);
	}

	template<typename Container>
	void log_copy(const cl::CommandQueue& queue, cl::Buffer& source, Container& dest) {
		cl_int ret = cl::copy(queue, source, dest.begin(), dest.end());
		if(ret != CL_SUCCESS) log_error("copy()", ret);
	}

	template<typename T>
	void log_kernel_setarg(cl::Kernel& kernel, cl_int arg_idx, T data) {
		cl_int ret = kernel.setArg(arg_idx, data);
		if(ret != CL_SUCCESS) log_error("Kernel::setArg()", ret);
	}
}
