#include <iostream>
#include <sstream>
#include <format>
#include "menu.h"
#include "../Colors/colors.h"
#include "../nlohmann/json.hpp"

using json = nlohmann::json;


void makeStringCertainLength(std::string& string, int size) {
    int diff = size - string.size();

    if (diff == 0) return;

    if (diff > 0) {
        int atBegin = diff / 2;
        string = std::string(atBegin, ' ') + string + std::string(diff - atBegin, '*');
    }
    else {
        string = string.substr(0, size - 1) + "…";
    }
}

std::string regular(json& data) {
    std::string text = data["data"]["text"].get<std::string>();
    return std::format(
        "╭───────╮\n"
        "|       |\n"
        "|{:^7}|\n"
        "|       |\n"
        "╰───────╯",
        text
    );
}

std::string highlight(json& data) {
    std::string text = data["data"]["text"].get<std::string>();
    return std::format(
        "{}╭───────╮{}\n"
        "{}|       |{}\n"
        "{}|{:^7}|{}\n"
        "{}|       |{}\n"
        "{}╰───────╯{}",
        RED, RESET, RED, RESET, RED, text, RESET, RED, RESET, RED, RESET
    );
}



int main() {
    json data = json::parse(R"(
    [
        [
            {
                "isVisible": true,
                "data": {
                    "text": "0:0"
                }
            },
            {
                "isVisible": true,
                "data": {
                    "text": "0:1"
                }
            },
            {
                "isVisible": true,
                "data": {
                    "text": "0:2"
                }
            }
        ], 
        [
            {
                "isVisible": false,
                "data": {
                    "text": "1:0"
                }
            },
            {
                "isVisible": true,
                "data": {
                    "text": "1:1"
                }
            },
            {
                "isVisible": false,
                "data": {
                    "text": "1:2"
                }
            }
        ]
    ])");


   

    

    Menu menu;

    int itemSize[2] = { 9, 5 };
    try {
        menu.getChoice(data, highlight, regular, itemSize, "Choose an option:");
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what();
        int i; std::cin >> i;
    }
    

}
