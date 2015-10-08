// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2015 King Abdullah Petroleum Studies and Research Center
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// --------------------------------------------

#include <cstdlib>   // get 'exit' function
#include <string>
#include <iostream>
#include <stdio.h>
#include <sqlite3.h>

using std::string;
using std::cout;
using std::endl;
using std::flush;

// ------------------------------------------------------------------------------------

namespace UDemo {
void demoSQLite () {

    auto callBack = [] (void *NotUsed, int argc, char **argv, char **azColName) {
        for(int i=0; i<argc; i++) {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return ((int) 0);
    };

    sqlite3 *db;
    char* zErrMsg = nullptr;
    int  rc;
    string sql;

    auto sOpen = [&db] (unsigned int n) {
        int rc = sqlite3_open("test.db", &db);
        if( rc != SQLITE_OK ) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            exit(0);
        } else {
            fprintf(stderr, "Opened database successfully (%i)\n", n);
        }
        return;
    };

    auto sExec = [callBack, &db, &zErrMsg] (string sql, string msg) {
        int rc = sqlite3_exec(db, sql.c_str(), callBack, nullptr, &zErrMsg);
        if( rc != SQLITE_OK ) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, msg.c_str());
        }
        return rc;
    };




    // Open database
    cout << endl << flush;
    sOpen(1);

    // Create SQL statement
    sql = "create table PETS("  \
          "ID INT PRIMARY KEY     NOT NULL," \
          "NAME           text    NOT NULL," \
          "AGE            int     NOT NULL," \
          "BREED         char(50)," \
          "COLOR         char(50) );";

    // Execute SQL statement
    rc = sExec(sql, "Created table successfully \n");
    sqlite3_close(db);

    
    cout << endl << flush;
    sOpen(2);

    // This SQL statement has one deliberate error.
    sql = "INSERT INTO PETS (ID, NAME, AGE, BREED, COLOR) "   \
          "VALUES (1, 'Alice', 6, 'Greyhound', 'Grey' ); "    \
          "INSERT INTO PETS (ID, NAME, AGE, BREED, COLOR) "   \
          "VALUES (2, 'Bob', 4, 'Newfoundland', 'Black' ); "  \
          "INSERT INTO PETS (ID, NAME, AGE, BREED, COLOR) "   \
          "VALUES (3, 'Carol', 7, 'Chihuahua', 'Tan' );"      \
          "INSERT INTO PETS (ID, NAME, AGE, SPECIES, COLOR) " \
          "VALUES (4, 'David', 5, 'Alsation', 'Mixed' );";    \
          "INSERT INTO PETS (ID, NAME, AGE, SPECIES, COLOR) " \
          "VALUES (5, 'Ellie', 8, 'Rhodesian', 'Red' );";

    cout << "NB: This should get one planned SQL error" << endl << flush;
    rc = sExec(sql, "Records inserted successfully \n"); 
    sqlite3_close(db);
 
    
    cout << endl << flush;
    sOpen(3); 
    sql = "SELECT * from PETS where AGE>5;"; 
    rc = sExec(sql, "Records selected successfully\n"); 
    cout << "NB: ID=5 was never inserted due to planned SQL error" << endl;
    sqlite3_close(db);

 
    cout << endl << flush;
    sOpen(4); 
    sql = "DROP TABLE PETS;"; 
    rc = sExec(sql, "Dropped table successfully \n"); 
    sqlite3_close(db);

    return;
}
} // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
