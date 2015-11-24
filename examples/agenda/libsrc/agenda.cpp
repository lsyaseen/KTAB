// ------------------------------------------
// Copyright KAPSARC. Open Source MIT License
// ------------------------------------------
#include "agenda.h" 


namespace AgendaControl {

  Agenda* Agenda::makeRandom(unsigned int n, PRNG* rng) {
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
    Agenda* ap = makeAgenda(xs, rng);
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

  Agenda* Agenda::makeAgenda(vector<int> xs, PRNG* rng) {
    const int n = xs.size();
    const int minM = minAgendaSize(PartitionRule::FullBalancedPR, n);


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
      auto a1 = makeAgenda(ys, rng);
      auto a2 = makeAgenda(zs, rng);
      ap = new Choice(a1, a2);
    }
    return ap;
  }

  // ------------------------------------------

  // TODO: recalculating the prob dist every time is quite inefficient.
  // Better to cache it after the first calc.
  double Choice::eval(const KMatrix& val, const KMatrix& cap, VotingRule vr, unsigned int i) {
    double valL = lhs->eval(val, cap, vr, i);
    double valR = rhs->eval(val, cap, vr, i);
    if ((lProb < 0) || (rProb < 0)) {
      const unsigned int numA = val.numR();
      double sL = 1E-6;
      double sR = 1E-6;
      for (unsigned int k = 0; k < numA; k++) {
        double lv = lhs->eval(val, cap, vr, k);
        double rv = rhs->eval(val, cap, vr, k);
        double voteLR = Model::vote(vr, cap(k, 0), lv, rv);
        sL = (voteLR > 0) ? (sL + voteLR) : sL;
        sR = (voteLR < 0) ? (sR - voteLR) : sR;
      }

      lProb = sL / (sL + sR);
      rProb = sR / (sL + sR);
    }
    return (lProb*valL) + (rProb*valR);
  };

  double Terminal::eval(const KMatrix& val, const KMatrix&, VotingRule, unsigned int i) {
    return val(i, item);
  };
}; // end of namespace

// ------------------------------------------
// Copyright KAPSARC. Open Source MIT License
// ------------------------------------------