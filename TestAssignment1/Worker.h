#pragma once
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <regex>
#include <ctime>
#include <sstream>

#include "Database.h"

using std::string;
using std::cout;
using std::cerr;

class Database;

class Worker {
public:
    enum class Sex
    {
        male = 0,
        female,
        unknwn
    };

    string getName() const;
    bool setName(const string& newName);
    string getBirthDate() const;
    bool setBirthDate(const string& newBirthDate);
    string getSex() const;
    int getSexNum() const;
    bool setSex(const string& newSex);
    bool setSex(const int& newSex);

    bool sendToDB(Database& db) const;
    int getAge() const;
    string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const Worker& worker);

    Worker();
    Worker(const string& name, const string& birthDate, const int& sex); // sex - male = 0, female = 1, unknwn = 2
    Worker(const string& name, const string& birthDate, const string& sex); // sex - male = 0, female = 1, unknwn = 2
private:
    string name;
    string birthDate;
    Sex s_;
};