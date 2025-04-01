#pragma once

#include <iostream>
#include <string>

#include "app.h"
#include "../../db_cpp/database.h"

class User {
private:
	unsigned int id;
	std::string username;
	
public:

	User(App* app): app(app) {}
	User(App* app, unsigned int id, std::string username) : app(app), id(id), username(username) {}
	User(App* app, Row& data) : app(app), id(std::stoul(data["id"])), username(data["username"]) {}

	App* app;


	std::string getUsername() const { return username; }
	void setUsername(std::string username) { username = username; }

	unsigned int getId() const { return id; }
	void setId(unsigned int id) { id = id; }

	/*
	* 1 - success
	* 2 - user already exists
	* 
	*/
	static int initUser(App* app, const std::string username, const std::string password) {
		std::string fields = "id", condition = "username = '" + username + "'";
		auto usersWithSuchUsername = DB::resultSetToVector( app->db->user->select(fields, condition) );

		if (usersWithSuchUsername.size() > 0) return 2;

		fields = "username, password"; std::string values = "'" + username + "', '" + password + "'";
		app->db->user->insert(fields, values);

		return 1;
	}
};