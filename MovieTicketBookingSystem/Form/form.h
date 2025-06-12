#pragma once

#include <iostream>
#include <map>
#include <vector>

struct Field {
	std::string name;
	std::string defaultValue = "";
};

// field pointer -> value inputted and cursor pos
using EnteredData = std::map<Field*, std::pair<std::string, size_t>>;



EnteredData initForm(const std::vector<Field>&& fields);