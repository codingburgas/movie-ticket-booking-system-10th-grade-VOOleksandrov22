#include "redirect.h"

#include "../Colors/colors.h"


void Redirect::print() const {
	system("cls");
	switch (getType())
	{
	case MessageType::SUCCESS:
		std::cout << GREEN; break;
	case MessageType::WARNING:
		std::cout << ORANGE; break;
	case MessageType::ERROR:
		std::cout << RED; break;
	}

	std::cout << getMessage() << RESET;
}