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


#include "vimcp.h"
#include "demoMatrixEMod.h"
#include "demoPMatData.h"
#include "minicsv.h"


namespace eModKEM {
using std::cout;
using std::endl;
using std::flush;
using std::string;
using std::vector;

using KBase::KMatrix;
using KBase::Model;
using KBase::KException;
using KBase::ReportingLevel;
using KBase::VotingRule;
using KBase::EModel;
using KBase::EState;
using KBase::EActor;
using KBase::EPosition;


FittingParameters pccCSV(const string fs) {


    csv::ifstream inStream(fs.c_str());
    inStream.set_delimiter(',', "$$");
    inStream.enable_trim_quote_on_str(true, '\"');

    assert(inStream.is_open());
    string dummy = "";
    unsigned int numAct = 0;
    unsigned int numScen = 0;
    unsigned int numCase = 0;


    inStream.read_line();
    inStream >> dummy >> numAct;
    assert(KBase::Model::minNumActor <= numAct);

    inStream.read_line();
    inStream >> dummy >> numScen;
    assert(1 < numScen);

    inStream.read_line();
    inStream >> dummy >> numCase;
    assert(1 <= numCase);

    printf("Actors %u, Scenarios %u, Cases %u \n", numAct, numScen, numCase);

    // skip headers
    inStream.read_line();

    vector<string> aNames = {};
    auto caseWeights = KMatrix(numCase, numAct, 1.0);
    vector<bool> maxVect = {};
    auto outcomes = KMatrix(numAct, numScen);
    auto probWeight = KMatrix(numScen, numCase, 1.0);

    for (unsigned int i = 0; i < numAct; i++) {
        inStream.read_line();
        string ani = "";
        inStream >> ani;
        aNames.push_back(ani);
        for (unsigned int j = 0; j < numCase; j++) {
            double cw = 0.0;
            inStream >> cw;
            assert(0.0 <= cw);
            assert(cw <= 100.0);
            caseWeights(j, i) = cw / 100.0;
        } // loop over j, cases
        inStream >> dummy;
        if ("up" == dummy) {
            maxVect.push_back(true);
        }
        else if ("down" == dummy) {
            maxVect.push_back(false);
        }
        else {
            throw (KException("Unrecognized group-optimization direction"));
        }

        for (unsigned int j = 0; j < numScen; j++) {
            double sv = 0.0;
            inStream >> sv;
            outcomes(i, j) = sv;
        } // loop over j, scenarios
    } // loop over i, actors

    cout << "Actor names (min/max)" << endl;
    for (unsigned int i = 0; i < numAct; i++) {
        cout << aNames[i] << "   " << (maxVect[i] ? "max" : "min") << endl;
    }
    cout << endl << flush;

    cout << "Case Weights:" << endl;
    caseWeights.mPrintf(" %5.2f ");
    cout << endl;

    cout << "Outcomes:" << endl;
    outcomes.mPrintf(" %+.4e  ");
    cout << endl << flush;


    vector<double> threshVal = {};
    vector<bool> overThresh = {};
    for (unsigned int j = 0; j < numCase; j++) {
        inStream.read_line();
        double tv = 0.0;
        string dir;
        inStream >> dummy; // skip "prob-n"
        inStream >> tv; // read a threshold value
        assert(0.0 <= tv);
        assert(tv <= 1.0);
        threshVal.push_back(tv);

        inStream >> dir; // read dir
        if ("higher" == dir) {
            overThresh.push_back(true);
        }
        else if ("lower" == dir) {
            overThresh.push_back(false);
        }
        else {
            throw (KException("Unrecognized threshold direction"));
        }

        for (unsigned int k = 0; k < numCase - 1; k++) {
            inStream >> dummy; // skip blanks, if any
        }

        for (unsigned int k = 0; k < numScen; k++) {
            double pw = 0.0;
            inStream >> pw;
            assert(0.0 <= pw);
            assert(pw <= 100.0);
            probWeight(k, j) = pw / 100.0;
        }
    } // loop over j, cases

    cout << "Prob threshholds" << endl;
    for (unsigned int k = 0; k < numCase; k++) {
        if (overThresh[k]) {
            printf("Over  %.3f \n", threshVal[k]);
        }
        else {
            printf("Under %.3f \n", threshVal[k]);
        }
    }

    cout << "ProbWeights:" << endl;
    probWeight.mPrintf(" %5.3f ");
    cout << endl << flush;

    auto rslt = FittingParameters (aNames, maxVect,
                                   outcomes, caseWeights,
                                   probWeight, threshVal, overThresh);

    return rslt;
}


void runEKEM(uint64_t s, bool cpP, const KMatrix& wMat, const KMatrix& uMat, const vector<string> & aNames) {
    assert (0 != s);
    cout << endl << "====================================="<<endl;
    printf("Creating EKEModel objects ... \n");

    auto eKEM = new PMatrixModel("EModel-Matrix-KEM", s);

    eKEM->pcem = KBase::PCEModel::MarkovIPCM;

    cout << "Actor weight vector: "<<endl;
    wMat.mPrintf("%6.2f  ");
    cout << endl;

    cout << "Utility(actor, option) matrix:"<<endl;
    uMat.mPrintf("%5.3f  ");
    cout << endl;

    eKEM->setWeights(wMat);
    eKEM->setPMatrix(uMat);
    eKEM->setActors(aNames, aNames);

    const unsigned int maxIter = 1000;

    eKEM->stop = [maxIter, eKEM](unsigned int iter, const KBase::State * s) {
        bool doneP = iter > maxIter;
        if (doneP) {
            printf("Max iteration limit of %u exceeded \n", maxIter);
        }
        auto s2 = ((const PMatrixState *)(eKEM->history[iter]));
        for (unsigned int i = 0; i < iter; i++) {
            auto s1 = ((const PMatrixState *)(eKEM->history[i]));
            if (eKEM->equivStates(s1, s2)) {
                doneP = true;
                printf("State number %u matched state number %u \n", iter, i);
            }
        }
        return doneP;
    };


    const unsigned int nOpt = eKEM->numOptions();
    printf("Number of options %i \n", nOpt);
    printf("Number of actors %i \n", eKEM->numAct);


    const auto probTheta = Model::scalarPCE(eKEM->numAct, nOpt ,
                                            wMat, uMat,
                                            VotingRule::Proportional,
                                            eKEM->vpm, eKEM->pcem,
                                            ReportingLevel::Silent);

    const auto p2 = trans(probTheta);
    cout << "PCE over entire option-space:"<<endl;
    p2.mPrintf(" %5.3f ");

    auto zeta = wMat * uMat;
    cout << "Zeta over entire option-space:"<<endl;
    zeta.mPrintf(" %5.1f ");

    auto aCorr = [] (const KMatrix & x, const KMatrix &y) {
        return lCorr(x-mean(x), y-mean(y));
    };

    printf("af-corr(p2,zeta): %.3f \n",  aCorr(p2, zeta));

    auto logP = KMatrix::map([](double x) {
        return log(x);
    }, p2);
    printf("af-corr(logp2,zeta): %.3f \n", aCorr(logP, zeta));

    for (unsigned int i=0; i<nOpt; i++) {
        printf("%2i  %6.4f  %+8.3f  %5.1f  \n", i, p2(0,i), logP(0,i), zeta(0,i));
    }

    double maxZ = -1.0;
    unsigned int ndxMaxZ = 0;
    for (unsigned int i=0; i<nOpt; i++) {
        if (zeta(0,i) > maxZ) {
            maxZ = zeta(0,i);
            ndxMaxZ = i;
        }
    }
    cout << "Central position is number "<<ndxMaxZ <<endl;


    auto es1 = new PMatrixState(eKEM);

    if (cpP) {
        cout << "Assigning actors to the central position"<<endl;
    }
    else {
        cout << "Assigning actors to their self-interested initial positions"<<endl;
    }
    for (unsigned int i=0; i<eKEM->numAct; i++) {
        double maxU = -1.0;
        unsigned int bestJ = 0;
        for (unsigned int j=0; j<nOpt; j++) {
            double uij = uMat(i,j);
            if (uij > maxU) {
                maxU = uij;
                bestJ = j;
            }
        }
        unsigned int ki = cpP ? ndxMaxZ : bestJ;
        auto pi = new PMatrixPos(eKEM, ki);
        es1->addPstn(pi);
    }

    es1->setUENdx();
    eKEM->addState(es1);

    cout << "--------------"<<endl;
    cout << "First state:"<<endl;
    es1->show();

    // see if the templates can be instantiated ...
    es1->step = [es1] { return es1->stepSUSN(); };

    eKEM->run();

    const unsigned int histLen = eKEM->history.size();
    PMatrixState* esA = (PMatrixState*) (eKEM->history[histLen-1]);
    printf("Last State %i \n", histLen-1);
    esA->show();
    return;
}

} // end of namespace eModKEM


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
