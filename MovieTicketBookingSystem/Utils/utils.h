//#include "../../nlohmann/json.hpp"
#include "../nlohmann/json.hpp"
#include <iostream>
#include <random>
#include <chrono>


using json = nlohmann::json;

namespace Utils {
	namespace File {
		json readJsonFile(std::string filename);

		bool writeJsonToFile(std::string filename, const json& data);
	}

	namespace String {
		std::string toString(double num, int digitsAfterPoint = 2);

		std::string stringRepeater(std::string s, int amount);

		std::string toUppercase(const std::string& s);

		std::string toLowercase(const std::string& s);

		std::vector<std::string> split(const std::string& s, const std::string delimiter);

		std::string center(const std::string str, const int width, const bool containsLargeChars = false, const std::string& fillSpaceWith = " ");
	
		std::string left(const std::string str, const int width, const std::string& fillSpaceWith);

		std::string right(const std::string str, const int width, const std::string& fillSpaceWith);
	
		enum class TimeRelation {
			InPast,
			InFuture,
			NOW,
			InvalidFormat
		};


		Utils::String::TimeRelation checkTimeRelation(const std::string& timeStr1, const std::string& timeStr2 = "");
	}

	int generateRandomSixDigitNumber();

}