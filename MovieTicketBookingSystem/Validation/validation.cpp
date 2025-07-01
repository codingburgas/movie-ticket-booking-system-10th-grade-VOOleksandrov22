

#include "validation.h"

#include <iostream>
#include <string>
#include <format>
#include <regex>
#include "../Form/form.h"

const size_t MIN_CARD_NUMBER_LENGTH = 13;
const size_t MAX_CARD_NUMBER_LENGTH = 19;
const int MAX_FUTURE_CARD_YEARS = 15;
const std::string specialCharacters = "!@#$%^&*()-_=+[]{}|;:,.<>?/~";


ValidationFunction validAll(const std::vector<ValidationFunction>& conditions) {
	return [&conditions](const FormResult& formData, const size_t& fieldIndex) -> void {
		for (const auto& condition : conditions) {
			condition(formData, fieldIndex);
		}
		};
}


void validateUsernameStr(const std::string& value) {
	const std::string& username = value;

	if (username.length() < MIN_USERNAME_LENGTH) {
		throw std::runtime_error("Username must be at least " + std::to_string(MIN_USERNAME_LENGTH) + " characters long.");
	}
	if (username.length() > MAX_USERNAME_LENGTH) {
		throw std::runtime_error("Username cannot exceed " + std::to_string(MAX_USERNAME_LENGTH) + " characters.");
	}

	if (!username.empty() && (std::isspace(static_cast<unsigned char>(username.front())) || std::isspace(static_cast<unsigned char>(username.back())))) {
		throw std::runtime_error("Username cannot start or end with a space.");
	}
}



