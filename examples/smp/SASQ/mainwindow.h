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

    //recentfile history
    QAction *separatorAct;

    enum { maxRecentFilesCount = 5 };
    QAction *recentFileActs[maxRecentFilesCount];

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
    void runModel(bool bl);
    void displayMessage(QString cls, QString message);


    //GUI Initialization
private :
    QFrame * centralMainFrame;
    QGridLayout * centralFrameGridLayout;
    QStackedWidget * sasStackedWidget;
    QFrame * modelFrame;
    QFrame * actorFrame;
    QFrame * specificationsFrame;
    QFrame * navigationFrame;

    void intializeGUI();
    void modelFrameInitialization();
    void actorFrameInitialization();
    void specsFrameInitialization();

    //navigation
    QPushButton * modelPushButton;
    QPushButton * actorPushButton;
    QPushButton * specsPushButton;

private slots :
    void modelNaviClicked();
    void actorNaviClicked();
    void specsNaviClicked();
    void setCsvItemModel(QStandardItemModel*, QStringList scenarioList);
    void openStatusXml(bool status);
    void xmlDataParsedFromFile(QStringList modelDesc,QStringList modelParametes,
                               QStringList dimensionNames,QStandardItemModel* xmlModel,
                               QList<QStringList>idealAdjustmentList);

private :
    ModelFrame * modelFrameObj;
    ActorFrame * actorFrameObj;
    CSV * csvParserObj;
    Xmlparser * xmlParserObj;
signals:
    void csvFilePath(QString);
    void setActorModel(QStandardItemModel *,QStringList);
    void readXMLFile();
    void openXMLFile(QString file);
    void setAccomodationTableModel(QStandardItemModel *actModel,
                                   QList<QStringList> idealAdjustmentList,QStringList dims);
};



#endif // MAINWINDOW_H


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
