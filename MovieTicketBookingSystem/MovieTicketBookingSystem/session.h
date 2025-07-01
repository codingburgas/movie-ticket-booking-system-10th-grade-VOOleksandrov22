#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "user.h"
#include "../nlohmann/json.hpp"
#include "../Utils/utils.h"

using json = nlohmann::json;


class App;

class Session {
private:
	const unsigned int id;
	const unsigned int userId;

public:
	Session(App* app, unsigned int id, unsigned int userId) : app(app), id(id), userId(userId) {}


	Session(App* app, Row& data) : app(app), id(std::stoul(data["id"])), userId(std::stoul(data["userId"])) {}

	App* app;


	unsigned int getId() { return id; }
	void setId(unsigned int id) { id = id; }

	const User getUser() const {
		auto userData = DB::resultSetToVector(app->db->execute(
			"select * from User where id = ?;", 
			{ static_cast<int>(userId) }))[0];
		return User(app, userData);
	}

	static bool initSession(App* app, const std::string& credential, const std::string& password) {
		auto users = DB::resultSetToVector(app->db->execute(
			"select id from User where (username = ? OR email = ?) AND password = ?", 
			{credential, credential, password}
		));

		if (users.size() == 0) return false;

		auto userId = std::stoi(users[0]["id"]);

		app->db->execute("insert into Session(userId, expiresAt) values(?, date_add(now(), interval 2 hour));", {userId});

		
		auto res = app->db->execute("SELECT LAST_INSERT_ID();");

		res->next();
		unsigned int sessionId = res->getInt(1);

		json cache = Utils::File::readJsonFile(app->config->pathToCache);
		cache["sessionId"] = sessionId;
		bool written = Utils::File::writeJsonToFile(app->config->pathToCache, cache);
		if (!written) std::cerr << "Failed to save cache" << std::endl;

		return true;
	}
};