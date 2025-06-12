// Form.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "form.h"




int main()
{
    auto data = initForm({ 
        {"email", "default email", "email placeholder"},
        {"password", "default password", "password placeholder"}
        });

    for (const auto& pair : data) {
        std::cout << pair.first->name << " - " << pair.second.first << "\n";
    }


}
