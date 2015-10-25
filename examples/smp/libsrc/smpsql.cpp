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




}; // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
