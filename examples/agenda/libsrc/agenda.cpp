// ------------------------------------------
// Copyright KAPSARC. Open Source MIT License
// ------------------------------------------
#include "agenda.h"


using std:: cout;
using std:: endl;
using std::flush;

namespace AgendaControl {

Agenda* Agenda::makeRandom(unsigned int n, PartitionRule pr, PRNG* rng) {
    auto xs = vector<int>();
    for (unsigned int i = 0; i < n; i++) {
        xs.push_back(i);
    }
    for (unsigned int i = 0; i < n; i++) {
        unsigned int j = rng->uniform() % n;
        auto xi = xs[i];
        auto xj = xs[j];
        xs[i] = xj;
        xs[j] = xi;
    }
    Agenda* ap = makeAgenda(xs, pr, rng);
    return ap;
}


unsigned int Agenda::minAgendaSize(PartitionRule pr, unsigned int n) {
    assert(0 < n); // could be 1, but not 0
    unsigned int minM = 0;
    switch (pr) {
    case PartitionRule::FullBalancedPR:
        minM = n / 2;
        break;
    case PartitionRule::ModBalancedPR:
        if (n <= 5) {
            minM = n / 2;
        }
        else { // 6 or more
            minM = n / 3;
        }
        break;

    case PartitionRule::FreePR:
        if (1 == n) {
            minM = 0;
        }
        else {
            minM = 1;
        }
        break;
    }

    return minM;
}

Agenda* Agenda::makeAgenda(vector<int> xs, PartitionRule pr, PRNG* rng) {
    const int n = xs.size();
    const int minM = minAgendaSize(pr, n);


    Agenda* ap = nullptr;
    if (1 == n) {
        ap = new Terminal(xs[0]);
    }
    else {
        int m = 0;
        while ((m < minM) || (n - m < minM)) {
            m = 1 + rng->uniform() % (n - 1);
        }
        auto ys = vector<int>();
        auto zs = vector<int>();
        for (unsigned int i = 0; i < m; i++) {
            ys.push_back(xs[i]);
        }
        for (unsigned int i = m; i < n; i++) {
            zs.push_back(xs[i]);
        }
        assert(minM <= ys.size());
        assert(minM <= zs.size());
        auto a1 = makeAgenda(ys, pr, rng);
        auto a2 = makeAgenda(zs, pr, rng);
        ap = new Choice(a1, a2);
    }
    return ap;
}

// ------------------------------------------

double Choice::eval(const KMatrix& val, const KMatrix& cap, VotingRule vr, unsigned int i) {
    setProbs(val, cap, vr);
    double valL = lhs->eval(val, cap, vr, i);
    double valR = rhs->eval(val, cap, vr, i);
    double ev = (lProb*valL) + (rProb*valR);
    //cout << "Eval " << i << " of " << *this << " using " << vrName(vr) << " = " << ev << endl << flush;
    return ev;
};

void Choice::clearProbs()  {
    lProb = -1.0;
    rProb = -1.0;
    return;
};

void Choice::assertProbs()  {
    assert (0.0 <= lProb);
    assert (0.0 <= rProb);
    assert (fabs(lProb+rProb - 1.0) < 1E-8);
    return;
};

void Choice::setProbs(const KMatrix& val, const KMatrix& cap, VotingRule vr) {
    if ((lProb < 0) || (rProb < 0)) {
        const unsigned int numA = val.numR();
        double sL = 1E-10;
        double sR = 1E-10;
        for (unsigned int k = 0; k < numA; k++) {
            double lv = lhs->eval(val, cap, vr, k);
            double rv = rhs->eval(val, cap, vr, k);
            double voteLR = Model::vote(vr, cap(k, 0), lv, rv);
            sL = (voteLR > 0) ? (sL + voteLR) : sL;
            sR = (voteLR < 0) ? (sR - voteLR) : sR;
        }
        assert (0 < sL);
        assert (0 < sR);
        lProb = sL / (sL + sR);
        rProb = sR / (sL + sR);
    }
    return;
}

void Choice::showProbs(double pArrive) {
    assertProbs();
    //cout << pArrive << endl;
    double pl = pArrive * lProb;
    double pr = pArrive * rProb;
    cout << "[";
    lhs->showProbs(pl);
    cout << ":";
    rhs->showProbs(pr);
    cout<<"]";
    return;
}

// ------------------------------------------
double Terminal::eval(const KMatrix& val, const KMatrix&, VotingRule, unsigned int i) {
    double ev = val(i, item);
    //cout << "Eval " << i << " of " << *this << " = " << ev << endl << flush;
    return ev;
};


void Terminal::showProbs(double pArrive) {
  printf("%.4f", pArrive); 
    return;
}

}; // end of namespace

// ------------------------------------------
// Copyright KAPSARC. Open Source MIT License
// ------------------------------------------
