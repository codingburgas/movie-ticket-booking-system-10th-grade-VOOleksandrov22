#include "form.h"
#include "../Colors/colors.h"
#include "../Utils/utils.h"


#include <conio.h>
#include <iomanip>
#include <string>
#include <format>
#include <iterator>


#define WIDTH 70
#define SUBMIT_BUTTON_WIDTH 30


std::string center(const std::string str, const bool containsLargeChars = false, const int width = SUBMIT_BUTTON_WIDTH) {
	const int size = containsLargeChars ? str.size() / 3 : str.size();
	if (size >= width) return str;
	int padding = (width - size) / 2;
	return std::string(padding, ' ') + str + std::string(width - size - padding, ' ');
}


void displayForm(EnteredData& data, const int& highlightIndex) {


    int currentIndex = 0;
    for (const auto& pair : data) {
        if (currentIndex == highlightIndex) {
            std::cout << YELLOW;
        }
        // header
        std::cout << "╭" << Utils::String::toUppercase(pair.first->name) << Utils::String::stringRepeater("─", WIDTH - pair.first->name.size()) << "╮\n";


		const std::string& value = pair.second.first;
        const size_t& caretPos = pair.second.second;

		const std::string strToPrint = 
            ((currentIndex == highlightIndex) ? value.substr(0, caretPos) + "|" + value.substr(caretPos) : value)
            +
            ((value == "") ? pair.first->placeholder : "");
        

        for (size_t i = 0; i < strToPrint.size(); i += WIDTH - 2) {
            std::vector<std::string> splittedLine = Utils::String::split(strToPrint.substr(i, WIDTH - 2), "\r");
            //lines.insert(lines.end(), splittedLine.begin(), splittedLine.end());

            for (const std::string& line : splittedLine) {
                std::cout << "| " << std::left << std::setw(WIDTH - 2) << line << " |\n";
            }
        }


        

        // footer
        std::cout << "╰" << Utils::String::stringRepeater("─", WIDTH) << "╯\n\n";

        if (currentIndex == highlightIndex) {
            std::cout << RESET;
        }

        currentIndex++;
    }

    if (currentIndex == highlightIndex) {
		std::cout << YELLOW;
    }

	std::cout << center("╭" + Utils::String::stringRepeater("─", SUBMIT_BUTTON_WIDTH) + "╮", true, WIDTH) << "\n"
              << center("|" + center(std::string("SUBMIT")) + "|", false, WIDTH) << "\n"
              << center("╰" + Utils::String::stringRepeater("─", SUBMIT_BUTTON_WIDTH) + "╯", true, WIDTH);

    if (currentIndex == highlightIndex) {
        std::cout << RESET;
    }

}

void fillInInitialData(EnteredData& data, const std::vector<Field*>& fields) {
    for (auto& field : fields) {
        data[field] = { field->defaultValue, field->defaultValue.size() };
    }
}


class HighlightData : public std::pair<int, EnteredData::iterator> {
private:
	EnteredData* dataContainer = nullptr;

    bool firstInRange() {
		return first >= 0 && first < dataContainer->size();
    }
public:
	HighlightData(int index, EnteredData& data) : dataContainer(&data) {
		first = index;
		second = data.begin();
        std::advance(second, index);
    }

	void inc() {
		if (first < dataContainer->size() - 1) std::advance(second, 1);
        first++;
	}

	void dec() {
		if (first != dataContainer->size()) std::advance(second, -1);
        first--;
	}

    void set1st(int val, bool updateSecond = true) {
        if (updateSecond) {
            if (val < 0 || val >= dataContainer->size()) return;
		    first = val;
		    second = dataContainer->begin();
		    std::advance(second, first);
        }
        else {
			first = val;
        }
        
    }
};

EnteredData initForm(const std::vector<Field*>&& fields) {

    EnteredData data = {};
    fillInInitialData(data, fields);
    //        highlight pos                       value        cursor pos
    // std::pair<int, std::pair<Field*, std::pair<std::string, size_t>>>
    HighlightData highlightData = { 0, data };

    while (true) {
        system("cls");
        displayForm(data, highlightData.first);
        int key = _getch();


		switch (key) {
		case 27: // Escape key
            throw 1;
		case 0: // Handle special keys (arrows, function keys, etc.)
        case 224:
			key = _getch(); // Get the actual key code after the 0 or 224 prefix

            switch (key) {
            case 72: // Up arrow
                if (highlightData.first > 0) highlightData.dec();
                break;
            case 80: // Down arrow
                if (highlightData.first < fields.size()) highlightData.inc();
                break;
            case 75: // left arrow
                if (highlightData.second->second.second > 0) highlightData.second->second.second--;
                break;
            case 77: // right arrow
                if (highlightData.second->second.second < highlightData.second->second.first.size()) highlightData.second->second.second++;
                break;
            case 83: // delete
                if (highlightData.second->second.second != highlightData.second->second.first.size()) highlightData.second->second.first.erase(highlightData.second->second.second, 1);
                break;
            }



			break;

        case '\b':
            if (highlightData.second->second.second > 0) {
                highlightData.second->second.second--;
                highlightData.second->second.first.erase(highlightData.second->second.second, 1);
            }
            break;

        default:
			if (key == '\r' && highlightData.first == data.size()) {
				return data;
			}
            highlightData.second->second.first.insert(highlightData.second->second.second, 1, key);
			highlightData.second->second.second++;
            break;
        }
    }
}