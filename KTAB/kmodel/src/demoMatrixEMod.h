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

#ifndef DEMO_EMOD_KEM_DATA_H
#define DEMO_EMOD_KEM_DATA_H

#include "kutils.h"
#include "prng.h"
#include "kmatrix.h"
#include "kmodel.h"

#include "emodel.h"
#include "emodel.cpp"


namespace eModKEM {
using std::string;
using std::vector;

using KBase::KMatrix;
using KBase::ReportingLevel;

using KBase::EActor;
using KBase::EModel;
using KBase::EPosition;
using KBase::EState;


// --------------------------------------------
// The Policy Matrix model uses a pre-specified matrix
// of actor-vs-policy utilities.
// The utility matrix provides a (basic) linkage to offline
// models such as KEM/GAMS.
// The unsigned int in the template is just the column of a policy.
class PMatrixModel : public EModel<unsigned int> {
public:
    PMatrixModel (string d = "", uint64_t s=KBase::dSeed, vector<bool> = {});
    virtual ~PMatrixModel();


    // check the matrix and set as utilities,
    // if valid for [numAct,numOpt] utilities in [0,1] range.
    // Also set theta = {0, 1, 2, ...}
    void setPMatrix(const KMatrix pm0);

    // check the row-matrix and set as weight,
    // if valid for non-negative [numAct,1] weights
    void setWeights(const KMatrix w0);

    // must set weights and pMatrix first
    void setActors(vector<string> names, vector<string> descriptions);

