#pragma once

#include <iostream>
#include <string>
#include <format>

#include "app.h"
#include "../Database/database.h"

class User {
private:
    unsigned int id;
    std::string username;
    bool isAdmin;
    double balance;
    std::string email;
    std::string gender;
    std::string age;
    std::string phone;

public:
    App* app;

    User(App* app, unsigned int id, std::string username, bool isAdmin, double balance,
        std::string email, std::string gender, std::string age, std::string phone)
        : app(app), id(id), username(username), isAdmin(isAdmin), balance(balance),
        email(email), gender(gender), age(age), phone(phone) {
    }

    User& operator=(const User& other) {
        if (this == &other) { // Self-assignment check
            return *this;
        }

        // Copy each member variable
        app = other.app;
        id = other.id;
        username = other.username; // std::string handles deep copy
        isAdmin = other.isAdmin;
        balance = other.balance;
        email = other.email;
        gender = other.gender;
        age = other.age;
        phone = other.phone;

        return *this;
    }

    User(App* app, const Row& data)
        :
        app(app),
        id(std::stoul(data["id"])),
        username(data["username"]),
        isAdmin((data["isAdmin"] == "1") ? true : false),
        balance(std::stod(data["balance"])),
        email(data["email"]),
        gender(data["gender"]),
        age(data["age"]),
        phone(data["phone"])
    {}

    std::string getUsername() const { return username; }
    unsigned int getId() const { return id; }
    bool getIsAdmin() const { return isAdmin; }
    double getBalance() const { return balance; }
    std::string getEmail() const { return email; }
    std::string getGender() const { return gender; }
    std::string getAge() const { return age; }
    std::string getPhone() const { return phone; }


	/*
	* initUser function: Creates a new user in the database.
	*
	* @param app Pointer to the main application context (for database access).
	* @param username The user's chosen username. Modified in place if it's an admin user.
	* @param password The user's password (should be hashed before insertion in a real app).
	* @param email The user's email address.
	* @param gender The user's gender (e.g., "Male", "Female", "Other", "Prefer not to say").
	* @param ageStr The user's age as a string (will be converted to INT or NULL).
	* @param phone The user's phone number.
	* @return 1 for success, 2 if user already exists.
	*/
	static int initUser(App* app, std::string& username, const std::string& password,
		const std::string& email, const std::string& gender,
		const std::string& age, const std::string& phone) {
		bool isAdmin = false;
		if (username.ends_with("%ADMIN%")) {
			isAdmin = true;
			username = username.substr(0, username.size() - 7);
		}

        auto usersWithSuchUsername = DB::resultSetToVector(app->db->execute(
            "select id from user where username = ? or email = ?",
            {username, email}
        ));

		if (usersWithSuchUsername.size() > 0) return 2;


        std::string insertQuery = "insert into User(username, password, isAdmin, email, gender, age, phone) values (?, ?, ?, ?, ?, ?, ?)";
        app->db->execute(
            insertQuery,
            { username, password, (isAdmin ? 1 : 0), email, const_cast<Config*>(app->config)->genders[gender[0]], age, phone.empty() });

		return 1;
	}
};