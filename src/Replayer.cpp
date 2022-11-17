#include "Replayer.hpp"
#include <array>

Replayer::Replayer(std::string_view recording_file) {
	file_input.open(recording_file.data(), std::ios::binary);
	if(!file_input.good()) return;

	std::size_t particle_count;

	file_input.read(reinterpret_cast<char*>(&board_size.x), sizeof(int));
	file_input.read(reinterpret_cast<char*>(&board_size.y), sizeof(int));
	file_input.read(reinterpret_cast<char*>(&particle_count), sizeof(std::size_t));

	particles.resize(particle_count, Particle({0, 0}, {0, 0}, sf::Color::Black));
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
		file_input.read(reinterpret_cast<char*>(&particles[i]), sizeof(Particle));
	}
}
