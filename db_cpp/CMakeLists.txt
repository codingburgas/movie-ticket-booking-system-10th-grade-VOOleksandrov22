cmake_minimum_required(VERSION 3.10)
project(CompileLibrary)

add_definitions(-DHAVE_STDINT_H)


set(CMAKE_C_COMPILER "C:/MinGW/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW/bin/g++.exe")



add_library(db_cpp STATIC
        database.cpp
        database.h
        dbModel.cpp
        dbModel.h
)


set(FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR "D:/cpp_libraries/mysql-connector-c++-9.1.0-winx64")


target_include_directories(db_cpp PUBLIC
    ${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include/jdbc
    ${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include/jdbc/cppconn
)

target_link_libraries(db_cpp PRIVATE 
    ${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/lib64/vs14/mysqlcppconn.lib
)
