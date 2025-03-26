#include <iostream>
#include <conio.h>
#include <windows.h>
#include "menu.h"

const std::vector<std::string>& Menu::getOptions() {
	return options;
}
void Menu::setOptions(const std::vector<std::string>& newOptions) {
	options = newOptions;
}

void displayChoices(Menu* menu, const int& highlightIndex) {
    system("cls"); // Clear the console screen
    for (int i = 0; i < menu->getOptions().size(); ++i) {
        if (i == highlightIndex) {
            std::cout << "> "; // Highlighted option
        }
        else {
            std::cout << "  "; // Non-highlighted option
        }
        std::cout << menu->getOptions()[i] << "\n";
    }
}

size_t Menu::getChoice() {
    int highlightIndex = 0;

    while (!exit) {
        displayChoices(this, highlightIndex);
        char key = _getch();

        switch (key) {
            case 72: // Up arrow key (ASCII code for UP key)
                if (highlightIndex > 0) highlightIndex--;
                break;
            case 80: // Down arrow key (ASCII code for DOWN key)
                if (highlightIndex < options.size() - 1) highlightIndex++;
                break;
            case 13:
                system("cls");
                return highlightIndex;

                
                break;
                
            default:
                break;
        }
    }
}

