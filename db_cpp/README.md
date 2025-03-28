# Access your mysql database with ease


## Installation

1. Clone the repository: 
```powershell Clone repo
git clone https://github.com/viniwell/db_cpp.git
```

2. Install the Connector C++ library from ```https://dev.mysql.com/downloads/connector/cpp/```
3. Link the Connector C++ library to your project in CMakeFiles:
```cmake
cmake_minimum_required(VERSION 3.29)
project(projectName)

set(CMAKE_CXX_STANDARD 14)

set(FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR "path-to-mysql-connector-cpp-installation-directory")
include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include/jdbc)
include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include/jdbc/cppconn)
link_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/lib64/vs14)

add_executable(projectName
        db_cpp/database.h
        db_cpp/database.cpp
        db_cpp/DBModel.h
        db_cpp/DBModel.cpp
        main.cpp
)

target_link_libraries(projectName mysqlcppconn)
```

## Usage

```cpp main.cpp

#include "../db_cpp/database.h"

#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include "config.h"
#include "../db_cpp/DBModel.h"
#include <vector>


int main() {
    try {
        const auto* config = new Config();

        auto* db = new DB::Database(config->url, config->username, config->password, config->schema, true);

        std::string name = "Student";
        std::string tableCode = "id INT AUTO_INCREMENT,\n"
                                "first_name VARCHAR(100),\n"
                                "last_name VARCHAR(100),\n"
                                "PRIMARY KEY(id)";
        auto* studentDB = new DB::DBModel(name, db, tableCode);
        std::cout << "initialized Student DBModel\n";

        /*
        //Insert example
        std::string fields = "first_name, last_name";
        std::string values = "'test_select', 'test_last_name'";
        studentDB->insert(fields, values);*/


        /*
        //Select example
        std::vector<std::string> names = {"id", "first_name", "last_name"};
        std::string fields = "*";
        std::string condition = "first_name = 'test_select'";


        std::cout << "Results from selection without condition:\n";
        DB::printResultSet( studentDB->select(fields), names );

        std::cout << "Results from selection with condition:\n";
        DB::printResultSet( studentDB->select(fields, condition), names );*/

        /*
        //Delete example
        std::string conditionForDelete = "first_name='test_select'";
        studentDB->sqlDelete(conditionForDelete);*/

        /*
        //Update example
        std::string data = "first_name='updateWithStr',\nlast_name='updateWithStr'";
        studentDB->update(data);
        */

        /*
        //Insert with vectors
        std::vector<std::string> f = {"first_name", "last_name"};
        std::vector<std::string> v = {"new first name 1", "new last name 2"};
        studentDB->insert(f, v);*/

        /*
        //Select with vectors
        std::vector<std::string> n = {"id", "first_name", "last_name"};
        std::vector<std::string> f = {"*"};
        DB::printResultSet(studentDB->select(f), n);*/

        /*
        //Update with vectors
        std::vector<std::string> f = {"first_name"};
        std::vector<std::string> v = {"updated_first_name"};
        std::string c = "id=27";
        studentDB->update(f, v, c);*/

        /*
        std::vector<std::string> names = {"id", "first_name", "last_name"};
        std::string fields = "*";
        std::string condition = "first_name = 'test_select'";

        std::cout << "DB now contains\n";
        DB::printResultSet( studentDB->select(fields), names );*/


        //getFields example
        auto *fields = studentDB->getFields();
        for (auto property : *fields) {
            std::cout << "Property '" << property.at(0) << "' : Type(" << property.at(1) << ") Null(" << property.at(2) << ") Key(" << property.at(3) << ") Default(" << property.at(4) << ") " << "Extra(" << property.at(5) << ");\n";
        }


        //delete studentDB, db;
        delete db;

        std::cout << studentDB->getName();
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
```

## Contributing

Contributions are welcome. Please open a pull request to contribute to the project.

## License

The project is licensed under the MIT License. See [LICENSE](LICENSE) for more information.
