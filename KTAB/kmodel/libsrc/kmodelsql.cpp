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

#include <assert.h>
#include <iostream>

#include "kmodel.h"


namespace KBase {

using std::cout;
using std::endl;
using std::flush;
using std::get;
using std::tuple;


// --------------------------------------------

void Model::demoSQLite() {
    cout << endl << "Starting basic demo of SQLite in Model class" << endl;

    auto callBack = [](void *data, int numCol, char **stringFields, char **colNames) {
        for (int i = 0; i < numCol; i++) {
            printf("%s = %s\n", colNames[i], stringFields[i] ? stringFields[i] : "NULL");
        }
        printf("\n");
        return ((int)0);
    };

    sqlite3 *db = nullptr;
    char* zErrMsg = nullptr;
    int  rc;
    string sql;

    auto sOpen = [&db](unsigned int n) {
        int rc = sqlite3_open("test.db", &db);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            exit(0);
        }
        else {
            fprintf(stderr, "Opened database successfully (%i)\n", n);
        }
        return;
    };

    auto sExec = [callBack, &db, &zErrMsg](string sql, string msg) {
        int rc = sqlite3_exec(db, sql.c_str(), callBack, nullptr, &zErrMsg); // nullptr is the 'data' argument
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
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
          "VALUES (4, 'David', 5, 'Alsation', 'Mixed' );";
    \
    "INSERT INTO PETS (ID, NAME, AGE, SPECIES, COLOR) " \
    "VALUES (5, 'Ellie', 8, 'Rhodesian', 'Red' );";

    cout << "NB: This should get one planned SQL error at ID=4" << endl << flush;
    rc = sExec(sql, "Records inserted successfully \n");
    sqlite3_close(db);


    cout << endl << flush;
    sOpen(3);
    sql = "SELECT * from PETS where AGE>5;";
    rc = sExec(sql, "Records selected successfully\n");
    cout << "NB: ID=5 was never inserted due to planned SQL error at ID=4" << endl;
    sqlite3_close(db);


    cout << endl << flush;
    sOpen(4);
    sql = "DROP TABLE PETS;";
    rc = sExec(sql, "Dropped table successfully \n");
    sqlite3_close(db);

    return;
}



// note that the function to write to table #k must be kept
// synchronized with the result of createTableSQL(k) !
string Model::createTableSQL(unsigned int tn) {
    string sql = "";
    switch (tn) {
    case 0:
        // position-utility table
        // the estimated utility to each actor of each other's position
        sql = "create table PosUtil ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Act_i	INTEGER NOT NULL DEFAULT 0, "\
              "Pos_j	INTEGER NOT NULL DEFAULT 0, "\
              "Util	REAL"\
              ");";
        break;

    case 1: // pos-vote table
        // estimated vote of each actor between each pair of positions
        sql = "create table PosVote ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Voter_k	INTEGER NOT NULL DEFAULT 0, "\
              "Pos_i	INTEGER NOT NULL DEFAULT 0, "\
              "Pos_j	INTEGER NOT NULL DEFAULT 0, "\
              "Vote	REAL"\
              ");";
        break;

    case 2: // pos-prob table. Note that there may be duplicates, unless we limit it to unique positions
        sql = "create table PosProb ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Pos_i	INTEGER NOT NULL DEFAULT 0, "\
              "Prob	REAL"\
              ");";
        break;

    case 3: // pos-equiv table. E(i)= lowest j s.t. Pos(i) ~ Pos(j). if j < i, it is not unique.
        sql = "create table PosEquiv ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Pos_i	INTEGER NOT NULL DEFAULT 0, "\
              "Eqv_j	INTEGER NOT NULL DEFAULT 0 "\
              ");";
        break;

    case 4:
        // Utilities are evaluated so that UtilSQ, UtilVict, UtilChlg, UtilContest,
        // UtilTPVict, UtilTPLoss are comparable, i.e. the differences are meaningful
        sql = "create table UtilContest ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Aff_k	INTEGER NOT NULL DEFAULT 0, "\
              "Init_i	INTEGER NOT NULL DEFAULT 0, "\
              "Rcvr_j	INTEGER NOT NULL DEFAULT 0, "\
              "Util	REAL"\
              ");";
        break;

    case 5:
        // Utilities are evaluated so that UtilSQ, UtilVict, UtilChlg, UtilContest,
        // UtilTPVict, UtilTPLoss are comparable, i.e. the differences are meaningful
        sql = "create table UtilChlg ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Aff_k	INTEGER NOT NULL DEFAULT 0, "\
              "Init_i	INTEGER NOT NULL DEFAULT 0, "\
              "Rcvr_j	INTEGER NOT NULL DEFAULT 0, "\
              "Util	REAL"\
              ");";
        break;

    case 6:
        // Utilities are evaluated so that UtilSQ, UtilVict, UtilChlg, UtilContest,
        // UtilTPVict, UtilTPLoss are comparable, i.e. the differences are meaningful
        sql = "create table UtilVict ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Aff_k	INTEGER NOT NULL DEFAULT 0, "\
              "Init_i	INTEGER NOT NULL DEFAULT 0, "\
              "Rcvr_j	INTEGER NOT NULL DEFAULT 0, "\
              "Util	REAL"\
              ");";
        break;

    case 7:
        // h's estimate that i will defeat j, including third party contributions
        sql = "create table ProbVict ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Init_i	INTEGER NOT NULL DEFAULT 0, "\
              "Rcvr_j	INTEGER NOT NULL DEFAULT 0, "\
              "Prob	REAL"\
              ");";
        break;

    case 8:
        // h's estimate of utility to k of status quo.
        // Utilities are evaluated so that UtilSQ, UtilVict, UtilChlg, UtilContest,
        // UtilTPVict, UtilTPLoss are comparable, i.e. the differences are meaningful
        sql = "create table UtilSQ ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Aff_k	INTEGER NOT NULL DEFAULT 0, "\
              "Util	REAL"\
              ");";
        break;

    case 9:  // probability ik > j
        sql = "create table ProbTPVict ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Init_i	INTEGER NOT NULL DEFAULT 0, "\
              "ThrdP_k	INTEGER NOT NULL DEFAULT 0, "\
              "Rcvr_j	INTEGER NOT NULL DEFAULT 0, "\
              "Prob	REAL"\
              ");";
        break;

    case 10: // utility to k of ik>j
        // Utilities are evaluated so that UtilSQ, UtilVict, UtilChlg, UtilContest,
        // UtilTPVict, UtilTPLoss are comparable, i.e. the differences are meaningful
        sql = "create table UtilTPVict ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Init_i	INTEGER NOT NULL DEFAULT 0, "\
              "ThrdP_k	INTEGER NOT NULL DEFAULT 0, "\
              "Rcvr_j	INTEGER NOT NULL DEFAULT 0, "\
              "Util	REAL"\
              ");";
        break;

    case 11: // utility to k of i>jk
        // Utilities are evaluated so that UtilSQ, UtilVict, UtilChlg, UtilContest,
        // UtilTPVict, UtilTPLoss are comparable, i.e. the differences are meaningful
        sql = "create table UtilTPLoss ("  \
              "Scenario	TEXT NOT NULL DEFAULT 'NoName', "\
              "Turn_t	INTEGER NOT NULL DEFAULT 0, "\
              "Est_h	INTEGER NOT NULL DEFAULT 0, "\
              "Init_i	INTEGER NOT NULL DEFAULT 0, "\
              "ThrdP_k	INTEGER NOT NULL DEFAULT 0, "\
              "Rcvr_j	INTEGER NOT NULL DEFAULT 0, "\
              "Util	REAL"\
              ");";
        break;

    default:
        throw(KException("Model::createTableSQL unrecognized table number"));
    }

    return sql;
}


void Model::sqlAUtil(unsigned int t) {
    assert(nullptr != smpDB);
    assert(t < history.size());
    State* st = history[t];
    assert(nullptr != st);
    assert(numAct == st->aUtil.size());

    // I don't like passing 'this' into lambda-functions,
    // so I copy the pointer into a local variable I can pass in
    // without exposing everything in 'this'. If this were a big,
    // mission-critical RDBMS, rather than a 1-off record of this run,
    // doing so might be disasterous in case the system crashed before
    // things were cleaned up.
    sqlite3 * db = smpDB;
    smpDB = nullptr;

    char* zErrMsg = nullptr;
    auto sqlBuff = newChars(200);
    sprintf(sqlBuff,
            "INSERT INTO PosUtil (Scenario, Turn_t, Est_h, Act_i, Pos_j, Util) VALUES ('%s', ?1, ?2, ?3, ?4, ?5)",
            scenName.c_str());
    const char* insStr = sqlBuff;
    sqlite3_stmt *insStmt;
    sqlite3_prepare_v2(db, insStr, strlen(insStr), &insStmt, NULL);
    // Prepared statements cache the execution plan for a query after the query optimizer has
    // found the best plan, so there is no big gain with simple insertions.
    // What makes a huge difference is bundling a few hundred into one atomic "transaction".
    // For this case, runtime droped from 62-65 seconds to 0.5-0.6 (vs. 0.30-0.33 with no SQL at all).

    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);
    for (unsigned int h = 0; h < numAct; h++) { // estimator is h
        KMatrix uij = st->aUtil[h]; // utility to actor i of the position held by actor j
        for (unsigned int i = 0; i < numAct; i++) {
            for (unsigned int j = 0; j < numAct; j++) {
                int rslt = 0;
                rslt = sqlite3_bind_int(insStmt, 1, t);
                assert(SQLITE_OK == rslt);
                rslt = sqlite3_bind_int(insStmt, 2, h);
                assert(SQLITE_OK == rslt);
                rslt = sqlite3_bind_int(insStmt, 3, i);
                assert(SQLITE_OK == rslt);
                rslt = sqlite3_bind_int(insStmt, 4, j);
                assert(SQLITE_OK == rslt);
                rslt = sqlite3_bind_double(insStmt, 5, uij(i, j));
                assert(SQLITE_OK == rslt);
                rslt = sqlite3_step(insStmt);
                assert(SQLITE_DONE == rslt);
                sqlite3_clear_bindings(insStmt);
                assert(SQLITE_DONE == rslt);
                rslt = sqlite3_reset(insStmt);
                assert(SQLITE_OK == rslt);
            }
        }
    }
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
    printf("Stored SQL for turn %u of all estimators, actors, and positions \n", t);

    delete sqlBuff;
    sqlBuff = nullptr;

    smpDB = db; // give it the new pointer

    return;
}

} // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
