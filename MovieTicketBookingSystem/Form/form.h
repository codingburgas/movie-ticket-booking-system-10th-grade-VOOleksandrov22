#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include "../Dict/dict.h"

struct Field {
	std::string name;
	std::string defaultValue = "";
	std::string placeholder = "";
	std::string instructions = "";
	bool isHidden = false;

	std::function<void(const std::string&)> validationCallback = [](const std::string&) {};
};


using FormResult = Dict<Field*, std::string>;




FormResult initForm(const std::vector<Field*>&& fields, const std::string&& submitButtonText = "SUBMIT");