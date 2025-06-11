#pragma once

#include <iostream>
#include <map>
#include <vector>

using EnteredData = std::map<std::string, std::string>;

struct Field {
	std::string name;
	std::string defaultValue = "";
};

EnteredData initForm(const std::vector<Field>&& fields);