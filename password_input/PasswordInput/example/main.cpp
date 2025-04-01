//
// Created by VOOleksandrov22 on 1/22/2025.
//

#include <iostream>
#include <conio.h>
#include <string>

#include "passwordInput.h"


int main(){
    // test options
    Options options = {
        .message = "Test message: ",
        .doubleCheck = true,
        .doubleCheckMessage = "Reenter the password second time: ",
        .validationCallback = [](const std::string &password) -> bool {
            return password.size() > 5;
        },
        .replaceSymbolsWith = '.',
    };

    std::string password;
    try {
        password = inputPassword(options);
    } catch(std::runtime_error &e) {
        int key = std::stoi(e.what());
        switch (key) {
            case 1: {
                std::cout << "Passwords didn't match! \n";
                break;
            }
            case 2: {
                std::cout << "Validation failed! \n";
                break;
            }
            default: {
                std::cout << "Unknown error occured! \n";
            }
        }

        return key;
    }


    std::cout << "Your password is " << password << "\n";
    std::cout << "Press any to exit: "; getch();
}