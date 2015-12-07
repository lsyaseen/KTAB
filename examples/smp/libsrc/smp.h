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

#ifndef SMP_LIB_H
#define SMP_LIB_H

#include <iostream>
#include <string>

#include "csv_parser.hpp"
#include "sqlite3.h"
#include "kutils.h"
#include "prng.h"
#include "kmatrix.h"
#include "gaopt.h"
#include "kmodel.h"

namespace SMPLib {
  // namespace to which KBase has no access
  using std::function;
  using std::shared_ptr;
  using std::string;
  using std::tuple;
  using std::vector;
  using KBase::newChars;
  using KBase::KMatrix;
  using KBase::PRNG;
  using KBase::Actor;
  using KBase::Position;
  using KBase::State;
  using KBase::Model;
  using KBase::VotingRule;
  using KBase::ReportingLevel;
  using KBase::VctrPstn;

  class SMPActor;
  class SMPState;
  class SMPModel;

  const string appVersion = "0.1";

  // -------------------------------------------------
  // Plain-Old-Data
  struct BargainSMP {
    BargainSMP(const SMPActor* ai, const SMPActor* ar, const VctrPstn & pi, const VctrPstn & pr);
    ~BargainSMP();


    const SMPActor* actInit = nullptr;
    const SMPActor* actRcvr = nullptr;
    VctrPstn posInit = VctrPstn();
    VctrPstn posRcvr = VctrPstn();
  };

  // -------------------------------------------------
  // Trivial, SMP-like actor with fixed attributes
  // the old smp.cpp file, SpatialState::developTwoPosBargain, for a discussion of
  // the interpolative bargaining rule used there. Note that the demoutil
  // results indicate that weighting by (P^2 * S^2) better approximates the NBS
  // does weighting by (P * S)
  class SMPActor : public Actor {

  public:

    enum class InterVecBrgn {
      S1P1, S2P2, S2PMax
        };
    // See the documentation in kutils/doc: eS2P2, dS2P2, and eS1P1 were the best, in that order.
    // Both estimators involving PMax were the least accurate.

    SMPActor(string n, string d);
    ~SMPActor();

    // interfaces to be provided
    double vote(unsigned int p1, unsigned int p2, const State* st) const;
    virtual double vote(const Position * ap1, const Position * ap2, const SMPState* as1) const;
    double posUtil(const Position * ap1, const SMPState* as1) const;


    void randomize(PRNG* rng, unsigned int numD);

    // These actors have a vector position in [0,1]^m
    // They have differing saliences for different dimensions,
    // which are used to determine weighted Euclidean
    // distance from their current ideal position
    // (which is their current position in the State).
    // The distance is curved with risk attitude to get utility.
    // Their vote is determined by applying their scalar capacity
    // to differences in utility
    double sCap = 0;
    //double bigR;
    KMatrix vSal = KMatrix();
    VotingRule vr = VotingRule::Proportional; // reasonable default

    // the attributes used in this method are not generally part of
    // other actors, and not all positions can be represented as a list of doubles.
    static BargainSMP* interpolateBrgn(const SMPActor* ai, const SMPActor* aj,
                                       const VctrPstn* posI, const VctrPstn * posJ,
                                       double prbI, double prbJ, InterVecBrgn ivb);


  protected:
    static void interpBrgnSnPm(unsigned int n, unsigned int m,
                               double tik, double sik, double prbI,
                               double tjk, double sjk, double prbJ,
                               double & bik, double & bjk);
    static void interpBrgnS2PMax(double tik, double sik, double prbI,
                                 double tjk, double sjk, double prbJ,
                                 double & bik, double & bjk);


  };

  class SMPState : public State {

  public:
    enum class BigRRange {
      Min, Mid, Max
        };
    enum class BigRAdjust {
      NoRA, HalfRA, FullRA
        };
    explicit SMPState(Model * m);
    virtual ~SMPState();

    virtual void setDiff();

    // this sets the values in the AUtil matrices
    virtual void setAUtil(ReportingLevel rl);

    // returns h's estimate of i's risk attitude, using the risk-adjustment-rule
    double estNRA(unsigned int h, unsigned int i, SMPState::BigRAdjust ra) const;

    // returns row-vector of actor's capabilities
    KMatrix actrCaps() const;

    KMatrix nra = KMatrix();
    SMPState* stepBCN();

    double  posProb(unsigned int i, vector<unsigned int> unq, const KMatrix & pdt) const;

    // The key steps of BCN are to identify a target (and perhaps other target-relevant info)
    // and then to develop a Bargain (possibly nullptr if no bargain is mutually preferable
    // to conflict)
    // you have to provide λ-fn for both of the following
    function<shared_ptr<void>(const Actor* ai, const State* s)> bestTarget = nullptr;
    function<shared_ptr<void>(const Actor* aInit, const Actor* aRcvr, shared_ptr<void> btData, const State* s)> bargain = nullptr;

    virtual void addPstn(Position* p);

    // use the parameters of your state to compute the relative probability of each actor's position
    virtual tuple< KMatrix, vector<unsigned int>> pDist(int persp) const;
    void showBargains(const vector < vector < BargainSMP* > > & brgns) const;


  protected:
    KMatrix diff = KMatrix();
    static KMatrix bigRfromProb(const KMatrix & p, BigRRange rr);


    SMPState* doBCN() const;
    virtual bool equivNdx(unsigned int i, unsigned int j) const;

    // returns estimated probability k wins (given likely coaltiions), and expected value of that challenge
    tuple<double, double> probEduChlg(unsigned int h, unsigned int k, unsigned int i, unsigned int j) const;

    // return best j, p[i>j], edu[i->j]
    tuple<int, double, double> bestChallenge(unsigned int i) const;

  };

  class SMPModel : public Model {
  public:
    explicit SMPModel(PRNG * rng, string desc="");
    virtual ~SMPModel();

    static double bsUtil(double d, double R);
    static double bvDiff(const KMatrix & d, const  KMatrix & s);
    static double bvUtil(const KMatrix & d, const  KMatrix & s, double R);

    static SMPModel * readCSV(string fName, PRNG * rng);

    static  SMPModel * initModel(vector<string> aName, vector<string> aDesc, vector<string> dName,
                                 KMatrix cap, KMatrix pos, KMatrix sal, PRNG * rng);

    // print history of each actor in CSV (might want to generalize to arbitrary VctrPstn)
    void showVPHistory(bool sqlP) const;

    // number of spatial dimensions in this SMP
    unsigned int numDim = 0;
    vector<string> dimName = {};
    double posTol = 5E-3; // on a scale of 0 to 100, this is a difference of just 0.5

    static double stateDist(const SMPState* s1 , const SMPState* s2 );

    // this does not set AUtil, just output it to SQLite
    //virtual void sqlAUtil(unsigned int t);

  protected:
    //sqlite3 *smpDB = nullptr; // keep this protected, to ease multi-threading
    //string scenName = "Scen";
  protected:
    static string createSMPTableSQL(unsigned int tn);
    // note that the function to write to table #k must be kept
    // synchronized with the result of createTableSQL(k) !

    void sqlTest();


    // note that the function to write to table #k must be kept
    // synchronized with the result of createTableSQL(k) !
    // string createTableSQL(unsigned int tn);

    void addDim(string dn);

  private:
  };



};// end of namespace

// --------------------------------------------
#endif
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
