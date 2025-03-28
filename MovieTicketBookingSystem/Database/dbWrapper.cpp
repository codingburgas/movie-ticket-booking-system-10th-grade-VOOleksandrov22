#include "dbWrapper.h"

DbWrapper::DbWrapper(std::string url, std::string username, std::string password, std::string schema, bool debugMode) {
	db = new DB::Database(url, username, password, schema, debugMode);

	std::string emptyString = "";

	std::string userTableName = "User";
	user = new DB::DBModel(userTableName, db, emptyString);
}