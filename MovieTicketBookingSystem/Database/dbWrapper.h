#pragma once

#include "../../db_cpp/database.h"
#include "../../db_cpp/DBModel.h"


class DbWrapper {
private:

public:
	DbWrapper(std::string url, std::string username, std::string password, std::string schema, bool debugMode);
	
	DB::Database* db;
	DB::DBModel* user;
	DB::DBModel* session;
};