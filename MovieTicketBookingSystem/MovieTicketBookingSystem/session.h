#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "user.h"


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
		std::string fields = "*"; std::string condition = "id=" + std::to_string(userId);
		auto userData = DB::resultSetToVector(app->db->user->select(fields, condition))[0];
		return User(app, userData);
	}

	static bool initSession(App* app, const std::string username, const std::string password) {
		std::string fields = "*"; std::string condition = "username = '" + username + "' AND password = '" + password + "'";
		auto users = DB::resultSetToVector(app->db->user->select(fields, condition));

		if (users.size() == 0) return false;

		auto userId = users[0]["id"];

		std::string fieldsInsert = "userId, expiresAt";
		std::string valuesInsert = userId + ", date_add(now(), interval 2 hour)";
		app->db->session->insert(fieldsInsert, valuesInsert);

		std::string lastId = "SELECT LAST_INSERT_ID();";
		auto* res = app->db->db->execute(lastId);

		res->next();
		unsigned int sessionId = res->getInt(1);

		json cache = Utils::File::readJsonFile(app->config->pathToCache);
		cache["sessionId"] = sessionId;
		bool written = Utils::File::writeJsonToFile(app->config->pathToCache, cache);
		if (!written) std::cerr << "Failed to save cache" << std::endl;

		return true;
	}
};