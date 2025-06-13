#include "form.h"
#include "../Colors/colors.h"
#include "../Utils/utils.h"


#include <conio.h>
#include <iomanip>
#include <string>
#include <format>
#include <iterator>
#include <windows.h>
#include <ranges>


#define WIDTH 70
#define SUBMIT_BUTTON_WIDTH 30

std::string SUBMIT_BUTTON_TEXT;


struct AdditionalFieldData {
    std::string value;
    size_t caretPos;
	std::string errorMessage;
    bool hidden;
};


class EnteredData; // forward declaration


FormResult normalizeData(EnteredData& data);

// field pointer -> value inputted and cursor pos
class EnteredData : public Dict<Field*, AdditionalFieldData> {
public:
    bool isValid(){
		bool valid = true;
        for (size_t i = 0; i < size(); i++) {
			auto& pair = at(i);
            try {
                pair.first->validationCallback(normalizeData(*this), i);
			}
			catch (const std::runtime_error& error) {
				valid = false;
				pair.second.errorMessage = error.what();
			}
			
		}
		return valid;
    }
};


FormResult normalizeData(EnteredData& data) {
    FormResult result;
    for (const auto& pair : data) {
        result.insert(pair.first, pair.second.value);
    }
    return result;
}



void displayForm(EnteredData& data, const int& highlightIndex) {

    std::cout << "Press \"esc\" to cancel form submission\nPress \"Ctrl + H\" to enable privacy mode\n\n";

    int currentIndex = 0;
    for (const auto& pair : data) {
        if (currentIndex == highlightIndex) {
            std::cout << YELLOW;
        }
        // header
        std::cout << "╭" << Utils::String::toUppercase(pair.first->name) << Utils::String::stringRepeater("─", WIDTH - pair.first->name.size()) << "╮\n";


		const std::string& value = pair.second.value;
        const size_t& caretPos = pair.second.caretPos;

        std::string strToPrint;

		if (pair.second.hidden) {
            strToPrint = ((currentIndex == highlightIndex)
                ? std::string(caretPos, '*') + "|" + std::string(value.size() - caretPos, '*') 
                : std::string(value.size(), '*'));
        }
        else {
            strToPrint = ((currentIndex == highlightIndex)
                ? value.substr(0, caretPos) + "|" + value.substr(caretPos)
                : value);
        }

		if (value == "") strToPrint += pair.first->placeholder;

		
        

        for (size_t i = 0; i < strToPrint.size(); i += WIDTH - 2) {
            std::vector<std::string> splittedLine = Utils::String::split(strToPrint.substr(i, WIDTH - 2), "\r");

            for (const std::string& line : splittedLine) {
                std::cout << "| " << std::left << std::setw(WIDTH - 2) << line << " |\n";
            }
        }


        

        // footer
        std::cout << "╰" << Utils::String::stringRepeater("─", WIDTH) << "╯\n";

        std::cout << RESET << ((pair.first->instructions.size() != 0) ? pair.first->instructions + "\n" : "") << RED << pair.second.errorMessage << RESET << "\n";
        

        currentIndex++;
    }

    if (currentIndex == highlightIndex) {
		std::cout << YELLOW;
    }

	std::cout << Utils::String::center("╭" + Utils::String::stringRepeater("─", SUBMIT_BUTTON_WIDTH) + "╮", WIDTH, true) << "\n"
              << Utils::String::center("|" + Utils::String::center(SUBMIT_BUTTON_TEXT, SUBMIT_BUTTON_WIDTH) + "|", WIDTH, false) << "\n"
              << Utils::String::center("╰" + Utils::String::stringRepeater("─", SUBMIT_BUTTON_WIDTH) + "╯", WIDTH, true) << "\n";

    if (currentIndex == highlightIndex) {
        std::cout << RESET;
    }

}

void fillInInitialData(EnteredData& data, const std::vector<Field*>& fields) {
    for (auto& field : fields) {
        data[field] = { 
            field->defaultValue, 
            field->defaultValue.size(),
            "",
			field->isHidden
        };
    }
}



FormResult initForm(const std::vector<Field*>&& fields, const std::string&& submitButtonText) {

	SUBMIT_BUTTON_TEXT = submitButtonText;

    EnteredData data = {};
    fillInInitialData(data, fields);
    //        highlight pos                       value        cursor pos
    // std::pair<int, std::pair<Field*, std::pair<std::string, size_t>>>
    //HighlightData highlightData = { 0, data };
	int highlightIndex = 0;

    while (true) {
        system("cls");
        displayForm(data, highlightIndex);
        int key = _getch();
        bool is_ctrl_pressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
		if (is_ctrl_pressed) {
            if (highlightIndex == data.size()) continue;
            switch (key) {
			case 8: // Ctrl + H
                data.at(highlightIndex).second.hidden = !data.at(highlightIndex).second.hidden;
                break;
            }
            continue;
		}

		switch (key) {
		case 27: // Escape key
            throw 1;
		case 0: // Handle special keys (arrows, function keys, etc.)
        case 224:
			key = _getch(); // Get the actual key code after the 0 or 224 prefix

            switch (key) {
            case 72: // Up arrow
                if (highlightIndex > 0) highlightIndex--;
                break;
            case 80: // Down arrow
                if (highlightIndex < fields.size()) highlightIndex++;
                break;
            case 75: // left arrow
                if (data.at(highlightIndex).second.caretPos > 0) data.at(highlightIndex).second.caretPos--;
                break;
            case 77: // right arrow
                if (data.at(highlightIndex).second.caretPos < data.at(highlightIndex).second.value.size()) data.at(highlightIndex).second.caretPos++;
                break;
            case 83: // delete
                if (data.at(highlightIndex).second.caretPos != data.at(highlightIndex).second.value.size()) data.at(highlightIndex).second.value.erase(data.at(highlightIndex).second.caretPos, 1);
                break;
            }



			break;

        case '\b':
			if (highlightIndex == data.size()) continue; // Ignore backspace if we are on the submit button
            if (data.at(highlightIndex).second.caretPos > 0) {
                data.at(highlightIndex).second.caretPos--;
                data.at(highlightIndex).second.value.erase(data.at(highlightIndex).second.caretPos, 1);
            }
            break;

        default:
			if (highlightIndex == data.size()) {
				if (key != '\r') continue; // Ignore any input if we are on the submit button
                
                if (!data.isValid()) break;
                return normalizeData(data);
                
			}
            data.at(highlightIndex).second.value.insert(data.at(highlightIndex).second.caretPos, 1, key);
			data.at(highlightIndex).second.caretPos++;
            break;
        }
    }
}