#include <iostream>
#include <vector>
#include "app.h"
#include "config.h"

#include <cppconn/resultset.h>

App::App() : db(new DbWrapper(config->url, config->username, config->password, config->schema, config->debugMode)) {
	std::vector<std::string> fields = { "*" };
	sql::ResultSet* users = db->user->select(fields);

	
	/*fields = {"id", "username", "password"};
	DB::printResultSet(users, fields);*/

	auto v = DB::resultSetToVector(users);

	for (int i = 0; i < v.size(); i++) {
		std::cout << "___________________\n";
		for (const auto& pair : v[i]) {
			std::cout << pair.first << " : " << pair.second << std::endl;
		}
	}
}