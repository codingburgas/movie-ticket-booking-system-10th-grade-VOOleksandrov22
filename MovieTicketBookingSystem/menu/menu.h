#pragma once
#include <iostream>
#include <vector>
#include "../nlohmann/json.hpp"

using json = nlohmann::json;


class Menu {
private:
	std::vector<std::string> options;

public:
	const std::vector<std::string>& getOptions();
	void setOptions(const std::vector<std::string>&);

	size_t getChoice(std::string question="");

	size_t getChoice(
		json& data,
		std::string(*getHighlightedItemAsString)(json&),
		std::string(*getRegularItemAsString)(json&),
		std::string question = ""
	);
};