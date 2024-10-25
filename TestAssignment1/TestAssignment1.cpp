#include "TestAssignment1.h"

using std::cout;
using std::string;
using std::cerr;
using std::to_string;

enum class Mode {database_creation = 0, new_entry, 
    database_print, batch_insertion, database_search, fast_search
};

constexpr std::string_view modeTypes[] =
{
    "database_creation",
    "new_entry",
    "database_print",
    "batch_insertion",
    "database_search",
    "fast_search"
};

string random_string(int& length, const string& characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
{
    // Create a random number generator
    std::random_device rd;
    std::mt19937 generator(rd());

    // Create a distribution to uniformly select from all
    // characters
    std::uniform_int_distribution<> distribution(
        0, characters.size() - 1);

    // Generate the random string
    string random_string;
    for (int i = 0; i < length; ++i) {
        random_string
            += characters[distribution(generator)];
    }

    return random_string;
}

int random_int(int x, int y) {
    std::random_device rd; // Seed with a real random value, if available
    std::mt19937 gen(rd()); // Initialize the random number generator with the seed
    std::uniform_int_distribution<> distrib(x, y); // Create a distribution range between x and y (inclusive)
    return distrib(gen); // Generate and return the random number
}

bool databaseCreation(Database& db) {
    string dbName = "workers.db";
    struct stat buffer;
    if (stat(dbName.c_str(), &buffer) == 0) {
        cout << "ERROR: Database \"" << dbName << "\" already exists!\n";
        return false;
    }

    db.bootDB(dbName);
    if (db.createWorkersTable())
        return true;
    else
        return false;
}

bool databasePrint(Database& db) {
    
    // Maximum SQL usage 
    const char* sqlSelectUnique = "SELECT Name, BirthDate, Sex FROM Workers GROUP BY Name, BirthDate ORDER BY Name;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db.getDB(), sqlSelectUnique, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to execute query: " << sqlite3_errmsg(db.getDB()) << std::endl;
        return false;
    }

    cout << "Unique entries:\n";

    // Loop through  results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string birthDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int sex = (sqlite3_column_int(stmt, 2));

        Worker worker(name, birthDate, sex);

        int age = worker.getAge();

        std::cout << name << " " << birthDate << " " << worker.getSex() << " " << age << "\n";
    }

    sqlite3_finalize(stmt);  // Clean up statement
    return true;

    // Minimum SQL usage (with std::map)
    /*
    std::map<string, string> uniqueEntries;
    cout << "All entries:\n";
    for (int i = 1; i <= db.getSize(); i++) { // filling Map in order to get unique combinations
        const Worker worker = db.getEntry(i);
        const string nameDate = worker.getName() + " " + worker.getBirthDate();
        const string sexAge = worker.getSex() + " " + std::to_string(worker.getAge());
        uniqueEntries.insert({nameDate, sexAge});

        // cout << worker.toString() << "\n"; // alternative way for printing
        cout << worker << "\n"; // printing using overloaded << operator
    }
    cout << "Unique entries:\n";
    for (auto i : uniqueEntries)
        cout << i.first << " " << i.second << "\n";

    return true;
    */
}

bool batchInsertion(Database& db, const int& amount) {
    std::vector<Worker> workers(amount);
    int specialCount = 0;
    for (int i = 0; i < amount; i++) {
        int nameLength = 5;
        string name = random_string(nameLength);
        string month = to_string(random_int(1, 12));
        if (month.size() == 1) // so month would be 04 instead of 4
            month = "0" + month;
        string day = to_string(random_int(1, 30));
        if (day.size() == 1) // so day would be 04 instead of 4
            day = "0" + day;
        string date = to_string(random_int(1970, 2005)) + 
            "-" + month +
            "-" + day;
        int sex = random_int(0, 2);
        if (i % 1000 == 0 && specialCount < 100) { // special F male
            name.replace(0, 1, "F");
            sex = 0;
            specialCount++;
        }
        workers[i].setName(name);
        workers[i].setBirthDate(date);
        workers[i].setSex(sex);
    }
    db.insertBatch(workers);

    return true;
}

bool databaseSearch(Database& db) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Worker> matches;

    // SQL query to select workers where Name starts with 'F' and Sex is 0
    const char* sqlQuery = "SELECT * FROM Workers WHERE Name LIKE 'F%' AND Sex = 0;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db.getDB(), sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "ERROR: statement preparation failed: " << sqlite3_errmsg(db.getDB()) << std::endl;
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Retrieve values for each column
        int id = sqlite3_column_int(stmt, 0);
        std::string name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        std::string birthDate(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        int sex = (sqlite3_column_int(stmt, 3));

        Worker worker(name, birthDate, sex);
        matches.push_back(worker);
    }

    sqlite3_finalize(stmt);

    // Minimum SQL usage
    /*
    for (int i = 1; i <= db.getSize(); i++) { // filling Map in order to get unique combinations
        const Worker worker = db.getEntry(i);
        if (worker.getName().at(0) == 'F' && worker.getSex() == "male")
            matches.push_back(worker);
    }
    */
   
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    cout << "Found " << matches.size() << " entries, time took: " << + duration.count() << " ms\n";
    return true;
    
}

