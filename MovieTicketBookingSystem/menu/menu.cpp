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
    system("cls");
    for (int i = 0; i < menu->getOptions().size(); ++i) {
        if (i == highlightIndex) {
            std::cout << " >  " << menu->getOptions()[i] << "\n"; // Highlighted option
        }
        else {
            std::cout << "  " << menu->getOptions()[i] << "\n"; // Non-highlighted option
        }
    }
}

size_t Menu::getChoice() {


    int highlightIndex = 0;

    while (true) {
        displayChoices(this, highlightIndex);
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

