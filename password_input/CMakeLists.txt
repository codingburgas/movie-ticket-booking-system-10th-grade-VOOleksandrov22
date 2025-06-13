cmake_minimum_required(VERSION 3.29)
project(password_input)

set(CMAKE_CXX_STANDARD 20)

include_directories(PasswordInput/include)

add_library(password_input
	STATIC
    PasswordInput/example/main.cpp
    PasswordInput/include/options.h
    PasswordInput/include/passwordInput.h
    PasswordInput/src/passwordInput.cpp)
