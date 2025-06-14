#pragma once

#include <iostream>
#include <string>
#include <format>

#include "app.h"
#include "../../db_cpp/database.h"

class User {
private:
	const unsigned int id;
	const std::string username;
	const bool isAdmin;
	const double balance;
	
public:


	User(App* app, unsigned int id, std::string username, bool isAdmin, double balance) : app(app), id(id), username(username), isAdmin(isAdmin), balance(balance) {}
	User(App* app, Row& data) 
		: 
		app(app), 
		id(std::stoul(data["id"])), 
		username(data["username"]), 
		isAdmin((data["isAdmin"] == "1") ? true : false),
		balance(std::stod(data["balance"]))
	{}


	App* app;


	std::string getUsername() const { return username; }

	unsigned int getId() const { return id; }

	bool getIsAdmin() const { return isAdmin; }

	double getBalance() const { return balance; }


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

		std::string fields = "id", condition = "username = '" + username + "'";
		auto usersWithSuchUsername = DB::resultSetToVector( app->db->user->select(fields, condition) );

		if (usersWithSuchUsername.size() > 0) return 2;

		fields = "username, password, isAdmin, email, gender, age, phone"; 
		std::string values = std::format("'{}', '{}', {}, '{}', '{}', {}, {}", username, password, (isAdmin ? "TRUE" : "FALSE"), email, const_cast<Config*>(app->config)->genders[gender[0]], age, phone.empty() ? "NULL" : "'" + phone + "'");
		app->db->user->insert(fields, values);

		return 1;
	}
};