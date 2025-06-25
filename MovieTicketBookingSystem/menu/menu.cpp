#include <iostream>
#include <conio.h>
#include <windows.h>
#include "menu.h"
#include "../Colors/colors.h"





const std::vector<std::string>& Menu::getOptions() {
	return options;
}
void Menu::setOptions(const std::vector<std::string>& newOptions) {
	options = newOptions;
}

void displayChoices(const std::vector<std::string>& options, const int& highlightIndex) {
    size_t size = 24; // for color modifiers
    for (const std::string& option : options) {
		size += option.size() + 5; // 5 for " > " and "   " and new-line char
    }
    std::string output;
    output.reserve(size);


    for (int i = 0; i < options.size(); ++i) {
        if (i == highlightIndex) {
            output += BOLD + std::string(YELLOW) + " >  " + options[i] + RESET + "\n"; // Highlighted option
        }
        else {
            output += "  " + options[i] + "\n"; // Non-highlighted option
        }
    }

    std::cout << output;
}


size_t Menu::getChoice(std::string question) {


    int highlightIndex = 0;

    while (true) {
        system("cls");
        std::cout << question << std::endl;
        displayChoices(getOptions(), highlightIndex);
        int key = _getch();

        switch (key) {
        case 0: // Handle special keys (arrows, function keys, etc.)
        case 224:
            key = _getch(); // Get the actual key code after the 0 or 224 prefix

            switch (key) {
            case 72: // Up arrow
                if (highlightIndex > 0) highlightIndex--;
                break;
            case 80: // Down arrow
                if (highlightIndex < options.size() - 1) highlightIndex++;
                break;
            }

            break;

        case 13: // Enter
            system("cls");
            return highlightIndex;

            break;
        }
    }
}


size_t Menu::getChoice(const std::vector<std::string>& options, std::string question) {


    int highlightIndex = 0;

    while (true) {
        system("cls");
        std::cout << question << std::endl;
        displayChoices(options, highlightIndex);
        int key = _getch();

        switch (key) {
        case 0: // Handle special keys (arrows, function keys, etc.)
        case 224:
            key = _getch(); // Get the actual key code after the 0 or 224 prefix
            switch (key) {
            case 72: // Up arrow
                if (highlightIndex > 0) highlightIndex--;
                break;
            case 80: // Down arrow
                if (highlightIndex < options.size() - 1) highlightIndex++;
                break;
            }
            break;
        case 13: // Enter
            system("cls");
            return highlightIndex;

            break;
        }
        
    }
}



