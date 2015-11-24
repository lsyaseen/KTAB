// ------------------------------------------
// Copyright KAPSARC. Open Source MIT License
// ------------------------------------------
#include "kmodel.h"
#include "agenda.h"
using std::cout;
using std::endl;
using std::flush;

namespace AgendaControl {



}; // end of namespace

int main(int ac, char **av) {
  using std::cout;
  using std::endl;
  using std::flush;
  using std::function;
  using KBase::KMatrix;
  using KBase::PRNG;
  using KBase::VotingRule;
  using AgendaControl::Agenda;
  using AgendaControl::Choice;
  using AgendaControl::Terminal;

  auto sTime = KBase::displayProgramStart();
  uint64_t dSeed = 0xD67CC16FE69C185C; // arbitrary
  uint64_t seed = dSeed;
  bool run = true;

  auto showHelp = [dSeed]() {
    printf("\n");
    printf("Usage: specify one or more of these options\n");
    printf("--help            print this message\n");
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
  const unsigned int numAgenda = 8000;
  // with eight runs, I bayesian-ly estimated that
  // the probability the optimum appears within x trials is
  // P[X<x] = 1-exp(-x/A), where A=1681.75
  // So we need 7745 to get a 99% of seeing the true optimum.
  const unsigned int numActor = 15;
  const unsigned int numItems = 7;

  auto vr = VotingRule::Binary;
  auto vals = KMatrix::uniform(rng, numActor, numItems, 1.0, 9.99);
  auto caps = KMatrix::uniform(rng, numActor, 1, 10.0, 99.99);

  // find what's best for agenda-setting actor 0
  caps(0, 0) = caps(0, 0) / 10.0; // agenda-setter has little voting power
  unsigned int bestK = 0;
  double bestV = -1.0;

  cout << "Value matrix" << endl;
  vals.mPrintf(" %4.2f ");
  cout << endl << flush;


  cout << "Capability matrix" << endl;
  caps.mPrintf(" %5.2f ");
  cout << endl << flush;

  cout << numAgenda << " random agendas over " << numItems << " items"<< endl;
  for (unsigned int ai = 0; ai < numAgenda; ai++) {
    auto ar = Agenda::makeRandom(numItems, rng);
    printf("%4u ", ai);
    cout << *ar << "    ";
    for (unsigned int k = 0; k < numActor; k++) {
      double vk = ar->eval(vals, caps, vr, k);
      printf(" %4.2f ", vk);
      if ((0 == k) && (bestV < vk)) {
        bestV = vk;
        bestK = ai;
      }
    }
    cout << endl << flush;
    delete ar;
  }

  printf("Best option for agenda-setting actor 0 is %3u with value %.2f \n", bestK, bestV);
   
  delete rng;
  KBase::displayProgramEnd(sTime);
  return 0;
}
// ------------------------------------------
// Copyright KAPSARC. Open Source MIT License
// ------------------------------------------