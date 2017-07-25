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
#ifndef MODELWINDOW_H
#define MODELWINDOW_H


#include <QFrame>
#include <QObject>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QtGui>
#include <QComboBox>
#include <QSplitter>
#include <QCheckBox>
#include <QPushButton>
#include <QListView>
#include <QMenu>

class ModelFrame: public QFrame
{
    Q_OBJECT

public:
    explicit ModelFrame(QObject *parent = Q_NULLPTR);
    ~ModelFrame();

    QGridLayout * frameMainLayout;

    QComboBox * victProbModelComboBox ;
    QComboBox * pCEModelComboBox ;
    QComboBox * stateTransitionsComboBox ;
    QComboBox * votingRuleComboBox ;
    QComboBox * bigRAdjustComboBox ;
    QComboBox * bigRRangeComboBox ;
    QComboBox * thirdPartyCommitComboBox ;
    QComboBox * interVecBrgnComboBox ;
    QComboBox * bargnModelComboBox ;

    QStringList modelParametersList;
    QStringList victList;
    QStringList pceList;
    QStringList stateList;
    QStringList votingList;
    QStringList bigAdjList;
    QStringList bigRangeList;
    QStringList thridpartyList;
    QStringList interVecList;
    QStringList bargnList;
    QVector <QStringList>  modelParameters;

    QComboBox * parameterName;
    QFrame * parmetersFrame;
    QCheckBox * addAllCheckBox;

    QFrame * modelParaFrame;
    QFrame * modelControlsFrame ;
    QFrame * modelSpecsFrame;

    QGridLayout * modelParaGridLayout ;
    QGridLayout * modelControlsGridLayout ;
    QGridLayout * modelSpecsGridLayout ;

    QGridLayout *parametersGLayout;
    QGridLayout * controlsLayout ;

    QListView * specsListView;
    QStandardItemModel *specsListModel;

    void initializeModelParameters();
    void initializeModelControls();
    void initializeModelSpecifications();

    using DataValues = QVector <QString>;
    using SpecsData = QVector <DataValues>;
    using SpecificationVector = QVector<SpecsData>;
    using Specifications = QVector<SpecificationVector>;

    QVector <QCheckBox *> parametersCheckBoxList;
    DataValues modelParaLHS;
    SpecsData modelParaRHS = SpecsData();

private :
    void initializeFrameLayout();

signals:
    void modelList(QStandardItemModel *,  QPair<DataValues,SpecsData> );


private slots:
    void parameterChanged(int index);
    void addAllClicked(bool bl);
    void addSpecClicked(bool bl);
    void listViewClicked();
     void modelListViewContextMenu(QPoint pos);

public slots:
    void listViewRemoveAllClicked();

};




#endif // MODELWINDOW_H
