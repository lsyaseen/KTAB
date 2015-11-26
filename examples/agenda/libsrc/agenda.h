// ------------------------------------------
// Copyright KAPSARC. Open Source MIT License
// ------------------------------------------
#ifndef AGENDA_H
#define AGENDA_H

#include "kutils.h"
#include "kmatrix.h"
#include "prng.h"
#include "kmodel.h"

// ------------------------------------------
namespace AgendaControl {
  using std::vector;
  using std::function;
  using std::ostream;
  using KBase::KMatrix;
  using KBase::Model;
  using KBase::PRNG;
  using KBase::VotingRule;

  class Agenda;
  class Choice;
  class Terminal;

  class Agenda {
  public:
    enum class PartitionRule { FullBalancedPR, ModBalancedPR, FreePR};
    Agenda() {};
    virtual  ~Agenda() {}; 
    virtual void print(ostream& os) const = 0;
    virtual double eval(const KMatrix& val, const KMatrix& cap, VotingRule vr, unsigned int i)  = 0;
    friend ostream& operator<< (ostream& os, const Agenda& a) {
      a.print(os);
      return os;
    };
    static Agenda* makeRandom(unsigned int n, PartitionRule pr, PRNG* rng);
    virtual void clearProbs() = 0;
    virtual void showProbs(double pArrive) = 0;
  private:
    static Agenda* makeAgenda(vector<int> xs, PartitionRule pr, PRNG* rng);
    static unsigned int minAgendaSize(PartitionRule pr, unsigned int n);
  };


  class Choice : public Agenda {
  public:
  Choice( Agenda* la,  Agenda* ra) : Agenda() {
      lhs = la;
      rhs = ra;
    };
    virtual  ~Choice() { delete lhs; delete rhs; }; 
    virtual void print(ostream& os) const {
      os << "[" << *lhs << ":" << *rhs << "]";
      return;
    };
    friend ostream& operator<< (ostream& os, const Choice& c) {
      c.print(os);
      return os;
    };
    virtual double eval(const KMatrix& val, const KMatrix& cap, VotingRule vr, unsigned int i) ;
    void clearProbs();
    void showProbs(double pArrive);
  protected:
    void assertProbs();
    void setProbs(const KMatrix& val, const KMatrix& cap, VotingRule vr);
    Agenda* lhs = nullptr;
    Agenda* rhs = nullptr;
    double lProb = -1.0;
    double rProb = -1.0;
  };


  class Terminal : public Agenda {
  public:
    explicit Terminal(unsigned int v = 0) : Agenda() { item = v;  };
    virtual  ~Terminal() {}; 
    virtual void print(ostream& os) const {
      os << item;
      return;
    };
    friend ostream& operator<< (ostream& os, const Terminal& t) {
      t.print(os);
      return os;
    };
    virtual double eval(const KMatrix& val, const KMatrix& cap, VotingRule vr, unsigned int i) ;
    void clearProbs() { return; }
    void showProbs(double pArrive);
  protected:
    unsigned int item=0;
  };


}; // end of namespace


// ------------------------------------------
#endif
// ------------------------------------------
// Copyright KAPSARC. Open Source MIT License
// ------------------------------------------
