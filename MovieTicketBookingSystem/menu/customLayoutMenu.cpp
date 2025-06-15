#include <iostream>
#include <string>
#include <sstream>
#include <conio.h>
#include <windows.h>
#include "menu.h"
#include "../Colors/colors.h"


std::string concatLinesFromVector(const std::vector<std::string>& strings, int itemSize[2]) {
    
    std::vector<std::istringstream> streams;
    for (const auto& str : strings) {
        streams.emplace_back(str);
    }

    std::string result;
    bool hasMoreLines = true;

    while (hasMoreLines) {
        hasMoreLines = false;
        std::string concatenatedLine;

        // Iterate through all streams and concatenate corresponding lines
        for (auto& stream : streams) {
            std::string line;
            if (std::getline(stream, line)) {
                hasMoreLines = true;
            }
            else {
                line = std::string(itemSize[0], ' ');
            }
            concatenatedLine += line;
        }

        if (hasMoreLines) {
            result += concatenatedLine + "\n";  // Add the concatenated line to the final result
        }
    }

    return result;
}

void displayChoices(
    json& data,
    const size_t* highlightPos,
    int itemSize[2],
    std::function<std::string(json&)> &getHighlightedItemAsString,
    std::function<std::string(json&)> &getRegularItemAsString,
    std::function<bool(json&)> skipCheck
) {
    for (size_t i = 0; i < data.size(); i++) {
        std::vector<std::string> row = {};
        for (size_t j = 0; j < data[i].size(); j++) {
            if (i == highlightPos[0] && j == highlightPos[1]) {
                //row = concatenateLineByLine(row, getHighlightedItemAsString(data[i][j])) + "";
                row.push_back(getHighlightedItemAsString(data[i][j]));
            }
            else {
                //row = concatenateLineByLine(row, getRegularItemAsString(data[i][j]));
                row.push_back(getRegularItemAsString(data[i][j]));
            }
        }
        std::cout << concatLinesFromVector(row, itemSize);

    }
}


bool setPosToNearestVisible(json& data, std::function<bool(json&)> &skipCheck,size_t pos[2], bool rightDirectionFirst = true) {
    const size_t& row = pos[0];

    auto directionToRight = [&]() -> bool {
        for (size_t i = pos[1] + 1; i < data[row].size(); i++) {
            if (i >= data[row].size()) break;
            if (!skipCheck(data[row][i])) pos[1] = i;
            return true;
        }
        return false;
        };

    auto directionToLeft = [&]() -> bool {
        for (size_t i = pos[1] - 1; i != std::string::npos; i--) {
            if (i == std::string::npos) break;
            if (!skipCheck(data[row][i])) pos[1] = i;
            return true;
        }
        return false;
        };

    bool res;
    if (rightDirectionFirst) {
        res = directionToRight();
        if (res) return true;
        res = directionToLeft();
        if (res) return true;
    }
    else {
        res = directionToLeft();
        if (res) return true;
        res = directionToRight();
        if (res) return true;
    }

    return false;
}


std::pair<size_t, size_t> Menu::getChoice(
    json& data,
    std::function<std::string(json&)>& getHighlightedItemAsString,
    std::function<std::string(json&)>& getRegularItemAsString,
    std::function<bool(json&)> skipCheck,
    int itemSize[2],
    std::string question
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

    while (true) {
        system("cls");
        std::cout << question << std::endl;
        displayChoices(data, highlightPos, itemSize, getHighlightedItemAsString, getRegularItemAsString, skipCheck);
        int key = _getch();

        switch (key) {
        case 0: // Handle special keys (arrows, function keys, etc.)
        case 224:
            key = _getch(); // Get the actual key code after the 0 or 224 prefix
            switch (key) {
            case 72: // Up arrow
                if (highlightPos[0] > 0) highlightPos[0]--;
                else highlightPos[0] = data.size() - 1;

                if (skipCheck(data[highlightPos[0]][highlightPos[1]])) {
                    bool found = false;
                    while (!found) {
                        found = setPosToNearestVisible(data, skipCheck, highlightPos);
                        if (!found) {
                            highlightPos[0]--;
                            if (highlightPos[0] < 0) highlightPos[0] = data.size() - 1;
                        }
                    }

                }

                break;
            case 80: // Down arrow
                if (highlightPos[0] < data.size() - 1) highlightPos[0]++;
                else highlightPos[0] = 0;

                if (skipCheck(data[highlightPos[0]][highlightPos[1]])) {
                    bool found = false;
                    while (!found) {
                        found = setPosToNearestVisible(data, skipCheck, highlightPos);
                        if (!found) {
                            highlightPos[0]++;
                            if (highlightPos[0] >= data.size() - 1) highlightPos[0] = 0;
                        }
                    }

                }
                break;
            case 77: // right arrow

                if (highlightPos[1] < data[highlightPos[0]].size() - 1) highlightPos[1]++;
                else {
                    highlightPos[1] = 0;
                }

                if (skipCheck(data[highlightPos[0]][highlightPos[1]])) {
                    bool found = false;
                    while (!found) {
                        found = setPosToNearestVisible(data, skipCheck, highlightPos);
                        if (!found) {
                            highlightPos[1]++;
                            if (highlightPos[1] >= data[highlightPos[0]].size() - 1) {
                                highlightPos[1] = 0;
                            }
                        }
                    }

                }
                break;

            case 75: // Left arrow
                if (highlightPos[1] > 0) highlightPos[1]--;
                else highlightPos[1] = data[highlightPos[0]].size() - 1;

                if (skipCheck(data[highlightPos[0]][highlightPos[1]])) {
                    bool found = false;
                    while (!found) {
                        found = setPosToNearestVisible(data, skipCheck, highlightPos, false);
                        if (!found) {
                            highlightPos[1]--;
                            if (highlightPos[1] < 0) highlightPos[1] = data[highlightPos[0]].size() - 1;
                        }
                    }

                }
                break;
            }

            break;

        case 13: // Enter
            system("cls");
            return { highlightPos[0], highlightPos[1] };
        }

    }
    
}

