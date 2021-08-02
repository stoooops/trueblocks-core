/*-------------------------------------------------------------------------------------------
 * qblocks - fast, easily-accessible, fully-decentralized data from blockchains
 * copyright (c) 2016, 2021 TrueBlocks, LLC (http://trueblocks.io)
 *
 * This program is free software: you may redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version. This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details. You should have received a copy of the GNU General
 * Public License along with this program. If not, see http://www.gnu.org/licenses/.
 *-------------------------------------------------------------------------------------------*/
#include "basetypes.h"
#include "database.h"
#include "exportcontext.h"
#include "options_base.h"
#include "colors.h"
#include "filenames.h"
#include "accountname.h"
#include "rpcresult.h"
#include "sqlite3.h"

namespace qblocks {

static int noopCallbackSQL(void* data, int nColumns, char** theRow, char** azColName) {
    return 0;
}
static int readCallbackSQL(void* data, int nColumns, char** theRow, char** azColName) {
    if (data) {
        CAccountName name;
        for (int i = 0; i < nColumns; i++)
            name.setValueByName(azColName[i], theRow[i] ? theRow[i] : "");
        CAccountNameArray* array = (CAccountNameArray*)data;
        array->push_back(name);
    }
    return 0;
}

extern const char* STR_NAMES_DB_CREATE;
extern const char* STR_NAMES_DB_INSERT;
bool insertNameRecords(sqlite3* db, const string_q& path) {
    CStringArray lines;
    asciiFileToLines(path, lines);
    CStringArray fields;
    bool first = true;
    for (auto line : lines) {
        if (first) {
            explode(fields, line, '\t');
            first = false;

        } else {
            CAccountName name;
            replaceAll(line, "'", "");
            name.parseText(fields, line);
            if (isZeroAddr(name.address))
                continue;
            char* zErrMsg = 0;
            int rc = sqlite3_exec(db, name.Format(STR_NAMES_DB_INSERT).c_str(), noopCallbackSQL, 0, &zErrMsg);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "Could not insert record: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
                return false;
            }
        }
    }
    return true;
}

bool COptionsBase::loadNamesDatabaseFromSQL(CAccountNameArray& names) {
    CAccountName::registerClass();

    string_q dbPath = getCachePath("names/names.db");

    sqlite3* db = NULL;
    char* zErrMsg = 0;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Could not open %s: %s\n", dbPath.c_str(), zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }

    if (!fileExists(dbPath)) {
        rc = sqlite3_exec(db, STR_NAMES_DB_CREATE, noopCallbackSQL, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Could not create table: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
            return false;
        }

        if (!insertNameRecords(db, configPath("names/names.tab"))) {
            sqlite3_close(db);
            return false;
        }
        if (!insertNameRecords(db, configPath("names/names_custom.tab"))) {
            sqlite3_close(db);
            return false;
        }

    } else {
        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Could not open %s: %s\n", dbPath.c_str(), zErrMsg);
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
            return false;
        }
    }

    SHOW_FIELD(CAccountName, "deleted");
    const char* selectCmd = "SELECT * from NAMES ORDER BY address";
    rc = sqlite3_exec(db, selectCmd, readCallbackSQL, &names, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}

const char* STR_NAMES_DB_CREATE =
    "CREATE TABLE NAMES("
    "tags CHAR(30) NOT NULL,"
    "address CHAR(42) NOT NULL,"
    "name CHAR(150) NOT NULL,"
    "symbol CHAR(45) NOT NULL,"
    "source CHAR(80) NOT NULL,"
    "description CHAR(125),"
    "decimals INT,"
    "deleted BOOL NOT NULL DEFAULT 0,"
    "is_custom BOOL NOT NULL DEFAULT 0,"
    "is_prefund BOOL NOT NULL DEFAULT 0,"
    "is_contract BOOL NOT NULL DEFAULT 0,"
    "is_erc20 BOOL NOT NULL DEFAULT 0,"
    "is_erc721 BOOL NOT NULL DEFAULT 0"
    ");";

const char* STR_NAMES_DB_INSERT =
    "INSERT INTO NAMES "
    "([{p:TAGS}],"
    "[{p:ADDRESS}],"
    "[{p:NAME}],"
    "[{p:SYMBOL}],"
    "[{p:SOURCE}],"
    "[{p:DECIMALS}],"
    "[{p:DESCRIPTION}],"
    "[{p:DELETED}],"
    "[{p:IS_CUSTOM}],"
    "[{p:IS_PREFUND}],"
    "[{p:IS_CONTRACT}],"
    "[{p:IS_ERC20}],"
    "[{p:IS_ERC721}]"
    ") VALUES ("
    "'[{TAGS}]',"
    "'[{ADDRESS}]',"
    "'[{NAME}]',"
    "'[{SYMBOL}]',"
    "'[{SOURCE}]',"
    "'[{DECIMALS}]',"
    "'[{DESCRIPTION}]',"
    "[{DELETED}],"
    "[{IS_CUSTOM}],"
    "[{IS_PREFUND}],"
    "[{IS_CONTRACT}],"
    "[{IS_ERC20}],"
    "[{IS_ERC721}]"
    "); ";

}  // namespace qblocks