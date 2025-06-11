#include <string>
#include <format>
#include <cmath>
#include <iomanip> 
#include <sstream>
#include <string>

#include "../utils.h"

std::string Utils::String::toString(double num, int digitsAfterPoint) {
	if (std::isnan(num)) return "NaN";
	if (std::isinf(num)) return "Inf";

	std::stringstream ss;
	ss << std::fixed << std::setprecision(digitsAfterPoint) << num;
	return ss.str();
}

std::string Utils::String::stringRepeater(std::string s, int amount) {
	std::string res = "";
	for (int i = 0; i < amount; i++) {
		res += s;
	}
	return res;
}


std::string Utils::String::toUppercase(const std::string& s) {
	std::string res = "";
	for (const char& ch : s) {
		res += std::toupper(ch);
	}

	return res;
}

std::string Utils::String::toLowercase(const std::string& s) {
	std::string res = "";
	for (const char& ch : s) {
		res += std::tolower(ch);
	}

	return res;
}