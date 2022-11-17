#include <string>
#include <optional>
#include <vector>

// command line args decoded
class ArgumentConfig {
public:
	enum RecordingState {
		None,
		Recording,
		Replaying
	};

private:
	RecordingState recording_state;
	std::string_view recipe_path;
	std::string_view recording_path;
	int framerate;
	std::string errors;

	std::optional<std::string_view> read_option(
			const std::vector<std::string_view>& args,
			std::string_view option_name);
	
public:
	ArgumentConfig(int argc, const char** argv);

	RecordingState get_recording_state() const;
	std::string_view get_recipe_path() const;
	std::string_view get_recording_path() const;
	int get_framerate() const;
	std::string_view get_errors() const;
};
