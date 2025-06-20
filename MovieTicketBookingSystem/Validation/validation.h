#pragma once


#include "../Form/form.h"


const size_t MIN_PASSWORD_LENGTH = 8;
const size_t MAX_PASSWORD_LENGTH = 64;
const std::string SPECIAL_CHARACTERS = "!@#$%^&*()-_=+[]{}|;:,.<>?/~";

const int MIN_USERNAME_LENGTH = 3;
const int MAX_USERNAME_LENGTH = 20;


/**
 * @brief Validates if a username meets specified complexity and character requirements.
 * Checks for minimum/maximum length, leading/trailing spaces, and allowed character set.
 * @param formData The collected form data.
 * @param fieldIndex The index of the username field in formData.
 * @throws std::runtime_error if the username fails any validation rule.
 */
void validateUsernameStr(const std::string& value);
void validateUsername(const FormResult& formData, const size_t& fieldIndex);


/**
 * @brief Validates if a password meets specified complexity requirements.
 * Checks for minimum/maximum length, and presence of lowercase, uppercase,
 * digit, and special characters.
 * @param formData The collected form data.
 * @param fieldIndex The index of the password field in formData.
 * @throws std::runtime_error if the password fails any validation rule.
 */
void validatePasswordStr(const std::string& value);
void validatePassword(const FormResult& formData, const size_t& fieldIndex);


/**
 * @brief Validates if the current password field matches a previously entered password.
 * Assumes the original password field is located at a specific relative index.
 * @param formData The collected form data.
 * @param fieldIndex The index of the password re-entry field in formData.
 * @throws std::runtime_error if the passwords do not match.
 */
void passwordMatch(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates if an email address conforms to a common email regex pattern.
 * @param formData The collected form data.
 * @param fieldIndex The index of the email field in formData.
 * @throws std::runtime_error if the email is empty or has an invalid format.
 */
void validateEmailStr(const std::string& value);
void validateEmail(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates if a verification code is a 6-digit number.
 * @param formData The collected form data.
 * @param fieldIndex The index of the verification code field in formData.
 * @throws std::runtime_error if the code is not a 6-digit number.
 */
void validateVerificationCodeStr(const std::string& value);
void validateVerificationCode(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates if a gender string matches one of the predefined allowed values.
 * Allows an empty string if gender is nullable in the database.
 * @param formData The collected form data.
 * @param fieldIndex The index of the gender field in formData.
 * @throws std::runtime_error if the gender is invalid.
 */
void validateGenderStr(const std::string& value);
void validateGender(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates if an age string represents a valid number within a reasonable range (0-150).
 * Allows an empty string if age is nullable in the database.
 * @param formData The collected form data.
 * @param fieldIndex The index of the age field in formData.
 * @throws std::runtime_error if the age is invalid.
 */
void validateAgeStr(const std::string& value);
void validateAge(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates if a phone number string contains only allowed characters (digits, spaces, hyphens, plus sign).
 * Allows an empty string if the phone is nullable in the database.
 * @param formData The collected form data.
 * @param fieldIndex The index of the phone field in formData.
 * @throws std::runtime_error if the phone number contains invalid characters.
 */
void validatePhoneStr(const std::string& value);
void validatePhone(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates a credit card number.
 * Checks for length and ensures it contains only digits.
 * @param formData The collected form data.
 * @param fieldIndex The index of the card number field in formData.
 * @throws std::runtime_error if the card number fails any validation rule.
 */
void validateCardNumberStr(const std::string& value);
void validateCardNumber(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates the card holder's name.
 * Checks for non-empty string and allowed characters (letters, spaces, hyphens, periods).
 * @param formData The collected form data.
 * @param fieldIndex The index of the card holder field in formData.
 * @throws std::runtime_error if the card holder name fails any validation rule.
 */
void validateCardHolderStr(const std::string& value);
void validateCardHolder(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates the CVC/CVV code.
 * Checks for length (3 or 4 digits) and ensures it contains only digits.
 * @param formData The collected form data.
 * @param fieldIndex The index of the CVC field in formData.
 * @throws std::runtime_error if the CVC fails any validation rule.
 */
void validateCVCStr(const std::string& value);
void validateCVC(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates the card's expiry date (MM/YY format) using regex.
 * Checks format, valid month, and ensures it's not in the past.
 * @param formData The collected form data.
 * @param fieldIndex The index of the expiry date field in formData.
 * @throws std::runtime_error if the expiry date fails any validation rule.
 */
void validateExpiryDateStr(const std::string& value);
void validateExpiryDate(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates a monetary amount.
 * Checks for non-empty string, valid number format (integers or decimals), and positive value.
 * @param formData The collected form data.
 * @param fieldIndex The index of the amount field in formData.
 * @throws std::runtime_error if the amount fails any validation rule.
 */
void validateAmountStr(const std::string& value);
void validateAmount(const FormResult& formData, const size_t& fieldIndex);