//
// Created by Oleksandr Vinichenko on 06.11.2024.
//

#ifndef DB_H
#define DB_H

#include <iostream>
#include <vector>
#include <map>


#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>



namespace DB {
    class DBModel;

    class Database {


    public:

        Database(const std::string& url, const std::string& username, const std::string& password, const std::string& schema, bool debugMode = false);

        //Executes query and tries to return a ResultSet. If impossible, return nullptr
        sql::ResultSet* execute(std::string &query);

        std::string getSchema(){ return schema; };
        sql::Connection* getConnection(){ return connection; };
        sql::mysql::MySQL_Driver* getDriver(){ return driver; };
        std::vector <DBModel*> getModelsInitialised(){ return modelsInitialised; };
        bool getDebugMode(){ return debugMode; };


        void addModel(DBModel* model);
        void removeModel(DBModel* model);

        ~Database();

    private:
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* connection;
        sql::Statement* statement;
        std::vector <DBModel*> modelsInitialised;
        bool debugMode = false;
        std::string schema;
    };


    //prints specified fields of ResultSet object
    void printResultSet(sql::ResultSet* resultSet, std::vector<std::string> &columnNames);

    const std::string columnProperties[6] = {"Field", "Type", "Null", "Key", "Default", "Extra"};

    std::vector<std::map<std::string, std::string>> resultSetToVector(sql::ResultSet* res);

}
#endif //DB_H
