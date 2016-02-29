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
//

#include "rplib.h"

namespace RfrmPri {
// namespace to hold everything related to the
// "priority of reforms" CDMP. Note that KBase has no access.

using std::cout;
using std::endl;
using std::flush;
using std::get;
using KBase::lCorr;
using KBase::VctrPstn;
using KBase::MtchPstn;
using KBase::VPModel;
using KBase::PCEModel;

// -------------------------------------------------
// function definitions


void RPModel::initScen2() { 
  // unfinished

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


    const double aCap[] = {
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



}; // end of namespace


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
