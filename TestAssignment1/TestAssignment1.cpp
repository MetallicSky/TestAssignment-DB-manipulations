#include "TestAssignment1.h"


enum class Mode {database_creation = 0, new_entry};

constexpr std::string_view modeTypes[] =
{
    "database_creation",
    "new_entry"
};

bool databaseCreation() {
    std::string dbName = "workers.db";
    struct stat buffer;
    if (stat(dbName.c_str(), &buffer) == 0) {
        std::cout << "Database \"" << dbName << "\" already exists!\n";
        return false;
    }

    Database db(dbName);
    if (db.createWorkersTable())
        return true;
    else
        return false;
}

bool newEntry() {
    std::string dbName = "worrkers.db";
    struct stat buffer;
    if (stat(dbName.c_str(), &buffer) != 0) {
        std::cout << "Database \"" << dbName << "\" does not yet exist! Launch program in mode 1 first\n";
        return false;
    }
}

int main(int argc, char* argv[])
{
    switch (argc) {
    case 1: {
        std::cout << "No arguement detected\n";
    }
        break;
    case 2: {
        int modeNum;
        modeNum = atoi(argv[1]);
        modeNum--;
        const int modesNum = sizeof(modeTypes) / sizeof(modeTypes[0]) - 1; // total array bytes divide by bytes per one array element minus one byte where terminator character is located, resulting in array size
        if (modeNum < 0 || modeNum > modesNum) {
            std::cout << "Unknown usage mode: " << modeNum + 1 << "\n";
            break;
        }
        
        std::cout << "Usage mode: " << modeTypes[modeNum] << "\n";
        Mode mode = static_cast<Mode>(modeNum);
        switch (mode) {
        case Mode::database_creation:
            databaseCreation();
            break;
        case Mode::new_entry:
            newEntry();
            break;
        }
    }
        break;
    default: {
        std::cout << "Too many arguements detected\n";
    }
    }
    return 0;
}