bool fastSearch(Database& db) {
    auto start = std::chrono::high_resolution_clock::now();

    int matchCount = 0;
    const char* sqlQuery = "SELECT COUNT(*) FROM Workers WHERE Name LIKE 'F%' AND Sex = 0;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getDB(), sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "ERROR: statement preparation failed: " << sqlite3_errmsg(db.getDB()) << std::endl;
        return false;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        matchCount = sqlite3_column_int(stmt, 0); // Retrieve the count from the first column
    }

    sqlite3_finalize(stmt);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "Found " << matchCount << " entries, time took: " << duration.count() << " ms\n";


    return true;
}

Worker newEntryCheck(const int& argc, char* argv[]) {
    int firstQuotes = 0;
    int lastQuotes = 0;
    Worker badEntry("BADENTRY", "0000-00-00", "unknwn"); // method returns bad entry if used entered data in incorrect format
    Worker validEntry("EMPTY", "0000-00-00", "unknwn"); // these fields will be changed during the course of the work
    for (int i = 2; i < argc; i++) {
        cout << argv[i] << "\n";
    }
    /* // UNTESTED, in case shell doesn't know how to work with quotes
    for (int i = 2; i < argc; i++) {
        string arg(argv[i]);
        if (i == 2 && (arg.find("\"") == string::npos)) { // first arguement in loop should always contain quotes, as worker name is declared first
            cerr << "ERROR: First arguement after mode arguement should always contain quotes\n";
            return badEntry;
        }
        firstQuotes = i;
        if (i != 2 && (arg.find("\"") != string::npos))
            lastQuotes = i;
    }
    if (lastQuotes == 0) // means that the whole name doesn't contain space character and was read as one arguement
        lastQuotes = firstQuotes;
    if (argc - lastQuotes != 2) { // date and sex should always be without quotes
        cerr << "ERROR: Quotes detected where date or sex should've been\n";
        return badEntry;
    }
    string firstQ(argv[firstQuotes]);
    if (firstQ.find("\"") != 0) { //first quotes should always be the first character of arguement 
        cerr << "ERROR: Characters detected before first quotes (" << firstQ << ")\n";
        return badEntry;
    }
    string lastQ(argv[lastQuotes]);
    if (lastQ.find("\"") != lastQ.size() - 1) { //last quotes should always be the last character of arguement 
        cerr << "ERROR: Characters detected after last quotes (" << lastQ << ")\n";
        return badEntry;
    }
    */
    string name = argv[2];
    if (validEntry.setName(name) == false) { // setter will return false if there was an error while trying to set name
        return badEntry;
    }
    /* // UNTESTED, in case shell doesn't know how to work with quotes
    for (int i = firstQuotes; i <= lastQuotes; i++) {
        string arg(argv[i]);
        if (i == firstQuotes) // remove first quotes before pushing arg into name
            arg.erase(0, 1);
        if (i == lastQuotes) // remove last quotes before pushing arg into name
            arg.pop_back();
        name += arg;
        if (i != lastQuotes) // add space if last quotes are in another arguement
            name += " ";
    }
    */
    string birthDate = argv[3];
    if (validEntry.setBirthDate(birthDate) == false) { // setter will return false if date's format is incorrect
        return badEntry;
    }
    string sex = argv[4];
    if (validEntry.setSex(sex) == false) { // setter will return false if sex doesn't match any enum definition
        return badEntry;
    }

    return validEntry;
}

int main(int argc, char* argv[])
{
    Database db;

    switch (argc) {
    case 1: {
        cout << "ERROR: No arguement detected\n";
    }
        break;
    case 2: {
        int modeNum;
        modeNum = atoi(argv[1]);
        modeNum--;
        const int modesNum = sizeof(modeTypes) / sizeof(modeTypes[0]) - 1; // total array bytes divide by bytes per one array element minus one byte where terminator character is located, resulting in array size
        if (modeNum < 0 || modeNum > modesNum) {
            cout << "ERROR: Unknown usage mode: " << modeNum + 1 << "\n";
            break;
        }
        
        cout << "Usage mode: " << modeTypes[modeNum] << "\n";
        Mode mode = static_cast<Mode>(modeNum);
        switch (mode) {
        case Mode::database_creation:
            databaseCreation(db);
            break;
        case Mode::new_entry:
            cout << "ERROR: Not enough arguements for mode 2 detected\n";
            break;
        case Mode::database_print:
            db.bootDB("workers.db");
            databasePrint(db);
            break;
        case Mode::batch_insertion:
            db.bootDB("workers.db");
            batchInsertion(db, 1000000);
            break;
        case Mode::database_search:
            db.bootDB("workers.db");
            databaseSearch(db);
            break;
        case Mode::fast_search:
            db.bootDB("workers.db");
            fastSearch(db);
            break;
        }
    }
        break;
    default: {
        if (atoi(argv[1]) != 2) { // mode 2 has 3 extra arguements
            cout << "ERROR: Too many arguements detected\n";
            return 0;
        }
        else {
            cout << "Usage mode: new_entry\n";
            if (argc < 5) {
                cout << "ERROR: Not enough arguements for mode 2 detected\n";
            }
            else if (argc == 5) {
                Worker newEntry = newEntryCheck(argc, argv);
                db.bootDB("workers.db");
                if (newEntry.getName() != "BADENTRY" || newEntry.getBirthDate() != "0000-00-00" || newEntry.getSex() != "unknwn") // making sure arguements we got were correct
                    newEntry.sendToDB(db);
                else
                    cout << "ERROR: Invalid entry\n";
            }
            else if (argc > 5) {
                cout << "ERROR: Too many arguements for mode 2 detected\n";
            }
        }
    }
    }
    return 0;
}
