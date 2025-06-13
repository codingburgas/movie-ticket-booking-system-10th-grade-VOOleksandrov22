#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <functional>

struct Field {
	std::string name;
	std::string defaultValue = "";
	std::string placeholder = "";
};

class FormResult : public std::map<Field*, std::string> {
public:
	FormResult(std::map<Field*, std::pair<std::string, size_t>>& dataEntered) {
		for (const auto& pair : dataEntered) {
			this->insert({ pair.first, pair.second.first });
		}
	}
};




FormResult initForm(const std::vector<Field*>&& fields);