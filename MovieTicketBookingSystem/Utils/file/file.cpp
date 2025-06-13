#include <iostream>
#include <fstream>
#include <Windows.h>
#include "../utils.h"



void create_path_if_not_exists(const std::string& path) {
    std::wstring wide_path(path.begin(), path.end());

    if (CreateDirectory(wide_path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {}
    else {
        std::cerr << "Failed to create path: " << path << std::endl;
    }
}

json Utils::File::readJsonFile(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "File " + filename + " not found\n";
        return json{};
    }

    json jsonData;
    try {
        file >> jsonData;
    }
    catch (const std::exception& e) {
        //std::cerr << "Error: Failed to parse JSON from file " << filename << " - " << e.what() << std::endl;
        return json{};
    }

    file.close();
    return jsonData;
}

bool Utils::File::writeJsonToFile(std::string filename, const json& data) {
    create_path_if_not_exists(filename);


    std::ofstream file(filename);
    if (!file.is_open()) {
        //std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return false;
    }

    try {
        file << data.dump(4);
    }
    catch (const std::exception& e) {
        //std::cerr << "Error: Failed to write JSON to file " << filename << " - " << e.what() << std::endl;
        return false;
    }

    file.close();
    return true;
}



int Utils::generateRandomSixDigitNumber() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(100000, 999999);
    return distribution(generator);
}