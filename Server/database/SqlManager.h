#pragma once

#include <sqlite3.h>
#include <cstdio>
#include <cstdlib>

#include "../client/ClientInfo.h"


class SqlManager {

public:
    explicit SqlManager(const char *path);

    void open(const char *path);

    void close();

    void createTables();

    void saveClient(const ClientInfo &client);

    bool isNameTaken(const std::string &name, const std::string &token);

    [[nodiscard]] sqlite3 *getDB() const {
        return db;
    }

private:
    sqlite3 *db{};


};


