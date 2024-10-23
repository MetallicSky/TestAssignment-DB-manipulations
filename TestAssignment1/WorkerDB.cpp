#include "WorkerDB.h"

string Worker::getName() const
{
	return name;
}

bool Worker::setName(const string& newName)
{
	try {
		name = newName;
		return true;
	}
	catch (const char* error_message) {
		std::cerr << error_message;
		return false;
	}
}

string Worker::getBirthDate() const
{
	return birthDate;
}

bool Worker::setBirthDate(const string& newBirthDate)
{
	try {
		std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
		if (std::regex_match(newBirthDate, dateRegex))
			birthDate = newBirthDate;
		else {
			std::cerr << "ERROR: Invalid date format (" << newBirthDate << "). Only YYYY-MM-DD is accepted";
			return false;
		}
	}
	catch (const char* error_message) {
		std::cerr << error_message;
		return false;
	}
	return true;
}

string Worker::getSex() const
{
	switch (s_) {
	case Sex::male:
		return "male";
		break;
	case Sex::female:
		return "female";
		break;
	case Sex::unknwn:
		return "unknwn";
		break;
	default:
		return "error";
	}
}

int Worker::getSexNum() const
{
	return static_cast<int>(s_);
}

bool Worker::setSex(const string& newSex)
{
	if (newSex == "male") {
		s_ = Sex::male;
	}
	else if (newSex == "female") {
		s_ = Sex::female;
	}
	else if (newSex == "unknwn") {
		s_ = Sex::unknwn;
	}
	else {
		std::cerr << "ERROR: Invalid sex (" << newSex << ")\n";
		return false;
	}
	return true;
}

bool Worker::setSexNum(const int& newSex)
{
	switch (newSex) {
	case 0:
		s_ = Sex::male;
		break;
	case 1:
		s_ = Sex::female;
		break;
	case 2:
		s_ = Sex::unknwn;
		break;
	default:
		std::cerr << "ERROR: Invalid sex (" << newSex << ")\n";
		return false;
	}
	return true;
}

bool Worker::sendToDB(Database& db) const
{
	// SQL insert statement with placeholders for the worker's data
	string insertSQL = "INSERT INTO Workers (Name, BirthDate, Sex) VALUES (?, ?, ?);";
	sqlite3_stmt* stmt;
	
	// Prepare the SQL statement
	if (sqlite3_prepare_v2(db.getDB(), insertSQL.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "ERROR: Preparing statement failed : " << sqlite3_errmsg(db.getDB()) << "\n";
		return false;
	}

	// Bind the worker data to the statement (1 = first placeholder, 2 = second, etc.)
	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, birthDate.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, static_cast<int>(s_));

	// Execute the statement
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		std::cerr << "ERROR: Executing statement failed: " << sqlite3_errmsg(db.getDB()) << std::endl;
		sqlite3_finalize(stmt);
		return false;
	}

	// Finalize the statement (clean up)
	sqlite3_finalize(stmt);
	std::cout << "Worker entry added to the database successfully." << std::endl;
	return true;

	return false;
}

int Worker::getAge() const
{
	std::time_t t = std::time(nullptr);      // Get current time as time_t
	std::tm* now = std::localtime(&t);       // Convert to local time

	int year = now->tm_year + 1900;          // tm_year is years since 1900
	int month = now->tm_mon + 1;             // tm_mon is months since January (0-11)
	int day = now->tm_mday;                  // tm_mday is the day of the month

	int birthYear = stoi(birthDate.substr(0, 4));
	int birthMonth = stoi(birthDate.substr(5, 2));
	int birthDay = stoi(birthDate.substr(8, 2));

	int age = year - birthYear;
	if (month < birthMonth)
		age--;
	else if (month == birthMonth)
		if (day < birthDay)
			age--;

	return age;
}

Worker::Worker(const string& name, const string& birthDate, const int& sex)
{
	setName(name);
	setBirthDate(birthDate);
	setSexNum(sex);
}

Worker::Worker(const string& name, const string& birthDate, const string& sex)
{
	setName(name);
	setBirthDate(birthDate);
	setSex(sex);
}

Database::Database()
{
}

Database::Database(const string& dbName)
{
	if (sqlite3_open(dbName.c_str(), &db)) {
		std::cerr << "ERROR: opening database failed: " << sqlite3_errmsg(db) << std::endl;
		db = nullptr;
	}
}

Database::~Database()
{
	if (db) {
		sqlite3_close(db);
	}
}

bool Database::bootDB(const string& dbName)
{
	if (sqlite3_open(dbName.c_str(), &db)) {
		std::cerr << "ERROR: opening database failed: " << sqlite3_errmsg(db) << std::endl;
		db = nullptr;
		return false;
	}
	return true;
}

bool Database::createWorkersTable()
{
	const char* sqlCreateTable = "CREATE TABLE IF NOT EXISTS Workers ("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
		"Name TEXT NOT NULL, "
		"BirthDate DATE NOT NULL, "
		"Sex INTEGER NOT NULL);";

	char* errMsg = nullptr;
	if (sqlite3_exec(db, sqlCreateTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "ERROR: creating table failed: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return false;
	}
	std::cout << "Workers table created successfully." << std::endl;
	return true;
}

sqlite3* Database::getDB() const {
	return db;
}
