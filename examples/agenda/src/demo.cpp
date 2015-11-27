// ------------------------------------------
// Copyright KAPSARC. Open Source MIT License
// ------------------------------------------

#include <assert.h>
#include <algorithm>

#include "kmodel.h"
#include "agenda.h"
#include "demo.h"
using std::cout;
using std::endl;
using std::flush;
using std::vector;
using KBase::KMatrix;
using KBase::PRNG;

namespace AgendaControl {

void setupScenario0(unsigned int &numActor, unsigned int &numItems,
                    KMatrix &vals, KMatrix &caps, PRNG* rng) {

    numActor = 15;
    numItems = 7;
    vals = KMatrix::uniform(rng, numActor, numItems, 0.01, 0.99);
    caps = KMatrix::uniform(rng, numActor, 1, 10.0, 99.99);
    return;
}



void setupScenario1(unsigned int &numActor, unsigned int &numItems,
                    KMatrix &vals, KMatrix &caps) {

    numActor = 22;
    numItems = 5;

    KMatrix plpM = KMatrix::arrayInit(plpA, numActor, numItems);
    KMatrix plwM = KMatrix::arrayInit(plwA, numActor, 1);

    vals = KMatrix(numActor, numItems);
    for (unsigned int i=0; i<numActor; i++) {
        double rowMax = 0.0;
        double rowMin = 10.0;
        for (unsigned int j=0; j<numItems; j++) {
            double vij = plpM(i,j);
            if (vij > rowMax) {
                rowMax = vij;
            }
            if (vij < rowMin) {
                rowMin = vij;
            }
        }
        for (unsigned int j=0; j<numItems; j++) {
            double vij = plpM(i,j);
            vals(i,j) = (vij - rowMin)/ (rowMax - rowMin);
        }
    }
    caps = plwM;
    return;
}

void bestAgendaChair(vector<Agenda*> ars, const KMatrix& vals, const KMatrix& caps, VotingRule vr) {
    unsigned int bestK = 0;
    double bestV = -1.0;
    unsigned int numAgenda = ars.size();
    for (unsigned int ai = 0; ai < numAgenda; ai++) {
        auto ar = ars[ai];
        ars.push_back(ar);
        double v0 = ar->eval(vals, caps, vr, 0); //
        assert (0.0 <= v0);
        assert (v0 <= 1.0);

        if (bestV < v0) {
            bestV = v0;
            bestK = ai;
        }
        //  printf("%4u ", ai); cout << *ar << "  "; printf("%.4f \n", v0);
    }
    printf("Best option for agenda-setting actor 0 is %4u with value %6.4f  is  ", bestK, bestV);
    cout << *(ars[bestK]) << endl << flush;
    ars[bestK]->showProbs(1.0);
    return;
}

void demoCounting() {
    cout << endl << flush;
    unsigned int n=9;
    unsigned int m=4;
    auto cat = AgendaControl::chooseSet(n,m);
    for (auto lst : cat) {
        for (auto i : lst) {
            // printf("%i ", i);
        }
        //cout << endl << flush;
    }
    printf("Found |chooseSet(%u, %u)| = %llu \n", n, m, cat.size());
    assert (cat.size() == AgendaControl::numSets(n,m));

    cout << endl;
    cout << "For 4 items, the agenda [[10:20]:[30:40]] is repeated as [[30:40]:[10:20]]"<<endl;
    cout << endl;
    
    unsigned int numI = 4;
    vector<unsigned int> testI = {};
    for (unsigned int i=0; i<numI; i++) {
        testI.push_back(10*(1+i));
    }


    for (unsigned int i=1; i<=numI+2; i++) {
        auto n = AgendaControl::numAgenda(i);
        printf("%i -> %llu \n", i, n);
    }
    cout << endl << flush;

    printf("Using %i items: ", numI);
    for (unsigned int i : testI) {
        printf("%i ", i);
    }
    cout << endl << flush;

    auto testA = AgendaControl::agendaSet(testI);
    printf("For %i items, found %i agendas \n", numI, testA.size());
    for (auto a : testA) {
        cout << *a << endl;
    }
    cout << endl << flush;

    assert (testA.size() == AgendaControl::numAgenda(numI));

    return;
}

}; // end of namespace

