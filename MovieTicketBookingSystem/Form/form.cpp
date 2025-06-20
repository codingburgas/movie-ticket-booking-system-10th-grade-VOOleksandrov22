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
#include <array>


#define WIDTH 70
#define SUBMIT_BUTTON_WIDTH 30

std::string SUBMIT_BUTTON_TEXT;


struct AdditionalFieldData {
    std::string value;
    std::pair<size_t, long> caretPos;
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
        result.emplaceOrUpdate(pair.first, pair.second.value);
    }
    return result;
}


void moveWindowUntilHighlightIsVisible(const int& highlightPosY, SMALL_RECT& windowRectBeforeRefresh, HANDLE& hConsole, CONSOLE_SCREEN_BUFFER_INFO& csbi) {
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    const int windowHeight = csbi.srWindow.Bottom - csbi.srWindow.Top;

    if (highlightPosY > windowRectBeforeRefresh.Bottom) {
		windowRectBeforeRefresh.Bottom = highlightPosY + 3;
        windowRectBeforeRefresh.Top = windowRectBeforeRefresh.Bottom - windowHeight;
    }
    else if (highlightPosY < windowRectBeforeRefresh.Top) {
		windowRectBeforeRefresh.Top = highlightPosY - 5;
		windowRectBeforeRefresh.Bottom = windowRectBeforeRefresh.Top + windowHeight;
    }
}


