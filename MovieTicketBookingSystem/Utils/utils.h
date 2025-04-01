//#include "../../nlohmann/json.hpp"
#include "../nlohmann/json.hpp"

using json = nlohmann::json;

namespace Utils {
	namespace File {
		json readJsonFile(std::string filename);

		bool writeJsonToFile(std::string filename, const json& data);
	}
}