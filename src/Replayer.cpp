#include "Replayer.hpp"
#include <bit>

Replayer::Replayer(std::string_view recording_file) {
	file_input.open(recording_file.data(), std::ios::binary);
	if(!file_input.good()) return;

	cl_int particle_count;

	file_input.read(reinterpret_cast<char*>(&board_size.x), sizeof(cl_int));
	file_input.read(reinterpret_cast<char*>(&board_size.y), sizeof(cl_int));
	file_input.read(reinterpret_cast<char*>(&particle_count), sizeof(cl_int));

	particles.resize(particle_count, Particle{{0, 0}, {0, 0}, {0, 0, 0}});
}

bool Replayer::is_good() const {
	return file_input.good();
}

const std::vector<Particle>& Replayer::get_particles() const {
	return particles;
}

cl_int2 Replayer::get_board_size() const {
	return board_size;
}

void Replayer::next_frame() {
	if(!file_input.good() || file_input.eof()) return;

	for(std::size_t i=0; i<particles.size(); ++i) {
		// the file should be little endian (reading in reverse not tested)
		if(std::endian::native == std::endian::big) {
			auto* ptr = reinterpret_cast<char*>(&particles[i]);
			for(int i = sizeof(Particle) - 1; i >= 0; --i) {
				file_input.read(ptr + i, 1);
			}
		} else {
			file_input.read(reinterpret_cast<char*>(&particles[i]), sizeof(Particle));
		}
	}
}
