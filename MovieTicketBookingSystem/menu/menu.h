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


	std::pair<size_t, size_t> getChoice(
		json& data,
		std::function<std::string(json&)>& getHighlightedItemAsString,
		std::function<std::string(json&)>& getRegularItemAsString,
		std::function<bool(json&)> skipCheck,
		int itemSize[2],
		std::string question = ""
	);

	size_t getChoice(const std::vector<std::string>& options,std::string question = "");

};