    KMatrix getWghtVect() const {
        return wghtVect;
    }; // row vector
    KMatrix getPolUtilMat() const {
        return polUtilMat;
    }; // rectangular

protected:
    KMatrix wghtVect; // column vector of actor weights
    KMatrix polUtilMat; // if set, the basic util(actor,option) matrix

private:

};


class PMatrixPos : public EPosition<unsigned int> {
public:
    PMatrixPos(PMatrixModel* pm, int n);
    virtual ~PMatrixPos();

protected:

private:

};


class PMatrixState : public EState<unsigned int> {
public:
    explicit PMatrixState(PMatrixModel* pm);
    virtual ~PMatrixState();

protected:
    virtual EState<unsigned int>* makeNewEState() const override;
    void setAllAUtil(ReportingLevel rl) override;
    vector<double> actorUtilVectFn( int h, int tj) const override;

private:

};


// --------------------------------------------

/*
const unsigned int numActKEM = 21;
const unsigned int numPolKEM = 30;

// the policy names are used only to instantiate a template
const vector<string> pNamesKEM = {
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29"
};



const vector<string> aNamesKEM = {
    "PA00", "PA01", "PA02", "PA03", "PA04", "PA05", "PA06", "PA07", "PA08", "PA09",
    "PA10", "PA11", "PA12", "PA13", "PA14", "PA15", "PA16", "PA17", "PA18", "PA19",
    "PA20"
};
const vector<string> aDescKEM = {
    "Policy Actor 00", "Policy Actor 01", "Policy Actor 02", "Policy Actor 03",
    "Policy Actor 04", "Policy Actor 05", "Policy Actor 06", "Policy Actor 07",
    "Policy Actor 08", "Policy Actor 09", "Policy Actor 10", "Policy Actor 11",
    "Policy Actor 12", "Policy Actor 13", "Policy Actor 14", "Policy Actor 15",
    "Policy Actor 16", "Policy Actor 17", "Policy Actor 18", "Policy Actor 19",
    "Policy Actor 20"
};
// --------------------------------------------
// weights of actors 0-20
// --------------------------------------------
// numActKEM rows, 1 column
const double weightArray[] = {
    85.0 ,
    22.9 ,
    19.8 ,
    73.8 ,
    25.4 ,
    29.2 ,
    35.5 ,
    102.2 ,
    115.3 ,
    24.3 ,
    18.4 ,
    20.0 ,
    40.1 ,
    34.1 ,
    80.2 ,
    31.3 ,
    54.7 ,
    7.4 ,
    120.5 ,
    1.2 ,
    12.2
};


// --------------------------------------------
// utility to actors 0-20 of policies 0-29
// --------------------------------------------
// numActKEM rows, numPolKEM columns
const double utilArray[] = {
    0.1491 , 0.4289 , 0.9973 , 0.2777 , 0.6942 , 0.9797 , 0.5568 , 0.3547 , 0.9138 , 0.3834 , 0.2918 , 0.7854 , 0.9316 , 0.1806 , 0.6769 , 0.9988 , 0.6328 , 0.7812 , 0.5812 , 0.2254 , 0.9768 , 0.6764 , 0.0000 , 0.8259 , 0.3568 , 0.1093 , 0.5615 , 0.9944 , 1.0000 , 0.9221 ,
    0.4643 , 0.9773 , 0.2573 , 0.6052 , 0.5132 , 0.3925 , 0.8403 , 0.2556 , 0.6755 , 0.9646 , 0.6107 , 0.6290 , 0.9995 , 0.9053 , 0.6015 , 0.2167 , 0.9831 , 0.7640 , 0.3831 , 0.9624 , 0.5432 , 0.7773 , 0.9854 , 0.1290 , 0.7219 , 0.0000 , 0.4962 , 1.0000 , 0.9529 , 0.8902 ,
    0.8392 , 0.2911 , 0.5729 , 0.6808 , 1.0000 , 0.0000 , 0.9804 , 0.9309 , 0.8132 , 0.2985 , 0.9959 , 0.7023 , 0.1665 , 0.9820 , 0.9611 , 0.9695 , 0.9940 , 0.7231 , 0.9875 , 0.9847 , 0.9833 , 0.0423 , 0.3779 , 1.0000 , 0.9818 , 0.1319 , 0.7792 , 0.7874 , 0.5504 , 0.8068 ,
    0.0923 , 0.4156 , 0.9991 , 0.3018 , 0.6945 , 0.9790 , 0.5274 , 0.3705 , 0.9060 , 0.3848 , 0.2635 , 0.7944 , 0.9319 , 0.1523 , 0.6732 , 0.9987 , 0.6570 , 0.7840 , 0.5783 , 0.2644 , 0.9823 , 0.6803 , 0.0000 , 0.8323 , 0.3152 , 0.0819 , 0.5600 , 0.9904 , 1.0000 , 0.9269 ,
    0.4745 , 0.9712 , 0.2298 , 0.5804 , 0.5135 , 0.4054 , 0.8135 , 0.2315 , 0.6897 , 0.9667 , 0.6033 , 0.6272 , 0.9996 , 0.8945 , 0.5796 , 0.2509 , 0.9793 , 0.7718 , 0.3958 , 0.9493 , 0.5623 , 0.7604 , 0.9782 , 0.1515 , 0.7280 , 0.0000 , 0.5068 , 1.0000 , 0.9521 , 0.8826 ,
    0.8193 , 0.3346 , 0.5583 , 0.6688 , 0.9992 , 0.0000 , 0.9713 , 0.9273 , 0.8094 , 0.2784 , 0.9912 , 0.6785 , 0.1899 , 0.9791 , 0.9519 , 0.9759 , 0.9959 , 0.7111 , 0.9836 , 0.9856 , 0.9787 , 0.0841 , 0.3853 , 1.0000 , 0.9828 , 0.1427 , 0.7812 , 0.7804 , 0.5601 , 0.8091 ,
    0.1336 , 0.4127 , 1.0000 , 0.2862 , 0.6853 , 0.9719 , 0.5553 , 0.3441 , 0.9183 , 0.3562 , 0.2538 , 0.8151 , 0.9400 , 0.1874 , 0.6451 , 0.9989 , 0.6797 , 0.7984 , 0.6096 , 0.2727 , 0.9853 , 0.6783 , 0.0000 , 0.8268 , 0.3473 , 0.0921 , 0.5300 , 0.9916 , 1.0000 , 0.9152 ,
    0.4423 , 0.9752 , 0.2348 , 0.5762 , 0.5152 , 0.4091 , 0.8082 , 0.1732 , 0.6471 , 0.9580 , 0.5643 , 0.6132 , 0.9981 , 0.8839 , 0.5666 , 0.2568 , 0.9762 , 0.7689 , 0.3468 , 0.9525 , 0.5395 , 0.7315 , 0.9740 , 0.0988 , 0.6979 , 0.0000 , 0.4776 , 1.0000 , 0.9399 , 0.8799 ,
    0.8954 , 0.2732 , 0.6190 , 0.6530 , 0.9984 , 0.0069 , 0.9928 , 0.9635 , 0.7588 , 0.3159 , 0.9957 , 0.6834 , 0.1388 , 0.9858 , 0.9605 , 0.9806 , 0.9987 , 0.8168 , 0.9966 , 0.9826 , 0.9838 , 0.0000 , 0.3310 , 1.0000 , 0.9896 , 0.1871 , 0.8335 , 0.7811 , 0.5775 , 0.7809 ,
    0.0814 , 0.3681 , 0.9991 , 0.3109 , 0.6859 , 0.9566 , 0.5536 , 0.3692 , 0.9182 , 0.3675 , 0.2801 , 0.7932 , 0.9376 , 0.1985 , 0.6255 , 0.9975 , 0.6825 , 0.8025 , 0.6021 , 0.2584 , 0.9762 , 0.6367 , 0.0000 , 0.8114 , 0.3176 , 0.0913 , 0.5173 , 0.9924 , 1.0000 , 0.9110 ,
    0.4560 , 0.9696 , 0.2516 , 0.5631 , 0.5082 , 0.4364 , 0.8259 , 0.2053 , 0.6172 , 0.9646 , 0.5840 , 0.5998 , 0.9990 , 0.8866 , 0.5357 , 0.2853 , 0.9676 , 0.7619 , 0.3324 , 0.9491 , 0.5146 , 0.7178 , 0.9658 , 0.1473 , 0.6784 , 0.0000 , 0.4886 , 1.0000 , 0.9251 , 0.8668 ,
    0.8784 , 0.2049 , 0.5842 , 0.6451 , 0.9990 , 0.0000 , 0.9851 , 0.9563 , 0.7540 , 0.2628 , 0.9910 , 0.6762 , 0.1106 , 0.9810 , 0.9653 , 0.9687 , 0.9971 , 0.7840 , 0.9904 , 0.9844 , 0.9860 , 0.0070 , 0.3070 , 1.0000 , 0.9800 , 0.1727 , 0.8304 , 0.7894 , 0.5625 , 0.7743 ,
    0.0818 , 0.3976 , 1.0000 , 0.2777 , 0.6685 , 0.9447 , 0.5578 , 0.3809 , 0.9246 , 0.3908 , 0.2996 , 0.7906 , 0.9492 , 0.2059 , 0.5965 , 0.9962 , 0.7057 , 0.8120 , 0.5882 , 0.2844 , 0.9682 , 0.6138 , 0.0000 , 0.7877 , 0.2707 , 0.0985 , 0.4925 , 0.9904 , 1.0000 , 0.9131 ,
    0.4526 , 0.9704 , 0.2348 , 0.5362 , 0.5166 , 0.4254 , 0.8301 , 0.2241 , 0.6048 , 0.9671 , 0.5571 , 0.5947 , 0.9987 , 0.8927 , 0.5524 , 0.3230 , 0.9796 , 0.7792 , 0.2961 , 0.9598 , 0.5149 , 0.7216 , 0.9623 , 0.1749 , 0.6731 , 0.0000 , 0.4950 , 1.0000 , 0.9236 , 0.8898 ,
    0.8617 , 0.2463 , 0.5693 , 0.6697 , 1.0000 , 0.0000 , 0.9807 , 0.9510 , 0.7743 , 0.2625 , 0.9923 , 0.6579 , 0.1169 , 0.9782 , 0.9695 , 0.9658 , 0.9951 , 0.7929 , 0.9850 , 0.9904 , 0.9860 , 0.0179 , 0.3218 , 1.0000 , 0.9748 , 0.1987 , 0.8109 , 0.7774 , 0.5669 , 0.7683 ,
    0.0157 , 0.3276 , 0.9967 , 0.2474 , 0.6153 , 0.9371 , 0.5203 , 0.3732 , 0.9146 , 0.3608 , 0.2604 , 0.7839 , 0.9427 , 0.1491 , 0.5433 , 0.9909 , 0.6817 , 0.7803 , 0.5726 , 0.2784 , 0.9675 , 0.5951 , 0.0000 , 0.7810 , 0.2801 , 0.1058 , 0.4311 , 0.9902 , 1.0000 , 0.9000 ,
    0.4351 , 0.9796 , 0.2356 , 0.6120 , 0.4643 , 0.3451 , 0.8296 , 0.2317 , 0.6667 , 0.9620 , 0.5733 , 0.6581 , 1.0000 , 0.9301 , 0.6279 , 0.1859 , 0.9812 , 0.7617 , 0.3809 , 0.9722 , 0.5416 , 0.7913 , 0.9928 , 0.0550 , 0.6829 , 0.0000 , 0.4182 , 1.0000 , 0.9647 , 0.8893 ,
    0.8609 , 0.2922 , 0.5617 , 0.6561 , 1.0000 , 0.0000 , 0.9750 , 0.9446 , 0.7958 , 0.2767 , 0.9919 , 0.6476 , 0.1581 , 0.9791 , 0.9584 , 0.9659 , 0.9964 , 0.7655 , 0.9767 , 0.9887 , 0.9778 , 0.0689 , 0.3338 , 0.9999 , 0.9738 , 0.1948 , 0.8009 , 0.7814 , 0.5345 , 0.7900 ,
    0.0213 , 0.2836 , 0.9966 , 0.1979 , 0.6196 , 0.9420 , 0.4754 , 0.3649 , 0.8976 , 0.3886 , 0.2267 , 0.7634 , 0.9299 , 0.1215 , 0.5342 , 0.9923 , 0.7039 , 0.7816 , 0.5968 , 0.2213 , 0.9609 , 0.5653 , 0.0000 , 0.7780 , 0.2674 , 0.1451 , 0.4401 , 0.9882 , 1.0000 , 0.8832 ,
    0.4145 , 0.9710 , 0.2212 , 0.6222 , 0.4858 , 0.3801 , 0.8168 , 0.2393 , 0.6393 , 0.9551 , 0.5869 , 0.6251 , 0.9994 , 0.9084 , 0.5953 , 0.1630 , 0.9764 , 0.7508 , 0.4024 , 0.9622 , 0.5413 , 0.7690 , 0.9851 , 0.0695 , 0.6988 , 0.0000 , 0.4487 , 1.0000 , 0.9488 , 0.8803 ,
    0.8521 , 0.2739 , 0.5391 , 0.6821 , 1.0000 , 0.0000 , 0.9734 , 0.9363 , 0.8173 , 0.3000 , 0.9927 , 0.6827 , 0.2013 , 0.9829 , 0.9657 , 0.9614 , 0.9954 , 0.7470 , 0.9806 , 0.9839 , 0.9770 , 0.0910 , 0.3800 , 0.9984 , 0.9739 , 0.1816 , 0.7823 , 0.7906 , 0.5222 , 0.8023
};
*/



const unsigned int numActKEM =  6;
const unsigned int numPolKEM = 10;

const vector<string> pNamesKEM = {
    "00", "01", "02", "03", "04", "05"
};

const vector<string> aNamesKEM = {
    "PA00", "PA01", "PA02", "PA03", "PA04", "PA05"
};

const vector<string> aDescKEM = {
    "Policy Actor 00", "Policy Actor 01", "Policy Actor 02", "Policy Actor 03",
    "Policy Actor 04", "Policy Actor 05"
};

const double weightArray[] = {
    85.0 ,
    22.9 ,
    19.8 ,
    73.8 ,
    25.4 ,
    29.2
};


const double utilArray[] = {
    0.1491 , 0.4289 , 0.9973 , 0.2777 , 0.6942 , 0.9797 , 0.5568 , 0.3547 , 0.9138 , 0.3834 ,
    0.4643 , 0.9773 , 0.2573 , 0.6052 , 0.5132 , 0.3925 , 0.8403 , 0.2556 , 0.6755 , 0.9646 ,
    0.8392 , 0.2911 , 0.5729 , 0.6808 , 1.0000 , 0.0000 , 0.9804 , 0.9309 , 0.8132 , 0.2985 ,
    0.0923 , 0.4156 , 0.9991 , 0.3018 , 0.6945 , 0.9790 , 0.5274 , 0.3705 , 0.9060 , 0.3848 ,
    0.4745 , 0.9712 , 0.2298 , 0.5804 , 0.5135 , 0.4054 , 0.8135 , 0.2315 , 0.6897 , 0.9667 ,
    0.8193 , 0.3346 , 0.5583 , 0.6688 , 0.9992 , 0.0000 , 0.9713 , 0.9273 , 0.8094 , 0.2784
};




// --------------------------------------------
void demoEKem(uint64_t s);


} // end of namespace eModKEM


#endif
// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
