#include "config.h"

const char Config::getShortenedGender(const std::string& gender) const {
	for (auto& pair : genders) {
		if (pair.second == gender) {
			return pair.first;
		}
	}
	return 'P'; // Default to 'Prefer not to say' if no match found
}