// Form.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "form.h"




int main()
{
    auto data = initForm({ 
        {"email", "default email" },
        {"password", "default password"}
        });

    for (const auto& pair : data) {
        std::cout << pair.first << " - " << pair.second << "\n";
    }


}
