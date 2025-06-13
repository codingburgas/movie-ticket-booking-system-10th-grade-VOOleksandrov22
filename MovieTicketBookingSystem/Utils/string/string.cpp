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


std::vector<std::string> Utils::String::split(const std::string& s, const std::string delimiter) {
	std::vector<std::string> tokens;
	size_t lastPos = 0;
	size_t findPos = s.find(delimiter, 0);

	while (findPos != std::string::npos) {
		std::string token = s.substr(lastPos, findPos - lastPos);
		tokens.push_back(token);
		lastPos = findPos + delimiter.size();
		findPos = s.find(delimiter, lastPos);
	}

	std::string lastToken = s.substr(lastPos);
	tokens.push_back(lastToken);

	return tokens;
}



std::string Utils::String::center(const std::string str, const int width, const bool containsLargeChars) {
	const int size = containsLargeChars ? str.size() / 3 : str.size();
	if (size >= width) return str;
	int padding = (width - size) / 2;
	return std::string(padding, ' ') + str + std::string(width - size - padding, ' ');
}