int main(int ac, char **av) {
    using std::cout;
    using std::endl;
    using std::flush;
    using std::function;
    using KBase::VotingRule;
    using AgendaControl::Agenda;
    using AgendaControl::Choice;
    using AgendaControl::Terminal;

    auto sTime = KBase::displayProgramStart();
    uint64_t dSeed = 0xD67CC16FE69C185C; // arbitrary
    uint64_t seed = dSeed;
    bool enumP = false;
    bool run = true;

    auto showHelp = [dSeed]() {
        printf("\n");
        printf("Usage: specify one or more of these options\n");
        printf("--help            print this message\n");
	printf("--enum            enumerate agendas\n");
        printf("--seed <n>        set a 64bit seed\n");
        printf("                  0 means truly random\n");
        printf("                  default: %020llu \n", dSeed);
    };

    // tmp args
    // seed = 0;


    if (ac > 1) {
        for (int i = 1; i < ac; i++) {
            if (strcmp(av[i], "--seed") == 0) {
                i++;
                seed = std::stoull(av[i]);
            }
            else if (strcmp(av[i], "--enum") == 0) {
               enumP = true;
            }
            else if (strcmp(av[i], "--help") == 0) {
                run = false;
            }
            else {
                run = false;
                printf("Unrecognized argument %s\n", av[i]);
            }
        }
    }

    if (!run) {
        showHelp();
        return 0;
    }

    PRNG * rng = new PRNG();
    seed = rng->setSeed(seed); // 0 == get a random number
    printf("Using PRNG seed:  %020llu \n", seed);
    printf("Same seed in hex:   0x%016llX \n", seed);
    
    if (enumP) {
      AgendaControl::demoCounting();
    }
    
    
    const unsigned int numAgenda = 8000;
    // with 100K random agendas and 7 items, I bayesian-ly estimated
    // that the probability the optimum appears within x trials is
    // P[X<x] = 1-exp(-x/A), where A=1681.75
    // So we need 7745 to get a 99% of seeing the true optimum.
    unsigned int numActor = 0;
    unsigned int numItems = 0;
    auto vals = KMatrix();
    auto caps = KMatrix();

    if (false) {
        AgendaControl::setupScenario0(numActor, numItems, vals, caps, rng);
    }
    else {
        AgendaControl::setupScenario1(numActor, numItems, vals, caps);
    }



    // find what's best for agenda-setting actor 0
    caps(0, 0) = caps(0, 0) / 25.0; // agenda-setter has little voting power

    cout << "Value matrix" << endl;
    vals.mPrintf(" %5.3f ");
    cout << endl << flush;


    cout << "Capability matrix" << endl;
    caps.mPrintf(" %5.2f ");
    cout << endl << flush;

    auto threeVotes = [vals, caps] ( vector<Agenda*> ars) {

        cout << endl << flush << "Binary"<<endl;
        for (auto ar : ars) {
            ar->clearProbs();
        }
        AgendaControl::bestAgendaChair(ars, vals, caps, VotingRule::Binary);

        cout << endl << flush << "Proportional"<<endl;
        for (auto ar : ars) {
            ar->clearProbs();
        }
        AgendaControl::bestAgendaChair(ars, vals, caps, VotingRule::Proportional);

        cout << endl << flush << "Cubic"<<endl;
        for (auto ar : ars) {
            ar->clearProbs();
        }
        AgendaControl::bestAgendaChair(ars, vals, caps, VotingRule::Cubic);
        cout << endl << flush;

        for (auto ar : ars) {
            ar->clearProbs();
            delete ar;
        }

        return;
    };


    auto ars = vector<Agenda*>();

    cout << "Making "<<numAgenda<<" random agendas (FreePR) over " << numItems << " items ... ";
    for (unsigned int ai = 0; ai < numAgenda; ai++) {
        auto ar = Agenda::makeRandom(numItems, Agenda::PartitionRule::FreePR, rng);
        ars.push_back(ar);
    }
    cout << "done"<< endl;
    threeVotes(ars);

    ars = vector<Agenda*>();

    cout << "Making "<<numAgenda<<" random agendas (FullBalancedPR) over " << numItems << " items ... ";
    for (unsigned int ai = 0; ai < numAgenda; ai++) {
        auto ar = Agenda::makeRandom(numItems, Agenda::PartitionRule::FullBalancedPR, rng);
        ars.push_back(ar);
    }
    cout << "done"<< endl;
    threeVotes(ars);

    delete rng;
    KBase::displayProgramEnd(sTime);
    return 0;
}
// ------------------------------------------
// Copyright KAPSARC. Open Source MIT License
// ------------------------------------------
