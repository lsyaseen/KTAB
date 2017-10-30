#ifndef RUNMODEL_H
#define RUNMODEL_H

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

#include "smp.h"
#include "demosmp.h"
#include <functional>
#include <QDebug>
#include <QFileDialog>
#include <QDateTime>
#include <QStringList>

#include <QMessageBox>
#include <QMenu>
#include <QtGlobal>
#include <easylogging++.h>

// Copied this code snippet from demosmp.cpp

using KBase::PRNG;
using KBase::KMatrix;
using KBase::Actor;
using KBase::Model;
using KBase::Position;
using KBase::State;
using KBase::VotingRule;
using KBase::VPModel;

namespace DemoSMP {

using std::function;
using std::get;
using std::string;
using std::vector;

using KBase::ReportingLevel;

using SMPLib::SMPModel;
using SMPLib::SMPActor;
using SMPLib::SMPState;

// -------------------------------------------------
// this binds the given parameters and returns the λ-fn necessary to stop the SMP appropriately
//function<bool(unsigned int, const State *)>
//smpStopFn(unsigned int minIter, unsigned int maxIter, double minDeltaRatio, double minSigDelta) {
//    auto  sfn = [minIter, maxIter, minDeltaRatio, minSigDelta](unsigned int iter, const State * s) {
//        bool tooLong = (maxIter <= iter);
//        bool longEnough = (minIter <= iter);
//        bool quiet = false;
//        auto sf = [](unsigned int i1, unsigned int i2, double d12) {
//            return KBase::getFormattedString("sDist [%2i,%2i] = %.2E   ", i1, i2, d12);;
//        };
//        auto s0 = ((const SMPState*)(s->model->history[0]));
//        auto s1 = ((const SMPState*)(s->model->history[1]));
//        auto d01 = SMPModel::stateDist(s0, s1) + minSigDelta;
//        string logMsg;
//        logMsg += sf(0, 1, d01);
//        auto sx = ((const SMPState*)(s->model->history[iter - 0]));
//        auto sy = ((const SMPState*)(s->model->history[iter - 1]));
//        auto dxy = SMPModel::stateDist(sx, sy);
//        logMsg += sf(iter - 1, iter - 0, dxy);
//        LOG(INFO) << logMsg;
//        const double aRatio = dxy / d01;
//        quiet = (aRatio < minDeltaRatio);
//        LOG(INFO) << KBase::getFormattedString(
//                         "Fractional change compared to first step: %.4f  (target=%.4f)\n",
//                         aRatio, minDeltaRatio);
//        return tooLong || (longEnough && quiet);
//    };
//    return sfn;
//};


} // end of namespace



class RunModel : public QObject
{
    Q_OBJECT

public:
    RunModel();
    ~RunModel();

public slots:
    void runSMPModel(QStringList fileNames);


private:
    QString connectionString;
    QString dbPath;
    QString configureDbRun();
    void runModel(QString conStr, QString fileName);
};

#endif // RUNMODEL_H
