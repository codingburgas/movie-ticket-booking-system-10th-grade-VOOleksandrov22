#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
//#include <cppconn/resultset.h>
#include "database.h"
#include <typeinfo>
#include <vector>
#include <map>

#include "DBModel.h"

//prints specified fields of ResultSet object
void DB::printResultSet(sql::ResultSet *resultSet, std::vector<std::string> &columnNames) {
    int count = 1;
    if (resultSet == nullptr) {
        throw std::runtime_error("ResultSet pointer is null\n");
    }
    while (resultSet->next()) {
        std::cout << "____________________________________\n";
        std::cout << "Element #" + std::to_string(count) + " of result set: " + "\n";
        for (const std::string& columnName : columnNames) {
            std:: cout << "Property '" << columnName << "' : " << resultSet->getString(columnName) << "\n";
        }
        std::cout << "____________________________________\n";
        count++;
    }

}




DB::Database::Database(const std::string &url, const std::string &username, const std::string &password, const std::string &schema, bool debugMode) {
    this->debugMode = debugMode;
    this->schema = schema;

    sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
    this->driver = driver;
    if (debugMode){std::cout << "Created driver instance\n";};

    //create Connection instance
    this->connection =  driver->connect(url, username, password);


    if (connection->isValid()) {
        if (debugMode){std::cout << "Connected to Azure MySQL database successfully!\n";};
    } else {
        //if (debugMode){std::cerr << "Connection failed.\n";};
        throw std::runtime_error("Connection failed.\n");
    }

    connection->setSchema(schema);

    statement = connection->createStatement();
    if (debugMode){std::cout << "Create Statement instance\n";};

}


//Executes query and tries to return a ResultSet. If impossible, return nullptr
sql::ResultSet *DB::Database::execute(std::string &query) {
    statement->execute(sql::SQLString(query));
    try {
        return statement->getResultSet();
    } catch (...) {
        return nullptr;
    }
}



void DB::Database::addModel(DBModel *model) {
    // check if already in a vector
    if (std::find(modelsInitialised.begin(), modelsInitialised.end(), model) != modelsInitialised.end()) {
        throw std::runtime_error(("Model '" + model->getName() + "' is already initialised.\n").c_str());
    }

    //append a model
    modelsInitialised.push_back(model);
}


void DB::Database::removeModel(DBModel *model) {

    // get index of a model in a modelsInitialised
    auto pos = std::find(modelsInitialised.begin(), modelsInitialised.end(), model);

    // check if not in a vector
    if (pos == modelsInitialised.end()) {
        throw std::runtime_error(("Model '" + model->getName() + "' is not yet initialised.\n").c_str());
    }

    //remove model
    modelsInitialised.erase(pos);
}




DB::Database::~Database() {
    //auto modelsInitialisedClone = modelsInitialised;
    for (auto* model : modelsInitialised) {
        delete model;
    }

    delete statement;
    connection->close();
    delete connection;
    delete driver;
}


std::vector<Row> DB::resultSetToVector(sql::ResultSet* res) {
    std::vector<Row> v = {};

    auto* metadata = res->getMetaData();
    int columnCount = metadata->getColumnCount();
    

    while (res->next()) {
        Row row = {};
        for (int i = 1; i <= columnCount; i++) {
            row[metadata->getColumnLabel(i)] = res->getString(i);
        }
        v.push_back(row);
    }

    return v;
}















