#pragma once
#include <iostream>
#include <vector>


class Menu {
private:
	std::vector<std::string> options;

public:
	const std::vector<std::string>& getOptions();
	void setOptions(const std::vector<std::string>&);

	size_t getChoice(std::string question="");

	size_t getChoice(const std::vector<std::string>& options,std::string question = "");
};