#include "WorkerDB.h"

std::string Worker::getName()
{
	return name;
}

bool Worker::setName(const std::string& newName)
{
	try {
		name = newName;
	}
	catch (const char* error_message) {
		std::cerr << error_message;
		return false;
	}
	return true;
}

std::string Worker::getBirthDate()
{
	return birthDate;
}

bool Worker::setBirthDate(const std::string& newBirthDate)
{
	try {
		std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
		if (std::regex_match(newBirthDate, dateRegex))
			birthDate = newBirthDate;
		else {
			std::cerr << "Invalid date format (" << newBirthDate << "). Only YYYY-MM-DD is accepted";
			return false;
		}
	}
	catch (const char* error_message) {
		std::cerr << error_message;
		return false;
	}
	return true;
}

std::string Worker::getSex()
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

int Worker::getSexNum()
{
	return static_cast<int>(s_);
}

bool Worker::setSex(const std::string& newSex)
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
		return false;
	}
	return true;
}

Worker::Worker(const std::string& name, const std::string& birthDate, const int& sex)
{
	setName(name);
	setBirthDate(birthDate);
	setSexNum(sex);
}

Worker::Worker(const std::string& name, const std::string& birthDate, const std::string& sex)
{
	setName(name);
	setBirthDate(birthDate);
	setSex(sex);
}

Database::Database(const std::string& dbName)
{
	if (sqlite3_open(dbName.c_str(), &db)) {
		std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
		db = nullptr;
	}
}

Database::~Database()
{
	if (db) {
		sqlite3_close(db);
	}
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
		std::cerr << "Error creating table: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return false;
	}
	std::cout << "Workers table created successfully." << std::endl;
	return true;
}
