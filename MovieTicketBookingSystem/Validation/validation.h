#pragma once


#include "../Form/form.h"


const size_t MIN_PASSWORD_LENGTH = 8;
const size_t MAX_PASSWORD_LENGTH = 64;
const std::string SPECIAL_CHARACTERS = "!@#$%^&*()-_=+[]{}|;:,.<>?/~";



/**
 * @brief Validates if a password meets specified complexity requirements.
 * Checks for minimum/maximum length, and presence of lowercase, uppercase,
 * digit, and special characters.
 * @param formData The collected form data.
 * @param fieldIndex The index of the password field in formData.
 * @throws std::runtime_error if the password fails any validation rule.
 */
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
void validateEmail(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates if a verification code is a 6-digit number.
 * @param formData The collected form data.
 * @param fieldIndex The index of the verification code field in formData.
 * @throws std::runtime_error if the code is not a 6-digit number.
 */
void validateVerificationCode(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates if a gender string matches one of the predefined allowed values.
 * Allows an empty string if gender is nullable in the database.
 * @param formData The collected form data.
 * @param fieldIndex The index of the gender field in formData.
 * @throws std::runtime_error if the gender is invalid.
 */
void validateGender(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates if an age string represents a valid number within a reasonable range (0-150).
 * Allows an empty string if age is nullable in the database.
 * @param formData The collected form data.
 * @param fieldIndex The index of the age field in formData.
 * @throws std::runtime_error if the age is invalid.
 */
void validateAge(const FormResult& formData, const size_t& fieldIndex);

/**
 * @brief Validates if a phone number string contains only allowed characters (digits, spaces, hyphens, plus sign).
 * Allows an empty string if the phone is nullable in the database.
 * @param formData The collected form data.
 * @param fieldIndex The index of the phone field in formData.
 * @throws std::runtime_error if the phone number contains invalid characters.
 */
void validatePhone(const FormResult& formData, const size_t& fieldIndex);