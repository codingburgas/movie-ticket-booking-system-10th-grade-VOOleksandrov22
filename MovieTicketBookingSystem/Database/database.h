//
// Created by Oleksandr Vinichenko on 06.11.2024.
//

#ifndef DB_H
#define DB_H

#include <iostream>
#include <vector>
#include <map>
#include <variant>


#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

#include "../Dict/dict.h"


using Row = Dict<std::string, std::string>;

using ParamVariant = std::variant<std::string, int, unsigned int, double, bool>;


namespace DB {
    class DBModel;

    class Database {


    public:

        Database(const std::string& url, const std::string& username, const std::string& password, const std::string& schema, bool debugMode = false);

        //Executes query and tries to return a ResultSet. If impossible, return nullptr
        std::unique_ptr<sql::ResultSet> execute(const std::string &query);
        std::unique_ptr<sql::ResultSet> execute(const std::string& queryTemplate, const std::vector<ParamVariant>&& params);

        std::string getSchema(){ return schema; };
        sql::Connection* getConnection(){ return connection; };
        sql::mysql::MySQL_Driver* getDriver(){ return driver; };
        bool getDebugMode(){ return debugMode; };


        ~Database();

    private:
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* connection;
        sql::Statement* statement;
        bool debugMode = false;
        std::string schema;
    };


    //prints specified fields of ResultSet object
    void printResultSet(sql::ResultSet* resultSet, std::vector<std::string> &columnNames);

    const std::string columnProperties[6] = {"Field", "Type", "Null", "Key", "Default", "Extra"};

    std::vector<Row> resultSetToVector(std::unique_ptr<sql::ResultSet> res);

}
#endif //DB_H
