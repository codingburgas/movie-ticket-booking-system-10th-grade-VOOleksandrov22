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



std::string Utils::String::center(const std::string str, const int width, const bool containsLargeChars, const std::string& fillSpaceWith) {
	const int size = containsLargeChars ? str.size() / 3 : str.size();
	if (size >= width) return str;
	int padding = (width - size) / 2;
	return Utils::String::stringRepeater(fillSpaceWith, padding) + str + Utils::String::stringRepeater(fillSpaceWith, width - size - padding);
}

std::string Utils::String::left(const std::string str, const int width, const std::string& fillSpaceWith) {
	const size_t size = str.size();
	if (size >= width) return str;
	int padding = width - size;
	return str + Utils::String::stringRepeater(fillSpaceWith, padding);
}

std::string Utils::String::right(const std::string str, const int width, const std::string& fillSpaceWith) {
	const size_t size = str.size();
	if (size >= width) return str;
	int padding = width - size;
	return Utils::String::stringRepeater(fillSpaceWith, padding) + str;
}



Utils::String::TimeRelation Utils::String::checkTimeRelation(const std::string& timeStr1, const std::string& timeStr2) {

	std::chrono::system_clock::time_point parsedTimePoint1;
	std::istringstream ss1(timeStr1);

	ss1 >> std::chrono::parse("%Y-%m-%d %H:%M:%S", parsedTimePoint1);

	if (ss1.fail()) {
		return Utils::String::TimeRelation::InvalidFormat;
	}

	std::chrono::system_clock::time_point parsedTimePoint2;
	std::istringstream ss2(timeStr2);

	ss2 >> std::chrono::parse("%Y-%m-%d %H:%M:%S", parsedTimePoint1);

	if (timeStr2.empty() || ss2.fail()) {
		 parsedTimePoint2 = std::chrono::system_clock::now();
	}

	if (parsedTimePoint1 == parsedTimePoint2) {
		return Utils::String::TimeRelation::NOW;
	}
	if (parsedTimePoint1 > parsedTimePoint2) {
		return Utils::String::TimeRelation::InFuture;
	}
	else {
		return Utils::String::TimeRelation::InPast;
	}

}