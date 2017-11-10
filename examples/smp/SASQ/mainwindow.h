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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modelwindow.h"
#include "actorwindow.h"
#include "specswindow.h"
#include "csv.h"
#include "xmlparser.h"
#include "runmodel.h"
#include <easylogging++.h>


#include <QMainWindow>
#include <QtCore>
#include <QObject>
#include <QtGlobal>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QFrame>
#include <QStackedWidget>
#include <QLayout>
#include <QLabel>
#include <QStatusBar>

class QTextEdit;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

public slots:
    void updateStatusMessage(QString message);


private :
    void createActions();
    void createStatusBar();
    void createConnections();

private slots:
    void importDataFileCSVXML(bool bl);
    void saveClicked(bool bl);

private :
    QMenu *fileMenu;
    QString homeDirectory;
    QString defaultDirectory;
    QSettings recentFileSettings;
    QString fileNameM;

    //recentfile history
    QAction *separatorAct;

    enum { maxRecentFilesCount = 5 };
    QAction *recentFileActs[maxRecentFilesCount];

    using DataValues = QVector <QString>;
    using SpecsData = QVector <DataValues>;
    using SpecificationVector = QVector<SpecsData>;
    using Specifications = QVector<SpecificationVector>;

    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);
    void loadRecentFile(const QString &fileName);
    void intializeHomeDirectory();

private slots:
    void openRecentFile();
    void clearRecentFile(bool bl);
    void changeHomeDirectory(bool bl);
    void about();
    void clearSpecifications(bool bl);
    void runSpecModel(bool bl);
    void displayMessage(QString cls, QString message);
    void logMinimumStatus(bool bl);


    //GUI Initialization
private :
    QFrame * centralMainFrame= nullptr;
    QGridLayout * centralFrameGridLayout= nullptr;
    QStackedWidget * sasStackedWidget= nullptr;
    QFrame * modelFrame= nullptr;
    QFrame * actorFrame= nullptr;
    QFrame * specificationsFrame= nullptr;
    QFrame * navigationFrame= nullptr;

    void intializeGUI();
    void modelFrameInitialization();
    void actorFrameInitialization();
    void specsFrameInitialization();

    //navigation
    QPushButton * modelPushButton= nullptr;
    QPushButton * actorPushButton= nullptr;
    QPushButton * specsPushButton= nullptr;

private slots :
    void modelNaviClicked();
    void actorNaviClicked();
    void specsNaviClicked();
    void setCsvItemModel(QStandardItemModel*, QStringList scenarioList);
    void openStatusXml(bool status);
    void xmlDataParsedFromFile(QStringList modelDesc,QStringList modelParametes,
                               QStringList dimensionNames,QStandardItemModel* xmlModel,
                               QVector<QStringList>idealAdjustmentList);

private :
    ModelFrame * modelFrameObj= nullptr;
    ActorFrame * actorFrameObj= nullptr;
    SpecificationFrame * specsFrameObj= nullptr;
    CSV * csvParserObj= nullptr;
    Xmlparser * xmlParserObj= nullptr;
    RunModel * runSmp=nullptr;

    QStandardItemModel * actorListModel= nullptr;
    QStandardItemModel * modelListModel= nullptr;
    QStandardItemModel * filterListModel= nullptr;
    QStandardItemModel * crossProductListModel= nullptr;
    QStandardItemModel * combinedSASModel= nullptr;

    QPair<DataValues,SpecsData>  modelSpecs;
    QPair<DataValues,SpecsData> actorSpecs;
    QPair<SpecsData,SpecificationVector> filterSpecs ;
    QPair<SpecsData,SpecificationVector> crossProductSpecs;

    int specsIndexlist[4]= {0};

    DataValues modelSpecificationsLHS ;
    DataValues actorSpecificationsLHS ;
    SpecsData filterSpecificationsLHS;
    SpecsData crossProdSpecificationsLHS;

    SpecsData modelSpecificationsRHS ;
    SpecsData actorSpecificationsRHS ;
    SpecificationVector filterSpecificationsRHS;
    SpecificationVector crossProdSpecificationsRHS;

    QStandardItemModel *actorDataModel;
    QStandardItemModel *actorAccModel;
    QStringList actorScenarioList;

    QVector<int> specType; //0 - ModelParameter, 1 - ActorData, 2 - ActorAccData

    QVector<int> modRowIndices;
    QVector<int> modColIndices;

    QVector<int> rowIndices;
    QVector<int> columnIndices;

    QVector<int> accRowIndices;
    QVector<int> accColumnIndices;

    QStringList modelDescNames;
    QStringList modelParams;
    QStringList modelDimNames;

    int dummy= 0;
    QStandardItemModel * updatedDataModel;
    QStandardItemModel * updatedAccModel;

    QStringList runFileNamesList;

    QLineEdit * seedLineEdit;
    bool logMin = true;
    QString seedVal;


    QActionGroup * logActions;

    QAction *logDefaultAct;
    QAction *logNewAct;
    QAction *logNoneAct;

signals:
    void csvFilePath(QString);
    void setActorModel(QStandardItemModel *,QStringList);
    void readXMLFile();
    void openXMLFile(QString file);
    void setAccomodationTableModel(QStandardItemModel *actModel,
                                   QVector<QStringList> idealAdjustmentList,
                                   QStringList dims,QStringList desc);

    void getFinalSpecs();
    void getActorDataModel();
    void runSmpModelXMLFiles(QStringList fileNames);
    void generateXMLFile(QStringList parameters,QStandardItemModel *smpData, QStandardItemModel *affMatrix, QString filename);

private slots:
    void finalSpecificationsListModel(DataValues modelLhs,SpecsData modelRhs,int specCount);
    void finalSpecificationsListActor(DataValues actorLhs,SpecsData actorRhs,int specCount);
    void finalSpecificationsListFilter(SpecsData filterLhs,SpecificationVector filterRhs,int specCount);
    void finalSpecificationsListCrossProduct(SpecsData crossProdLhs,SpecificationVector crossProdRhs,int specCount);
    void dataAccModel(QStandardItemModel *dataModel,QStandardItemModel *accModel, QStringList scenarioList);

private:
    void updateModelwithSpecChanges();
    void getRowColumnIndicesModelActor(QString lhsList);
    void getRowColumnIndicesFilterCrossProd(QVector<QString> lhsList);
    void generateModelforScenZ();
    void updateDataModelRowColumn(QVector<QString> rhsList);
    void updateFilterCrossProdRowColumn(QVector<QString> rhsList);
    void saveSpecsToFile(int specTypeIndex);

    //    void logSMPDataOptionsAnalysis();
};

#endif // MAINWINDOW_H


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
