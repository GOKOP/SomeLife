#include "OpenClRunner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

void log_error(std::string_view msg, cl_int code) {
	std::cerr << "OpenCL error: " << msg << ": " << code << '\n';
}

OpenClRunner::OpenClRunner(std::string_view program_file, std::string_view function_name):
	buffer1_is_new(true)
{
	std::vector<cl::Platform> platforms;
	cl_int ret = cl::Platform::get(&platforms);
	if(ret != CL_SUCCESS) log_error("Platform::get", ret);

	std::vector<cl::Device> devices;
	ret = platforms[0].getDevices(CL_DEVICE_TYPE_DEFAULT, &devices);
	if(ret != CL_SUCCESS) log_error("Platform::getDevices", ret);

	context = cl::Context(devices, NULL, NULL, NULL, &ret);
	if(ret != CL_SUCCESS) log_error("Context()", ret);

	command_queue = cl::CommandQueue(context, 0, &ret);
	if(ret != CL_SUCCESS) log_error("CommandQueue()", ret);

	std::ifstream program_file_handler(program_file.data());
	if(!program_file_handler.is_open()) {
		log_error(std::string("can't open ") + program_file.data(), 0);
		return;
	}

	std::stringstream ss;
	ss << program_file_handler.rdbuf();
	std::string program_source(ss.str());
	program_file_handler.close();

	program = cl::Program(context, program_source, true, &ret);
	if(ret != CL_SUCCESS) {
		log_error("Program()", ret);
		std::string log;
		program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &log);
		std::cerr << log << '\n';
	}

	kernel = cl::Kernel(program, function_name.data(), &ret);
	if(ret != CL_SUCCESS) log_error("Kernel()", ret);
}

void OpenClRunner::init_data(const std::vector<Particle> &particles, const std::vector<Rule>& rules, cl_int2 board_size, cl_float friction) {
	cl_int ret;
	buffer1 = cl::Buffer(context, CL_MEM_READ_WRITE, particles.size() * sizeof(Particle), NULL, &ret);
	if(ret != CL_SUCCESS) log_error("Buffer() buffer1", ret);
	buffer2 = cl::Buffer(context, CL_MEM_READ_WRITE, particles.size() * sizeof(Particle), NULL, &ret);
	if(ret != CL_SUCCESS) log_error("Buffer() buffer2", ret);
	rule_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, rules.size() * sizeof(Rule), NULL, &ret);
	if(ret != CL_SUCCESS) log_error("Buffer() rule_buffer", ret);

	ret = cl::copy(command_queue, particles.begin(), particles.end(), buffer1);
	if(ret != CL_SUCCESS) log_error("copy() buffer1", ret);

	ret = cl::copy(command_queue, particles.begin(), particles.end(), buffer2);
	if(ret != CL_SUCCESS) log_error("copy() buffer2", ret);

	ret = cl::copy(command_queue, rules.begin(), rules.end(), rule_buffer);
	if(ret != CL_SUCCESS) log_error("copy() buffer3", ret);

	ret = kernel.setArg(2, static_cast<cl_int>(particles.size()));
	if(ret != CL_SUCCESS) log_error("Kernel::setArg 2", ret);

	ret = kernel.setArg(3, rule_buffer);
	if(ret != CL_SUCCESS) log_error("Kernel::setArg 3", ret);

	ret = kernel.setArg(4, static_cast<cl_int>(rules.size()));
	if(ret != CL_SUCCESS) log_error("Kernel::setArg 4", ret);

	ret = kernel.setArg(5, board_size);
	if(ret != CL_SUCCESS) log_error("Kernel::setArg 5", ret);

	ret = kernel.setArg(6, friction);
	if(ret != CL_SUCCESS) log_error("Kernel::setArg 6", ret);

	global_ndrange = cl::NDRange(particles.size());
}

void OpenClRunner::run_and_fetch(std::vector<Particle>& result) {
	buffer1_is_new = !buffer1_is_new;

	cl_int ret = kernel.setArg(0, get_old_buffer());
	if(ret != CL_SUCCESS) log_error("Kernel::setArg 0", ret);

	ret = kernel.setArg(1, get_new_buffer());
	if(ret != CL_SUCCESS) log_error("Kernel::setArg 1", ret);

	ret = command_queue.enqueueNDRangeKernel(kernel, 0, global_ndrange);
	if(ret != CL_SUCCESS) log_error("CommandQueue::enqueueNDRangeKernel", ret);

	ret = cl::copy(command_queue, get_new_buffer(), result.begin(), result.end());
	if(ret != CL_SUCCESS) log_error("copy new_buffer", ret);
}
