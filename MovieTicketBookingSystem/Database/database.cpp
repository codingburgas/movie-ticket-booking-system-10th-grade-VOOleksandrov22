#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <memory>
#include "database.h"
#include <typeinfo>
#include <vector>
#include <map>


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
    this->connection = driver->connect(url, username, password);


    if (connection->isValid()) {
        if (debugMode){std::cout << "Connected to Azure MySQL database successfully!\n";};
    } else {
        throw std::runtime_error("Connection failed.\n");
    }

    connection->setSchema(schema);

    statement = connection->createStatement();
    if (debugMode){std::cout << "Create Statement instance\n";};

}


//Executes query and tries to return a ResultSet. If impossible, return nullptr
std::unique_ptr<sql::ResultSet> DB::Database::execute(const std::string& query) {
    if (!connection || !connection->isValid()) {
        throw std::runtime_error("Database connection is not valid for executing queries.\n");
    }

    statement->execute(sql::SQLString(query));
    try {
        return std::unique_ptr<sql::ResultSet>(statement->getResultSet());
    } catch (...) {
        return nullptr;
    }
}

struct ParamBinder {
    sql::PreparedStatement* pstmt;
    int index;

    void operator()(const std::string& val) const { pstmt->setString(index, val); }
    void operator()(int val) const { pstmt->setInt(index, val); }
    void operator()(unsigned int val) const { pstmt->setUInt(index, val); }
    void operator()(double val) const { pstmt->setDouble(index, val); }
    void operator()(bool val) const { pstmt->setBoolean(index, val); }
};

std::unique_ptr<sql::ResultSet> DB::Database::execute(const std::string& queryTemplate, const std::vector<ParamVariant>&& params){
    if (!connection || !connection->isValid()) {
        throw std::runtime_error("Database connection is not valid for executing queries.\n");
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(connection->prepareStatement(queryTemplate));

    for (size_t i = 0; i < params.size(); ++i) {
        std::visit(ParamBinder{ pstmt.get(), static_cast<int>(i + 1) }, params[i]);
    }

    sql::ResultSet* res = pstmt->executeQuery();
    return std::unique_ptr<sql::ResultSet>(res);
}





DB::Database::~Database() {

    delete statement;
    connection->close();
    delete connection;
    delete driver;
}


std::vector<Row> DB::resultSetToVector(std::unique_ptr<sql::ResultSet> res) {
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















