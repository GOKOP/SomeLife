#include "RuleStore.hpp"
#include <ranges>

void RuleStore::init_buffers_with_rules(const cl::Context& context, const cl::CommandQueue& queue) {
	colors1 = clutils::log_create_buffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar3) * rules.size());
	colors2 = clutils::log_create_buffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar3) * rules.size());
	first_cuts = clutils::log_create_buffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * rules.size());
	second_cuts = clutils::log_create_buffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * rules.size());
	peaks = clutils::log_create_buffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * rules.size());

	std::vector<cl_uchar3> uchar3s;
	std::ranges::transform(rules, std::back_inserter(uchar3s),
		[](const Rule& r) -> cl_uchar3 { return { r.particle1_color.r, r.particle1_color.g, r.particle1_color.b }; });
	clutils::log_copy(queue, uchar3s, colors1);

	std::ranges::transform(rules, uchar3s.begin(),
		[](const Rule& r) -> cl_uchar3 { return { r.particle2_color.r, r.particle2_color.g, r.particle2_color.b }; });
	clutils::log_copy(queue, uchar3s, colors2);

	std::vector<cl_float> floats;
	std::ranges::transform(rules, std::back_inserter(floats),
		[](const Rule& r) -> cl_float { return r.first_cut; });
	clutils::log_copy(queue, floats, first_cuts);

	std::ranges::transform(rules, floats.begin(),
		[](const Rule& r) -> cl_float { return r.second_cut; });
	clutils::log_copy(queue, floats, second_cuts);

	std::ranges::transform(rules, floats.begin(),
		[](const Rule& r) -> cl_float { return r.peak; });
	clutils::log_copy(queue, floats, peaks);
}
