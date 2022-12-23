#include "opencl-utils.hpp"
#include <iostream>

namespace clutils {
	void log_error(std::string_view where, cl_int code) {
		std::cerr << "OpenCL error in " << where << ": " << code << '\n';
	}

	cl::Device log_get_default_device() {
		std::vector<cl::Platform> platforms;
		cl_int ret = cl::Platform::get(&platforms);
		if(ret != CL_SUCCESS) log_error("Platform::get()", ret);
	
		std::vector<cl::Device> devices;
		ret = platforms[0].getDevices(CL_DEVICE_TYPE_DEFAULT, &devices);
		if(ret != CL_SUCCESS) log_error("Platform::getDevices()", ret);
		if(devices.empty()) log_error("Platform::getDevices(): result empty", ret);

		return devices[0];
	}

	cl::Context log_create_context(const cl::Device& device) {
		cl_int ret;
		auto context = cl::Context(device, NULL, NULL, NULL, &ret);
		if(ret != CL_SUCCESS) log_error("Context()", ret);
		return context;
	}

	cl::CommandQueue log_create_command_queue(const cl::Context& context) {
		cl_int ret;
		auto command_queue = cl::CommandQueue(context, 0, &ret);
		if(ret != CL_SUCCESS) log_error("CommandQueue()", ret);
		return command_queue;
	}

	cl::Program log_create_program(const cl::Context& context, const cl::Device& device, std::string_view source) {
		cl_int ret;
		auto program = cl::Program(context, source.data(), false, &ret);
		if(ret != CL_SUCCESS) log_error("Program()", ret);

		ret = program.build("-I res/opencl");
		if(ret != CL_SUCCESS) {
			log_error("Program()", ret);
			std::string log;
			program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, &log);
			std::cerr << "OpenCL build log:\n" << log << '\n';
		}

		return program;
	}

	cl::Kernel log_create_kernel(const cl::Program& program, std::string_view function_name) {
		cl_int ret;
		auto kernel = cl::Kernel(program, function_name.data(), &ret);
		if(ret != CL_SUCCESS) log_error("Kernel()", ret);
		return kernel;
	}

	cl::Buffer log_create_buffer(const cl::Context &context, cl_mem_flags flags, cl::size_type size) {
		cl_int ret;
		auto buffer = cl::Buffer(context, flags, size, NULL, &ret);
		if(ret != CL_SUCCESS) log_error("Buffer()", ret);
		return buffer;
	}

	void log_enqueue_ndrange_kernel(const cl::CommandQueue &queue, cl::Kernel &kernel, const cl::NDRange &global) {
		cl_int ret = queue.enqueueNDRangeKernel(kernel, 0, global);
		if(ret != CL_SUCCESS) log_error("CommandQueue::enqueueNDRangeKernel()", ret);
	}
}
