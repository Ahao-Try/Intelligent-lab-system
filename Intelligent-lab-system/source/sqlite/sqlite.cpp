#include "../../include/sqlite/sqlite.h"


using json = nlohmann::json;

Database::Database(std::string& dbName) : dbName(dbName), db(nullptr), errMsg(nullptr) {}

Database::~Database() {
    close();
}

bool Database::open() {
    if (sqlite3_open(dbName.c_str(), &db) == SQLITE_OK) {
        return true;
    }
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    return false;
}

void Database::close() {
    if (db) {
        sqlite3_close(db);
    }
}

bool Database::createTable() {
    const char* sqlCreateTable = "CREATE TABLE IF NOT EXISTS my_table ("
                                 "id INTEGER PRIMARY KEY, "
                                 "array TEXT);"; // Store array as JSON text
    if (sqlite3_exec(db, sqlCreateTable, nullptr, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::insertData(int id, std::vector<float>& array) {
    json jArray = array;
    std::string sqlInsert = "INSERT INTO my_table (id, array) VALUES (" + std::to_string(id) + ", '" + jArray.dump() + "');";
    if (sqlite3_exec(db, sqlInsert.c_str(), nullptr, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::deleteData(int id) {
    std::string sqlDelete = "DELETE FROM my_table WHERE id = " + std::to_string(id) + ";";
    if (sqlite3_exec(db, sqlDelete.c_str(), nullptr, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::updateData(int id, std::vector<float>& array) {
    json jArray = array;
    std::string sqlUpdate = "UPDATE my_table SET array = '" + jArray.dump() + "' WHERE id = " + std::to_string(id) + ";";
    if (sqlite3_exec(db, sqlUpdate.c_str(), nullptr, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::queryAllData() {
    std::string sqlQuery = "SELECT * FROM my_table;";
    int rc = sqlite3_exec(db, sqlQuery.c_str(), callback, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// 参数说明：
    // NotUsed - 用户提供的数据指针，
    // argc - 结果集中的列数
    // argv - 一个数组，包含每列的值
    // azColName - 一个数组，包含每列的名称
static int Database::callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for(int i = 0; i < argc; i++) {
        // 打印出 id 值
        if (std::string(azColName[i]) == "id") {
            std::cout << "ID: " << (argv[i] ? argv[i] : "NULL") << std::endl;
        }
        // 解析 JSON 字符串，打印出 array
        else if (std::string(azColName[i]) == "array" && argv[i]) {
            std::string jsonArray = argv[i];
            try {
                auto array = nlohmann::json::parse(jsonArray);
                for (const auto& val : array) {
                    std::cout << "Array value: " << val << std::endl;
                }
            } catch (nlohmann::json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }            
        }
    }
    std::cout << "---------------------------------\n";
    return 0;
}