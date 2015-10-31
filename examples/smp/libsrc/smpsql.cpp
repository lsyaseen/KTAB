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
// Demonstrate a very basic, but highly parameterized, Spatial Model of Politics.
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


  string SMPModel::createTableSQL(unsigned int tn) {
    string sql = "";
    switch (tn) {
    case 0: // position-utility table
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
      throw(KException("SMPModel::createTableSQL unrecognized table number"));
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
    int  rc;
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

    // Create & execute SQL statements
    for (unsigned int i = 0; i < 12; i++) {
      auto buff = newChars(50);
      sprintf(buff, "Created table %i successfully \n", i);
      sql = createTableSQL(i);
      rc = sExec(sql, buff);
      buff = nullptr;
      cout << flush;
    }
    cout << endl << flush;

    smpDB = db;
    return;
  }

  void SMPModel::sqlAUtil(unsigned int t) {
    // output the actor util table to sqlite 

    assert(nullptr != smpDB);
    assert(t < history.size());
    auto st = ((SMPState*)(history[t]));
    assert(nullptr != st);
    assert(numAct == st->aUtil.size());

    sqlite3 * db = smpDB; // I don't like passing 'this' into lambda-functions
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
    // What makes a huge difference is bundling them up in one atomic "transaction".
    // For this case, runtime droped from 62-65 seconds to 0.5-0.6 (vs. 0.30-0.33 with no SQL at all).
    
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg); 
    for (unsigned int h = 0; h < numAct; h++) { // estimator is h
      KMatrix uij = st->aUtil[h]; // utility to actor i of the position held by actor j
      for (unsigned int i = 0; i < numAct; i++){
        for (unsigned int j = 0; j < numAct; j++){
          int rslt = 0;
          rslt = sqlite3_bind_int(insStmt, 1, t); assert(SQLITE_OK == rslt);
          rslt = sqlite3_bind_int(insStmt, 2, h); assert(SQLITE_OK == rslt);
          rslt = sqlite3_bind_int(insStmt, 3, i); assert(SQLITE_OK == rslt);
          rslt = sqlite3_bind_int(insStmt, 4, j); assert(SQLITE_OK == rslt);
          rslt = sqlite3_bind_double(insStmt, 5, uij(i, j)); assert(SQLITE_OK == rslt);
          rslt = sqlite3_step(insStmt); assert(SQLITE_DONE == rslt);
          sqlite3_clear_bindings(insStmt); assert(SQLITE_DONE == rslt);
          rslt = sqlite3_reset(insStmt); assert(SQLITE_OK == rslt);
        }
      }
    }
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
    printf("Stored SQL for turn %i of all estimators, actors, and positions \n", t);

    delete sqlBuff;
    sqlBuff = nullptr;

    smpDB = db; // give it the new pointer
    return;
  }



}; // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
