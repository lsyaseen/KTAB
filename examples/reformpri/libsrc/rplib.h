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
// -------------------------------------------------
// Simple model of negotiating over the order of reform priorities
// -------------------------------------------------
#ifndef REF_PRI_LIB
#define REF_PRI_LIB

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <kutils.h>
#include <kmatrix.h>
#include <prng.h>


#include "kmodel.h"
#include "hcsearch.h"

using namespace std;


namespace RfrmPri {
// namespace to hold everything related to the
// "priority of reforms" CDMP. Note that KBase has no access.

const string appVersion = "0.1";

using std::string;
using std::tuple;
using std::vector;

using KBase::KMatrix;
using KBase::PRNG;

using KBase::Actor;
using KBase::Position;
using KBase::State;
using KBase::Model;
using KBase::VotingRule;
using KBase::ReportingLevel;
using KBase::VUI;

using KBase::MtchPstn;
using KBase::MtchGene;

class RPActor;
class RPState;
class RPModel;

// -------------------------------------------------

KMatrix rescaleRows(const KMatrix& m1, const double vMin, const double vMax);

// -------------------------------------------------
// class declarations

class RPActor : public Actor {
public:
    enum class PropModel {
        ExpUtil, Probability, AgreeUtil
    };

    RPActor(string n, string d, const RPModel* rm);
    ~RPActor();
    double vote(unsigned int p1, unsigned int p2, const State* st) const;
    virtual double vote(const Position * ap1, const Position * ap2) const;
    double posUtil(const Position * ap1) const;

    static MtchPstn* rPos(unsigned int numI, unsigned int numA, PRNG * rng); // make a random position
    static RPActor* rAct(unsigned int numI, double minCap,
                         double maxCap, PRNG* rng, unsigned int i); // make a random actor

    void randomize(PRNG* rng, double minCap, double maxCap, unsigned int id, unsigned int numI); // randomly alter this actor

    tuple<double, MtchPstn> maxProbEUPstn(PropModel pm, const RPState * rpst) const;

    unsigned int idNum = 0;

    VotingRule vr = VotingRule::Proportional;
    PropModel pMod = PropModel::ExpUtil;

    // scalar capacity, positive
    double sCap = 0;

    // Similar to the LeonActor, this is a listing of how
    // much this actor values each reform item.
    // The values are all non-negative
    vector<double> riVals {};

    const RPModel *rpMod = nullptr;
    // these particular actors need model-parameters to compute utility.

protected:
private:
};


class RPModel : public Model {
public:
    explicit RPModel(PRNG* rng, string d="");
    virtual ~RPModel();

    static RPModel* randomMS(unsigned int numA, unsigned int numI,
                             VotingRule vr, RPActor::PropModel pMod, PRNG * rng);

    double utilActorPos(unsigned int ai, const VUI &pstn) const;

    unsigned int govBudget = 0;
    KMatrix  govCost = KMatrix();
    double pDecline = 0.850;
    vector<double> prob = {};
    double obFactor = 0.1;
    unsigned int numItm = 0; // number of reform items
    vector<string> rpNames = {};

    unsigned int numCat = 0;  // happens to equal numItm, in this demo, at the categories are 1, 2, ... numItm.

    void initScen(unsigned int ns);
    void readXML(string fileName);
    void showHist() const;

    static bool equivStates(const RPState * rs1, const RPState * rs2);

protected:
    void initScen0(); // random
    void initScen1(); // fixed, but dummy data
    void initScen2Avrg(unsigned int ns); // unfinished
    void initScen3Top4(unsigned int ns); // unfinished
    void configScen(unsigned int numA, const double aCap[], const KMatrix & utils);
    
private:
};


class RPState : public State {
public:
    explicit RPState(Model* mod);
    ~RPState();
    //KMatrix actrCaps() const;

    // use the parameters of your state to compute the relative probability of
    // each actor's position. persp = -1 means use everyone's separate perspectives
    //(i.e. get actual probabilities, not one actor's beliefs)
    tuple <KMatrix, VUI> pDist(int persp) const;
    RPState * stepSUSN();
    RPState * stepBCN();

    void show() const;

protected:
    virtual void setAllAUtil(ReportingLevel rl);
    void setOneAUtil(unsigned int perspH, ReportingLevel rl);
    
    RPState * doSUSN(ReportingLevel rl) const;
    RPState * doBCN(ReportingLevel rl) const;
    // bool stableRPState(unsigned int iter, const State* s);
    const RPModel * rpMod = nullptr; // saves a lot of type-casting later

    virtual bool equivNdx(unsigned int i, unsigned int j) const;

private:
};



} // end of namespace


#endif
// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
