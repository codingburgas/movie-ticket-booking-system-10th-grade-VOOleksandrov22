#include <iostream>
#include <conio.h>
#include <windows.h>
#include "menu.h"
#include "../Colors/colors.h"


void displayChoices(
    json& data,
    const size_t* highlightPos,
    std::string(*getHighlightedItemAsString)(json&),
    std::string(*getRegularItemAsString)(json&)
) {
    for (size_t i = 0; i < data.size(); i++) {
        for (size_t j = 0; j < data[i].size(); j++) {
            std::cout << " | ";
            if (i == highlightPos[0] && j == highlightPos[1]) {
                std::cout << getHighlightedItemAsString(data[i][j]);
            }
            else {
                std::cout << getRegularItemAsString(data[i][j]);
            }
        }
        std::cout << std::endl;

    }
}


size_t Menu::getChoice(
    json& data,
    std::string(*getHighlightedItemAsString)(json&),
    std::string(*getRegularItemAsString)(json&),
    std::string question
) {


    // find first enabled
    size_t highlightPos[2] = { 0, 0 };
    bool set = false;
    for (size_t i = 0; i < data.size(); i++) {
        if (set) break;
        for (size_t j = 0; j < data[i].size(); j++) {
            if (data[i][j]["isVisible"]) {
                highlightPos[0] = i;
                highlightPos[1] = j;
                break;
            }
        }
    }

    /*switch (key) {
        case 72: // Up arrow
            if (highlightPos > 0) highlightIndex--;

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
        }*/
}