void displayForm(EnteredData& data, const int& highlightIndex, int& highlightPosY, HANDLE& hConsole, CONSOLE_SCREEN_BUFFER_INFO& csbi) {

    std::cout << "Press \"esc\" to cancel form submission\nPress \"Ctrl + H\" to enable privacy mode\n\n";

    int currentIndex = 0;
    for (const auto& pair : data) {
        if (currentIndex == highlightIndex) {
            std::cout << YELLOW;
        }
        // header
        std::cout << "╭" << Utils::String::toUppercase(pair.first->name) << Utils::String::stringRepeater("─", WIDTH - pair.first->name.size()) << "╮\n";


        const std::string& value = pair.second.value;
        const size_t& caretPos = pair.second.caretPos.first;
		const long& selectionDirection = pair.second.caretPos.second;

        std::string strToPrint = (pair.second.hidden) ? std::string(value.size(), '*') : value;

        if (value == "") strToPrint += pair.first->placeholder;


        if (currentIndex != highlightIndex) {
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            highlightPosY = csbi.dwCursorPosition.Y;


            for (size_t i = 0; i < strToPrint.size(); i += WIDTH - 2) {
                std::vector<std::string> splittedLine = Utils::String::split(strToPrint.substr(i, WIDTH - 2), "\r");

                for (const std::string& line : splittedLine) {
                    std::cout << "| " << std::left << std::setw(WIDTH - 2) << line << " |\n";
                }
            }
        }
        else {
			size_t printedOnLine = 0;
			std::cout << "| ";

            for (size_t i = 0; i < strToPrint.size(); i += 1) {
                if (i == caretPos || i == caretPos + selectionDirection) {
                    std::cout << RESET << BG_WHITE << MAGENTA << '|';
                    printedOnLine++;
                }
                if (strToPrint[i] == '\n' || strToPrint[i] == '\r') {
                    std::cout << std::string(WIDTH - 2 - printedOnLine, ' ') << RESET << YELLOW << " |\n| ";
					printedOnLine = 0;
                    continue;
                }
				if (printedOnLine >= WIDTH - 2) {
					std::cout << RESET << YELLOW << " |\n| ";
					printedOnLine = 0;
                    continue;
				}
                
                if ((i >= caretPos + selectionDirection && i <= caretPos) || (i >= caretPos && i <= caretPos + selectionDirection)) {
					std::cout << RESET << BG_WHITE << MAGENTA << strToPrint[i];
                    printedOnLine++;
                    continue;
                }
				std::cout << RESET << YELLOW << strToPrint[i];
                printedOnLine++;
            }
            if (caretPos == strToPrint.size() || caretPos + selectionDirection == strToPrint.size()) {
                std::cout << RESET << BG_WHITE << MAGENTA << "|" << RESET;
				printedOnLine++;
            }
            std::cout << std::string(WIDTH - 2 - printedOnLine, ' ') << RESET << YELLOW << " |\n";
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
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        highlightPosY = csbi.dwCursorPosition.Y;
    }

}

void fillInInitialData(EnteredData& data, const std::vector<Field*>& fields) {
    for (auto& field : fields) {
        data[field] = {
            field->defaultValue,
            { field->defaultValue.size(), 0 },
            "",
			field->isHidden
        };
    }
}


FormResult initForm(const std::vector<Field*>&& fields, const std::string&& submitButtonText) {

	SUBMIT_BUTTON_TEXT = submitButtonText;

    EnteredData data = {};
    fillInInitialData(data, fields);
   

	int highlightIndex = 0;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (hConsole == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Unable to obtain console handle");
    }

	GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT windowRectBeforeRefresh;
	windowRectBeforeRefresh = csbi.srWindow;

    while (true) {
        system("cls");

        int highlightPosY;

        displayForm(data, highlightIndex, highlightPosY, hConsole, csbi);
		moveWindowUntilHighlightIsVisible(highlightPosY, windowRectBeforeRefresh, hConsole, csbi);
		SetConsoleWindowInfo(hConsole, TRUE, &windowRectBeforeRefresh);

        int key = _getch();


        bool isCtrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool isShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

		if (isCtrlPressed) {
            if (highlightIndex == data.size()) continue;
            switch (key) {
			case 8: // Ctrl + H
                data.at(highlightIndex).second.hidden = !data.at(highlightIndex).second.hidden;
                break;
            }
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            windowRectBeforeRefresh = csbi.srWindow;
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
                if (!isShiftPressed) {
                    if (data.at(highlightIndex).second.caretPos.first > 0) data.at(highlightIndex).second.caretPos.first--;
                }
                else {
                    if (data.at(highlightIndex).second.caretPos.first + data.at(highlightIndex).second.caretPos.second - 1 >= 0) data.at(highlightIndex).second.caretPos.second--;
                }
                
                break;
            case 77: // right arrow
                if (!isShiftPressed) {
                    if (data.at(highlightIndex).second.caretPos.first < data.at(highlightIndex).second.value.size()) data.at(highlightIndex).second.caretPos.first++;
                }
                else {
                    if (data.at(highlightIndex).second.caretPos.first + data.at(highlightIndex).second.caretPos.second + 1 <= data.at(highlightIndex).second.value.size()) data.at(highlightIndex).second.caretPos.second++;
                }
                break;
            case 83: // delete
                if (data.at(highlightIndex).second.caretPos.first != data.at(highlightIndex).second.value.size()) data.at(highlightIndex).second.value.erase(data.at(highlightIndex).second.caretPos.first, 1);
                break;
            }

			break;

        case '\b':
            if (highlightIndex == data.size()) {
                GetConsoleScreenBufferInfo(hConsole, &csbi);
                windowRectBeforeRefresh = csbi.srWindow;
                continue; // Ignore backspace if we are on the submit button
            }
            if (data.at(highlightIndex).second.caretPos.first > 0) {
                data.at(highlightIndex).second.caretPos.first--;
                data.at(highlightIndex).second.value.erase(data.at(highlightIndex).second.caretPos.first, 1);
            }
            break;

        default:
			if (highlightIndex == data.size()) {
                if (key != '\r') {
                    GetConsoleScreenBufferInfo(hConsole, &csbi);
                    windowRectBeforeRefresh = csbi.srWindow;
                    continue;
                }
                
                if (!data.isValid()) break;
                return normalizeData(data);
                
			}
            data.at(highlightIndex).second.value.insert(data.at(highlightIndex).second.caretPos.first, 1, key);
			data.at(highlightIndex).second.caretPos.first++;
            break;
        }

        GetConsoleScreenBufferInfo(hConsole, &csbi);
        windowRectBeforeRefresh = csbi.srWindow;
    }
}