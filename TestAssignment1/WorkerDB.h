#pragma once
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <regex>
#include <ctime>
#include <sstream>

using std::string;
using std::cout;

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
    bool setSexNum(const int& newSex);

    bool sendToDB(Database& db) const;
    int getAge() const;

    Worker(const string& name, const string& birthDate, const int& sex); // sex - male = 0, female = 1, unknwn = 2
    Worker(const string& name, const string& birthDate, const string& sex); // sex - male = 0, female = 1, unknwn = 2
private:
    string name;
    string birthDate;
    Sex s_;
};

class Database {
private:
    sqlite3* db;

public:
    Database();
    Database(const string& dbName);

    ~Database();

    bool bootDB(const string& dbName);
    bool createWorkersTable();
    sqlite3* getDB() const;
};