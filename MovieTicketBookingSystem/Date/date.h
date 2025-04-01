#include <iostream>
#include <string>
#include <ctime>

class Date {
private:
    int year;
    int month;
    int day;
    int hours;
    int minutes;
    int seconds;

    bool isLeapYear(int y) {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    bool isValidDate(int y, int m, int d, int h, int min, int s) {
        if (y < 1900 || y > 2100) return false;
        if (m < 1 || m > 12) return false;
        if (h < 0 || h > 23 || min < 0 || min > 59 || s < 0 || s > 59) return false;

        int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        if (m == 2 && isLeapYear(y)) {
            daysInMonth[2] = 29;
        }

        return d >= 1 && d <= daysInMonth[m];
    }

public:
    // yyyy-mm-dd hh:mm:ss
    Date(std::string dateTime) {
        size_t firstDash = dateTime.find('-');
        int y = std::stoi(dateTime.substr(0, firstDash));

        size_t secondDash = dateTime.find('-', firstDash + 1);
        int m = std::stoi(dateTime.substr(firstDash + 1, secondDash - firstDash - 1));

        size_t spacePos = dateTime.find(' ', secondDash + 1);
        int d = std::stoi(dateTime.substr(secondDash + 1, spacePos - secondDash - 1));

        size_t firstColon = dateTime.find(':', spacePos + 1);
        int h = std::stoi(dateTime.substr(spacePos + 1, firstColon - spacePos - 1));

        size_t secondColon = dateTime.find(':', firstColon + 1);
        int min = std::stoi(dateTime.substr(firstColon + 1, secondColon - firstColon - 1));

        int s = std::stoi(dateTime.substr(secondColon + 1));

        if (isValidDate(y, m, d, h, min, s)) {
            year = y;
            month = m;
            day = d;
            hours = h;
            minutes = min;
            seconds = s;
        }
        else {
            std::cerr << "Invalid date! Setting default (2000-01-01 00:00:00)." << std::endl;
            year = 2000;
            month = 1;
            day = 1;
            hours = 0;
            minutes = 0;
            seconds = 0;
        }
    }

    Date(int y, int m, int d, int h, int min, int s) {
        if (isValidDate(y, m, d, h, min, s)) {
            year = y;
            month = m;
            day = d;
            hours = h;
            minutes = min;
            seconds = s;
        }
        else {
            std::cerr << "Invalid date! Setting default (2000-01-01 00:00:00)." << std::endl;
            year = 2000;
            month = 1;
            day = 1;
            hours = 0;
            minutes = 0;
            seconds = 0;
        }
    }

    Date() {
        std::time_t t = std::time(nullptr);
        std::tm now;
        localtime_s(&now, &t);

        year = now.tm_year + 1900;
        month = now.tm_mon + 1;
        day = now.tm_mday;
        hours = now.tm_hour;
        minutes = now.tm_min;
        seconds = now.tm_sec;
    }

    // Getters
    int getYear() const { return year; }
    int getMonth() const { return month; }
    int getDay() const { return day; }
    int getHours() const { return hours; }
    int getMinutes() const { return minutes; }
    int getSeconds() const { return seconds; }

    // Setters
    void setYear(int y) { if (isValidDate(y, month, day, hours, minutes, seconds)) year = y; }
    void setMonth(int m) { if (isValidDate(year, m, day, hours, minutes, seconds)) month = m; }
    void setDay(int d) { if (isValidDate(year, month, d, hours, minutes, seconds)) day = d; }
    void setHours(int h) { if (isValidDate(year, month, day, h, minutes, seconds)) hours = h; }
    void setMinutes(int min) { if (isValidDate(year, month, day, hours, min, seconds)) minutes = min; }
    void setSeconds(int s) { if (isValidDate(year, month, day, hours, minutes, s)) seconds = s; }

    void printDate() const {
        std::cout << "Date: " << year << "-"
            << (month < 10 ? "0" : "") << month << "-"
            << (day < 10 ? "0" : "") << day << " "
            << (hours < 10 ? "0" : "") << hours << ":"
            << (minutes < 10 ? "0" : "") << minutes << ":"
            << (seconds < 10 ? "0" : "") << seconds << std::endl;
    }



    unsigned long toSeconds() const {
        std::tm timeStruct = {};
        timeStruct.tm_year = year - 1900;
        timeStruct.tm_mon = month - 1;
        timeStruct.tm_mday = day;
        timeStruct.tm_hour = hours;
        timeStruct.tm_min = minutes;
        timeStruct.tm_sec = seconds;

        return std::mktime(&timeStruct);
    }

    bool operator<(const Date& other) const {
        return toSeconds() < other.toSeconds();
    }

    bool operator>(const Date& other) const {
        return toSeconds() > other.toSeconds();
    }

    bool operator==(const Date& other) const {
        return toSeconds() == other.toSeconds();
    }

    bool operator!=(const Date& other) const {
        return !(*this == other);
    }
};
