

#include "validation.h"

#include <iostream>
#include <string>
#include <regex>
#include "../Form/form.h"


const std::string specialCharacters = "!@#$%^&*()-_=+[]{}|;:,.<>?/~";


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
	//   password                   reentered password
	if (formData.at(2).second != formData.at(fieldIndex).second) {
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


void validateGender(const FormResult& formData, const size_t& fieldIndex) {
    const std::string& gender = formData.at(fieldIndex).second;
    if (gender != "M" && gender != "F" && gender != "O" && gender != "P") {
        throw std::runtime_error("Gender must be 'M', 'F', 'O', or 'P'.");
    }
}

void validateAge(const FormResult& formData, const size_t& fieldIndex) {
    const std::string& ageStr = formData.at(fieldIndex).second;

    for (char c : ageStr) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            throw std::runtime_error("Age must contain only digits.");
        }
    }

    try {
        int age = std::stoi(ageStr);
        if (age < 0 || age > 150) { // Reasonable age range for a human
            throw std::runtime_error("Age must be between 0 and 150.");
        }
    }
    catch (const std::out_of_range& e) {
        throw std::runtime_error("Age value is too large.");
    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error("Invalid age format.");
    }
}


void validatePhone(const FormResult& formData, const size_t& fieldIndex) {
    const std::string& phone = formData.at(fieldIndex).second;
    if (phone.empty()) {
        return;
    }

    for (char c : phone) {
        if (!std::isdigit(static_cast<unsigned char>(c)) &&
            c != ' ' && c != '-' && c != '+') {
            throw std::runtime_error("Phone number contains invalid characters. Use only digits, spaces, hyphens, or a leading plus sign.");
        }
    }
}