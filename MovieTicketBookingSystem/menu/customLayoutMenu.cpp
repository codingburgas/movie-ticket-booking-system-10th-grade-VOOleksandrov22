#include <iostream>
#include <sstream>
#include <conio.h>
#include <windows.h>
#include "menu.h"
#include "../Colors/colors.h"


std::string concatLinesFromVector(const std::vector<std::string>& strings) {
    
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
                line = "";
            }
            concatenatedLine += line;
        }

        if (hasMoreLines) {
            result += concatenatedLine + "\n";  // Add the concatenated line to the final result
        }
    }

    return result;
}

/*std::string concatenateLineByLine(const std::string& str1, const std::string str2) {
    std::istringstream stream1(str1);
    std::istringstream stream2(str2);

    std::string line1, line2;
    std::string result;

    while (std::getline(stream1, line1) || std::getline(stream2, line2)) {
        if (line1.empty()) line1 = ""; // Ensure correct concatenation
        if (line2.empty()) line2 = "";

        result += line1 + line2; // Concatenating lines and adding newline

        // Reset for next iteration
        line1.clear();
        line2.clear();
    }

    return result;
}*/


void displayChoices(
    json& data,
    const size_t* highlightPos,
    std::string(*getHighlightedItemAsString)(json&),
    std::string(*getRegularItemAsString)(json&)
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
        std::cout << concatLinesFromVector(row);

    }
}


bool setPosToNearestVisible(json& data, size_t pos[2], bool rightDirectionFirst = true) {
    const size_t& row = pos[0];

    auto directionToRight = [&]() -> bool {
        for (size_t i = pos[1] + 1; i < data[row].size(); i++) {
            if (data[row][i]["isVisible"]) pos[1] = i;
            return true;
        }
        return false;
        };

    auto directionToLeft = [&]() -> bool {
        for (size_t i = pos[1] - 1; i >= 0; i--) {
            if (data[row][i]["isVisible"]) pos[1] = i;
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

    data.erase(row);

    return false;
}


size_t* Menu::getChoice(
    json& data,
    std::string(*getHighlightedItemAsString)(json&),
    std::string(*getRegularItemAsString)(json&),
    int itemSize[2],
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

                set = true;
                break;
            }
        }
    }

    while (true) {
        system("cls");
        std::cout << question << std::endl;
        displayChoices(data, highlightPos, getHighlightedItemAsString, getRegularItemAsString);
        char key = _getch();


        switch (key) {
        case 72: // Up arrow
            if (highlightPos[0] > 0) highlightPos[0]--;
            else highlightPos[0] = data.size() - 1;

            if (!data[highlightPos[0]][highlightPos[1]]["isVisible"]) {
                bool found = false;
                while (!found) {
                    found = setPosToNearestVisible(data, highlightPos);
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

            if (!data[highlightPos[0]][highlightPos[1]]["isVisible"]) {
                bool found = false;
                while (!found) {
                    found = setPosToNearestVisible(data, highlightPos);
                    if (!found) {
                        highlightPos[0]++;
                        if (highlightPos[0] >= data.size() - 1) highlightPos[0] = 0;
                    }
                }

            }
            break;
        case 77: // right arrow
            
            if (highlightPos[1] < data[highlightPos[0]].size() - 1) highlightPos[1]++;
            else{
                highlightPos[1] = 0;
            }

            if (!data[highlightPos[0]][highlightPos[1]]["isVisible"]) {
                bool found = false;
                while (!found) {
                    found = setPosToNearestVisible(data, highlightPos);
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

            if (!data[highlightPos[0]][highlightPos[1]]["isVisible"]) {
                bool found = false;
                while (!found) {
                    found = setPosToNearestVisible(data, highlightPos, false);
                    if (!found) {
                        highlightPos[1]--;
                        if (highlightPos[1] < 0) highlightPos[1] = data[highlightPos[0]].size() - 1;
                    }
                }

            }
            break;
        case 13: // Enter
            system("cls");
            return highlightPos;

            break;

        default:
            break;
        }

    }
    
}

