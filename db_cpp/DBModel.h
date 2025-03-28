//
// Created by Oleksandr Vinichenko on 08.11.2024.
//

#ifndef DBMODEL_H
#define DBMODEL_H

#include <iostream>
#include <vector>

static std::string emptyString;
static std::string singleQuote = "'";
namespace DB {

    class DBModel {

    public:
        //Constructor
        DBModel(std::string &name, Database* db, std::string &tableCode = emptyString);

        std::string &getName(){ return name; };

        //checks whether table with the same name exists
        bool isInitialized();

        //Executes 'INSERT' sql query with specified fields and condition, returns nothing
        void insert(std::string &fields, std::string &values);
        //Executes 'INSERT' sql query with specified fields and condition, returns nothing
        void insert(std::vector<std::string> &fields, std::vector<std::string> &values);



        //Executes 'SELECT' sql query with specified fields and condition, returns ResultSet
        sql::ResultSet* select(std::string &fields, std::string &condition=emptyString);
        //Executes 'SELECT' sql query with specified fields and condition, returns ResultSet
        sql::ResultSet* select(std::vector<std::string> &fields, std::string &condition=emptyString);



        //Executes 'delete' sql query with specified condition, returns nothing
        void sqlDelete(std::string &condition=emptyString);


        //Executes 'update' sql query with specified fields, values and condition, returns nothing
        void update(std::string &dataToUpdate, std::string &condition=emptyString);
        //Executes 'update' sql query with specified fields, values and condition, returns nothing
        void update(std::vector<std::string> &fields, std::vector<std::string> &values, std::string &condition=emptyString);




        ~DBModel();


        /*returns vector with vectors, each representing a field. Element on index ...
         *[0] - Field,
         *[1] - Type,
         *[2] - Null,
         *[3] - Key,
         *[4] - Default,
         *[5] - Extra
         */
        std::vector< std::vector<std::string> > *getFields();

    private:
        std::string name;
        Database* db;
    };
}





#endif //DBMODEL_H
