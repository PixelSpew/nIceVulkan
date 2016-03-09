#include <string>
#include <vector>

namespace nif
{
	namespace file
	{
		std::string read_all_text(const std::string &filename);
		std::vector<char> read_all_bytes(const std::string &filename);
		void write_all_text(const std::string &filename, const std::string &text);
		void append_all_text(const std::string &filename, const std::string &text);
	}
}
