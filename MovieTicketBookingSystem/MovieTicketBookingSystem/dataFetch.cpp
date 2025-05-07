#include "app.h"
#include "../../db_cpp/database.h"



std::vector<std::string> App::getCities() {
	//std::string query = "SELECT * FROM Cinema WHERE id IN (SELECT MIN(id) FROM Cinema GROUP BY city);";
	std::string query = "select distinct city from Cinema;";

	auto res = db->db->execute(query);

	std::vector<std::string> cities = {};

	while (res->next()) {
		cities.push_back(res->getString(1));
	}

	//return DB::resultSetToVector(res);
	return cities;
}