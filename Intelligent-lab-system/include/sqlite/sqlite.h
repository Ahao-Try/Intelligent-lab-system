#ifndef __SQLITE_H__
#define __SQLITE_H__

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../../include/nlohmann/json.hpp"

#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <string>
#include <vector>

class Database {
public:
    Database(std::string& dbName);
    ~Database();

    bool open();
    void close();

    bool createTable();
    bool insertData(int id, std::vector<float>& array);
    bool deleteData(int id);
    bool updateData(int id,std::vector<float>& array);
    bool queryAllData();

private:
    static int callback(void* NotUsed, int argc, char** argv, char** azColName)

    sqlite3* db;
    std::string dbName;
    char* errMsg;
};


#endif