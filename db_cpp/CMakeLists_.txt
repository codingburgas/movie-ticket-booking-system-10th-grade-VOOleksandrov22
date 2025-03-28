#cmake_minimum_required(VERSION 3.29)
#project(db_cpp)

#set(CMAKE_CXX_STANDARD 14)

#add_executable(db_cpp
#        database.cpp
#        main.cpp
#        database.h
#        dbModel.cpp
#        dbModel.h)

cmake_minimum_required(VERSION 3.29)

set(FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR "D:/cpp_libraries/mysql-connector-c++-9.1.0-winx64")
include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include/jdbc)
include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include/jdbc/cppconn)
link_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/lib64/vs14)

# Add the executable
add_executable(db_cpp
        database.cpp
        database.h
        dbModel.cpp
        dbModel.h
        testing/main.cpp
        testing/config.h
        #testing/test.cpp
)

target_link_libraries(db_cpp mysqlcppconn)
