#include <iostream>
#include "menu.h"
#include "../nlohmann/json.hpp"

using json = nlohmann::json;

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

    // Fix: Explicitly declare lambdas as function pointers
    auto highlight = [](json& data) -> std::string {
        return "h|" + data["isVisible"].dump() + "|" + data["data"]["text"].get<std::string>();
        };

    auto regular = [](json& data) -> std::string {
        return "r|" + data["isVisible"].dump() + "|" + data["data"]["text"].get<std::string>();
        };

    Menu menu; // Avoid unnecessary heap allocation
    menu.getChoice(data, highlight, regular, "Choose an option:");
}
