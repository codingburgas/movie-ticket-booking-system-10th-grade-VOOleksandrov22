
#include <iostream>
#include "form.h"




int main()
{
    try {
        auto data = initForm({ 
                new Field({"email", "default email", "email placeholder"}),
                new Field({"password", "default password", "password placeholder"})
        });

        for (const auto& pair : data) {
            std::cout << pair.first->name << " - " << pair.second << "\n";
        }
	}
	catch (const int& code) {
        std::cout << "You exited";
	}
    

    


}
