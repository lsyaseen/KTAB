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
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QCoreApplication>

namespace SMPLib {
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
// JAH 20160728 modified to return a KTable object instead of the SQL string
KTable * SMPModel::createSQL(unsigned int n)  {
  string sql = "";
  string name = "";
  unsigned int grpID = 0;
  // check total number of table exceeds
  //assert(n < Model::NumTables + NumTables);
  if (n >= Model::NumTables + NumTables) {
    throw KException("SMPModel::createSQL: Asked to create more number of tables than required");
  }
  if (n < Model::NumTables) {
    return Model::createSQL(n);
  }
  else
  {
    switch (n-Model::NumTables) {
    case  0: // coordinates of each actor's position
        sql = "create table if not exists VectorPosition ("  \
            "ScenarioId VARCHAR(32) NOT NULL DEFAULT 'None', "\
            "Turn_t  INTEGER NOT NULL DEFAULT 0, "\
            "Act_i  INTEGER NOT NULL DEFAULT 0, "\
            "Dim_k  INTEGER NOT NULL DEFAULT 0, "\
            "Pos_Coord  FLOAT NOT NULL DEFAULT 0,"\
            "Idl_Coord  FLOAT NOT NULL DEFAULT 0, " \
            "Mover_BargnId INTEGER NULL DEFAULT 0" \
      ");";
      name = "VectorPosition";
      grpID = 4;// JAH 20161010 put in group 4 all by itself
      break;

    case 1: // salience to each actor of each dimension
      sql = "create table if not exists SpatialSalience ("  \
            "ScenarioId VARCHAR(32) NOT NULL DEFAULT 'None', "\
            "Turn_t  INTEGER NOT NULL DEFAULT 0, "\
            "Act_i  INTEGER NOT NULL DEFAULT 0, "\
            "Dim_k  INTEGER NOT NULL DEFAULT 0, "\
            "Sal  FLOAT NOT NULL DEFAULT 0.0"\
            ");";
      name = "SpatialSalience";
      grpID = 0;
      break;

    case 2: // scalar capability of each actor
      sql = "create table if not exists SpatialCapability ("  \
            "ScenarioId VARCHAR(32) NOT NULL DEFAULT 'None', "\
            "Turn_t  INTEGER NOT NULL DEFAULT 0, "\
            "Act_i  INTEGER NOT NULL DEFAULT 0, "\
            "Cap  FLOAT NOT NULL DEFAULT 0.0"\
            ");";
      name = "SpatialCapability";
      grpID = 0;
      break;


    case 3: // names of dimensions, so the GUI can use it JAH 20160727
    {
      char *sqlBuff = newChars(500);
      sprintf(sqlBuff, "create table if not exists DimensionDescription ("  \
                       "ScenarioId VARCHAR(32) NOT NULL DEFAULT 'None', "\
                       "Dim_k INTEGER NOT NULL DEFAULT 0, "\
                       "\"Desc\" VARCHAR(%u) NOT NULL DEFAULT 'NoDesc' "\
                       ");", maxDimDescLen);
      sql = std::string(sqlBuff);
      delete sqlBuff;
      sqlBuff = nullptr;

      name = "DimensionDescription";
      grpID = 0;
    }
      break;
    case 4: // affinities of actors, so the GUI can use it
    {
        sql = "create table if not exists Accommodation ("  \
            "ScenarioId VARCHAR(32) NOT NULL DEFAULT 'None', "\
            "Act_i      INTEGER NOT NULL DEFAULT 0, "\
            "Act_j      INTEGER NOT NULL DEFAULT 0, "\
            "Affinity   FLOAT NOT NULL DEFAULT 0.0"\
            ");";
        name = "Accommodation";
        grpID = 0;
        break;
    }
    default:
      throw(KException("SMPModel::createSQL unrecognized table number"));
    }
  }

  //assert(grpID<(Model::NumSQLLogGrps+NumSQLLogGrps));
  if (grpID >= (Model::NumSQLLogGrps + NumSQLLogGrps)) {
    throw KException("SMPModel::createSQL: grpID not valid");
  }
  auto tab = new KTable(n,name,sql,grpID);
  return tab;
}


void SMPModel::sqlTest() {
  QCoreApplication::addLibraryPath("./plugins");
  initDBDriver(QString("smpDB"));

  if (0 == dbDriver.compare("QPSQL")) {
    if (!connectDB()) {
      // connect with the default postgres db (the user should have admin privilege)
      if(!connect(server, port, "postgres", userName, password)) {
        LOG(INFO) << "Please check the login credentials, ip address or port number";
        //assert(false);
        throw KException("SMPModel::sqlTest: Invalid login credentials to connect with database");
      }

      query = QSqlQuery(*qtDB);

      // Check if the database exists
      if (!isDB(databaseName)) {
        // if doesn't exist create one
        if (createDB(databaseName)) {
          // close the connection to the postgres db
          qtDB->close();
          // connect to the newly created database
          if (connectDB()) {
            LOG(INFO) << "Connected to newly created database.";
            query = QSqlQuery(*qtDB);
          }
          else {
            LOG(INFO) << "Not connected to new db";
            LOG(INFO) << qtDB->lastError().text().toStdString();
            //assert(false);
            throw KException("SMPModel::sqlTest: Could not connect with newly created database");
          }
        }
        else {
          //assert(false);
          throw KException("SMPModel::sqlTest: Could not create a new database");
        }
      }
      else {
        LOG(INFO) << "Database " << databaseName.toStdString()
          << " exists but not able to connect to it.";
        //assert(false);
        throw KException("SMPModel::sqlTest: Could not connect with the database");
      }
    }
    else {
      query = QSqlQuery(*qtDB);
    }
  }
  else if (0 == dbDriver.compare("QSQLITE")) {
    qtDB->setDatabaseName(databaseName);
    qtDB->open();
    query = QSqlQuery(*qtDB);
    configSqlite();
  }
  else {
      LOG(INFO) << "Invalid DB driver name";
      //assert(false);
      throw KException("SMPModel::sqlTest: Please specify correct DB driver (QPSQL or QSQLITE)");
  }

  // Create & execute SQL statements
  // JAH 20160728 rewritten to complete the vector of KTables before creating the table
  for (unsigned int i = 0; i < SMPModel::NumTables + Model::NumTables; i++) {
    // get the table and add to the vector
    auto thistable = SMPModel::createSQL(i);
    //assert(nullptr != thistable);
    if (nullptr == thistable) {
      throw KException("SMPModel::sqlTest: Could not create a database table");
    }
    KTables.push_back(thistable);
    // create the table
    execQuery(thistable->tabSQL);
  }

  return;
}

// JAH 20160731 added this function in replacement to the separate
// populate* functions that separately logged information tables
// this calls the kmodel version for Actors and Scenarios and then handles
// salience, capability, and dimensions tables
void SMPModel::LogInfoTables()
{
  // first call the KModel version to do the actors and scenarios tables
  Model::LogInfoTables();

  // now do mine
  // assert tests for all tables here at the start
  //assert(numDim == dimName.size());
  if (numDim != dimName.size()) {
    throw KException("SMPModel::LogInfoTables: dimension count mismatch");
  }

  // for efficiency sake, we'll do all tables in a single transaction
  // form insert commands
  string sqlD = string("INSERT INTO DimensionDescription (ScenarioId,Dim_k,\"Desc\") VALUES ('")
    + scenId + "', :dim_k, :desc)";

  string sqlC = string("INSERT INTO SpatialCapability (ScenarioId, Turn_t, Act_i, Cap) VALUES ('")
    + scenId + "', :turn_t, :act_i, :cap)";

  string sqlS = string("INSERT INTO SpatialSalience (ScenarioId, Turn_t, Act_i, Dim_k,Sal) VALUES ('")
    + scenId + "', :turn_t, :act_i, :dim_k, :sal)";

  string sqlSc = string("UPDATE ScenarioDesc SET VotingRule = :vr, BigRAdjust = :br, "
    "BigRRange = :brr, ThirdPartyCommit = :tpc, InterVecBrgn = :ivb, BargnModel = :bm "
    " WHERE ScenarioId = '")
    + scenId + "'";

  string sqlAcc = string("INSERT INTO Accommodation (ScenarioId, Act_i, Act_j, Affinity) VALUES ('")
    + scenId
    + "', :act_i, :act_j, :affinity)";

  qtDB->transaction();

  // Retrieve accommodation matrix
  auto st = dynamic_cast<SMPState *>(history.back());
  //assert(st != nullptr);
  if (st == nullptr) {
    throw KException("SMPModel::LogInfoTables: st is null pointer");
  }
  auto accM = st->getAccomodate();

  // Accomodation table to record affinities
  query.prepare(QString::fromStdString(sqlAcc));
  //assert((accM.numR() == numAct) && (accM.numC() == numAct));
  if ((accM.numR() != numAct) || (accM.numC() != numAct)) {
    throw KException("SMPModel::LogInfoTables: accM matrix shape is not correct");
  }
  for (unsigned int Act_i = 0; Act_i < numAct; ++Act_i) {
      for (unsigned int Act_j = 0; Act_j < numAct; ++Act_j) {
          //bind the data
          query.bindValue(":act_i", Act_i);
          query.bindValue(":act_j", Act_j);
          query.bindValue(":affinity", accM(Act_i, Act_j));
          // record
          if (!query.exec()) {
            LOG(INFO) << query.lastError().text().toStdString();
            //assert(false);
            throw KException("SMPModel::LogInfoTables: Failed to write Accommodation record");
          }
      }
  }

  // Dimension Description Table
  query.prepare(QString::fromStdString(sqlD));
  for (unsigned int k = 0; k < dimName.size(); k++)
  {
    // bind the data
    query.bindValue(":dim_k", k);
    query.bindValue(":desc", dimName[k].c_str());
    // record
    if (!query.exec()) {
      LOG(INFO) << query.lastError().text().toStdString();
      //assert(false);
      throw KException("SMPModel::LogInfoTables: Failed to write DimensionDescription record");
    }
  }

  // Spatial Capability
  query.prepare(QString::fromStdString(sqlC));
  // for each turn extract the information
  for (unsigned int t = 0; t < history.size(); t++) {
    auto st = history[t];
    // get each actors capability value for each turn
    auto cp = (const SMPState*)history[t];
    auto caps = cp->actrCaps();
    for (unsigned int i = 0; i < numAct; i++) {
      // bind data
      query.bindValue(":turn_t", t);
      query.bindValue(":act_i", i);
      query.bindValue(":cap", caps(0, i));
      // record
      if (!query.exec()) {
        LOG(INFO) << query.lastError().text().toStdString();
        //assert(false);
        throw KException("SMPModel::LogInfoTables: Failed to write SpatialCapability record");
      }
    }
  }

  // Spatial Salience
  query.prepare(QString::fromStdString(sqlS));
  // for each turn extract the information
  for (unsigned int t = 0; t < history.size(); t++) {
    // Get the individual turn
    auto st = history[t];
    // get the SMPState for turn
    auto cp = (const SMPState*)history[t];
    // Extract information for each actor and dimension
    for (unsigned int i = 0; i < numAct; i++) {
      for (unsigned int k = 0; k < numDim; k++) {
        // Populate the actor
        auto ai = ((const SMPActor*)actrs[i]);
        // Get the Salience Value for each actor
        //                double sal = ai->vSal(k, 0);
        //bind the data
        query.bindValue(":turn_t", t);
        query.bindValue(":act_i", i);
        query.bindValue(":dim_k", k);
        query.bindValue(":sal", ai->vSal(k, 0));
        // record
        if (!query.exec()) {
          LOG(INFO) << query.lastError().text().toStdString();
          //assert(false);
          throw KException("SMPModel::LogInfoTables: Failed to write SpatialSalience record");
        }
      }
    }
  }

  query.prepare(QString::fromStdString(sqlSc));
  //ScenarioDesc table
  query.bindValue(":vr", static_cast<int>(vrCltn));
  query.bindValue(":br", static_cast<int>(bigRAdj));
  query.bindValue(":brr", static_cast<int>(bigRRng));
  query.bindValue(":tpc", static_cast<int>(tpCommit));
  query.bindValue(":ivb", static_cast<int>(ivBrgn));
  query.bindValue(":bm", static_cast<int>(brgnMod));

  if (!query.exec()) {
    LOG(INFO) << query.lastError().text().toStdString();
    //assert(false);
    throw KException("SMPModel::LogInfoTables: Failed to write ScenarioDesc record");   
  }

  // finish
  qtDB->commit();

  return;
}


// --------------------------------------------
void SMPState::updateBargnTable(const vector<vector<BargainSMP*>> & brgns,
                                map<unsigned int, KBase::KMatrix>  actorBargains,
                                map<unsigned int, unsigned int>   actorMaxBrgNdx) const {

  string sql = string("UPDATE Bargn SET Init_Prob = :init_prob, Init_Seld = :init_seld, "
    "Recd_Prob = :recd_prob, Recd_Seld = :recd_seld "
    "WHERE ('" + model->getScenarioID() + "' = ScenarioId) "
    "and (:turn_t = Turn_t) and (:bgnId = BargnId) "
    "and (:init_act_i = Init_Act_i) and (:recd_act_j = Recd_Act_j)");

  QSqlQuery query = model->getQuery();

  query.prepare(QString::fromStdString(sql));

  auto updateBargn = [&query, this](int bargnID,
    int initActor, double initProb, int isInitSelected,
    int recvActor, double recvProb, int isRecvSelected) {

    query.bindValue(":init_prob", initProb);

    query.bindValue(":init_seld", isInitSelected);

    // For SQ cases, there would be no receiver
    if (initActor != recvActor) {
      query.bindValue(":recd_prob", recvProb);

      query.bindValue(":recd_seld", isRecvSelected);
    }
    else {
      // Pass NULL values for SQ cases
      query.bindValue(":recd_prob", QVariant::Double);

      query.bindValue(":recd_seld", QVariant::Int);
    }

    query.bindValue(":turn_t", turn);

    query.bindValue(":bgnId", bargnID);

    query.bindValue(":init_act_i", initActor);

    query.bindValue(":recd_act_j", recvActor);

    if (!query.exec()) {
      LOG(INFO) << query.lastError().text().toStdString();
      //assert(false);
      throw KException("SMPState::updateBargnTable: DB query failed");
    }

    return;
  };

  // start for the transaction
  //model->beginDBTransaction();

  // Update the bargain table for the bargain values for init actor and recd actor
  // along with the info whether a bargain got selected or not in the respective actor's queue
  for (unsigned int i = 0; i < brgns.size(); i++) {
    auto ai = ((const SMPActor*)(model->actrs[i]));
    double initProb = -0.1;
    int initSelected = -1;
    auto bargains_i = brgns[i];
    int initBgnNdx = 0;
    auto initActr = -1;
    auto rcvrActr = -1;
    //uint64_t bgID = 0; // tag uninitialized value
    int countDown = 2; // Stop iterating if cases for i:i and i:j processed
    for (auto bg : bargains_i) {
      //assert(nullptr != bg);
      if (nullptr == bg) {
        throw KException("SMPState::updateBargnTable: invalid bargain object");
      }
      if (bg->actInit == bg->actRcvr) { // For SQ case
        initActr = model->actrNdx(bg->actInit);
        rcvrActr = initActr;
        initProb = (actorBargains[initActr])(initBgnNdx, 0);
        initSelected = initBgnNdx == actorMaxBrgNdx[initActr] ? 1 : 0;

        updateBargn(bg->getID(),
          initActr, initProb, initSelected,
          rcvrActr, -1.0, 0);

        if (0 == --countDown) {
          break;
        }
      }
      else {
        if (ai == bg->actInit) { // this bargain is initiated by current actor
          initActr = model->actrNdx(bg->actInit);
          initProb = (actorBargains[initActr])(initBgnNdx, 0);
          initSelected = initBgnNdx == actorMaxBrgNdx[initActr] ? 1 : 0;
          rcvrActr = model->actrNdx(bg->actRcvr);

          // Get the bargains of receiver actor
          auto brgnRcvr = brgns[rcvrActr];
          int rcvrBgNdx = 0;
          double rcvrProb = -1.0;
          int rcvrSelected = -1;
          for (auto bgRcv : brgnRcvr) {
            //assert(nullptr != bgRcv);
            if (nullptr == bgRcv) {
              throw KException("SMPState::updateBargnTable: invalid receiver bargain object");
            }
            if (ai == bgRcv->actInit) {
              rcvrProb = (actorBargains[rcvrActr])(rcvrBgNdx, 0);

              // Check if it is the selected bargain for receiver actor
              rcvrSelected = actorMaxBrgNdx[rcvrActr] == rcvrBgNdx ? 1 : 0;

              --countDown;
              updateBargn(bg->getID(),
                initActr, initProb, initSelected,
                rcvrActr, rcvrProb, rcvrSelected);
              break;
            }
            ++rcvrBgNdx;
          }

          if (0 == countDown) {
            break;
          }
        }
      }

      ++initBgnNdx;
    }
  }

  //model->commitDBTransaction();

  return;
}

void SMPState::recordProbEduChlg() const {
  size_t basePos = -1;
  size_t digCount = 0;
  size_t nextCommaPos = string::npos;

  auto nextActor = [&basePos, &nextCommaPos](string &actors) {
    basePos = nextCommaPos + 1;
    nextCommaPos = actors.find_first_of(",", basePos);
    size_t digCount = nextCommaPos - basePos;
    return actors.substr(basePos, digCount);
  };

  QSqlQuery query = model->getQuery();
  string qsql;
  qsql = string("INSERT INTO TPProbVictLoss "
    "(ScenarioId, Turn_t, Est_h, Init_i, ThrdP_k, Rcvr_j, Prob, Util_V, Util_L) "
    "VALUES ("
    "'") + model->getScenarioID() + "',"
    " :t, :h, :i, :thrdp_k, :j, :prob, :util_v, :util_l )";

  query.prepare(QString::fromStdString(qsql));

  //model->beginDBTransaction();
  for (auto &tpv : tpvData) {
    auto thij = tpv.first;
    auto tpvArray = tpv.second;

    basePos = -1;
    digCount = 0;
    nextCommaPos = string::npos;

    auto t = std::stoi(nextActor(thij));
    auto h = std::stoi(nextActor(thij));
    auto i = std::stoi(nextActor(thij));
    auto j = std::stoi(nextActor(thij));

    query.bindValue(":t", t);
    query.bindValue(":h", h);
    query.bindValue(":i", i);
    query.bindValue(":j", j);

    const unsigned int na = model->numAct;

    for (int tpk = 0; tpk < na; tpk++) {  // third party voter, tpk
      query.bindValue(":thrdp_k", tpk);

      // bind the data
      query.bindValue(":prob", tpvArray(tpk, 0));
      query.bindValue(":util_v", tpvArray(tpk, 1));
      query.bindValue(":util_l", tpvArray(tpk, 2));

      // actually record it
      if (!query.exec()) {
        LOG(INFO) << query.lastError().text().toStdString();
        //assert(false);
        throw KException("SMPState::recordProbEduChlg: DB query failed.");
      }
    }
  }

  qsql = string("INSERT INTO ProbVict "
    "(ScenarioId, Turn_t, Est_h,Init_i,Rcvr_j,Prob) VALUES ('")
    + model->getScenarioID() + "', :t, :h, :i, :j, :phij)";

  query.prepare(QString::fromStdString(qsql));

  for (auto &phijVal : phijData) {
    auto thij = phijVal.first;
    auto phij = phijVal.second;

    basePos = -1;
    digCount = 0;
    nextCommaPos = string::npos;

    auto t = std::stoi(nextActor(thij));
    auto h = std::stoi(nextActor(thij));
    auto i = std::stoi(nextActor(thij));
    auto j = std::stoi(nextActor(thij));

    query.bindValue(":t", t);
    query.bindValue(":h", h);
    query.bindValue(":i", i);
    query.bindValue(":j", j);
    query.bindValue(":phij", phij);

    // actually record it
    if (!query.exec()) {
      LOG(INFO) << query.lastError().text().toStdString();
      //assert(false);
      throw KException("SMPState::recordProbEduChlg: DB query failed.");
    }
  }

  qsql = string("INSERT INTO UtilChlg "
    "(ScenarioId, Turn_t, Est_h,Aff_k,Init_i,Rcvr_j,Util_SQ,Util_Vict,Util_Cntst,Util_Chlg) VALUES ('")
    + model->getScenarioID() + "', :t, :h, :k, :i, :j, :euSQ, :euVict, :euCntst, :euChlg)";

  query.prepare(QString::fromStdString(qsql));

  for (auto &euVal : euData) {
    auto thkij = euVal.first;
    basePos = -1;
    digCount = 0;
    nextCommaPos = string::npos;

    auto t = std::stoi(nextActor(thkij));
    auto h = std::stoi(nextActor(thkij));
    auto k = std::stoi(nextActor(thkij));
    auto i = std::stoi(nextActor(thkij));
    auto j = std::stoi(nextActor(thkij));

    auto eu = euVal.second;
    auto euSQ = eu[0];
    auto euVict = eu[1];
    auto euCntst = eu[2];
    auto euChlg = eu[3];

    query.bindValue(":t", t);
    query.bindValue(":h", h);
    query.bindValue(":k", k);
    query.bindValue(":i", i);
    query.bindValue(":j", j);
    query.bindValue(":euSQ", euSQ);
    query.bindValue(":euVict", euVict);
    query.bindValue(":euCntst", euCntst);
    query.bindValue(":euChlg", euChlg);

    // actually record it
    if (!query.exec()) {
      LOG(INFO) << query.lastError().text().toStdString();
      //assert(false);
      throw KException("SMPState::recordProbEduChlg: DB query failed.");
    }
  }

  //model->commitDBTransaction();
  return;
}

};
// end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