void validatePasswordStr(const std::string& value) {
	const std::string& password = value;
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

void validateEmailStr(const std::string& value) {
	const std::regex email_pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
	const std::string& email = value;

	if (email.empty()) {
		throw std::runtime_error("Email cannot be empty.");
	}

	if (!std::regex_match(email, email_pattern)) {
		throw std::runtime_error("Invalid email format.");
	}
}

void validateVerificationCodeStr(const std::string& value) {
	const std::string& code = value;

	if (code.length() != 6 || !std::all_of(code.begin(), code.end(), isdigit)) {
		throw std::runtime_error("Verification code must be a 6-digit number.");
	}
}


void validateGenderStr(const std::string& value) {
    const std::string& gender = value;
    if (gender != "M" && gender != "F" && gender != "O" && gender != "P") {
        throw std::runtime_error("Gender must be 'M', 'F', 'O', or 'P'.");
    }
}

void validateAgeStr(const std::string& value) {
    const std::string& ageStr = value;

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


void validatePhoneStr(const std::string& value) {
    const std::string& phone = value;
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


/**
 * @brief Validates a credit card number.
 * Checks for length and ensures it contains only digits.
 * @param formData The collected form data.
 * @param fieldIndex The index of the card number field in formData.
 * @throws std::runtime_error if the card number fails any validation rule.
 */
void validateCardNumberStr(const std::string& value) {
	const std::string& cardNumber = value;

	if (cardNumber.empty()) {
		throw std::runtime_error("Card number cannot be empty.");
	}

	if (cardNumber.length() < MIN_CARD_NUMBER_LENGTH || cardNumber.length() > MAX_CARD_NUMBER_LENGTH) {
		throw std::runtime_error("Card number must be between " + std::to_string(MIN_CARD_NUMBER_LENGTH) + " and " + std::to_string(MAX_CARD_NUMBER_LENGTH) + " digits long.");
	}

	// Ensure all characters are digits
	if (!std::all_of(cardNumber.begin(), cardNumber.end(), [](char c) { return std::isdigit(static_cast<unsigned char>(c)); })) {
		throw std::runtime_error("Card number must contain only digits.");
	}
}

/**
 * @brief Validates the card holder's name.
 * Checks for non-empty string and allowed characters (letters, spaces, hyphens, periods).
 * @param formData The collected form data.
 * @param fieldIndex The index of the card holder field in formData.
 * @throws std::runtime_error if the card holder name fails any validation rule.
 */
void validateCardHolderStr(const std::string& value) {
	const std::string& cardHolder = value;

	if (cardHolder.empty()) {
		throw std::runtime_error("Card holder name cannot be empty.");
	}

	// Allow letters, spaces, hyphens, and periods (common for names)
	if (!std::all_of(cardHolder.begin(), cardHolder.end(), [](char c) {
		return std::isalpha(static_cast<unsigned char>(c)) ||
			std::isspace(static_cast<unsigned char>(c)) ||
			c == '-' || c == '.' || c == '\'';
		})) {
		throw std::runtime_error("Card holder name contains invalid characters. Only letters, spaces, hyphens, periods, and apostrophes are allowed.");
	}

	if (cardHolder.starts_with(' ') || cardHolder.ends_with(' ')) {
		throw std::runtime_error("Card holder name cannot start or end with a space.");
	}
}


/**
 * @brief Validates the CVC/CVV code.
 * Checks for length (3 or 4 digits) and ensures it contains only digits.
 * @param formData The collected form data.
 * @param fieldIndex The index of the CVC field in formData.
 * @throws std::runtime_error if the CVC fails any validation rule.
 */
void validateCVCStr(const std::string& value) {
	const std::string& cvc = value;

	if (cvc.empty()) {
		throw std::runtime_error("CVC cannot be empty.");
	}

	if (cvc.length() != 3 && cvc.length() != 4) {
		throw std::runtime_error("CVC must be 3 or 4 digits long.");
	}

	// Ensure all characters are digits
	if (!std::all_of(cvc.begin(), cvc.end(), [](char c) { return std::isdigit(static_cast<unsigned char>(c)); })) {
		throw std::runtime_error("CVC must contain only digits.");
	}
}



/**
 * @brief Validates the card's expiry date (MM/YY format) using regex.
 * Checks format, valid month, and ensures it's not in the past.
 * @param formData The collected form data.
 * @param fieldIndex The index of the expiry date field in formData.
 * @throws std::runtime_error if the expiry date fails any validation rule.
 */
void validateExpiryDateStr(const std::string& value) {
	const std::string& expiryDate = value;

	if (expiryDate.empty()) {
		throw std::runtime_error("Expiry date cannot be empty.");
	}

	std::regex expiryDateRegex("^(0[1-9]|1[0-2])\\/(\\d{2})$");
	std::smatch matches; // To store the matched groups

	if (!std::regex_match(expiryDate, matches, expiryDateRegex)) {
		throw std::runtime_error("Expiry date format must be MM/YY (e.g., 01/25) and month must be between 01 and 12.");
	}

	// Extract month and year from regex matches
	// matches[0] is the whole matched string
	// matches[1] is the first captured group (month)
	// matches[2] is the second captured group (year)
	int month = std::stoi(matches[1].str());
	int year = std::stoi(matches[2].str()); // This is YY, not YYYY

	// Get current year (YY) and month for comparison
	time_t t = time(0);
	tm now;

	if (localtime_s(&now, &t) != 0) {
		throw std::runtime_error("Failed to get current local time for expiry date validation.");
	}

	int currentYearYY = now.tm_year % 100;
	int currentMonth = now.tm_mon + 1;

	// Check if the expiry year is in the past
	if (year < currentYearYY) {
		throw std::runtime_error("Expiry year (" + std::to_string(year) + ") cannot be in the past (" + std::to_string(currentYearYY) + ").");
	}
	// If the expiry year is the current year, check if the month is in the past
	if (year == currentYearYY && month < currentMonth) {
		throw std::runtime_error("Expiry date (" + std::to_string(month) + "/" + std::to_string(year) + ") cannot be in the past (Current: " + std::to_string(currentMonth) + "/" + std::to_string(currentYearYY) + ").");
	}

	if (year - currentYearYY > MAX_FUTURE_CARD_YEARS) {
		throw std::runtime_error(std::format("Warning, your card expiry date is more than {} years in future. Submit form one more time in order to procede.", MAX_FUTURE_CARD_YEARS));
	}
}



/**
 * @brief Validates a monetary amount.
 * Checks for non-empty string, valid number format (integers or decimals), and positive value.
 * @param formData The collected form data.
 * @param fieldIndex The index of the amount field in formData.
 * @throws std::runtime_error if the amount fails any validation rule.
 */
void validateAmountStr(const std::string& value) {
	const std::string& amountStr = value;

	if (amountStr.empty()) {
		throw std::runtime_error("Amount cannot be empty.");
	}

	// Check for valid number format (digits and at most one decimal point)
	bool hasDecimalPoint = false;
	bool isValidNumberFormat = true;
	if (amountStr.length() > 0 && (amountStr[0] == '.' || amountStr[0] == '-')) { // Cannot start with . or - (if not a negative check)
		isValidNumberFormat = false;
	}
	else {
		for (size_t i = 0; i < amountStr.length(); ++i) {
			char c = amountStr[i];
			if (std::isdigit(static_cast<unsigned char>(c))) {
				// Digit, allowed
			}
			else if (c == '.' && !hasDecimalPoint) {
				hasDecimalPoint = true; // First decimal point allowed
			}
			else {
				isValidNumberFormat = false; // Other characters or multiple decimal points
				break;
			}
		}
	}

	if (!isValidNumberFormat) {
		throw std::runtime_error("Amount must be a valid number (e.g., 100 or 100.50).");
	}

	// Convert to double to check if positive
	try {
		double amount = std::stod(amountStr);
		if (amount <= 0) {
			throw std::runtime_error("Amount must be a positive number.");
		}
	}
	catch (const std::out_of_range& e) {
		throw std::runtime_error("Amount value is too large or too small.");
	}
	catch (const std::invalid_argument& e) {
		throw std::runtime_error("Amount is not a valid number (parsing error).");
	}
}


/**
 * @brief Validates if a string is not empty
 * @param formData The collected form data.
 * @param fieldIndex The index of the string field in formData.
 * @throws std::runtime_error if the username fails any validation rule.
 */
void notEmptyStr(const std::string& value) {
	if (value.empty()) {
		throw std::runtime_error("This field cannot be empty");
	}
}



void validateUsername(const FormResult& formData, const size_t& fieldIndex) {
	validateUsernameStr(formData.at(fieldIndex).second);
}

void validatePassword(const FormResult& formData, const size_t& fieldIndex) {
	validatePasswordStr(formData.at(fieldIndex).second);
}



void validateEmail(const FormResult& formData, const size_t& fieldIndex) {
	validateEmailStr(formData.at(fieldIndex).second);
}

void validateVerificationCode(const FormResult& formData, const size_t& fieldIndex) {
	validateVerificationCodeStr(formData.at(fieldIndex).second);
}

void validateGender(const FormResult& formData, const size_t& fieldIndex) {
	validateGenderStr(formData.at(fieldIndex).second);
}

void validateAge(const FormResult& formData, const size_t& fieldIndex) {
	validateAgeStr(formData.at(fieldIndex).second);
}

void validatePhone(const FormResult& formData, const size_t& fieldIndex) {
	validatePhoneStr(formData.at(fieldIndex).second);
}

void validateCardNumber(const FormResult& formData, const size_t& fieldIndex) {
	validateCardNumberStr(formData.at(fieldIndex).second);
}

void validateCardHolder(const FormResult& formData, const size_t& fieldIndex) {
	validateCardHolderStr(formData.at(fieldIndex).second);
}

void validateCVC(const FormResult& formData, const size_t& fieldIndex) {
	validateCVCStr(formData.at(fieldIndex).second);
}

void validateExpiryDate(const FormResult& formData, const size_t& fieldIndex) {
	validateExpiryDateStr(formData.at(fieldIndex).second);
}

void validateAmount(const FormResult& formData, const size_t& fieldIndex) {
	validateAmountStr(formData.at(fieldIndex).second);
}


void notEmpty(const FormResult& formData, const size_t& fieldIndex) {
	notEmptyStr(formData.at(fieldIndex).second);
}