//
// Created by Oleksandr Vinichenko on 08.11.2024.
//

#include <iostream>
#include <utility>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

#include "database.h"
#include "dbModel.h"


std::string &vectorElementsRepresentation(std::vector<std::string> &v, std::string &wrapIn=emptyString) {
    std::string result;
    bool hasWrap = !wrapIn.empty();
    for (int i=0; i<v.size(); i++) {
        result += ( (hasWrap ? wrapIn : "") + v.at(i) + (hasWrap ? wrapIn : "") +(i!=v.size()-1 ? ", " : ""));
    }
    return result;
}



DB::DBModel::DBModel(std::string &name, Database *db, std::string &tableCode) {
    //assign properties
    this->name = std::move(name);
    this->db = db;

    //create table, if does not exists
    if (!isInitialized()) {
        const char* message = ("Table with name '" + this->name + "' is not initialized. Creating a new one.").c_str();
        std::wcerr << message;

        //check if SQL code is valid
        if (tableCode.empty()) {
            const char* message = ("Table with name '" + this->name + "' is not yet created and tableCode is not valid.").c_str();
            throw std::runtime_error(message);
        }

        //create table
        std::string query = "create table " + this->name + "(" +
                            tableCode+
                            ");";
        db->execute(query);
    }

    //add model to db.modelsInitialised vector
    db->addModel(this);
}


//checks whether table with the same name exists
bool DB::DBModel::isInitialized() {
    std::string query = "show tables like '" + this->name + "'";
    sql::ResultSet* res = db->execute(query);
    try {
        //checks if ResultSet contains something, true means it returned table info
        return res->next();
    } catch(...) {
        return false;
    }

}




//Executes 'INSERT' sql query with specified fields and condition, returns nothing
void DB::DBModel::insert(std::string &fields, std::string &values) {
    std::string query = "insert into " + name + " ( " + fields + " ) values ( " + values + " );";
    db->execute(query);
}
//Executes 'INSERT' sql query with specified fields and condition, returns nothing
void DB::DBModel::insert(std::vector<std::string> &fields, std::vector<std::string> &values) {
    if (fields.size() != values.size()) {
        throw std::runtime_error("Can not insert, because fields and values vectors have different size\n");
    }
    std::string query = "insert into " + name + " ( " + vectorElementsRepresentation(fields) + " ) values ( " + vectorElementsRepresentation(values, singleQuote) + " );";
    db->execute(query);
}




//Executes 'SELECT' sql query with specified fields and condition, returns ResultSet
sql::ResultSet* DB::DBModel::select(std::string &fields, std::string &condition) {
    std::string query = "select " + fields + " from " + name + (!condition.empty() ? (" where "+condition) : "") + ";";
    return db->execute(query);
}
//Executes 'SELECT' sql query with specified fields and condition, returns ResultSet
sql::ResultSet *DB::DBModel::select(std::vector<std::string> &fields, std::string &condition)
{
    std::string query = "select " + vectorElementsRepresentation(fields) + " from " + name + (!condition.empty() ? (" where "+condition) : "") + ";";
    return db->execute(query);
}





//Executes 'DELETE' sql query with specified condition, returns nothing
void DB::DBModel::sqlDelete(std::string &condition) {
    std::string query = "delete from " + name + (!condition.empty() ? (" where " + condition) : "") + ";";
    db->execute(query);
}





//Executes 'update' sql query with specified fields, values and condition, returns nothing
void DB::DBModel::update(std::string &dataToUpdate, std::string &condition) {
    std::string query = "update " + name + " set \n" + dataToUpdate + (!condition.empty() ? (" where " + condition) : "") + ";";
    db->execute(query);
}
//Executes 'update' sql query with specified fields, values and condition, returns nothing
void DB::DBModel::update(std::vector<std::string> &fields, std::vector<std::string> &values, std::string &condition) {
    if (fields.size() != values.size()) {
        throw std::runtime_error("Can not update, because fields and values vectors have different size\n");
    }

    std::string query = "update " + name + " set \n";
    for (int i=0; i<fields.size(); i++) {
        query += (fields.at(i) + " = '" +  values.at(i) + (i!=fields.size()-1 ? "',\n" : "'\n"));
    }
    query += (!condition.empty() ? (" where " + condition) : "") + ";";
    db->execute(query);
}





/*
 *Returns vector with vectors, each representing a field. Element on index ...
 *[0] - Field,
 *[1] - Type,
 *[2] - Null,
 *[3] - Key,
 *[4] - Default,
 *[5] - Extra
 */
std::vector< std::vector<std::string> > *DB::DBModel::getFields() {
    auto *result = new std::vector< std::vector<std::string> >;
    std::string query = "show columns from " + name + " from " + db->getSchema()+";";

    sql::ResultSet* res = db->execute(query);
    std::vector<std::string> column;
    while (res->next()) {
        for (const std::string& property : DB::columnProperties) {
            column.push_back(res->getString(property).asStdString());
        }
        std::vector<std::string> columnCopy = column;
        result->push_back(columnCopy);
        column.clear();
    }

    return result;
}





DB::DBModel::~DBModel() {
    db->removeModel(this);
}
