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
    for (int i = 0; i < options.size(); ++i) {
        if (i == highlightIndex) {
            std::cout << BOLD << YELLOW << " >  " << options[i] << RESET << "\n"; // Highlighted option
        }
        else {
            std::cout << "  " << options[i] << "\n"; // Non-highlighted option
        }
    }
}


size_t Menu::getChoice(std::string question) {


    int highlightIndex = 0;

    while (true) {
        system("cls");
        std::cout << question << std::endl;
        displayChoices(getOptions(), highlightIndex);
        char key = _getch();

        switch (key) {
            case 72: // Up arrow
                if (highlightIndex > 0) highlightIndex--;
                break;
            case 80: // Down arrow
                if (highlightIndex < options.size() - 1) highlightIndex++;
                break;
            case 13: // Enter
                system("cls");
                return highlightIndex;

                break;
                
            default:
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
        char key = _getch();

        switch (key) {
        case 72: // Up arrow
            if (highlightIndex > 0) highlightIndex--;
            break;
        case 80: // Down arrow
            if (highlightIndex < options.size() - 1) highlightIndex++;
            break;
        case 13: // Enter
            system("cls");
            return highlightIndex;

            break;

        default:
            break;
        }
    }
}



