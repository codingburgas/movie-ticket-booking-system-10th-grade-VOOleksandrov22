#pragma once
#include <iostream>
#include <vector>
#include <array>
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
		const std::array<int, 2>& itemSize,
		const std::string& question = "",
		bool axesVisible = true
	);

	size_t getChoice(const std::vector<std::string>& options,std::string question = "");

};


std::string displayChoices(
	json& data,
	const size_t* highlightPos,
	const std::array<int, 2>& itemSize,
	std::function<std::string(json&)>& getHighlightedItemAsString,
	std::function<std::string(json&)>& getRegularItemAsString,
	std::function<bool(json&)> skipCheck,
	const bool& axesVisible
);