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
//
// Demonstrate a very basic, but highly parameterizable, Spatial Model of Politics.
//
// --------------------------------------------

#include "smp.h"
#include "minicsv.h"


namespace SMPLib {
using std::cout;
using std::endl;
using std::flush;
using std::function;
using std::get;
using std::string;

using KBase::PRNG;
using KBase::KMatrix;
using KBase::KException;
using KBase::Actor;
using KBase::Model;
using KBase::Position;
using KBase::VctrPstn;
using KBase::BigRAdjust;
using KBase::BigRRange;
using KBase::VPModel;
using KBase::State;
using KBase::StateTransMode;
using KBase::VotingRule;
using KBase::PCEModel;
using KBase::ReportingLevel;

// --------------------------------------------
// TODO: consolidate error checking for read functions.
// These reader functions to a lot of very similar
// error checking, so triplicate code should be put
// in sub-functions.
// --------------------------------------------

SMPModel * SMPModel::readCSVStream(string fName, uint64_t s, vector<bool> f) {
  using KBase::KException;
  char * errBuff; // as sprintf requires

  csv::ifstream inStream(fName.c_str());
  inStream.set_delimiter(',', "$$");
  inStream.enable_trim_quote_on_str(true, '\"');

  assert (inStream.is_open());

  string scenName = "";
  string scenDesc = "";
  unsigned int numActor = 0;
  unsigned int numDim = 0;
  inStream.read_line();
  inStream >> scenName >> scenDesc >> numActor >> numDim;


  cout << "Scenario name: |" << scenName << "|" << endl;
  cout << flush;
  assert(scenName.length() <= Model::maxScenNameLen);

  printf("Number of actors: %u \n", numActor);
  printf("Number of dimensions: %i \n", numDim);
  cout << endl << flush;

  if (numDim < 1) { // lower limit
    throw(KBase::KException("SMPModel::readCSVStream: Invalid number of dimensions"));
  }
  assert(0 < numDim);
  if ((numActor < minNumActor) || (maxNumActor < numActor)) { // avoid impossibly low or ridiculously large
    throw(KBase::KException("SMPModel::readCSVStream: Invalid number of actors"));
  }
  assert(minNumActor <= numActor);
  assert(numActor <= maxNumActor);


  // get the names of dimensions.
  // format (for 3 dimensions) is like this:
  // Actor,Description,Power,Pstn1,Sal1,Pstn2,Sal2,Pstn3,Sal3,
  inStream.read_line();
  string dummyField;
  inStream >> dummyField; // skip "Actor"
  inStream >> dummyField; // skip "Descripton"
  inStream >> dummyField; // skip "Power"
  auto dNames = vector<string>();
  for (unsigned int d = 0; d < numDim; d++) {
    string dimName, salName;
    inStream >> dimName;
    inStream >> salName;
    assert(dimName.length() <= maxDimDescLen);
    dNames.push_back(dimName);
    printf("Dimension %2u: %s \n", d, dNames[d].c_str());
  }
  cout << endl;

  // Read actor data
  auto actorNames = vector<string>();
  auto actorDescs = vector<string>();
  auto cap = KMatrix(numActor, 1);
  auto nra = KMatrix(numActor, 1);
  auto pos = KMatrix(numActor, numDim);
  auto sal = KMatrix(numActor, numDim);

  for (unsigned int i = 0; i < numActor; i++) {
    string aName = "";
    string aDesc = "";
    double aCap = 0.0;
    inStream.read_line();
    inStream >> aName >> aDesc >> aCap;

    // names must have at least 1 character
    assert(0 < aName.length());
    assert(aName.length() <= Model::maxActNameLen);
    actorNames.push_back(aName);
    printf("Actor %3u name: %s \n", i, actorNames[i].c_str());

    // empty descriptions are allowed
    assert(aDesc.length() <= Model::maxActDescLen);
    actorDescs.push_back(aDesc);
    printf("Actor %3u desc: %s \n", i, actorDescs[i].c_str());


    printf("Actor %3u power: %5.1f \n", i, aCap);
    assert(0 <= aCap); // zero weight is pointless, but not incorrect
    assert(aCap < 1E8); // no real upper limit, so this is just a sanity-check
    cap(i, 0) = aCap;


    // get position and salience for each dimension
    double salI = 0.0;
    for (unsigned int d = 0; d < numDim; d++) {
      double dPos = 0.0; // on [0, 100] scale
      double dSal = 0.0; // on [0, 100] scale
      inStream >> dPos >> dSal;

      printf("pos[%3u , %3u] =  %5.3f \n", i, d, dPos);
      cout << flush;
      if ((dPos < 0.0) || (+100.0 < dPos)) { // lower and upper limit
        errBuff = newChars(100);
        sprintf(errBuff, "SMPModel::readCSVStream: Out-of-bounds position for actor %u on dimension %u:  %f",
                i, d, dPos);
        throw(KException(errBuff));
      }
      assert(0.0 <= dPos);
      assert(dPos <= 100.0);
      pos(i, d) = dPos;

      if ((dSal < 0.0) || (+100.0 < dSal)) { // lower and upper limit
        errBuff = newChars(100);
        sprintf(errBuff, "SMPModel::readCSVStream: Out-of-bounds salience for actor %u on dimension %u:  %f",
                i, d, dSal);
        throw(KException(errBuff));
      }
      assert(0.0 <= dSal);
      salI = salI + dSal;
      printf("sal[%3u, %3u] = %5.3f \n", i, d, dSal);
      cout << flush;
      if (+100.0 < salI) { // upper limit: no more than 100% of attention to all issues
        errBuff = newChars(100);
        sprintf(errBuff,
                "SMPModel::readCSVStream: Out-of-bounds total salience for actor %u:  %f",
                i, salI);
        throw(KException(errBuff));
      }
      assert(salI <= 100.0);
      sal(i, d) = dSal;
    }
    cout << endl << flush;
  }

  cout << "Position matrix:" << endl;
  pos.mPrintf("%5.1f  ");
  cout << endl << endl << flush;
  cout << "Salience matrix:" << endl;
  sal.mPrintf("%5.1f  ");
  cout << endl << flush;

  // get them into the proper internal scale:
  pos = pos / 100.0;
  sal = sal / 100.0;

  // TODO: figure out representation of "accomodate" matrix
  cout << "Setting ideal-accomodation matrix to identity matrix" << endl;
  auto accM = KBase::iMat(numActor);

  // now that it is read and verified, use the data

  auto sm0 = SMPModel::initModel(actorNames, actorDescs, dNames, cap, pos, sal, accM,  s, f);
  return sm0;
}
// end of readCSVStream

SMPModel * SMPModel::readXML(string fName, uint64_t s, vector<bool> f) {
  SMPModel* smp = nullptr;

  assert (smp != nullptr);
  return smp;
}


}; // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
