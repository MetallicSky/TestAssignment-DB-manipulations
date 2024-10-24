#pragma once
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <regex>
#include <ctime>
#include <sstream>

#include "Worker.h"

using std::string;
using std::cout;
using std::cerr;

class Worker;

class Database {
private:
    sqlite3* db;

public:
    Database();
    Database(const string& dbName);

    ~Database();

    bool bootDB(const string& dbName);
    bool createWorkersTable();
    size_t getSize() const;
    sqlite3* getDB() const;
    Worker getEntry(const int& id) const;
    bool insertBatch(const std::vector<Worker>& workers);
};