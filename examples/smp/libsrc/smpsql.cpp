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
//
// Demonstrate a very basic SQLite database for the Spatial Model of Politics.
//
// --------------------------------------------

#include "smp.h"
#include "sqlite3.h"
#include "csv_parser.hpp"


namespace SMPLib {
using std::cout;
using std::endl;
using std::flush;
using std::function;
using std::get;
using std::string;

using KBase::PRNG;
using KBase::KMatrix;
using KBase::KException;
using KBase::Actor;
using KBase::Model;
using KBase::Position;
using KBase::State;
using KBase::VotingRule;
using KBase::ReportingLevel;

// --------------------------------------------

string SMPModel::createSQL(unsigned int n)  {
    string sql = "";
	// check total number of table exceeds
	assert(n < Model::NumTables + NumTables);
	if (n < Model::NumTables) {
		return Model::createSQL(n);
	}
	else
	{
		switch (n-Model::NumTables) {
		case  0: // coordinates of each actor's position
			sql = "create table if not exists VectorPosition ("  \
				"ScenarioId TEXT(32) NOT NULL DEFAULT 'None', "\
				"Turn_t		INTEGER NOT NULL DEFAULT 0, "\
				"Act_i		INTEGER NOT NULL DEFAULT 0, "\
				"Dim_k		INTEGER NOT NULL DEFAULT 0, "\
				"Coord		REAL NOT NULL DEFAULT 0"\
				 ");";
			break;

		case 1: // salience to each actor of each dimension
			sql = "create table if not exists SpatialSalience ("  \
				"ScenarioId TEXT(32) NOT NULL DEFAULT 'None', "\
				"Turn_t		INTEGER NOT NULL DEFAULT 0, "\
				"Act_i		INTEGER NOT NULL DEFAULT 0, "\
				"Dim_k		INTEGER NOT NULL DEFAULT 0, "\
				"Sal		REAL NOT NULL DEFAULT 0.0"\
				");";
			break;

		case 2: // scalar capability of each actor
			sql = "create table if not exists SpatialCapability ("  \
				"ScenarioId TEXT(32) NOT NULL DEFAULT 'None', "\
				"Turn_t		INTEGER NOT NULL DEFAULT 0, "\
				"Act_i		INTEGER NOT NULL DEFAULT 0, "\
				"Cap		REAL NOT NULL DEFAULT 0.0"\
				");";
			break;

		default:
			throw(KException("SMPModel::createSQL unrecognized table number"));
		}
	}
    return sql;
}


void SMPModel::sqlTest() {
    // just a test to get linkages correct

    auto callBack = [](void *NotUsed, int argc, char **argv, char **azColName) {
        for (int i = 0; i < argc; i++) {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return ((int)0);
    };

    smpDB = nullptr;
    sqlite3 * db; // I don't like passing 'this' into lambda-functions
    char* zErrMsg = nullptr;
    string sql;

    auto sOpen = [&db](unsigned int n) {
        int rc = sqlite3_open("test.db", &db);
        if (rc != SQLITE_OK) {
            fprintf(stdout, "Can't open database: %s\n", sqlite3_errmsg(db));
            exit(0);
        }
        else {
            fprintf(stdout, "Successfully opened database #%i\n", n);
        }
        cout << endl << flush;
        return;
    };

    auto sExec = [&db, callBack, &zErrMsg](string sql, string msg) {
        int rc = sqlite3_exec(db, sql.c_str(), callBack, nullptr, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stdout, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        else {
            fprintf(stdout, msg.c_str());
        }
        return rc;
    };

    // Open database
    cout << endl << flush;
    sOpen(1);

    // we are not dealing with a long-term, mission-critical database,
    // so we can shut off some of the journaling stuff intended to protect
    // the DB in case the system crashes in mid-operation
    sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
    sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &zErrMsg);
	sqlite3_exec(db, "PRAGMA foreign_keys = ON", NULL, NULL, &zErrMsg);

    // Create & execute SQL statements
    for (unsigned int i = 0; i < SMPModel::NumTables +Model::NumTables; i++) {
        auto buff = newChars(50);
        sprintf(buff, "Created SMPModel table %u successfully \n", i);
        sql = SMPModel::createSQL(i);
        sExec(sql, buff); // ignore return-code
        delete buff;
        buff = nullptr;
        cout << flush;
    }
    cout << endl << flush;

    smpDB = db;
    return;
}

 
  
}; // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
