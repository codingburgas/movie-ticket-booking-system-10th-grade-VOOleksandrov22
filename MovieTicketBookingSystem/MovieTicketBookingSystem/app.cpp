#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <functional>
#include "app.h"
#include "config.h"

//#include <cppconn/resultset.h>
#include "../nlohmann/json.hpp"
#include "../Utils/utils.h";
#include "../../db_cpp/database.h"
#include "../Date/date.h"
#include "session.h"

#include "../../password_input/PasswordInput/include/options.h"
#include "../../password_input/PasswordInput/include/passwordInput.h"

using RedirectFunction = std::function<void()>;


using json = nlohmann::json;

App::App() 
	: 
	db(new DbWrapper(config->url, config->username, config->password, config->schema, config->debugMode)),
	menu(new Menu())
{
	loginBySavedSession();
	mainLoop();
}






void App::mainLoop() {
	bool running = true;

	auto user = currentSession->getUser();

	std::map<std::string, RedirectFunction> redirects = {
		{"Log out", [this]() -> void { this->logout(); } },
		{"Exit",[]() -> void { exit(0); }},
	};

	std::vector<std::string> startingPageOptions = {
		"Something",
		"Log out",
		"Exit"
	};

	if (currentSession->getUser().getIsAdmin()) {
		auto elementBeforeLast = startingPageOptions.end() - 2;
		startingPageOptions.insert(elementBeforeLast, "Admin Options");
	}

	while (running) {
		size_t choice = menu->getChoice(startingPageOptions, "");
		if (redirects.find(startingPageOptions[choice]) != redirects.end()) {
			redirects[startingPageOptions[choice]]();
		}
		else std::cerr << "Unexpected error occured!" << std::endl;
	}
}

