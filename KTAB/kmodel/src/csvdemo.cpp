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
// This is a very simple demonstration of reading CSV files that are in the format
// which SMPC expects.
// --------------------------------------------


#include "kutils.h"
#include "kmatrix.h"
#include "kmodel.h"
#include "csvdemo.h"
#include <easylogging++.h>

// --------------------------------------------

namespace MDemo { // a namespace of which M, A, P, S know nothing

using std::string;
using std::vector;
using KBase::KMatrix;
using KBase::KException;

void pccCSV(const string) {
  const string fs2 = "2016-08-27-A-to-L-changing-results.csv";

  csv::ifstream inStream(fs2.c_str());
  inStream.set_delimiter(',', "$$");
  inStream.enable_trim_quote_on_str(true, '\"');

  //assert(inStream.is_open());
  if (!inStream.is_open()) {
    throw KException("pccCSV: could not open file");
  }
  string dummy = "";
  unsigned int numAct = 0;
  unsigned int numScen = 0;
  unsigned int numCase = 0;


  inStream.read_line();
  inStream >> dummy >> numAct;
  //assert(KBase::Model::minNumActor <= numAct);
  if (KBase::Model::minNumActor > numAct) {
    throw KException("pccCSV: inaccurate number of actors");
  }

  inStream.read_line();
  inStream >> dummy >> numScen;
  //assert(1 < numScen);
  if (1 >= numScen) {
    throw KException("pccCSV: numScen must be more than 1");
  }

  inStream.read_line();
  inStream >> dummy >> numCase;
  //assert(1 <= numCase);
  if (1 > numCase) {
    throw KException("pccCSV: numCase must be at least 1");
  }

  LOG(INFO) << KBase::getFormattedString("Actors %u, Scenarios %u, Cases %u", numAct, numScen, numCase);

  // skip headers
  inStream.read_line();

  vector<string> aNames = {};
  auto caseWeights = KMatrix(numAct, numCase);
  vector<bool> maxVect = {};
  auto outcomes = KMatrix(numAct, numScen);
  auto probWeight = KMatrix(numCase, numScen);

  for (unsigned int i = 0; i < numAct; i++) {
    inStream.read_line();
    string ani = "";
    inStream >> ani;
    aNames.push_back(ani);
    for (unsigned int j = 0; j < numCase; j++) {
      double cw = 0.0;
      inStream >> cw;
      //assert(0.0 <= cw);
      if (0.0 > cw) {
        throw KException("pccCSV: cw must be non-negative");
      }
      //assert(cw <= 100.0);
      if (cw > 100.0) {
        throw KException("pccCSV: cw must not be greater than 100.0");
      }
      caseWeights(i, j) = cw / 100.0;
    } // loop over j, cases
    inStream >> dummy;
    if ("up" == dummy) {
      maxVect.push_back(true);
    }
    else if ("down" == dummy) {
      maxVect.push_back(false);
    }
    else {
      throw (KException("pccCSV: Unrecognized group-optimization direction"));
    }

    for (unsigned int j = 0; j < numScen; j++) {
      double sv = 0.0;
      inStream >> sv;
      outcomes(i, j) = sv;
    } // loop over j, scenarios
  } // loop over i, actors

  LOG(INFO) << "Actor names (min/max)";
  for (unsigned int i = 0; i < numAct; i++) {
    LOG(INFO) << aNames[i] << "   " << (maxVect[i] ? "max" : "min");
  }

  LOG(INFO) << "Case Weights:";
  caseWeights.mPrintf(" %5.2f ");

  LOG(INFO) << "Outcomes:";
  outcomes.mPrintf(" %+.4e  ");


  vector<double> threshVal = {};
  vector<bool> overThresh = {};
  for (unsigned int j = 0; j < numCase; j++) {
    inStream.read_line();
    double tv = 0.0;
    string dir;
    inStream >> dummy; // skip "prob-n"
    inStream >> tv; // read a threshold value
    //assert(0.0 <= tv);
    if (0.0 > tv) {
      throw KException("pccCSV: tv must be non-negative");
    }
    //assert(tv <= 1.0);
    if (tv > 1.0) {
      throw KException("pccCSV: tv must not be greater than 1.0");
    }
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
      //assert(0.0 <= pw);
      if (0.0 > pw) {
        throw KException("pccCSV: pw must be non-negative");
      }
      //assert(pw <= 100.0);
      if (pw > 100.0) {
        throw KException("pccCSV: pw must not be more than 100.0");
      }
      probWeight(j, k) = pw / 100.0;
    }
  } // loop over j, cases

  LOG(INFO) << "Prob threshholds";
  for (unsigned int k = 0; k < numCase; k++) {
    if (overThresh[k]) {
      LOG(INFO) << KBase::getFormattedString("Over  %.3f", threshVal[k]);
    }
    else {
      LOG(INFO) << KBase::getFormattedString("Under %.3f", threshVal[k]);
    }
  }

  LOG(INFO) << "ProbWeights:";
  probWeight.mPrintf(" %5.3f ");



  return;
}

void demoMiniCSV(const string fs) {
  csv::ifstream inStream(fs.c_str());
  inStream.set_delimiter(',', "$$");
  inStream.enable_trim_quote_on_str(true, '\"');

  if (inStream.is_open()) {
    string scenName = "";
    string scenDesc = "";
    unsigned int numAct = 0;
    unsigned int numDim = 0;
    inStream.read_line();
    inStream >> scenName >> scenDesc >> numAct >> numDim;
    LOG(INFO) << KBase::getFormattedString("SName: %s, SDesc: %s", scenName.c_str(), scenDesc.c_str());
    LOG(INFO) << KBase::getFormattedString("Num actors: %i, num dim: %i", numAct, numDim);

    // skip row of headers
    inStream.read_line();

    for (unsigned int i = 0; i < numAct; i++) {
      string aName = "";
      string aDesc = "";
      double aCap = 0.0;
      inStream.read_line();
      inStream >> aName >> aDesc >> aCap;
      string logMsg;
      logMsg += KBase::getFormattedString("Read %2i: %s   %s   %7.3f", i, aName.c_str(), aDesc.c_str(), aCap);
      for (unsigned int d = 0; d < numDim; d++) {
        double dPos = 0.0; // on [0, 100] scale
        double dSal = 0.0; // on [0, 100] scale
        inStream >> dPos >> dSal;
        logMsg += KBase::getFormattedString("  %5.2f  %5.2f", dPos, dSal);
      }
      LOG(INFO) << logMsg;
    }
  }

  return;
}

}
// end of namespace MDemo 

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
