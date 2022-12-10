#include "Replayer.hpp"
#include <array>

Replayer::Replayer(std::string_view recording_file, bool cpu_is_big_endian):
	cpu_is_big_endian(cpu_is_big_endian)
{
	file_input.open(recording_file.data(), std::ios::binary);
	if(!file_input.good()) return;

	std::size_t particle_count;

	file_input.read(reinterpret_cast<char*>(&board_size.x), sizeof(int));
	file_input.read(reinterpret_cast<char*>(&board_size.y), sizeof(int));
	file_input.read(reinterpret_cast<char*>(&particle_count), sizeof(std::size_t));

	particles.resize(particle_count, Particle{{0, 0}, {0, 0}, {0, 0, 0}});
}

bool Replayer::is_good() const {
	return file_input.good();
}

const std::vector<Particle>& Replayer::get_particles() const {
	return particles;
}

const sf::Vector2i& Replayer::get_board_size() const {
	return board_size;
}

void Replayer::next_frame() {
	if(!file_input.good() || file_input.eof()) return;

	for(std::size_t i=0; i<particles.size(); ++i) {
		// the file should be little endian (reading in reverse not tested)
		if(cpu_is_big_endian) {
			auto* ptr = reinterpret_cast<char*>(&particles[i]);
			for(int i = sizeof(Particle) - 1; i >= 0; --i) {
				file_input.read(ptr + i, 1);
			}
		} else {
			file_input.read(reinterpret_cast<char*>(&particles[i]), sizeof(Particle));
		}
	}
}
