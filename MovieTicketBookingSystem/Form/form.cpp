#include "form.h"
#include "../Colors/colors.h"
#include "../Utils/utils.h"


#include <conio.h>
#include <iomanip>
#include <string>
#include <format>



void displayForm(EnteredData& data, const int& highlightIndex) {
    unsigned int width = 70;


    int currentIndex = 0;
    for (const auto& pair : data) {
        if (currentIndex == highlightIndex) {
            std::cout << YELLOW;
        }
        // header
        std::cout << "╭" << Utils::String::toUppercase(pair.first) << Utils::String::stringRepeater("─", width - pair.first.size()) << "╮\n";


        std::cout << "| " << std::left << std::setw(width - 2) << pair.second << " |\n";

        // footer
        std::cout << "╰" << Utils::String::stringRepeater("─", width) << "╯\n";

        if (currentIndex == highlightIndex) {
            std::cout << RESET;
        }

        currentIndex++;
    }

    
}

void fillInInitialData(EnteredData& data, const std::vector<Field>& fields) {
    for (const auto& field : fields) {
        data[field.name] = field.defaultValue;
    }
}

EnteredData initForm(const std::vector<Field>&& fields) {

    EnteredData data = {};
    fillInInitialData(data, fields);

    int highlightIndex = 0;

    while (true) {
        system("cls");
        displayForm(data, highlightIndex);
        char key = _getch();

        switch (key) {
        case 72: // Up arrow
            if (highlightIndex > 0) highlightIndex--;
            else highlightIndex = fields.size() - 1;
            break;
        case 80: // Down arrow
            if (highlightIndex < fields.size() - 1) highlightIndex++;
            else highlightIndex = 0;
            break;
        case 13: // Enter
            if (highlightIndex < fields.size() - 1) highlightIndex++;
            else highlightIndex = 0;
            break;

        default:
            if (highlightIndex >= fields.size()) {}

            switch (key) {
            case 75: // left arrow

                break;
            case 77: // right arrow

                break;

            case 83: // delete
                std::cout << "delete";
                break;
            case '\b':
                std::cout << "Backspace";
                break;
            }

        
        }
    }
}