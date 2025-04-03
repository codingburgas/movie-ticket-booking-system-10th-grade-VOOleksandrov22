#include "app.h"
#include "session.h"
#include "user.h"
#include "../Utils/utils.h";
#include "../../db_cpp/database.h"
#include "../Date/date.h"

#include "../../password_input/PasswordInput/include/options.h"
#include "../../password_input/PasswordInput/include/passwordInput.h"

#include "../nlohmann/json.hpp"
using json = nlohmann::json;




void App::loginBySavedSession() {
	try {
		json cache = Utils::File::readJsonFile(config->pathToCache);
		if (!cache.contains("sessionId")) {
			auth();
			return;
		}

		int sessionId = cache["sessionId"];

		std::string fields = "*";
		std::string condition = "id=" + std::to_string(sessionId);


		auto* res = db->session->select(fields, condition);
		auto sessions = DB::resultSetToVector(res);


		if (sessions.size() == 0) {
			auth();
			return;
		}

		bool validSessionFound = false;
		int i = 0;
		do {
			if (Date(sessions[i]["expiresAt"]) > Date()) {
				validSessionFound = true;
				currentSession = new Session(this, sessions[i]);
				std::cout << "Logged in as " << currentSession->getUser().getUsername() << std::endl;
			}
			i++;
		} while (!validSessionFound && i < sessions.size());
		if (!validSessionFound) auth();

	}
	catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
}




void App::auth(std::string message) {
	std::vector<std::string> loginOptions = { "Login", "Signup", "Exit" };
	menu->setOptions(loginOptions);


	int choice = menu->getChoice(message + "Choose an option:");

	switch (choice) {
	case 0:
		login();
		break;
	case 1:
		signup();
		break;
	case 2:
		exit(1);
		break;
	};
}

void App::login() {
	std::cout << "Enter your username\n> ";
	std::string username; std::getline(std::cin, username);

	Options passwordInputOptions = {
		.message = "Enter password for user \"" + username + "\"\n> ",
		.doubleCheck = false,
		.replaceSymbolsWith = '*',
	};

	std::string password = inputPassword(passwordInputOptions);

	bool success = Session::initSession(this, username, password);

	if (!success) {
		auth("Incorrect credentials entered\n\n");
	}

	loginBySavedSession();
}

void App::signup() {
	std::cout << "Enter your username\n> ";
	std::string username; std::getline(std::cin, username);

	Options passwordInputOptions = {
		.message = "Enter password for user \"" + username + "\"\n> ",
		.doubleCheck = true,
		.doubleCheckMessage = "Reenter the password:\n> ",
		.replaceSymbolsWith = '*',
	};

	std::string password;
	try {
		password = inputPassword(passwordInputOptions);
	}
	catch (std::runtime_error& e) {
		int key = std::stoi(e.what());
		switch (key) {
		case 1: {
			auth("Passwords didn't match! \n");
			break;
		}
		case 2: {
			auth("Validation failed! \n");
			break;
		}
		default: {
			auth("Unknown error occured! \n");
			break;
		}
		}
	}

	int status = User::initUser(this, username, password);
	switch (status) {
	case 1:
		Session::initSession(this, username, password);

		loginBySavedSession();
		break;
	case 2:
		auth("User with such username already exists!\n");
		break;
	default:
		auth("Unknown error occured! \n");
		break;
	}

}


void App::logout() {
	delete currentSession;

	try {
		json cache = Utils::File::readJsonFile(config->pathToCache);
		if (!cache.contains("sessionId")) {
			return;
		}

		cache.erase("sessionId");

		bool successful = Utils::File::writeJsonToFile(config->pathToCache, cache);

		if (!successful) std::cerr << "Failed to save cache" << std::endl;

		auth();

	}
	catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}
}