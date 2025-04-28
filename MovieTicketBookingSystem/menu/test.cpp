#include <iostream>
#include <sstream>
#include <format>
#include "menu.h"
#include "../Colors/colors.h"
#include "../nlohmann/json.hpp"

using json = nlohmann::json;

#ifdef _WIN32
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
// Setting UTF-8 encoding.
bool enabled() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return false;

    SetConsoleOutputCP(CP_UTF8);

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return false;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
        return false;

    
    return true;
}
#endif


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

// FUNCTION WHICH CREATES REGULAR MENU ITEM ON GIVEN DATA
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

// FUNCTION WHICH CREATES HIGHLIGHTED MENU ITEM ON GIVEN DATA
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
    #ifdef _WIN32
    if (!enabled()) return 1;
    #endif

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
