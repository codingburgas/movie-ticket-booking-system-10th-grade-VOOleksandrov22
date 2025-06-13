#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <functional>

struct Field {
	std::string name;
	std::string defaultValue = "";
	std::string placeholder = "";
	std::string instructions = "";
	bool isHidden = false;

	std::function<void(const std::string&)> validationCallback = [](const std::string&) {};
};

using FormResult = std::map<Field*, std::string>;




FormResult initForm(const std::vector<Field*>&& fields);