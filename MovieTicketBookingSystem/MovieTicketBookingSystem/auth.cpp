#include <regex>

#include "app.h"
#include "session.h"
#include "user.h"
#include "../Database/database.h"
#include "../Date/date.h"

#include "../EmailHandler/email.h"

#include "../Form/form.h"
#include "../Validation/validation.h"

#include "../nlohmann/json.hpp"
using json = nlohmann::json;




const std::string passwordInstructions = R"(
- 8 to 64 characters.
- At least one uppercase letter, one lowercase, one digit, and one special character (e.g., !@#$%^&*).
)";


void App::loginBySavedSession() {
	try {
		json cache = Utils::File::readJsonFile(config->pathToCache);
		if (!cache.contains("sessionId")) {
			auth();
			return;
		}

		int sessionId = cache["sessionId"];

		auto sessions = DB::resultSetToVector(db->execute(
			"SELECT * FROM Session WHERE id = ? AND expiresAt > now()",
			{sessionId}
		));



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


void App::login() {
	FormResult input;
	try {
		input = initForm({
			new Field({"credential", "", "Enter your username or email", "Any quote symbols are forbidden", false}),
			new Field({"password", "", "Enter your password", "", true})
			}, "LOGIN");
	}
	catch (const int& code) {
		auth("The form submission was cancelled.\n\n");
	}


	bool success = Session::initSession(this, input.at(0).second, input.at(1).second);

	if (!success) {
		auth("Incorrect credentials entered\n\n");
	}

	loginBySavedSession();
	
}

void App::signup() {
	auto validateUsernameWithUniqueness = [this](const FormResult& formData, const size_t& fieldIndex) {
			validateUsername(formData, fieldIndex);
			const std::string& username = formData.at(fieldIndex).second;

			if (DB::resultSetToVector(db->execute(
				"SELECT id FROM User WHERE username = ?",
				{username})).size() > 0) {
				throw std::runtime_error("User with such username already exists");
			}
		};
	auto validateEmailWithUniqueness = [this](const FormResult& formData, const size_t& fieldIndex) {
		validateEmail(formData, fieldIndex);
		const std::string& email = formData.at(fieldIndex).second;

		if (DB::resultSetToVector(db->execute(
			"SELECT id FROM User WHERE email = ?",
			{email})).size() > 0) {
			throw std::runtime_error("User with such email already exists");
		}
		};

	FormResult input;
	try {
		input = initForm({
			new Field({"username", "", "Enter username", "Any quote symbols are forbidden", false, validateUsernameWithUniqueness}),
			new Field({"email", "@gmail.com", "Enter email", "Any quote symbols are forbidden", false, validateEmailWithUniqueness}),
			new Field({"password", "", "Enter password", passwordInstructions, true, validatePassword}),
			new Field({"password", "", "Reenter the password", "", true, passwordMatch}),
			new Field({"gender", "", "Enter your gender - Choose from predefined options", "M(Male), F(Female), O(Other), P(Prefer not to say)", false, validateGender}),
			new Field({"age", "", "Enter your age", "Must be a number between 0 and 150.", false, validateAge}),
			new Field({"phone", "", "Enter your phone number", "Digits, spaces, hyphens, and a leading plus sign are allowed.", false, validatePhone})
		}, "SIGNUP");
	}
	catch (const int& code) {
		auth("The form submission was cancelled.\n\n");
	}
	
	const std::string email = input.at(1).second;

	int verificationCode = Utils::generateRandomSixDigitNumber();
	send(config, email, "Verification code for movie ticket booking system", std::format("Your verification code is <b>{}</b>", verificationCode));

	std::string username = input.at(0).second;
	const std::string password = input.at(2).second;
	const std::string gender = input.at(4).second;
	const std::string age = input.at(5).second;
	const std::string phone = input.at(6).second;
	

	try {
		input = initForm({
			new Field({"Verification code", "", "Enter verification code", "6 digits", false, validateVerificationCode})
		}, "CHECK");
	}
	catch (const int& code) {
		auth("The form submission was cancelled.\n\n");
	}
	

	if (std::stoi(input.at(0).second) != verificationCode) {
		auth("Incorrect verification code entered\n\n");
		return;
	}

	int status = User::initUser(this, username, password, email, gender, age, phone);
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