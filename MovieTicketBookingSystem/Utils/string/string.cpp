#include <string>
#include <format>
#include <cmath>
#include <iomanip> 
#include <sstream> 

#include "../utils.h"

std::string Utils::String::toString(double num, int digitsAfterPoint) {
	if (std::isnan(num)) return "NaN";
	if (std::isinf(num)) return "Inf";

	std::stringstream ss;
	ss << std::fixed << std::setprecision(digitsAfterPoint) << num;
	return ss.str();
}