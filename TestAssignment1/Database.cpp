#include "Database.h"


Database::Database()
{
}

Database::Database(const string& dbName)
{
	if (sqlite3_open(dbName.c_str(), &db)) {
		cerr << "ERROR: opening database failed: " << sqlite3_errmsg(db) << "\n";
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
		cerr << "ERROR: opening database failed: " << sqlite3_errmsg(db) << "\n";
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
		"BirthDate TEXT NOT NULL, "
		"Sex INTEGER NOT NULL);";

	char* errMsg = nullptr;
	if (sqlite3_exec(db, sqlCreateTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
		cerr << "ERROR: creating table failed: " << errMsg << "\n";
		sqlite3_free(errMsg);
		return false;
	}
	cout << "Workers table created successfully." << "\n";
	return true;
}

size_t Database::getSize() const
{
	const char* sqlCount = "SELECT COUNT(*) FROM Workers;";
	sqlite3_stmt* stmt;
	size_t count = 0;

	if (sqlite3_prepare_v2(db, sqlCount, -1, &stmt, nullptr) == SQLITE_OK) {
		if (sqlite3_step(stmt) == SQLITE_ROW)
			count = static_cast<size_t>(sqlite3_column_int(stmt, 0));  // Cast result to size_t
		sqlite3_finalize(stmt);
	}
	else
		cerr << "ERROR: counting rows failed: " << sqlite3_errmsg(db) << "\n";

	return count;
}


sqlite3* Database::getDB() const {
	return db;
}

Worker Database::getEntry(const int& id) const
{
	Worker badEntry("BADENTRY", "0000-00-00", "unknwn"); // method returns bad entry if used entered data in incorrect format
	Worker validEntry("EMPTY", "0000-00-00", "unknwn"); // these fields will be changed during the course of the work

	if (id < 1 || id > getSize()) {
		cerr << "ERROR: id (" << id << ") for getEntry(id) is out of bounds\n";
		return badEntry;
	}

	const char* sqlSelect = "SELECT Name, BirthDate, Sex FROM Workers WHERE ID = ?;";
	sqlite3_stmt* stmt;
	string name;
	string birthDate;
	int sex;

	if (sqlite3_prepare_v2(db, sqlSelect, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, id);  // Bind the id to the query

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
			birthDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
			sex = (sqlite3_column_int(stmt, 2));
		}
		sqlite3_finalize(stmt);
		validEntry.setName(name);
		validEntry.setBirthDate(birthDate);
		validEntry.setSex(sex);
	}
	else {
		cerr << "ERROR: retrieving entry by ID failed: " << sqlite3_errmsg(db) << "\n";
		return badEntry;
	}

	return validEntry;
}