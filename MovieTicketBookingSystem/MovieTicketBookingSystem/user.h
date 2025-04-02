#pragma once

#include <iostream>
#include <string>

#include "app.h"
#include "../../db_cpp/database.h"

class User {
private:
	const unsigned int id;
	const std::string username;
	const bool isAdmin;
	
public:


	User(App* app, unsigned int id, std::string username, bool isAdmin) : app(app), id(id), username(username), isAdmin(isAdmin) {}
	User(App* app, Row& data) 
		: 
		app(app), 
		id(std::stoul(data["id"])), 
		username(data["username"]), 
		isAdmin((data["isAdmin"] == "1") ? true : false) {}


	App* app;


	std::string getUsername() const { return username; }

	unsigned int getId() const { return id; }

	bool getIsAdmin() const { return isAdmin;  }

	/*
	* 1 - success
	* 2 - user already exists
	* 
	*/
	static int initUser(App* app, std::string& username, const std::string password) {
		bool isAdmin = false;
		if (username.ends_with("%ADMIN%")) {
			isAdmin = true;
			username = username.substr(0, username.size() - 7);
		}

		std::string fields = "id", condition = "username = '" + username + "'";
		auto usersWithSuchUsername = DB::resultSetToVector( app->db->user->select(fields, condition) );

		if (usersWithSuchUsername.size() > 0) return 2;

		fields = "username, password, isAdmin"; std::string values = "'" + username + "', '" + password + "', " + (isAdmin ? "TRUE" : "FALSE");
		app->db->user->insert(fields, values);

		return 1;
	}
};