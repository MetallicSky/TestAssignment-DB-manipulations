#pragma once
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <regex>

class Worker {
public:
    enum class Sex
    {
        male = 0,
        female,
        unknwn
    };

    std::string getName();
    bool setName(const std::string& newName);
    std::string getBirthDate();
    bool setBirthDate(const std::string& newBirthDate);
    std::string getSex();
    int getSexNum();
    bool setSex(const std::string& newSex);
    bool setSexNum(const int& newSex);

    

    Worker(const std::string& name, const std::string& birthDate, const int& sex); // sex - male = 0, female = 1, unknwn = 2
    Worker(const std::string& name, const std::string& birthDate, const std::string& sex); // sex - male = 0, female = 1, unknwn = 2
private:
    std::string name;
    std::string birthDate;
    Sex s_;
};

class Database {
private:
    sqlite3* db;

public:
    Database(const std::string& dbName);

    ~Database();

    bool createWorkersTable();
};