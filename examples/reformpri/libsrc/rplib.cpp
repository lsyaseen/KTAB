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

#include "rplib.h"
#include "hcsearch.h"
#include "kmodel.h"

namespace RfrmPri
{
// namespace to hold everything related to the
// "priority of reforms" CDMP. Note that KBase has no access.

using std::get;
using std::thread;
using KBase::lCorr;
using KBase::VctrPstn;
using KBase::MtchPstn;
using KBase::VPModel;
using KBase::PCEModel;
using KBase::printVUI;
using KBase::KException;

// -------------------------------------------------
// function definitions


// return vector of neighboring 1- and 2-permutations
vector <MtchPstn>  nghbrPerms(const MtchPstn & mp0)
{
  const unsigned int numI = mp0.match.size();
  auto mpVec = vector <MtchPstn>();
  mpVec.push_back(MtchPstn(mp0));

  // one-permutations
  for (unsigned int i = 0; i < numI; i++)
  {
    for (unsigned int j = i + 1; j < numI; j++)
    {
      unsigned int ei = mp0.match[i];
      unsigned int ej = mp0.match[j];

      auto mij = MtchPstn(mp0);
      mij.match[i] = ej;
      mij.match[j] = ei;
      mpVec.push_back(mij);
    }
  }


  // two-permutations
  for (unsigned int i = 0; i < numI; i++)
  {
    for (unsigned int j = i + 1; j < numI; j++)
    {
      for (unsigned int k = j + 1; k < numI; k++)
      {
        unsigned int ei = mp0.match[i];
        unsigned int ej = mp0.match[j];
        unsigned int ek = mp0.match[k];

        auto mjki = MtchPstn(mp0);
        mjki.match[i] = ej;
        mjki.match[j] = ek;
        mjki.match[k] = ei;
        mpVec.push_back(mjki);

        auto mkij = MtchPstn(mp0);
        mkij.match[i] = ek;
        mkij.match[j] = ei;
        mkij.match[k] = ej;
        mpVec.push_back(mkij);

      }
    }
  }
  //unsigned int mvs = mpVec.size() ;
  //cout << mvs << endl << flush;
  //cout << flush;
  return mpVec;
}; // end of nghbrPerms
// -------------------------------------------------
// class-method definitions

RPActor::RPActor(string n, string d, const RPModel* rm) : Actor(n, d)
{
  rpMod = rm;
}

RPActor::~RPActor()
{
  // nothing yet
}

double RPActor::vote(unsigned int est, unsigned int p1, unsigned int p2, const State* st) const
{
  /// vote between the current positions to actors at positions p1 and p2 of this state
  auto rPos1 = ((const MtchPstn*)(st->pstns[p1]));
  auto rPos2 = ((const MtchPstn*)(st->pstns[p2]));
  double v = vote(rPos1, rPos2);
  return v;
}

double RPActor::vote(const Position* ap1, const Position* ap2) const
{
  double u1 = posUtil(ap1);
  double u2 = posUtil(ap2);
  const double v12 = Model::vote(vr, sCap, u1, u2);
  return v12;
}

double RPActor::posUtil(const Position * ap1) const
{
  auto rp = ((const MtchPstn *)ap1);
  unsigned int ai = rpMod->actrNdx(this);
  double u0 = rpMod->utilActorPos(ai, rp->match);
  return u0;
}

// Given the utility matrix, uMat, calculate the expected utility to each actor,
// as a column-vector. Again, this is from the perspective of whoever developed uMat.
KMatrix RPState::expUtilMat(KBase::ReportingLevel rl,
                            unsigned int numA,
                            unsigned int numP,
                            KBase::VPModel vpm,
                            const KMatrix & uMat) const
{
  // BTW, be sure to lambda-bind uMat *after* it is modified.
  //assert(uMat.numR() == numA); // must include all actors
  if (uMat.numR() != numA) { // must include all actors
    throw KException("KMatrix RPState::expUtilMat: Number of rows in uMat should be equal to actor's count");
  }
  //assert(uMat.numC() <= numP); // might have dropped some duplicates
  if (uMat.numC() > numP) { // might have dropped some duplicates
    throw KException("KMatrix RPState::expUtilMat: Number of columns in uMat should be equal to num of positions");
  }

  auto assertRange = [](const KMatrix& m, unsigned int i, unsigned int j)
  {
    // due to round-off error, we must have a tolerance factor
    const double tol = 1E-10;
    const double mij = m(i, j);
    if ((mij + tol < 0.0) || (1.0 + tol < mij))
    {
      LOG(INFO) << KBase::getFormattedString("%f  %i  %i ", mij, i, j);
    }
    //assert(0.0 <= mij + tol);
    if (0.0 > mij + tol) {
      throw KException("KMatrix RPState::expUtilMat: mij should be slightly above 0.0");
    }
    //assert(mij <= 1.0 + tol);
    if (mij > 1.0 + tol) {
      throw KException("KMatrix RPState::expUtilMat: mij should be slightly below 1.0");
    }
    return;
  };

  auto uRng = [assertRange, uMat](unsigned int i, unsigned int j)
  {
    assertRange(uMat, i, j);
    return;
  };
  KMatrix::mapV(uRng, uMat.numR(), uMat.numC());

  auto vkij = [this, uMat](unsigned int k, unsigned int i, unsigned int j)   // vote_k(i:j)
  {
    auto ak = (const RPActor*)(rpMod->actrs[k]);
    auto v_kij = Model::vote(ak->vr, ak->sCap, uMat(k, i), uMat(k, j));
    return v_kij;
  };

  // the following uses exactly the values in the given euMat,
  // which may or may not be square
  const KMatrix c = Model::coalitions(vkij, uMat.numR(), uMat.numC());
  const auto pv2 = Model::probCE2(rpMod->pcem, vpm, c);
  const auto p = get<0>(pv2); // column
  const auto pv = get<1>(pv2); //square
  const KMatrix eu = uMat*p; // column
  //assert(numA == eu.numR());
  if (numA != eu.numR()) {
    throw KException("KMatrix RPState::expUtilMat: Number of rows in eu should be equal to actor's count");
  }
  //assert(1 == eu.numC());
  if (1 != eu.numC()) {
    throw KException("KMatrix RPState::expUtilMat: eu must be a column matrix");
  }
  auto euRng = [assertRange, eu](unsigned int i, unsigned int j) {
    assertRange(eu, i, j);
    return;
  };
  KMatrix::mapV(euRng, eu.numR(), eu.numC());

  if (ReportingLevel::Low < rl)
  {
    LOG(INFO) << "Util matrix is" << uMat.numR() << "x" << uMat.numC();
    LOG(INFO) << "Assessing EU from util matrix: ";
    uMat.mPrintf(" %.6f ");

    LOG(INFO) << "Coalition strength matrix";
    c.mPrintf(" %12.6f ");

    LOG(INFO) << "Probability Opt_i > Opt_j";
    pv.mPrintf(" %.6f ");

    LOG(INFO) << "Probability Opt_i";
    p.mPrintf(" %.6f ");

    LOG(INFO) << "Expected utility to actors: ";
    eu.mPrintf(" %.6f ");
  }

  return eu;
};
// end of expUtilMat


// --------------------------------------------
// JAH 20160711 added rng seed
RPModel::RPModel(string d, uint64_t s, vector<bool> f) : Model(d, s, f) {
  // nothing yet
}


RPModel::~RPModel()
{
  // nothing yet
}

bool RPModel::equivStates(const RPState * rs1, const RPState * rs2)
{
  const unsigned int numA = rs1->pstns.size();
  //assert(numA == rs2->pstns.size());
  if (numA != rs2->pstns.size()) {
    throw KException("RPModel::equivStates: size of rs2->pstns must be equal to actor count");
  }
  bool rslt = true;
  for (unsigned int i = 0; i < numA; i++)
  {
    auto m1 = ((MtchPstn*)(rs1->pstns[i]));
    auto m2 = ((MtchPstn*)(rs2->pstns[i]));
    bool same = (m1->match == m2->match);
    //cout << "Match on position " << i << "  " << same << endl << flush;
    rslt = rslt && same;
  }
  return rslt;
}


void RPModel::initScen(unsigned int ns)
{
  switch (ns)
  {
  case 0:
    initScen0();
    break;
  case 1:
    initScen1();
    break;
    // hard-coded cases replaced by XML
    /*
    case 2:
    case 20:
    case 21:
    case 22:
    case 23:
      initScen2Avrg(ns);
      break;
    case 3:
    case 30:
    case 31:
    case 32:
    case 33:
      initScen3Top4(ns);
      break;
      */
  default:
    LOG(INFO) << "Unrecognized scenario number" << ns;
    break;
  }

  return;
}



void RPModel::initScen0()
{
  //assert(nullptr != rng);
  if (nullptr == rng) {
    throw KException("RPModel::initScen0: rng is null pointer");
  }
  const unsigned int numA = 40;
  numItm = 7;
  numCat = numItm;

  govCost = KMatrix::uniform(rng, 1, numItm, 25, 100);
  govBudget = 0.6 * sum(govCost); // cannot afford everything
  obFactor = 0.10;

  const KMatrix utils = KMatrix::uniform(rng, numA, numItm, 10, 100);
  // The 'utils' matrix shows the utilities to the actor (row) of each reform item (clm)
  double aCap[numA];
  for (unsigned int i = 0; i < numA; i++)
  {
    aCap[i] = rng->uniform(1.0, 100.0);
  }


  configScen(numA, aCap, utils);
  return;
}


void RPModel::readXML(string fileName)
{
  using tinyxml2::XMLDocument;
  using tinyxml2::XMLElement;
  using KBase::KException;
  // read XML file with tinyXML2 then do equivalent of
  // initScen and configScen

  XMLDocument d1;
  try
  {
    d1.LoadFile(fileName.c_str());
    auto eid = d1.ErrorID();
    if (0 != eid)
    {
      string errMsg = string("Tinyxml2 ErrorID: ") + std::to_string(eid)
                + ", Error Name: " + d1.ErrorName(); //  this fails to link: d1.GetErrorStr1();
            throw KException(errMsg);
    }
    else
    {
      // missing data causes the missing XMLElement* to come back as nullptr,
      // so we get a segmentation violation, which is not catchable.

      XMLElement* scenEl = d1.FirstChildElement("Scenario");
      XMLElement* scenNameEl = scenEl->FirstChildElement("name");
      //assert(nullptr != scenNameEl);
      if (nullptr == scenNameEl) {
        throw KException("RPModel::readXML: scenNameEl is null pointer");
      }
      try
      {
        const char * sName = scenNameEl->GetText();
        LOG(INFO) << "Name of scenario:" << sName;
      }
      catch (...)
      {
        throw (KException("Error reading file header"));
      }

      XMLElement* scenDescEl = scenEl->FirstChildElement("desc");
      //assert(nullptr != scenDescEl);
      if (nullptr == scenDescEl) {
        throw KException("RPModel::readXML: scenDescEl is null pointer");
      }
      const char* sn2 = scenDescEl->GetText();
      //assert(nullptr != sn2);
      if (nullptr == sn2) {
        throw KException("RPModel::readXML: sn2 is null pointer");
      }

      // get some top level values from the corresponding Elements (no Attributes)


      XMLElement* gbEl = scenEl->FirstChildElement("govBudget");
      //assert(nullptr != gbEl);
      if (nullptr == gbEl) {
        throw KException("RPModel::readXML: gbEl is null pointer");
      }
      double gb = 1000.0; // impossible value
      gbEl->QueryDoubleText(&gb);
      //assert(0.0 < gb);
      if (0.0 >= gb) {
        throw KException("RPModel::readXML: gb must be positive");
      }
      //assert(gb <= 100.0);
      if (gb > 100.0) {
        throw KException("RPModel::readXML: gb must not be more than 100.0");
      }
      govBudget = ((unsigned int)(0.5 + gb));

      XMLElement* obEl = scenEl->FirstChildElement("outOfBudgetFactor");
      //assert(nullptr != obEl);
      if (nullptr == obEl) {
        throw KException("RPModel::readXML: obEl is null pointer");
      }
      double obf = 1000.0; // impossible value
      obEl->QueryDoubleText(&obf);
      //assert(0.0 < obf);
      if (0.0 >= obf) {
        throw KException("RPModel::readXML: obf must be positive");
      }
      //assert(obf < 1.0);
      if (obf >= 1.0) {
        throw KException("RPModel::readXML: obf must be less than 1.0");
      }
      obFactor = obf;

      XMLElement* pdEl = scenEl->FirstChildElement("orderFactor");
      //assert(nullptr != pdEl);
      if (nullptr == pdEl) {
        throw KException("RPModel::readXML: pdEl is null pointer");
      }
      double pdf = 1000.0; // impossible value
      pdEl->QueryDoubleText(&pdf);
      //assert(0.0 < pdf);
      if (0.0 >= pdf) {
        throw KException("RPModel::readXML: pdf must be positive");
      }
      //assert(pdf < 1.0);
      if (pdf >= 1.0) {
        throw KException("RPModel::readXML: pdf must be less than 1.0");
      }
      pDecline = pdf;

      // TODO: read these two parameters from XML
      KBase::VotingRule vrScen = KBase::VotingRule::Proportional;
      RfrmPri::RPActor::PropModel pmScen = RfrmPri::RPActor::PropModel::ExpUtil;

      // read all the categories
      unsigned int nc = 0;
      try
      {
        XMLElement* catsEl = scenEl->FirstChildElement("Categories");
        //assert(nullptr != catsEl);
        if (nullptr == catsEl) {
          throw KException("RPModel::readXML: catsEl is null pointer");
        }
        XMLElement* cEl = catsEl->FirstChildElement("category");
        //assert(nullptr != cEl); // has to be at least one
        if (nullptr == cEl) {
          throw KException("RPModel::readXML: cEl is null pointer");
        }
        while (nullptr != cEl)
        {
          nc++;
          cEl = cEl->NextSiblingElement("category");
        }
        LOG(INFO) << "Found" << nc << "categories";
        numCat = nc;
      }
      catch (...)
      {
        throw (KException("Error reading Categories data"));
      }

      // In this case, the number of items should equal to the number of categories,
      // so we can setup the matrix with the following size.
      govCost = KMatrix(1, numCat);
      // read all the items
      unsigned int ni = 0;
      try
      {
        XMLElement* itemsEl = scenEl->FirstChildElement("Items");
        //assert(nullptr != itemsEl);
        if (nullptr == itemsEl) {
          throw KException("RPModel::readXML: itemsEl is null pointer");
        }
        XMLElement* iEl = itemsEl->FirstChildElement("Item");
        //assert(nullptr != iEl); // has to be at least one
        if (nullptr == iEl) {
          throw KException("RPModel::readXML: iEl is null pointer");
        }
        while (nullptr != iEl)
        {
          double gci = 0.0;
          XMLElement* gcEl = iEl->FirstChildElement("cost");
          gcEl->QueryDoubleText(&gci);
          //assert(0.0 < gci);
          if (0.0 >= gci) {
            throw KException("RPModel::readXML: gci must be positive");
          }
          govCost(0, ni) = gci;
          ni++;
          iEl = iEl->NextSiblingElement("Item");
        }
        LOG(INFO) << "Found" << ni << "items" ;
        //assert(ni == nc); // for this problem, number of items and categories are equal
        if (ni != nc) { // for this problem, number of items and categories are equal
          throw KException("RPModel::readXML: number of items and categories must be equal");
        }
        numItm = ni;
      }
      catch (...)
      {
        throw (KException("Error reading Items data"));
      }

      // We now know numItm and obFactor, so we can fill in prob vector.
      prob = vector<double>();
      double pj = 1.0;
      LOG(INFO) << KBase::getFormattedString("pDecline factor: %.3f", pDecline);
      LOG(INFO) << KBase::getFormattedString("obFactor: %.3f", obFactor);
      // rate of decline has little effect on the results.
      for (unsigned int j = 0; j < numItm; j++)
      {
        prob.push_back(pj);
        pj = pj * pDecline;
      }

      // try to read all the actors
      try
      {
        unsigned int na = 0;
        XMLElement* actorsEl = scenEl->FirstChildElement("Actors");
        //assert(nullptr != actorsEl);
        if (nullptr == actorsEl) {
          throw KException("RPModel::readXML: actorsEl is null pointer");
        }
        XMLElement* aEl = actorsEl->FirstChildElement("Actor");
        //assert(nullptr != aEl); // has to be at least one
        if (nullptr == aEl) {
          throw KException("RPModel::readXML: aEl is null pointer");
        }
        while (nullptr != aEl)
        {
          const char* aName = aEl->FirstChildElement("name")->GetText();
          const char* aDesc = aEl->FirstChildElement("description")->GetText();
          double cap = 0.0; // another impossible value
          aEl->FirstChildElement("capability")->QueryDoubleText(&cap);
          //assert(0.0 < cap);
          if (0.0 >= cap) {
            throw KException("RPModel::readXML: cap must be positive");
          }

          auto ri = new RPActor(aName, aDesc, this);
          ri->sCap = cap;
          ri->riVals = vector<double>();
          ri->idNum = na;
          ri->vr = vrScen;
          ri->pMod = pmScen;

          XMLElement* ivsEl = aEl->FirstChildElement("ItemValues");
          unsigned int numIVS = 0;
          XMLElement* ivEl = ivsEl->FirstChildElement("iVal");
          //assert(nullptr != ivEl);
          if (nullptr == ivEl) {
            throw KException("RPModel::readXML: ivEl is null pointer");
          }
          while (nullptr != ivEl)
          {
            numIVS++;
            double iv = -1.0; // impossible value
            ivEl->QueryDoubleText(&iv);
            //assert(0 <= iv);
            if (0 > iv) {
              throw KException("RPModel::readXML: iv must be non-negative");
            }
            ri->riVals.push_back(iv);
            ivEl = ivEl->NextSiblingElement("iVal");
          }
          //assert(ni == numIVS); // must have a value for each item
          if (ni != numIVS) { // must have a value for each item
            throw KException("RPModel::readXML: numIVS must have a value for each item");
          }
          addActor(ri);
          // move to the next, if any
          na++;
          aEl = aEl->NextSiblingElement("Actor");
        }
        LOG(INFO) << "Found" << na << "actors";
        //assert(minNumActor <= na);
        if (minNumActor > na) {
          throw KException("RPModel::readXML: actor count can not be less than a minimum value");
        }
        //assert(na <= maxNumActor);
        if (na > maxNumActor) {
          throw KException("RPModel::readXML: actor count can not be more than a maximum value");
        }
      }
      catch (KException &ke) {
        throw ke;
      }
      catch (...)
      {
        throw (KException("Error reading Actors data"));
      }
    }
  }
  catch (const KException& ke)
  {
    LOG(INFO) << "Caught KException in RPModel::readXML: " << ke.msg;
  }
  catch (...)
  {
    LOG(INFO) << "Caught unidentified exception in RPModel::readXML";
  }

  return;
}

void RPModel::initScen1()
{
  // Notionally, we have 15 actors who are negotiating over what the government's
  // priorities will be. There are seven kinds reforms contemplated.
  // Each item has a "cost", so not everything can get done. Each actor values
  // each item differently, and hence would like to see a different priority.
  // Notice that, because costs differ, it would not be smart to just put the
  // most valuable items first. Suppose the government budget is 100. If item A
  // costs 100 and is worth 20, while B and C each cost 50 and are worth 15, then
  // it would be much better to put B and C as high priority.
  // As an individual optimization problem, it would be almost a knapsack problem.
  // But as collective decision making, it is more complex.
  // Further, because of the budget limit and the discounting of items toward the end
  // of the list, the utility of an ordering is not the sum of items. Because of these
  // iteractions, it is not quite the same structure as a knapsack problem.
  //
  // This structure could easily be extended to the choice of research projects to fund,
  // within a budget limit, taking into account the interaction of projects.

  const unsigned int numA = 15;
  numItm = 7;
  numCat = numItm;


  // Reform items are (A, B, C, D, E, F, G), in that order
  // Categories are (First, Second, Third, ... Seventh), in that order

  const double gcArray[] = { 32, 38, 29, 15, 18, 41, 27 };
  govCost = KMatrix::arrayInit(gcArray, 1, numItm);
  govBudget = 100;
  obFactor = 0.10;

  const double uArray[] =
  {
    65, 60, 40, 25, 10, 100, 40, //  0
    70, 35, 80, 50, 0, 20, 100, //  1
    60, 75, 25, 0, 60, 100, 45, //  2
    55, 25, 60, 80, 30, 50, 30, //  3
    65, 100, 40, 80, 0, 60, 25, //  4
    45, 60, 100, 80, 40, 60, 20, //  5
    35, 100, 50, 90, 0, 80, 100, //  6
    35, 100, 20, 60, 0, 50, 25, //  7
    40, 80, 100, 60, 50, 25, 50, //  8
    60, 80, 100, 25, 40, 60, 35, //  9
    65, 60, 100, 80, 50, 30, 25, //  10
    60, 80, 100, 40, 50, 60, 35, //  11
    50, 50, 60, 0, 20, 100, 25, //  12
    50, 0, 60, 0, 100, 80, 0, //  13
    60, 0, 50, 0, 100, 80, 0  //  14
  };
  // rows are actors, columns are reform items

  const KMatrix utils = KMatrix::arrayInit(uArray, numA, numItm);
  // The 'utils' matrix shows the utilities to the actor (row) of each reform item (clm)


  const double aCap[] =
  {
    40,  //  0
    30,  //  1
    15,  //  2
    30,  //  3
    20,  //  4
    10,  //  5
    20,  //  6
    5,   //  7
    15,  //  8
    25,  //  9
    20,  // 10
    10,  // 11
    5,   // 12
    5,   // 13
    10   // 14
  };

  configScen(numA, aCap, utils);
  return;
}

void RPModel::configScen(unsigned int numA, const double aCap[], const KMatrix & utils)
{

  auto makeActrName = [](unsigned int i)
  {
    const unsigned int nbSize = 15;
    char * nameBuff = new char[nbSize];
    for (unsigned int j = 0; j < nbSize; j++)
    {
      nameBuff[j] = (char)0;
    }
    sprintf(nameBuff, "RPActor-%02i", i);
    auto ni = string(nameBuff);
    return ni;
  };
  for (unsigned int i = 0; i < numA; i++)
  {
    auto ri = new RPActor(makeActrName(i), "generic RP actor", this);
    ri->sCap = aCap[i];
    ri->riVals = vector<double>();
    ri->idNum = i;
    ri->vr = KBase::VotingRule::Proportional;
    ri->pMod = RfrmPri::RPActor::PropModel::ExpUtil;
    for (unsigned int j = 0; j < numItm; j++)
    {
      ri->riVals.push_back(utils(i, j));
    }
    //cout << "Adding actor " << i << endl << flush;
    addActor(ri);
    //cout << flush;
    //cout << flush;
  }

  //assert(numAct == actrs.size());
  if (numAct != actrs.size()) {
    throw KException("RPModel::initScen1: Size of actrs should be equal to actor's count");
  }
  //assert(numA == numAct);
  if (numA != numAct) {
    throw KException("RPModel::initScen1: numA should be equal to numAct");
  }

  prob = vector<double>();
  double pj = 1.0;
  LOG(INFO) << KBase::getFormattedString("pDecline factor: %.3f", pDecline);
  LOG(INFO) << KBase::getFormattedString("obFactor: %.3f", obFactor);
  // rate of decline has little effect on the results.
  for (unsigned int j = 0; j < numItm; j++) {
    prob.push_back(pj);
    pj = pj * pDecline;
  }
  return;
}

double RPModel::utilActorPos(unsigned int ai, const VUI &pstn) const {
  //assert(ai < numAct);
  if (ai >= numAct) {
    throw KException("RPModel::utilActorPos: actor index should be less than actor count");
  }
  //assert(numAct == actrs.size());
  if (numAct != actrs.size()) {
    throw KException("RPModel::utilActorPos: Size of actrs should be equal to actor's count");
  }
  auto rai = ((const RPActor*)(actrs[ai]));
  const double pvMin = rai->posValMin;
  const double pvMax = rai->posValMax;
  //assert(nullptr != rai);
  if (nullptr == rai) {
    throw KException("RPModel::utilActorPos: rai is null pointer");
  }
  double costSoFar = 0;
  double uip = 0.0;
  //assert(0 < govBudget);
  if (0 >= govBudget) {
    throw KException("RPModel::utilActorPos: govtBudget must be positive");
  }
  //cout << "govBudget: " << govBudget << endl;


  for (unsigned int j = 0; j < pstn.size(); j++)
  {
    unsigned int rj = pstn[j];
    double cj = govCost(0, rj);
    double uij = prob[j] * rai->riVals[rj];
    if (govBudget < costSoFar + cj)
    {
      uij = uij * obFactor;
    }
    uip = uip + uij;
    costSoFar = costSoFar + cj;
  }
  if (pvMin < pvMax)   // normalization is configured
  {
    uip = (uip - pvMin) / (pvMax - pvMin);
    //cout << "using normalized utilities for actor " << ai << endl << flush;
  }
  else
  {
    // using raw utilities is necessary while
    // determining the normalization parameters.
    // cout << "using raw utilities for actor " << ai << endl << flush;
  }
  return uip;
}


void RPModel::showHist() const
{
  for (unsigned int i = 0; i < history.size(); i++)
  {
    auto si = ((const RPState *)(history[i]));
    LOG(INFO) << "State" << i;
    si->show();

  }

  return;
}

// --------------------------------------------

RPState::RPState(Model* mod) : State(mod)
{
  step = nullptr;
  rpMod = ((const RPModel *)mod);
}

RPState::~RPState()
{
  // nothing yet
}

bool RPState::equivNdx(unsigned int i, unsigned int j) const
{
  /// Compare two actual positions in the current state
  auto mpi = ((const MtchPstn *)(pstns[i]));
  auto mpj = ((const MtchPstn *)(pstns[j]));
  //assert(mpi != nullptr);
  if (mpi == nullptr) {
    throw KException("RPState::equivNdx: mpi is null pointer");
  }
  //assert(mpj != nullptr);
  if (mpj == nullptr) {
    throw KException("RPState::equivNdx: mpj is null pointer");
  }
  bool rslt = ((*mpi) == (*mpj));
  return rslt;
}


tuple <KMatrix, VUI> RPState::pDist(int persp) const
{
  /// Calculate the probability distribution over states from this perspective

  // TODO: convert this to a single, commonly used setup function

  const unsigned int numA = model->numAct;
  const unsigned int numP = numA; // for this demo, the number of positions is exactly the number of actors

  // get unique indices and their probability
  //assert(0 < uIndices.size()); // should have been set with setUENdx();
  if (0 >= uIndices.size()) { // should have been set with setUENdx();
    throw KException("RPState::equivNdx: uIndices size must be positive");
  }
  //auto uNdx2 = uniqueNdx(); // get the indices to unique positions

  const unsigned int numU = uIndices.size();
  //assert(numU <= numP); // might have dropped some duplicates
  if (numU > numP) { // might have dropped some duplicates
    throw KException("RPState::equivNdx: numU must not be more than numP");
  }

  LOG(INFO) << "Number of aUtils: " << aUtil.size();

  const KMatrix u = aUtil[0]; // all have same beliefs in this demo

  auto uufn = [u, this](unsigned int i, unsigned int j1)
  {
    return u(i, uIndices[j1]);
  };

  auto uMat = KMatrix::map(uufn, numA, numU);
  //assert(uMat.numR() == numA); // must include all actors
  if (uMat.numR() != numA) {
    throw KException("RPState::equivNdx: uMat row count must be equal to number of actors");
  }
  //assert(uMat.numC() == numU);
  if (uMat.numC() != numU) {
    throw KException("RPState::equivNdx: uMat column count must be equal to numU");
  }

  // vote_k ( i : j )
  auto vkij = [this, uMat](unsigned int k, unsigned int i, unsigned int j)
  {
    auto ak = (RPActor*)(model->actrs[k]);
    auto v_kij = Model::vote(ak->vr, ak->sCap, uMat(k, i), uMat(k, j));
    return v_kij;
  };

  // the following uses exactly the values in the given euMat,
  // which may or may not be square
  const auto c = Model::coalitions(vkij, uMat.numR(), uMat.numC());
  const auto pv2 = Model::probCE2(model->pcem, model->vpm, c);
  const auto p = get<0>(pv2); // column
  const auto pv = get<1>(pv2); // square
  const auto eu = uMat*p; // column
  //assert(numA == eu.numR());
  if (numA != eu.numR()) {
    throw KException("RPState::equivNdx: eu must have one row for each actor");
  }
  //assert(1 == eu.numC());
  if (1 != eu.numC()) {
    throw KException("RPState::equivNdx: eu must be a column vector");
  }
  return tuple <KMatrix, VUI>(p, uIndices);
}



RPState* RPState::stepSUSN()
{
  LOG(INFO) << "State number " << model->history.size() - 1;
  if ((0 == uIndices.size()) || (0 == eIndices.size()))
  {
    setUENdx();
  }
  setAUtil(-1, ReportingLevel::Silent);
  show();

  auto s2 = doSUSN(ReportingLevel::Silent);
  s2->step = [s2]()
  {
    return s2->stepSUSN();
  };
  return s2;
}


RPState* RPState::doSUSN(ReportingLevel rl) const {
  const unsigned int numA = model->numAct;
  //assert(numA == rpMod->actrs.size());
  if (numA != rpMod->actrs.size()) {
    throw KException("RPState::equivNdx: Size of actrs vector should be equal to actor's count");
  }
  const unsigned int numU = uIndices.size();
  //assert((0 < numU) && (numU <= numA));
  if ((0 >= numU) || (numU > numA)) {
    throw KException("RPState::equivNdx: numU out of valid range");
  }
  //assert(numA == eIndices.size());
  if (numA != eIndices.size()) {
    throw KException("RPState::equivNdx: eIndices size must be equal to actor count");
  }
  const KMatrix u = aUtil[0]; // all have same beliefs in this demo
  auto vpm = model->vpm;
  auto euMat = [rl, numA, vpm, this](const KMatrix & uMat)
  {
    const unsigned int numP = pstns.size();
    return expUtilMat(rl, numA, numP, vpm, uMat);
  };
  auto euState = euMat(u);
  LOG(INFO) << "Actor expected utilities: ";
  KBase::trans(euState).mPrintf("%6.4f, ");

  if (ReportingLevel::Low < rl)
  {
    LOG(INFO) << "---------------------------------------";
    LOG(INFO) << "Assessing utility of actual state to all actors";
    for (unsigned int h = 0; h < numA; h++)
    {
      LOG(INFO) << "  actual utilities not available";
    }
    string log = "Out of " + std::to_string(numA) + " positions, "
      + std::to_string(numU) + " were unique, with these indices: ";
    for (auto i : uIndices)
    {
      log += " " + std::to_string(i);
    }
    LOG(INFO) << log;
  }


  auto uufn = [u, this](unsigned int i, unsigned int j1)
  {
    return u(i, uIndices[j1]);
  };
  auto uUnique = KMatrix::map(uufn, numA, numU);


  // Get expected-utility vector, one entry for each actor, in the current state.
  const KMatrix eu0 = euMat(uUnique); // 'u' with duplicates, 'uUnique' without duplicates

  RPState* s2 = new RPState(model);
  //assert(numA == s2->pstns.size()); // pre-allocated by constructor, all nullptr's
  if (numA != s2->pstns.size()) { // pre-allocated by constructor, all nullptr's
    throw KException("RPState::equivNdx: postions size of s2 must be equal to actor count");
  }

  // TODO: clean up the nesting of lambda-functions.
  // need to create a hypothetical state and run setOneAUtil(h,Silent) on it
  //
  // The newPosFn does a generic hill climb to find the best next position for actor h,
  // and stores it in s2.
  // To do that, it defines three functions for evaluation, neighbors, and show:
  // efn, nghbrPerms, and sfn.
  auto newPosFn = [this, rl, euMat, u, eu0, s2](const unsigned int h)
  {
    s2->pstns[h] = nullptr;
    auto ph = ((const MtchPstn *)(pstns[h]));

    // Evaluate h's estimate of the expected utility, to h, of
    // advocating position mp. To do this, build a hypothetical utility matrix representing
    // h's estimates of the direct utilities to all other actors of h adopting this
    // Position. Do that by modifying the h-column of h's matrix.
    // Then compute the expected probability distribution, over h's hypothetical position
    // and everyone else's actual position. Finally, compute the expected utility to
    // each actor, given that distribution, and pick out the value for h's expected utility.
    // That is the expected value to h of adopting the position.
    auto efn = [this, euMat, rl, u, h](const MtchPstn & mph)
    {
      // This correctly handles duplicated/unique options
      // We modify the given euMat so that the h-column
      // corresponds to the given mph, but we need to prune duplicates as well.
      // This entails some type-juggling.
      const KMatrix uh0 = aUtil[h];
      //assert(KBase::maxAbs(u - uh0) < 1E-10); // all have same beliefs in this demo
      if (KBase::maxAbs(u - uh0) >= 1E-10) { // all have same beliefs in this demo
        throw KException("RPState::equivNdx: inaccurate value of uh0");
      }
      if (mph.match.size() != rpMod->numItm)
      {
        LOG(INFO) << mph.match.size();
        LOG(INFO) << rpMod->numItm;
      }
      //assert(mph.match.size() == rpMod->numItm);
      if (mph.match.size() != rpMod->numItm) {
        throw KException("RPState::equivNdx: match's size in mph must be same as numItm in rpMod");
      }
      auto uh = uh0;
      for (unsigned int i = 0; i < rpMod->numAct; i++)
      {
        auto ai = (RPActor*)(rpMod->actrs[i]);
        double uih = ai->posUtil(&mph);
        uh(i, h) = uih; // utility to actor i of this hypothetical position by h
      }

      // 'uh' now has the correct h-column. Now we need to see how many options
      // are unique in the hypothetical state, and keep only those columns.
      // This entails juggling back and forth between the all current positions
      // and the one hypothetical position (mph at h).
      // Thus, the next call to euMat will consider only unique options.
      auto equivHNdx = [this, h, mph](const unsigned int i, const unsigned int j)
      {
        // this little function takes care of the different types needed to compare
        // dynamic pointers to positions (all but h) with a constant position (h itself).
        // In other words, the comparisons for index 'h' use the hypothetical mph, not pstns[h]
        bool rslt = false;
        auto mpi = ((const MtchPstn *)(pstns[i]));
        auto mpj = ((const MtchPstn *)(pstns[j]));
        //assert(mpi != nullptr);
        if (mpi == nullptr) {
          throw KException("RPState::equivNdx: mpi is null pointer");
        }
        //assert(mpj != nullptr);
        if (mpj == nullptr) {
          throw KException("RPState::equivNdx: mpj is null pointer");
        }
        if (i == j)
        {
          rslt = true; // Pi == Pj, always
        }
        else if (h == i)
        {
          rslt = (mph == (*mpj));
        }
        else if (h == j)
        {
          rslt = ((*mpi) == mph);
        }
        else
        {
          rslt = ((*mpi) == (*mpj));
        }
        return rslt;
      };

      auto ns = KBase::uiSeq(0, model->numAct - 1);
      const VUI uNdx = get<0>(KBase::ueIndices<unsigned int>(ns, equivHNdx));
      const unsigned int numU = uNdx.size();
      auto hypUtil = KMatrix(rpMod->numAct, numU);
      // we need now to go through 'uh', copying column J the first time
      // the J-th position is determined to be equivalent to something in the unique list
      for (unsigned int i = 0; i < rpMod->numAct; i++) {
        for (unsigned int j1 = 0; j1 < numU; j1++) {
          unsigned int j2 = uNdx[j1];
          hypUtil(i, j1) = uh(i, j2); // hypothetical utility in column h
        }
      }

      if (false) {
        LOG(INFO) << "constructed hypUtil matrix:";
        hypUtil.mPrintf(" %8.2f ");
      }


      if (ReportingLevel::Low < rl)
      {
        LOG(INFO) << "---------------------------------------";
        LOG(INFO) << "Assessing utility to" << h << "of hypo-pos:";
        printVUI(mph.match);
        LOG(INFO) << "Hypo-util minus base util:";
        (uh - uh0).mPrintf(" %+.4E ");
      }
      const KMatrix eu = euMat(hypUtil); // uh or hypUtil
      // BUG: If we use 'uh' here, it passes the (0 <= delta-EU) test, because
      // both hypothetical and actual are then calculated without dropping duplicates.
      // If we use 'hypUtil' here, it sometimes gets (delta-EU < 0), because
      // the hypothetical drops duplicates but the actual (computed elsewhere) does not.
      // FIX: fix  the 'elsewhere'
      const double euh = eu(h, 0);
      //assert(0 < euh);
      if (0 >= euh) {
        throw KException("RPState::equivNdx: euh must be positive");
      }
      //cout << euh << endl << flush;
      //printPerm(mp.match);
      //cout << endl << flush;
      //cout << flush;
      return euh;
    }; // end of efn

    // show some representation of this position on cout
    auto sfn = [](const MtchPstn & mp0)
    {
      printVUI(mp0.match);
      return;
    };

    auto ghc = new KBase::GHCSearch<MtchPstn>();
    ghc->eval = efn;
    ghc->nghbrs = nghbrPerms;
    ghc->show = sfn;

    auto rslt = ghc->run(*ph, // start from h's current positions
                         ReportingLevel::Silent,
                         100, // iter max
                         3, 0.001); // stable-max, stable-tol

    if (ReportingLevel::Low < rl)
    {
      LOG(INFO) << "----------------------------------------";
      LOG(INFO) << "Search for best next-position of actor" << h;;
      //printf("Search for best next-position of actor %2i starting from ", h);
      //trans(*aPos).printf(" %+.6f ");
    }

    double vBest = get<0>(rslt);
    MtchPstn pBest = get<1>(rslt);
    unsigned int iterN = get<2>(rslt);
    unsigned int stblN = get<3>(rslt);

    delete ghc;
    ghc = nullptr;
    if (ReportingLevel::Medium < rl)
    {
      LOG(INFO) << "Iter:" << iterN << "Stable:" << stblN;
      LOG(INFO) << KBase::getFormattedString("Best value for %2i: %+.6f", h, vBest);
      LOG(INFO) << "Best position:    ";
      LOG(INFO) << "numCat: " << pBest.numCat;
      LOG(INFO) << "numItm: " << pBest.numItm;
      LOG(INFO) << "perm: ";
      printVUI(pBest.match);
    }
    MtchPstn * posBest = new MtchPstn(pBest);
    s2->pstns[h] = posBest;
    // no need for mutex, as s2->pstns is the only shared var,
    // and each h is different.

    double du = vBest - eu0(h, 0); // (hypothetical, future) - (actual, current)
    if (ReportingLevel::Low < rl)
    {
      LOG(INFO) << KBase::getFormattedString("EU improvement for %2i of %+.4E", h, du);
    }
    //printf("  vBest = %+.6f \n", vBest);
    //printf("  eu0(%i, 0) for %i = %+.6f \n", h, h, eu0(h,0));
    //cout << endl << flush;
    // Logically, du should always be non-negative, as GHC never returns a worse value than the starting point.
    // However, actors plan on the assumption that all others do not change - yet they do.
    const double eps = 0.05; // 0.025; // enough to avoid problems with round-off error
    //assert(-eps <= du);
    if (-eps > du) {
      throw KException("RPState::equivNdx: inaccurate eps");
    }
    return;
  }; // end of newPosFn


  const bool parP = KBase::testMultiThreadSQLite(false, rl);

  auto ts = vector<thread>();
  // Each actor, h, finds the position which maximizes their EU in this situation.
  for (unsigned int h = 0; h < numA; h++) {
    if (parP) { // launch all, concurrent
      // printf("Starting concurrent thread for new position %i \n", h);
      ts.push_back(thread([newPosFn, h]() {
        newPosFn(h);
        return;
      }));
    }
    else { // do each, sequential
      // printf("Calculating new position %i \n", h);
      newPosFn(h);
    }
  }

  if (parP)   // now join them all before continuing
  {
    for (auto& t : ts)
    {
      t.join();
    }
  }

  //assert(nullptr != s2);
  if (nullptr == s2) {
    throw KException("RPState::equivNdx: s2 is null pointer");
  }
  //assert(pstns.size() == s2->pstns.size());
  if (pstns.size() != s2->pstns.size()) {
    throw KException("RPState::equivNdx: inaccurate number of positions");
  }
  //assert(numA == s2->model->numAct);
  if (numA != s2->model->numAct) {
    throw KException("RPState::equivNdx: inaccurate number of actors");
  }
  for (auto p : s2->pstns)
  {
    //assert(nullptr != p);
    if (nullptr == p) {
      throw KException("RPState::equivNdx: p is null pointer");
    }
  }
  s2->setUENdx();
  return s2;
}
// end of doSUSN

void RPState::setAllAUtil(ReportingLevel rl)
{
  const unsigned int na = model->numAct;

  // make sure prerequisities are at least somewhat setup
  //assert(na == eIndices.size());
  if (na != eIndices.size()) {
    throw KException("RPState::setAllAUtil: eIndices size must be equal to actor count");
  }
  //assert(0 < uIndices.size());
  if (0 >= uIndices.size()) {
    throw KException("RPState::setAllAUtil: uIndices size must be positive");
  }
  //assert(uIndices.size() <= na);
  if (uIndices.size() > na) {
    throw KException("RPState::setAllAUtil: uIndices size must not be more than actor count");
  }

  /// For all states aUtil[h](i,j) is h's estimate of the utility to A_i of Pos_j,
  /// and this function calculates those matrices. Note that for this demo,
  /// all actors have the same perception.
  unsigned int numA = rpMod->numAct;
  //unsigned int numR = rpMod->numItm;;
  auto uFn1 = [this](unsigned int i, unsigned int j)
  {
    auto ai = ((RPActor*)(rpMod->actrs[i]));
    auto pi = pstns[j];
    auto rp = ((const MtchPstn *)pi);
    double uij = ai->posUtil(pstns[j]);
    if (!(0.0 <= uij))
    {
      LOG(INFO) << i << " " << j << "  " << uij;
      printVUI(rp->match);
    }
    //assert(0.0 <= uij);
    if (0.0 > uij) {
      throw KException("RPState::setAllAUtil: must be non-negative");
    }
    return uij;
  };

  // Notice that the actors had their pvMin and pvMax correctly configured
  // using all possible positions during the initial scan.
  // Therefore, rescaling rows based on only the urrent set of options is unneeded.
  auto normU = KMatrix::map(uFn1, numA, numA);
  if (KBase::ReportingLevel::Low < rl)
  {
    LOG(INFO) << "Normalized actor-pos util matrix";
    normU.mPrintf(" %.4f ");

    /// For the purposes of this demo, I consider each actor to know exactly what
    /// the others value. They know what consequences the others expect,
    /// and how they will value those consequences,
    /// even if they disagree on both facts and values.
    LOG(INFO) << "aUtil size: " << aUtil.size();
  }

  //assert(0 == aUtil.size());
  if (0 != aUtil.size()) {
    throw KException("RPState::setAllAUtil: size of aUtil must be zero");
  }
  for (unsigned int i = 0; i < numA; i++)
  {
    aUtil.push_back(normU);
  }
  return;
}




void RPState::setOneAUtil(unsigned int perspH, ReportingLevel rl)
{
  const unsigned int numAct = model->numAct;
  const unsigned int numUnq = uIndices.size();

  //assert(perspH < numAct);
  if (perspH >= numAct) {
    throw KException("RPState::setOneAUtil: perspH must be less than actor count");
  }
  //assert(numAct == aUtil.size());
  if (numAct != aUtil.size()) {
    throw KException("RPState::setOneAUtil: aUtil's size must be equal to actor count");
  }
  //assert((0 == aUtil[perspH].numR()) && (0 == aUtil[perspH].numC()));
  if ((0 != aUtil[perspH].numR()) || (0 != aUtil[perspH].numC())) {
    throw KException("RPState::setOneAUtil: aUtil[perspH] must be empty");
  }
  //assert(numAct == eIndices.size());
  if (numAct != eIndices.size()) {
    throw KException("RPState::setOneAUtil: eIndices size must be equal to actor count");
  }
  //assert(0 < numUnq);
  if (0 >= numUnq) {
    throw KException("RPState::setOneAUtil: numUnq must be positive");
  }
  //assert(numUnq < numAct);
  if (numUnq >= numAct) {
    throw KException("RPState::setOneAUtil: numUnq must be less than actor count");
  }

  auto uh = KMatrix(numAct, numUnq);


  return;
}

void RPState::show() const
{
  const unsigned int numA = model->numAct;
  for (unsigned int i = 0; i < numA; i++)
  {
    auto pi = ((MtchPstn*)(pstns[i]));
    LOG(INFO) << "Position:" << i;
    printVUI(pi->match);
  }
  auto pu = pDist(-1);
  KMatrix p = get<0>(pu);
  auto uNdx = get<1>(pu);
  LOG(INFO) << "There are" << uNdx.size() << "unique positions";
  for (unsigned int i1 = 0; i1 < uNdx.size(); i1++)
  {
    unsigned int i2 = uNdx[i1];
    LOG(INFO) << KBase::getFormattedString("  %2u:  %.4f", i2, p(i1, 0));
  }
  return;
} // end of RPState::show()


}; // end of namespace


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
