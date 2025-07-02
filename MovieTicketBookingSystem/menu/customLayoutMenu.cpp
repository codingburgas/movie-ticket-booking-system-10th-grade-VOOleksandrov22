#include <iostream>
#include <string>
#include <sstream>
#include <conio.h>
#include <windows.h>
#include "menu.h"
#include "../Colors/colors.h"
#include "../Utils/utils.h"
#include <array>

const std::string SEPARATOR_BETWEEN_ITEMS = " ";

std::string concatLinesFromVector(const std::vector<std::string>& strings, const std::array<int, 2>& itemSize) {
    
    std::vector<std::istringstream> streams;
    for (const auto& str : strings) {
        streams.emplace_back(str);
    }

    std::string result;
    bool hasMoreLines = true;

    while (hasMoreLines) {
        hasMoreLines = false;
        std::string concatenatedLine = SEPARATOR_BETWEEN_ITEMS;

        // Iterate through all streams and concatenate corresponding lines
        for (auto& stream : streams) {
            std::string line;
            if (std::getline(stream, line)) {
                hasMoreLines = true;
            }
            else {
                line = std::string(itemSize[0], ' ');
            }
            concatenatedLine += SEPARATOR_BETWEEN_ITEMS + line;
        }

        if (hasMoreLines) {
            result += concatenatedLine + "\n";  // Add the concatenated line to the final result
        }
    }

    return result;
}

std::string getYAxisString(const std::string& index, const int& itemHeight, const size_t& longestIndexSize) {
    std::string res = Utils::String::stringRepeater("─", longestIndexSize) + "╮\n";
    size_t middle = (itemHeight - 2) / 2;

    res += Utils::String::stringRepeater(std::string(longestIndexSize, ' ') + "│\n", middle);

    res += Utils::String::center(index, longestIndexSize) + "│\n";

    res += Utils::String::stringRepeater(std::string(longestIndexSize, ' ') + "│\n", itemHeight - 2 - middle - 1);

    res += Utils::String::stringRepeater("─", longestIndexSize) + "╯";

    return res;
}

std::string displayChoices(
    json& data,
    const size_t* highlightPos,
    const std::array<int, 2>& itemSize,
    std::function<std::string(json&)> &getHighlightedItemAsString,
    std::function<std::string(json&)> &getRegularItemAsString,
    std::function<bool(json&)> skipCheck,
    const bool& axesVisible
) {
    std::string output;
    output.reserve(itemSize[0] * data.size() * itemSize[1] * data[0].size());


    size_t longestYIndexSize;
    for (size_t i = 0; i < data.size(); i++) {
        std::vector<std::string> row;
        if (axesVisible) {
            longestYIndexSize = std::to_string(data.size()).size();
            row = { getYAxisString(std::to_string(i + 1), itemSize[1] - 1, longestYIndexSize) + "\n" + std::string(longestYIndexSize + 1, ' ') };
        }
        else row = {};

        for (size_t j = 0; j < data[i].size(); j++) {
            if (i == highlightPos[0] && j == highlightPos[1]) {
                row.push_back(getHighlightedItemAsString(data[i][j]));
            }
            else {
                row.push_back(getRegularItemAsString(data[i][j]));
            }
        }
        output += concatLinesFromVector(row, itemSize);
    }
    
	if (axesVisible) {
		size_t maxRowLength = 0;
		for (const json& row : data){
			if (row.size() > maxRowLength) maxRowLength = row.size();
        }

        output += std::string(longestYIndexSize + 3, ' ');

		for (size_t i = 0; i < maxRowLength; i++) {
            output += 
                std::string(SEPARATOR_BETWEEN_ITEMS.size(), ' ')
                + "╰" + Utils::String::center(std::to_string(i+1), itemSize[0] - 2, false, "─")
                + "╯";
		}
	}

	return output;
}


enum Direction {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

void movePosInDirection(size_t pos[2], json& data, const Direction& direction) {
    std::string debugInfo = std::format("({}, {})\n\n{}", pos[0], pos[1], data[pos[0]][pos[1]].dump(4));

    switch (direction) {
    case Direction::LEFT:
        if (pos[1] > 0) {
            pos[1]--;
        }
        else {
			if (pos[0] == 0) pos[0] = data.size() - 1; // Wrap around to the last row if at the first row
			else pos[0]--; // Move to the previous row
            pos[1] = data[pos[0]].size() - 1; 
        }
        break;
   
    case Direction::RIGHT:
        if (pos[1] < data[pos[0]].size() - 1) {
            pos[1]++;
        }
        else {
			if (pos[0] == data.size() - 1) pos[0] = 0; // Wrap around to the first row if at the last row
			else pos[0]++; // Move to the next row
            pos[1] = 0;
        }
        break;
   
    case Direction::UP:
        if (pos[0] > 0) {
            pos[0]--;
        }
        else {
            pos[0] = data.size() - 1;
        }
        break;
   
    case Direction::DOWN:
        if (pos[0] < data.size() - 1) {
            pos[0]++;
        }
        else {
            pos[0] = 0;
        }
        break;
    }

}


void setPosToNearestValid(json& data, std::function<bool(json&)> &skipCheck, size_t pos[2], const Direction& directionToMove) {
	size_t startingPos[2] = { pos[0], pos[1] };
    do {
		movePosInDirection(pos, data, directionToMove);
        if (!skipCheck(data[pos[0]][pos[1]])) {
            return;
        }
    } while (pos[0] != startingPos[0] || pos[1] != startingPos[1]);
    
}


std::pair<size_t, size_t> Menu::getChoice(
    json& data,
    std::function<std::string(json&)>& getHighlightedItemAsString,
    std::function<std::string(json&)>& getRegularItemAsString,
    std::function<bool(json&)> skipCheck,
    const std::array<int, 2>& itemSize,
    const std::string& question,
    bool axesVisible
) {

    // find first enabled
    size_t highlightPos[2] = { 0, 0 };
    bool set = false;
    for (size_t i = 0; i < data.size(); i++) {
        if (set) break;
        for (size_t j = 0; j < data[i].size(); j++) {
            if (!skipCheck(data[i][j])) {
                highlightPos[0] = i;
                highlightPos[1] = j;

                set = true;
                break;
            }
        }
    }

	if (!set) {
		throw std::runtime_error("No valid item found in the data.");
	}

    while (true) {
        system("cls");
        std::cout << question << std::endl;
        std::cout << displayChoices(data, highlightPos, itemSize, getHighlightedItemAsString, getRegularItemAsString, skipCheck, axesVisible);
        int key = _getch();

        switch (key) {
        case 0: // Handle special keys (arrows, function keys, etc.)
        case 224:
            key = _getch(); // Get the actual key code after the 0 or 224 prefix
            switch (key) {
            case 72: // Up arrow
				setPosToNearestValid(data, skipCheck, highlightPos, Direction::UP);

                break;
            case 80: // Down arrow
				setPosToNearestValid(data, skipCheck, highlightPos, Direction::DOWN);
                break;
            case 77: // right arrow
				setPosToNearestValid(data, skipCheck, highlightPos, Direction::RIGHT);
                break;

            case 75: // Left arrow
				setPosToNearestValid(data, skipCheck, highlightPos, Direction::LEFT);
                break;
            }

            break;

        case 13: // Enter

            system("cls");
            return { highlightPos[0], highlightPos[1] };
        }

    }
    
}

