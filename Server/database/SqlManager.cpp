#pragma once

#include "SqlManager.h"


SqlManager::SqlManager(const char *path) {

    open(path);
    createTables();
}

void SqlManager::open(const char *path) {
    int rc = sqlite3_open(path, &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(-1);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

}


void SqlManager::close() {
    sqlite3_close(db);
}

void SqlManager::saveClient(const ClientInfo &client) {
    char *zErrMsg = 0;
    int rc;
    char sql[100];

    //get name
    std::string name = client.getName();
    //get token
    std::string token = client.getToken();
    //get pin
    std::string pin = "125";

    // Create SQL statement
    sprintf(sql, "INSERT INTO CLIENTS (NAME,TOKEN,PIN) VALUES ('%s', '%s', '%s');", name.c_str(), token.c_str(), pin.c_str());

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

void SqlManager::createTables() {
    int rc;
    char *zErrMsg = 0;
    char *sql;

    /* Create SQL statement */
    sql = "CREATE TABLE CLIENTS("  \
         "ID INTEGER PRIMARY KEY     AUTOINCREMENT," \
         "NAME           TEXT    NOT NULL," \
         "TOKEN          TEXT    NOT NULL," \
         "PIN            TEXT    NOT NULL);";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

}

bool SqlManager::isNameTaken(const std::string &name, const std::string &token) {

    char *zErrMsg = 0;
    int rc;
    char sql[100];
    sqlite3_stmt *stmt;
    const char *tail;

    //should return true if a entry with the same name but different token exists
    sprintf(sql, "SELECT * FROM CLIENTS WHERE NAME = '%s' AND TOKEN != '% s';", name.c_str(), token.c_str());

    // Execute SQL statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, &tail);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        return true;
    }
    return false;

}

