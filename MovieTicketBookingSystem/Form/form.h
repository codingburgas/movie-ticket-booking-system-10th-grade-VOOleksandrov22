#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include "../Dict/dict.h"
#include "../Clipboard/clipboard.h"


struct Field;

using FormResult = Dict<Field*, std::string>;

struct Field {
	std::string name;
	std::string defaultValue = "";
	std::string placeholder = "";
	std::string instructions = "";
	bool isHidden = false;

	std::function<void(const FormResult&, const size_t&)> validationCallback = [](const FormResult&, const size_t&) {};
};



FormResult initForm(const std::vector<Field*>&& fields, const std::string&& submitButtonText = "SUBMIT");