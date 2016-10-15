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


#include "demoMatrixEMod.h"


namespace eModKEM {
using std::cout;
using std::endl;
using std::flush;
using std::string;
using std::vector;

using KBase::KMatrix;
using KBase::Model;
using KBase::ReportingLevel;
using KBase::VotingRule;
using KBase::EModel;
using KBase::EState;
using KBase::EActor;
using KBase::EPosition;


const auto weightMat = KMatrix::arrayInit(weightArray, 1, numActKEM);
const auto utilMat = KMatrix::arrayInit(utilArray, numActKEM, numPolKEM);


// --------------------------------------------
PMatrixModel::PMatrixModel(string d, uint64_t s, vector<bool> vb) : EModel< unsigned int >(d,s,vb) {
    // nothing yet
}


PMatrixModel::~PMatrixModel() {
    // nothing yet
}


// check the matrix and set as utilities,
// if valid for [numAct,numOpt] utilities in [0,1] range.
void PMatrixModel::setPMatrix(const KMatrix pm0) {
    const unsigned int nr = pm0.numR();
    const unsigned int nc = pm0.numC();
    if (0 < numAct) {
        assert (nr == numAct);
    }
    else {
        numAct = nr;
    }

    if (0 < numOptions()) {
        assert (nc == numOptions());
    }
    else {
        theta.resize(nc); // was size zero
        for (unsigned int i=0; i<nc; i++) {
            theta[i] = i;
        }
    }

    assert (minNumActor <= numAct);
    assert (numAct <= maxNumActor);

    assert (minNumOptions <= numOptions());

    for (auto u : pm0) {
        assert (0.0 <= u);
        assert (u <= 1.0);
    }

    // if all OK, set it
    polUtilMat = pm0;

    return;
}


// check the row-matrix and set as weight,
// if valid for non-negative [numAct,1] weights
void PMatrixModel::setWeights(const KMatrix w0) {
    const unsigned int nr = w0.numR();
    const unsigned int nc = w0.numC();

    assert (1 == nr);

    if (0 < numAct) {
        assert (nc == numAct);
    }
    else {
        numAct = nc;
    }

    for (auto w : w0) {
        assert (0.0 <= w);
    }
    assert (minNumActor <= numAct);
    assert (numAct <= maxNumActor);

    // if it is OK, set it
    wghtVect = w0;
    return;
}


void PMatrixModel::setActors(vector<string> names, vector<string> descriptions) {
    const unsigned int na = numAct;
    numAct = 0;

    assert (0 < na);
    assert (na == names.size());
    assert (na == descriptions.size());
    assert (na == wghtVect.numC());
    assert (na == polUtilMat.numR());

    for (unsigned int i=0; i<na; i++) {
        auto ai = new EActor<unsigned int>(this, names[i], descriptions[i]);
        ai->vr = VotingRule::Proportional;
        ai->sCap = wghtVect(0,i);
        addActor(ai);
    }
    assert (na == numAct);
    return;
}


// --------------------------------------------
PMatrixPos::PMatrixPos(PMatrixModel* pm, int n) : EPosition< unsigned int >(pm,n) {
    // nothing else, yet
}

PMatrixPos::~PMatrixPos() {
    // nothing else, yet
}


// --------------------------------------------
PMatrixState::PMatrixState(PMatrixModel* pm) : EState<unsigned int>(pm) {
    // nothing else, yet
}


PMatrixState::~PMatrixState()  {
    // nothing else, yet
}

EState<unsigned int>* PMatrixState::makeNewEState() const  {
    PMatrixModel* pMod = (PMatrixModel*) model;
    PMatrixState* s2 = new PMatrixState(pMod);
    return s2;
}


vector<double> PMatrixState::actorUtilVectFn( int h, int tj) const {
    // no difference in perspective for this demo
    const unsigned int na = eMod->numAct;
    const auto pMod = (const PMatrixModel*) eMod;
    const auto pMat = pMod->getPolUtilMat();
    assert (na == pMat.numR());
    assert (0 <= tj);
    assert (tj < pMat.numC());
    vector<double> rslt = {};
    rslt.resize(na);
    for (unsigned int i=0; i<na; i++) {
        rslt[i] = pMat(i, tj);
    }
    return rslt;
}

// build the square matrix of position utilities.
// Row is actor i, Column is position of actor j,
// U(i,j) is utility to actor i of position of actor j.
// If there are duplicate positions, there will be duplicate columns.
void PMatrixState::setAllAUtil(ReportingLevel) {
    const unsigned int na = eMod->numAct;
    assert (Model::minNumActor <= na);
    assert (na <= Model::maxNumActor);
    aUtil = {};
    aUtil.resize(na);
    auto uMat = KMatrix(na,na); // they will all be the same in this demo
    for (unsigned int j=0; j<na; j++) {
        unsigned int nj = posNdx(j);
        auto utilJ = actorUtilVectFn(-1, nj); // all have objective perspective
        for (unsigned int i=0; i<na; i++) {
            uMat(i,j) = utilJ[i];
        }
    }
    for (unsigned int i=0; i<na; i++) {
        aUtil[i] = uMat;
    }
    return;
}


// --------------------------------------------

void demoEKem(uint64_t s) {
    using KBase::lCorr;
    using KBase::mean;

    printf("Using PRNG seed: %020lu \n", s);
    auto rng = new KBase::PRNG(s);

    cout << endl;
    printf("Creating EModel<string> objects ... \n");

    string nChar = "EModel-KEM-Policy";
    auto ekm = new EModel<string>(nChar, s);
    cout << "Populating " << nChar << endl;

    // with such a small set of options, the enumerator
    // function just returns a fixed vector of names.
    ekm->enumOptions = []() {
        return pNamesKEM;
    };
    ekm->setOptions();
    cout << "Now have " << ekm->numOptions() << " enumerated options" << endl;

    const unsigned int maxIter = 50;
    ekm->stop = [maxIter](unsigned int iter, const KBase::State * s) {
        return (iter > maxIter);
    };

    // these just to force instantiation during compilation.
    // It is NOT yet ready to run.
    //auto eks = new EState<string>(ekm);
    //eks->step = [eks] { return eks->stepSUSN(); };
    //ekm->addState(eks);

    auto ekp = new EPosition<string>(ekm, 0); // needs ::print
    auto eka = new EActor<string>(ekm, "Bob", "The second cryptographer");

    cout << "Length of state history: " << ekm->history.size() << endl << flush;

    //delete eks;
    //eks = nullptr;
    delete ekp;
    ekp = nullptr;
    delete eka;
    eka = nullptr;

    // the following gets SIGSEGV
    //delete ekm;
    //ekm = nullptr;


    cout << endl << "====================================="<<endl;
    printf("Creating EKEModel objects ... \n");

    auto eKEM = new PMatrixModel("EModel-Matrix-KEM", s);

    eKEM->pcem = KBase::PCEModel::MarkovIPCM;

    auto wMat = KMatrix::uniform(rng, 1, utilMat.numR(), 1.0, 10.0);
    wMat = KMatrix::map(
        [](double x, unsigned int, unsigned int) {return x*x;}, 
        wMat);
    auto uMat = KMatrix::uniform(rng, utilMat.numR(), utilMat.numC(), 0.0, 1.0);
    uMat = KBase::rescaleRows(uMat, 0.0, 1.0);
    
    cout << "Actor weight vector: "<<endl;
    wMat.mPrintf("%6.2f ");
    cout << endl;
    
    cout << "Utility(actor, option) matrix:"<<endl;
    uMat.mPrintf("%5.3f ");
    cout << endl;

    eKEM->setWeights(wMat);
    eKEM->setPMatrix(uMat);
    eKEM->setActors(aNamesKEM, aDescKEM);

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


    auto lij = [p2] (unsigned int i, unsigned int j) {
        return log(p2(i,j));
    };
    auto logP = KMatrix::map(lij, p2.numR(), p2.numC());
    printf("af-corr(logp2,zeta): %.3f \n", aCorr(logP, zeta));

    for (unsigned int i=0; i<nOpt; i++) {
        printf("%2i  %6.4f  %+8.3f  %5.1f  \n", i, p2(0,i), logP(0,i), zeta(0,i));
    }

    auto es1 = new PMatrixState(eKEM);
    for (unsigned int i=0; i<eKEM->numAct; i++) {
        unsigned int ki = rng->uniform() % nOpt;
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

    delete eKEM;
    eKEM = nullptr;

    delete rng;
    rng = nullptr;

    return;
}

} // end of namespace eModKEM


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
