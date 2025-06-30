#include "redirect.h"

#include "../Colors/colors.h"


void Redirect::print(const Redirect& r) {
	system("cls");
	switch (r.getType())
	{
	case MessageType::SUCCESS:
		std::cout << GREEN; break;
	case MessageType::WARNING:
		std::cout << ORANGE; break;
	case MessageType::ERROR:
		std::cout << RED; break;
	}

	std::cout << r.getMessage() << RESET;
}