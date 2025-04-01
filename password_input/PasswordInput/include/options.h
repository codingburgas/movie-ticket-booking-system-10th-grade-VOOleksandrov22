//
// Created by VOOleksandrov22 on 1/22/2025.
//





#ifndef PASSWORDINPUT_OPTIONS_H
#define PASSWORDINPUT_OPTIONS_H

#include "iostream"



struct Options{
    std::string message = "Input password: ";
    bool doubleCheck = true;
    std::string doubleCheckMessage = "Reenter the password: ";
    bool (*validationCallback)(const std::string&) = nullptr;
    char replaceSymbolsWith = '*';
};



#endif //PASSWORDINPUT_OPTIONS_H
