#include "Worker.h"
#include "Worker.h"

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
		cerr << error_message;
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
			cerr << "ERROR: Invalid date format (" << newBirthDate << "). Only YYYY-MM-DD is accepted";
			return false;
		}
	}
	catch (const char* error_message) {
		cerr << error_message;
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
		cerr << "ERROR: Invalid sex (" << newSex << ")\n";
		return false;
	}
	return true;
}

bool Worker::setSex(const int& newSex)
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
		cerr << "ERROR: Invalid sex (" << newSex << ")\n";
		return false;
	}
	return true;
}

bool Worker::sendToDB(Database& db) const
{
	cout << "sendToDB method start\n";
	// SQL insert statement with placeholders for the worker's data
	string insertSQL = "INSERT INTO Workers (Name, BirthDate, Sex) VALUES (?, ?, ?);";
	sqlite3_stmt* stmt;
	cout << "sendToDB preparing statement \n";
	// Prepare the SQL statement
	if (sqlite3_prepare_v2(db.getDB(), insertSQL.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		cerr << "ERROR: Preparing statement failed : " << sqlite3_errmsg(db.getDB()) << "\n";
		return false;
	}

	cout << "sendToDB binding name\n";
	// Bind the worker's name to the first placeholder
	if (sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
		cerr << "ERROR: binding name failed: " << sqlite3_errmsg(db.getDB()) << "\n";
		sqlite3_finalize(stmt);
		return false;
	}

	cout << "sendToDB binding date\n";
	// Bind the worker's birth date to the second placeholder
	if (sqlite3_bind_text(stmt, 2, birthDate.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
		cerr << "ERROR: binding birth date failed: " << sqlite3_errmsg(db.getDB()) << "\n";
		sqlite3_finalize(stmt);
		return false;
	}

	cout << "sendToDB binding sex\n";
	// Bind the worker's sex (as an integer) to the third placeholder
	if (sqlite3_bind_int(stmt, 3, static_cast<int>(s_)) != SQLITE_OK) {
		cerr << "ERROR: binding sex failed: " << sqlite3_errmsg(db.getDB()) << "\n";
		sqlite3_finalize(stmt);
		return false;
	}

	/*
	// Bind the worker data to the statement (1 = first placeholder, 2 = second, etc.)
	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, birthDate.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, static_cast<int>(s_));
	*/

	cout << "sendToDB executing statement\n";
	// Execute the statement
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		cerr << "ERROR: Executing statement failed: " << sqlite3_errmsg(db.getDB()) << "\n";
		sqlite3_finalize(stmt);
		return false;
	}
	cout << "sendToDB finalizing statement\n";
	// Finalize the statement
	sqlite3_finalize(stmt);
	cout << "Worker entry added to the database successfully." << "\n";
	return true;
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

string Worker::toString() const
{
	return name + " " + birthDate + " " + getSex();
}

Worker::Worker()
{
	name = "John Doe";
	birthDate = "1984-04-20";
	s_ = Sex::male;
}

Worker::Worker(const string& name, const string& birthDate, const int& sex)
{
	setName(name);
	setBirthDate(birthDate);
	setSex(sex);
}

Worker::Worker(const string& name, const string& birthDate, const string& sex)
{
	setName(name);
	setBirthDate(birthDate);
	setSex(sex);
}

std::ostream& operator<<(std::ostream& os, const Worker& worker)
{
	os << worker.getName() << " " << worker.getBirthDate() << " " << worker.getSex();
	return os;
}
