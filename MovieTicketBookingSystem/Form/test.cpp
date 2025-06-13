
#include <iostream>
#include "form.h"




int main()
{
    try {
        auto data = initForm({ 
                new Field({"email", "default email", "email placeholder", "test intruction", false}),
                new Field({"password", "default password", "password placeholder", "", true, [](const std::string& val) -> void { if (val.size() < 5) throw std::runtime_error("Password is too short!");  }}),
	        });

        for (const auto& pair : data) {
            std::cout << pair.first->name << " - " << pair.second << "\n";
        }
	}
	catch (const int& code) {
        std::cout << "You exited";
	}
    

   
}
