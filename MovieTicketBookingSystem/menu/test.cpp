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
    if (data["data"]["isBlank"].get<bool>()) {
        return
            "          \n"
            "          \n"
            "          \n"
            "          \n"
            "          ";
    }

    std::string text = data["data"]["text"].get<std::string>();
    return std::format(
        "╭────────╮\n"
        "|        |\n"
        "|{:^8}|\n"
        "|{}|\n"
        "╰────────╯",
        text,
        data["data"]["isVIP"].get<bool>()
        ? std::format("{}{}{}", YELLOW, std::format("{:^8}", "VIP"), RESET)
        : std::format("{:^8}", "")
    );
}

// FUNCTION WHICH CREATES HIGHLIGHTED MENU ITEM ON GIVEN DATA
std::string highlight(json& data) {
    if (data["data"]["isBlank"].get<bool>()) {
        return
            "          \n"
            "          \n"
            "          \n"
            "          \n"
            "          ";
    }

    std::string text = data["data"]["text"].get<std::string>();
    return std::format(
        "{}╭────────╮{}\n"
        "{}|        |{}\n"
        "{}|{:^8}|{}\n"
        "{}|{:^8}|{}\n"
        "{}╰────────╯{}",
        RED, RESET, RED, RESET, RED, text, RESET, RED, 
        data["data"]["isVIP"].get<bool>()
        ? std::format("{}{}{}", YELLOW, std::format("{:^8}", "VIP"), RED)
        : std::format("{:^8}", ""),
        RESET, RED, RESET
    );
}

bool skipCheck(json& data) {
	return data["data"]["isBlank"].get<bool>();
}



int main() {
    #ifdef _WIN32
    if (!enabled()) return 1;
    #endif


    /*

    seat structure:

    {
        position: identifier for customer(e.g 12 or A5 or whatever)
        booked: 0 for false and 1 for true
        isVIP:  for false and 1 for true
        isBlank: 0 for false and 1 for true (just for the space)
    }

    */

    json data = json::parse(R"(
        [
        [
            {
                "isVisible": true,
                "data": {
                    "text": "A1",
                    "booked": false,
                    "isVIP": true,
                    "isBlank": false
                }
            },
            {
                "isVisible": true,
                "data": {
                    "text": "A2",
                    "booked": false,
                    "isVIP": true,
                    "isBlank": false
                }
            },
            {
                "isVisible": true,
                "data": {
                    "text": "A3",
                    "booked": true,
                    "isVIP": true,
                    "isBlank": false
                }
            }
        ],
        [
            {
                "isVisible": true,
                "data": {
                    "text": "B1",
                    "booked": false,
                    "isVIP": false,
                    "isBlank": false
                }
            },
            {
                "isVisible": true,
                "data": {
                    "text": "B2",
                    "booked": false,
                    "isVIP": false,
                    "isBlank": false
                }
            },
            {
                "isVisible": true,
                "data": {
                    "text": "",
                    "booked": false,
                    "isVIP": false,
                    "isBlank": true
                }
            }
        ]
    ]
    )");


   

    

    Menu menu;

    int itemSize[2] = { 9, 5 };



    try {
        menu.getChoice(data, highlight, regular, skipCheck, itemSize, "Choose an option:");
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what();
        int i; std::cin >> i;
    }
    

}
