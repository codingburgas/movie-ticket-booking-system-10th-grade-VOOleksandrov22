#include <regex>

#include "app.h"
#include "session.h"
#include "user.h"
#include "../../db_cpp/database.h"
#include "../Date/date.h"

#include "../EmailHandler/email.h"

#include "../Form/form.h"

#include "../nlohmann/json.hpp"
using json = nlohmann::json;


#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 64

const std::string passwordInstructions = R"(
- 8 to 64 characters.
- At least one uppercase letter, one lowercase, one digit, and one special character (e.g., !@#$%^&*).
)";

const std::string specialCharacters = "!@#$%^&*()-_=+[]{}|;:,.<>?/~";


void App::loginBySavedSession() {
	try {
		json cache = Utils::File::readJsonFile(config->pathToCache);
		if (!cache.contains("sessionId")) {
			auth();
			return;
		}

		int sessionId = cache["sessionId"];

		std::string query = std::format("SELECT * FROM Session WHERE id = {} AND expiresAt > now()", sessionId);
		auto sessions = DB::resultSetToVector(db->db->execute(query));



		if (sessions.size() == 0) {
			auth();
			return;
		}

		currentSession = new Session(this, sessions[0]);
		std::cout << "Logged in as " << currentSession->getUser().getUsername() << std::endl;

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

void validatePassword(const FormResult& formData, const size_t& fieldIndex) {
	const std::string& password = formData.at(fieldIndex).second;
	if (password.length() < MIN_PASSWORD_LENGTH) {
		throw std::runtime_error("Password must be at least " + std::to_string(MIN_PASSWORD_LENGTH) + " characters long.");
	}

	if (password.length() > MAX_PASSWORD_LENGTH) {
		throw std::runtime_error("Password cannot exceed " + std::to_string(MAX_PASSWORD_LENGTH) + " characters.");
	}

	bool hasLower = false;
	bool hasUpper = false;
	bool hasDigit = false;
	bool hasSpecial = false;

	for (char c : password) {
		if (std::islower(static_cast<unsigned char>(c))) {
			hasLower = true;
		}
		else if (std::isupper(static_cast<unsigned char>(c))) {
			hasUpper = true;
		}
		else if (std::isdigit(static_cast<unsigned char>(c))) {
			hasDigit = true;
		}
		else if (specialCharacters.find(c) != std::string::npos) {
			hasSpecial = true;
		}
	}

	if (!hasLower) {
		throw std::runtime_error("Password must contain at least one lowercase letter.");
	}
	if (!hasUpper) {
		throw std::runtime_error("Password must contain at least one uppercase letter.");
	}
	if (!hasDigit) {
		throw std::runtime_error("Password must contain at least one digit.");
	}
	if (!hasSpecial) {
		throw std::runtime_error("Password must contain at least one special character (e.g., !@#$%^&*).");
	}
}

void passwordMatch(const FormResult& formData, const size_t& fieldIndex) {
	if (formData.at(formData.size() - 2).second != formData.at(fieldIndex).second) {
		throw std::runtime_error("Passwords do not match");
	}
}

void validateEmail(const FormResult& formData, const size_t& fieldIndex) {
	const std::regex email_pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
	const std::string& email = formData.at(fieldIndex).second;

	if (email.empty()) {
		throw std::runtime_error("Email cannot be empty.");
	}

	if (!std::regex_match(email, email_pattern)) {
		throw std::runtime_error("Invalid email format.");
	}
}

void validateVerificationCode(const FormResult& formData, const size_t& fieldIndex) {
	const std::string& code = formData.at(fieldIndex).second;

	if (code.length() != 6 || !std::all_of(code.begin(), code.end(), isdigit)) {
		throw std::runtime_error("Verification code must be a 6-digit number.");
	}
}

void App::login() {
	FormResult input = initForm({
		new Field({"username", "", "Enter your username", "Any quote symbols are forbidden", false}),
		new Field({"password", "", "Enter your password", "", true})
		}, "LOGIN");


	bool success = Session::initSession(this, input.at(0).second, input.at(1).second);

	if (!success) {
		auth("Incorrect credentials entered\n\n");
	}

	loginBySavedSession();
}

void App::signup() {
	FormResult input = initForm({
		new Field({"username", "", "Enter username", "Any quote symbols are forbidden", false}),
		new Field({"email", "@gmail.com", "Enter email", "Any quote symbols are forbidden", false, validateEmail}),
		new Field({"password", "", "Enter password", passwordInstructions, true, validatePassword}),
		new Field({"password", "", "Reenter the password", "", true, passwordMatch})
		}, "SIGNUP");

	std::string username = input.at(0).second;
	std::string email = input.at(1).second;
	std::string password = input.at(2).second;

	int verificationCode = Utils::generateRandomSixDigitNumber();
	send(config, email, "Verification code for movie ticket booking system", std::format("Your verification code is <b>{}</b>", std::to_string(verificationCode)));

	input = initForm({
		new Field({"Verification code", "", "Enter verification code", "6 digits", false, validateVerificationCode})
		}, "CHECK");

	if (std::stoi(input.at(0).second) != verificationCode) {
		auth("Incorrect verification code entered\n\n");
		return;
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