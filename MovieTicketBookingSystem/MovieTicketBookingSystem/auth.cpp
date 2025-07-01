#include <regex>

#include "app.h"
#include "session.h"
#include "user.h"
#include "../Database/database.h"

#include "../EmailHandler/email.h"

#include "../Form/form.h"
#include "../Validation/validation.h"

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

		auto sessions = DB::resultSetToVector(db->execute(
			"SELECT * FROM Session WHERE id = ? AND expiresAt > now()",
			{sessionId}
		));



		if (sessions.size() == 0) {
			auth();
			return;
		}

		currentSession = new Session(this, sessions[0]);
		throw Redirect("Login was successful\n\n", [this]() -> void { this->mainMenu(); }, MessageType::SUCCESS);

	}
	catch (const std::exception& e) {
		throw Redirect("Login was unsuccessful\n\n", [this]() -> void { this->auth(); }, MessageType::ERROR);
	}
}




void App::auth() {
	std::vector<std::string> loginOptions = { "Login", "Signup", "Forgot my password", "Exit" };


	int choice = menu->getChoice(loginOptions, "Choose an option:");

	switch (choice) {
	case 0:
		login();
		break;
	case 1:
		signup();
		break;
	case 2:
		forgotPassword();
		break;
	case 3:
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
		throw Redirect("The form submission was cancelled.\n\n", [this]() -> void { this->auth(); }, MessageType::WARNING);
	}


	bool success = Session::initSession(this, input.at(0).second, input.at(1).second);

	if (!success) {
		throw Redirect("Incorrect credentials entered\n\n", [this]() -> void { this->auth(); }, MessageType::ERROR);
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
		throw Redirect("The form submission was cancelled.\n\n", [this]() -> void { this->auth(); }, MessageType::WARNING);
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
		throw Redirect("The form submission was cancelled.\n\n", [this]() -> void { this->auth(); }, MessageType::WARNING);
	}
	

	if (std::stoi(input.at(0).second) != verificationCode) {
		throw Redirect("Incorrect verification code\n\n", [this]() -> void { this->auth(); }, MessageType::ERROR);
		return;
	}

	int status = User::initUser(this, username, password, email, gender, age, phone);
	switch (status) {
	case 1:
		Session::initSession(this, username, password);

		loginBySavedSession();
		break;
	case 2:
		throw Redirect("User with such username exists.\n\n", [this]() -> void { this->auth(); }, MessageType::ERROR);
		break;
	default:
		throw Redirect("Unknown error occured.\n\n", [this]() -> void { this->auth(); }, MessageType::ERROR);
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

		if (!successful) {
			throw Redirect("Failed to save cache\n\n", [this]() -> void { this->auth(); }, MessageType::ERROR);
		}

		throw Redirect("Logout was successful\n\n", [this]() -> void { this->auth(); }, MessageType::SUCCESS);


	}
	catch (const std::exception& e) {
		throw Redirect("Failed to save cache\n\n", [this]() -> void { this->auth(); }, MessageType::ERROR);
	}
}


void App::forgotPassword() {
	FormResult input;
	try {
		input = initForm({
			new Field({"credential", "", "Enter your username or email", "Any quote symbols are forbidden", false}),
			}, "SUBMIT");
	}
	catch (const int& code) {
		throw Redirect("The form submission was cancelled.\n\n", [this]() -> void { this->auth(); }, MessageType::WARNING);
	}
	std::string email, credential = input.at(0).second;
	try {
		validateEmailStr(credential);
		email = credential;
	}
	catch (...) {
		email = DB::resultSetToVector(db->execute(
			"SELECT email FROM User WHERE username = ?",
			{ credential }
		))[0]["email"];
	}
	
	int verificationCode = Utils::generateRandomSixDigitNumber();
	send(config, email, "Verification code for password change for movie ticket booking system", std::format("Your verification code for password change is <b>{}</b>", verificationCode));

	try {
		input = initForm({
			new Field({"Verification code", "", "Enter verification code", "6 digits", false, validateVerificationCode})
			}, "CHECK");
	}
	catch (const int& code) {
		throw Redirect("The form submission was cancelled.\n\n", [this]() -> void { this->auth(); }, MessageType::WARNING);
		return;
	}

	if (std::stoi(input.at(0).second) != verificationCode) {
		throw Redirect("Incorrect verification code entered.\n\n", [this]() -> void { this->auth(); }, MessageType::WARNING);
		return;
	}

	try {
		input = initForm({
			new Field({"password", "", "Enter new password", passwordInstructions, true, validatePassword}),
			new Field({"password confirmation", "", "Reenter the password", "", true, [](const FormResult& formData, const size_t& fieldIndex) -> void {
				if (formData.at(1).second != formData.at(fieldIndex).second) {
					throw std::runtime_error("Passwords do not match");
				}
				}})
			}, "CHANGE");
	}
	catch (const int& code) {
		throw Redirect("The form submission was cancelled.\n\n", [this]() -> void { this->auth(); }, MessageType::WARNING);
		return;
	}

	db->execute("UPDATE User SET password = ? WHERE email = ?", { input.at(0).second, email });
	throw Redirect("Password changed successfully! You can now login with your new password.\n\n", [this]() -> void { this->auth(); }, MessageType::SUCCESS);
}