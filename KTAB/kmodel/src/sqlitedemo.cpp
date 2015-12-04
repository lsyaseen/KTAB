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
#include "sqlitedemo.h"

using std::cout;
using std::endl;
using std::flush;
using std::string;
using std::vector;

// ------------------------------------------------------------------------------------

namespace MDemo {

  SQLDB::SQLDB(char* filename) {
    database = NULL;
    dbOpen = open(filename);
  }

  SQLDB::~SQLDB() {
    if (dbOpen && (nullptr != database)) {
      close();
    }
    database = nullptr;
  }

  bool SQLDB::open(char* filename) {
    if (sqlite3_open(filename, &database) == SQLITE_OK) {
      return true;
    }
    else {
      return false;
    }
  }

  vector<vector<string> > SQLDB::query(char* query) {
    assert(nullptr != database);
    assert(dbOpen);
    sqlite3_stmt *statement = nullptr;
    vector<vector<string> > results = {};
    if (sqlite3_prepare_v2(database, query, -1, &statement, 0) == SQLITE_OK) {
      int cols = sqlite3_column_count(statement);
      int result = 0;
      while (true) {
        result = sqlite3_step(statement);
        if (result == SQLITE_ROW) {
          vector<string> values;
          for (int col = 0; col < cols; col++) {
            values.push_back((char*)sqlite3_column_text(statement, col));
          }
          results.push_back(values);
        }
        else {
          break; // end the loop
        }
      }
      sqlite3_finalize(statement);
    }
    string error = sqlite3_errmsg(database);
    if (error != "not an error") {
      cout << query << " " << error << endl;
    }
    return results;
  }

  void SQLDB::close() {
    if (dbOpen) {
      sqlite3_close(database);
      dbOpen = false;
    }
    return;
  }

  void demoDBObject() {
    auto db = new SQLDB("myDB.db");
    db->query("CREATE TABLE ta (a INTEGER, b INTEGER, c INTEGER);");
    db->query("INSERT INTO ta VALUES(1, 2, 3);");
    db->query("INSERT INTO ta VALUES(5, 4, 6);");
    vector<vector<string> > result = db->query("SELECT * FROM ta WHERE c>5;");
    for (vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it) {
      vector<string> row = *it;
      cout << "Values: (A=" << row.at(0) << ", B=" << row.at(1) << ", C=" << row.at(2) << ")" << endl;
    }

    //db->close();
    delete db;
    db = nullptr;
    return;
  }


} // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
