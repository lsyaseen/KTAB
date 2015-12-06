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

  Position::Position() {}
  Position::~Position() {}


  Model::Model(PRNG * r) {
    history = vector<State*>();
    actrs = vector<Actor*>();
    numAct = 0;
    stop = nullptr;
    assert(nullptr != r);
    rng = r;
  }


  Model::~Model() {
    while (0 < history.size()){
      State* s = history[history.size() - 1];
      delete s;
      history.pop_back();
    }

    while (0 < actrs.size()) {
      Actor * a = actrs[actrs.size() - 1];
      delete a;
      actrs.pop_back();
    }
    numAct = 0;
    rng = nullptr;
  }


  void Model::run() {
    assert(1 == history.size());
    State* s0 = history[0];
    bool done = false;
    unsigned int iter = 0;
    while (!done) {
      assert(nullptr != s0);
      assert(nullptr != s0->step);
      iter++;
      cout << "Starting Model::run iteration " << iter << endl;
      auto s1 = s0->step();
      addState(s1);
      done = stop(iter, s1);
      s0 = s1;
    }
    return;
  }

  string Model::VPMName(VPModel vpm) {
    string s;
    switch (vpm) {
    case  Model::VPModel::Linear:
      s = "Linear";
      break;
    case Model::VPModel::Square:
      s = "Square";
      break;
    case Model::VPModel::Quartic:
      s = "Quartic";
      break;
    case Model::VPModel::Binary:
      s = "Binary";
      break;
    default:
      cout << "Model::VPMName: unrecognized VPModel" << endl;
      assert(false);
      break;
    }
    return s;
  }


  unsigned int Model::addActor(Actor* a) {
    assert(nullptr != a);
    actrs.push_back(a);
    numAct = actrs.size();
    return numAct;
  }


  int Model::addState(State* s) {
    assert(nullptr != s);
    assert(this == s->model);
    s->model = this;
    history.push_back(s);
    return history.size();
  }


  void Model::demoSQLite() {
    cout << endl << "Starting basic demo of SQLite in Model class"<<endl;
    
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
    printf("Stored SQL for turn %u of all estimators, actors, and positions \n", t);

    delete sqlBuff;
    sqlBuff = nullptr;

    smpDB = db; // give it the new pointer
    
    return;
  }

  int Model::actrNdx(const Actor* a) const {
    int ai = -1;
    for (unsigned int i = 0; i < numAct; i++) {
      if (a == actrs[i]) {
        ai = i;
      }
    }
    return ai; // negative iff this "actor" was not found
  }

  double Model::nProd(double x, double y) {
    if ((0.0 < x) && (0.0 < y)) { return (x * y); }
    if ((x < 0.0) && (y < 0.0)) { return (x + y); }
    return ((x < y) ? x : y);
  }

  string vrName(VotingRule vr) {
    string vrn = "";
    switch (vr) {
    case VotingRule::Binary:
      vrn = "Binary";
      break;
    case VotingRule::PropBin:
      vrn = "PropBin";
      break;
    case VotingRule::Proportional:
      vrn = "Prop";
      break;
    case VotingRule::PropCbc:
      vrn = "PropCbc";
      break;
    case VotingRule::Cubic:
      vrn = "Cubic";
      break;
    default:
      throw KException("vrName - Unrecognized VotingRule");
      break;
    }
    return vrn;
  }

  string tpcName(ThirdPartyCommit tpc) {
    string tpcn;
    switch (tpc) {
    case ThirdPartyCommit::FullCommit:
      tpcn = "FullCommit";
      break;
    case ThirdPartyCommit::SemiCommit:
      tpcn = "SemiCommit";
      break;
    case ThirdPartyCommit::NoCommit:
      tpcn = "NoCommit";
      break;

    default:
      throw KException("tpcName - Unrecognized ThirdPartyCommit");
      break;

    }

    return tpcn;
  }

  double Model::vote(VotingRule vr, double wi, double uij, double uik) {
    double v = 0.0;
    const double du = uij - uik;

    double rBin = 0; // binary response
    const double sTol = 1E-10;
    rBin = du / sTol;
    rBin = (rBin > +1) ? +1 : rBin;
    rBin = (rBin < -1) ? -1 : rBin;

    double rProp = du; // proportional response
    double rCubic = du * du * du; // cubic reponse

    // the following weights determine how much the hybrids deviate from proportional
    const double rbp = 0.8;
    // rbp = 0.8 makes LHS slope and RHS slope of equal size (0.8 each), and twice the center jump (0.4)
    const double rpc = 0.5196;
    // rpc is chosen so max deviation of PropCbc (at 1/sqrt(3)) equals max deviation by PropBin (at 0):
    // rpc = (3*sqrt(3)*rbp)/2
    switch (vr) {
    case VotingRule::Binary:
      v = wi * rBin;
      break;

    case VotingRule::PropBin:
      v = wi * ((1 - rbp)*rProp + rbp*rBin);
      break;

    case VotingRule::Proportional:
      v = wi * rProp;
      break;

    case VotingRule::PropCbc:
      v = wi * ((1 - rpc)*rProp + rpc*rCubic);
      break;

    case VotingRule::Cubic:
      v = wi* rCubic;
      break;

    default:
      throw KException("Model::vote - Unrecognized VotingRule");
      break;
    }
    return v;
  }

  tuple<double, double> Model::vProb(VPModel vpm, const double s1, const double s2) {
    const double tol = 1E-8;
    const double minX = 1E-6;
    double x1 = 0;
    double x2 = 0;
    switch (vpm) {
    case VPModel::Linear:
      x1 = s1;
      x2 = s2;
      break;
    case VPModel::Square:
      x1 = KBase::sqr(s1);
      x2 = KBase::sqr(s2);
      break;
    case VPModel::Quartic:
      x1 = KBase::qrtc(s1);
      x2 = KBase::qrtc(s2);
      break;
    case VPModel::Binary:
    {
    // this is setup so that 10% or more advantage either way gives a guaranteed
    // result. As with binary voting, it is necessary to have interpolation between
    // to avoid weird round-off effects.
      const double thresh = 1.10; 
      if (s1 >= thresh*s2){
        x1 = 1.0;
        x2 = minX;
      }
      else if (s2 >= thresh*s1){
        x1 = minX; 
        x2 = 1.0;
      }
      else { // less than the threshold difference
	double r12 = s1/(s1+s2);
	// We now need a linear rescaling so that
	// when s1/s2 = t, or r12 = t/(t+1), p12 = 1, and 
	// when s2/s1 = t, or r12 = 1/(1+t), p12 =0.
	// We can work out (a,b) so that
	// a*(t/(t+1)) + b = 1, and 
	// a*(1/(1+t)) + b = 0, then
	// verify that a*(1/(1+1))+b = 1/2.
	//
	double p12 = (r12*(thresh+1.0)-1.0)/(thresh-1.0);
        x1 = p12;
        x2 = 1-p12;
      }
    }
    break;
    }
    double p1 = x1 / (x1 + x2);
    double p2 = x2 / (x1 + x2);
    /*
    printf("s1: %+.4f \n", s1);
    printf("s2: %+.4f \n", s2);
    printf("x1: %+.4f \n", x1);
    printf("x2: %+.4f \n", x2);
    printf("p1: %+.4f \n", p1);
    printf("p2: %+.4f \n", p2);
    cout << endl << flush;
    */
    assert(0 <= p1);
    assert(0 <= p2);
    assert(fabs(p1 + p2 - 1.0) < tol);

    return tuple<double, double>(p1, p2);
  }

  // note that while the C_ij can be any arbitrary positive matrix
  // with C_kk = 0, the p_ij matrix has the symmetry pij + pji = 1
  // (and hence pkk = 1/2).
  KMatrix Model::vProb(VPModel vpm, const KMatrix & c) {
    unsigned int numOpt = c.numR();
    assert(numOpt == c.numC());
    auto p = KMatrix(numOpt, numOpt);
    for (unsigned int i = 0; i < numOpt; i++) {
      for (unsigned int j = 0; j < i; j++) {
        double cij = c(i, j);
        assert(0 <= cij);
        double cji = c(j, i);
        assert(0 <= cji);
        assert((0 < cij) || (0 < cji));
        auto ppr = vProb(vpm, cij, cji);
        p(i, j) = get<0>(ppr); // set the lower left  probability: if Linear, cij / (cij + cji)
        p(j, i) = get<1>(ppr); // set the upper right probability: if Linear, cji / (cij + cji)
      }
      p(i, i) = 0.5; // set the diagonal probability
    }
    return p;
  }

  KMatrix Model::coalitions(function<double(unsigned int ak, unsigned int pi, unsigned int pj)> vfn,
    unsigned int numAct, unsigned int numOpt) {
    // if several actors occupy the same position, then numAct > numOpt 
    const double minC = 1E-8;
    auto c = KMatrix(numOpt, numOpt);
    for (unsigned int i = 0; i < numOpt; i++) {
      for (unsigned int j = 0; j < numOpt; j++) {
        if (j < i) { // I have my reasons for doing it this way this time
          double cij = minC;
          double cji = minC;
          for (unsigned int k = 0; k < numAct; k++) {
            double vkij = vfn(k, i, j);
            if (vkij > 0) { cij = cij + vkij; }
            if (vkij < 0) { cji = cji - vkij; }
          }
          c(i, j) = cij;  // set the lower left coalition
          c(j, i) = cji;  // set the upper right coalition
        }
      }
      c(i, i) = minC; // set the diagonal coalition
    }
    return c;
  }

  // these are assumed to be unique options.
  // returns a square matrix.
  KMatrix Model::vProb(VotingRule vr, VPModel vpm, const KMatrix & w, const KMatrix & u) {
    // u_ij is utility to actor i of the position advocated by actor j
    unsigned int numAct = u.numR();
    unsigned int numOpt = u.numC();
    // w_j is row-vector of actor weights, for simple voting 
    assert(numAct == w.numC()); // require 1-to-1 matching of actors and strengths
    assert(1 == w.numR()); // weights must be a row-vector

    auto vfn = [vr, &w, &u](unsigned int k, unsigned int i, unsigned int j) {
      double vkij = vote(vr, w(0, k), u(k, i), u(k, j));
      return vkij;
    };

    auto c = coalitions(vfn, numAct, numOpt); // c(i,j) = strength of coaltion for i against j
    KMatrix p = vProb(vpm, c);  // p(i,j) = prob Ai defeats Aj
    return p;
  }



  // Given square matrix of Prob[i>j] returns a column vector for Prob[i]
  KMatrix Model::probCE(const KMatrix & pv) {
    const double pTol = 1E-6;
    unsigned int numOpt = pv.numR();
    assert(numOpt == pv.numC()); // must be square
    auto test = [&pv, pTol](unsigned int i, unsigned int j) {
      assert(0 <= pv(i, j));
      assert(fabs(pv(i, j) + pv(j, i) - 1.0) < pTol);
      return;
    };
    KMatrix::mapV(test, numOpt, numOpt); // catch gross errors 

    // TODO: add a switch between markovPCE and condPCE?
    KMatrix p = markovPCE(pv); 
    //KMatrix p = condPCE(pv);

    assert(fabs(sum(p) - 1.0) < pTol);
    return p;
  }


  // Given square matrix of Prob[i>j] returns a column vector for Prob[i].
  // Uses Markov process, not 1-step conditional probability
  KMatrix Model::markovPCE(const KMatrix & pv) {
    const double pTol = 1E-6;
    unsigned int numOpt = pv.numR();
    auto p = (KMatrix(numOpt, 1) + 1) / numOpt;  // all 1/n
    auto q = p;
    unsigned int iMax = 1000;  // 10-30 is typical
    unsigned int iter = 0;
    double change = 1.0;
    while (pTol < change) {
      change = 0;
      for (unsigned int i = 0; i < numOpt; i++){
        double pi = 0.0;
        for (unsigned int j = 0; j < numOpt; j++) {
          pi = pi + pv(i, j)*(p(i, 0) + p(j, 0));
        }
        assert(0 <= pi); // double-check
        q(i, 0) = pi / numOpt;
        double c = fabs(q(i, 0) - p(i, 0));
        change = (c>change) ? c : change;
      }
      p = q;
      iter++;
      assert(fabs(sum(p) - 1.0) < pTol); // double-check
    }
    assert(iter < iMax); // no way to recover
    return p;
  }


  // Given square matrix of Prob[i>j] returns a column vector for Prob[i].
  // Uses 1-step conditional probabilities, not Markov process
  KMatrix Model::condPCE(const KMatrix & pv) {
    unsigned int numOpt = pv.numR();
    auto p = KMatrix(numOpt, 1);
    for (unsigned int i = 0; i < numOpt; i++) {
      double pi = 1.0;
      for (unsigned int j = 0; j < numOpt; j++) {
        pi = pi * pv(i, j);
      }
      // double-check
      assert(0 <= pi);
      assert(pi <= 1);
      p(i, 0) = pi; // probability that i beats all alternatives
    }
    double probOne = sum(p); // probability that one option, any option, beats all alternatives
    p = (p / probOne); // conditional probability that i is that one.
    return p;
  }

  // This assumes scalar capabilities of actors (w), so that the voting strength
  // is a direct function of difference in utilities.Therefore, we can use 
  // Model::vProb(VotingRule vr, const KMatrix & w, const KMatrix & u)
  KMatrix Model::scalarPCE(unsigned int numAct, unsigned int numOpt, const KMatrix & w, const KMatrix & u,
    VotingRule vr, VPModel vpm, ReportingLevel rl) {

    auto pv = Model::vProb(vr, vpm, w, u);
    auto p = Model::probCE(pv);

    if (ReportingLevel::Low < rl) {
      printf("Num actors: %i \n", numAct);
      printf("Num options: %i \n", numOpt);


      if ((numAct <= 20) && (numOpt <= 20)) {
        cout << "Actor strengths: " << endl;
        w.mPrintf(" %6.2f ");
        cout << endl << flush;
        cout << "Voting rule: " << KBase::vrName(vr) << endl;
        // printf("         aka %s \n", KBase::vrName(vr).c_str());
        cout << flush;
        cout << "Utility to actors of options: " << endl;
        u.mPrintf(" %+8.3f ");
        cout << endl;

        auto vfn = [vr, &w, &u](unsigned int k, unsigned int i, unsigned int j) {
          double vkij = vote(vr, w(0, k), u(k, i), u(k, j));
          return vkij;
        };

        auto c = coalitions(vfn, numAct, numOpt); // c(i,j) = strength of coaltion for i against j
        KMatrix p2 = vProb(vpm, c);  // p(i,j) = prob Ai defeats Aj

        cout << "Coalition strengths of (i:j): " << endl;
        c.mPrintf(" %8.3f ");
        cout << endl;

        assert(norm(pv - p2) < 1E-8); // better be close

        cout << "Probability Opt_i > Opt_j" << endl;
        pv.mPrintf(" %.4f ");
        cout << "Probability Opt_i" << endl;
        p.mPrintf(" %.4f ");
      }
      cout << "Found stable PCE distribution" << endl << flush;
    }
    return p;
  }


  // -------------------------------------------------
  Actor::Actor(string n, string d) {
    name = n;
    desc = d;
  }


  Actor::~Actor() {
    // empty
  }

  // this uses the evenly-weighted "Sum of Utilities" model
  // Note that the pi and pj numbers already include the contribution of k to the hypothetical little 
  // i:j conflict with just i,j,k involved
  // TODO: implement and offer the "Expected Value of Utilities" model
  double Actor::thirdPartyVoteSU(double wk, VotingRule vr, ThirdPartyCommit comm,
    double pik, double pjk, double uki, double ukj, double ukk) {
    const double pTol = 1E-8;
    assert(0 <= pik);
    assert(0 <= pjk);
    assert(fabs(pik + pjk - 1.0) < pTol);
    double p_ik_j = pik;
    double p_jk_i = pjk;
    double p_j_ik = pjk;
    double p_i_jk = pik;
    double u_ik_def_j = 0;
    double u_j_def_ik = 0;
    double u_i_def_jk = 0;
    double u_jk_def_i = 0;
    // strictly speaking, we should add in all the utilities of unchanged positions, then divide by numAct,
    // so that the utility of each alternative state is between 0 and 1. 
    // However, 
    // A: subtraction will make all utilities of unchanged positions cancel here when vk is determined, and 
    // B: dividing in P[1>2] = C12 / (C12 + C21) will cancel the factors of 1/4.
    //
    // Therefore, we can drop those terms as long we make sure all comparisons use the sum 
    // of three position-utilities, and we obviously do so.
    switch (comm) {
    case ThirdPartyCommit::FullCommit:
      u_ik_def_j = uki + uki + uki;
      u_j_def_ik = ukj + ukj + ukj;
      u_i_def_jk = uki + uki + uki;
      u_jk_def_i = ukj + ukj + ukj;
      break;
    case ThirdPartyCommit::SemiCommit:
      u_ik_def_j = uki + uki + ukk;
      u_j_def_ik = ukj + ukj + ukj;
      u_i_def_jk = uki + uki + uki;
      u_jk_def_i = ukj + ukj + ukk;
      break;
    case ThirdPartyCommit::NoCommit:
      u_ik_def_j = uki + uki + ukk;
      u_j_def_ik = ukj + ukj + ukk;
      u_i_def_jk = uki + uki + ukk;
      u_jk_def_i = ukj + ukj + ukk;
      break;
    }
    double u_ik_j = (p_ik_j * u_ik_def_j) + (p_j_ik * u_j_def_ik);
    double u_i_jk = (p_i_jk * u_i_def_jk) + (p_jk_i * u_jk_def_i);
    double vk = Model::vote(vr, wk, u_ik_j, u_i_jk);
    return vk;
  }


  // Actor n determines his contribution on influence by analyzing the hypothetical 
  // "little conflict" of just three actors, i.e.  (i:j) with n weighing in on whichever side it favors.
  double Actor::vProbLittle(VotingRule vr, double wn, double uni, double unj, double contrib_i_ij, double contrib_j_ij) {

    // the primary actors are assigned at least the minisucle
    // minimum influence contribution, to avoid 0/0 errors.
    if (0 < contrib_i_ij){
      assert(0 < contrib_i_ij);
    }
    if (0 < contrib_j_ij){
      assert(0 < contrib_j_ij);
    }

    double contrib_n_ij = Model::vote(vr, wn, uni, unj);
    double cni = (contrib_n_ij > 0) ? contrib_i_ij + contrib_n_ij : contrib_i_ij;
    double cnj = (contrib_n_ij < 0) ? contrib_j_ij - contrib_n_ij : contrib_j_ij;
    if (0 < cni) {
      assert(0 < cni);
    }
    if (0 < cnj) {
      assert(0 < cnj);
    }
    double pin = cni / (cni + cnj);
    //double pjn = cnj / (cni + cnj); // FYI
    return pin;
  }

} // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
