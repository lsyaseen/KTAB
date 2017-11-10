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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow()
{

    setWindowTitle(tr("KTAB: Sensitivity Analysis Specification for SMP (SAS)"));

    createActions();
    createStatusBar();
    intializeHomeDirectory();
    intializeGUI();
    createConnections();

    runSmp = new RunModel;
    updatedDataModel= new QStandardItemModel();
    updatedAccModel = new QStandardItemModel();
}

MainWindow::~MainWindow()
{
    delete runSmp;
    delete csvParserObj;
    delete xmlParserObj;
    delete actorFrameObj;
    delete modelFrameObj;
    delete specsFrameObj;

    delete updatedDataModel;
    delete updatedAccModel;

}

void MainWindow::updateStatusMessage(QString message)
{
    statusBar()->showMessage(message);
}

void MainWindow::createConnections()
{
    csvParserObj = new CSV;
    xmlParserObj = new Xmlparser(homeDirectory);

    //connect statement to pass the csv file path
    connect(this,SIGNAL(csvFilePath(QString)), csvParserObj, SLOT(readCSVFile(QString)));
    //connect statement to receive processed model
    connect(csvParserObj,SIGNAL(csvModel(QStandardItemModel*,QStringList)),
            this,SLOT(setCsvItemModel(QStandardItemModel*,QStringList)));
    //To display any message to user
    connect(csvParserObj,SIGNAL(sendMessage(QString,QString)),this,SLOT(displayMessage(QString,QString)));

    //setActor Table with csv data
    connect(this,SIGNAL(setActorModel(QStandardItemModel*,QStringList))
            ,actorFrameObj,SLOT(setActorTableModel(QStandardItemModel*,QStringList)));

    connect(this,SIGNAL(openXMLFile(QString)),xmlParserObj,SLOT(openXmlFile(QString)));
    connect(this,SIGNAL(readXMLFile()),xmlParserObj,SLOT(readXmlFile()));
    connect(xmlParserObj,SIGNAL(openXMLStatus(bool)),this,SLOT(openStatusXml(bool)));
    connect(xmlParserObj,SIGNAL(xmlParsedData(QStringList,QStringList,QStringList,QStandardItemModel*,
                                              QVector<QStringList>)),this,
            SLOT(xmlDataParsedFromFile(QStringList,QStringList,QStringList,QStandardItemModel*,
                                       QVector<QStringList>)));

    connect(this,SIGNAL(setAccomodationTableModel(QStandardItemModel*,QVector<QStringList>,
                                                  QStringList,QStringList)),actorFrameObj,
            SLOT(setAccTableModel(QStandardItemModel*,QVector<QStringList>,QStringList,QStringList)));

    connect(this,SIGNAL(saveXMLDataToFile(QStringList,QStandardItemModel*,QStandardItemModel*,QString)),
            xmlParserObj,SLOT(saveToXmlFile(QStringList,QStandardItemModel*,QStandardItemModel*,QString)));
    connect(xmlParserObj,SIGNAL(newXmlFilePath(QString)),this,SLOT(savedXmlName(QString)));
    connect(this,SIGNAL(saveNewSMPDataToXMLFile(QStringList,QTableWidget*,QTableWidget*)),
            xmlParserObj,SLOT(saveNewDataToXmlFile(QStringList,QTableWidget*,QTableWidget*)));
    connect(this,SIGNAL(homeDirChanged(QString)),xmlParserObj,SLOT(updateHomeDir(QString)));

    //    generateXMLFile
    connect(this,SIGNAL(generateXMLFile(QStringList,QStandardItemModel*,QStandardItemModel*,QString)),
            xmlParserObj,SLOT(saveToXmlFile(QStringList,QStandardItemModel*,QStandardItemModel*,QString)));

}

void MainWindow::intializeGUI()
{
    centralMainFrame = new QFrame;
    setCentralWidget(centralMainFrame);
    centralFrameGridLayout = new QGridLayout;

    sasStackedWidget = new QStackedWidget; //holds model, actor & specs screens
    modelFrame = new QFrame(sasStackedWidget);
    actorFrame = new QFrame(sasStackedWidget);
    specificationsFrame = new QFrame(sasStackedWidget);

    modelFrameInitialization(); // 1st Window
    actorFrameInitialization(); // 2nd Window
    specsFrameInitialization(); // 3rd window

    sasStackedWidget->addWidget(modelFrame);
    sasStackedWidget->addWidget(actorFrame);
    sasStackedWidget->addWidget(specificationsFrame);

    navigationFrame = new QFrame;

    modelPushButton = new QPushButton("Model",navigationFrame);
    actorPushButton = new QPushButton("Actor",navigationFrame);
    specsPushButton = new QPushButton("Specification",navigationFrame);

    modelPushButton->setFixedWidth(200);
    modelPushButton->setFixedHeight(30);
    actorPushButton->setFixedWidth(200);
    actorPushButton->setFixedHeight(30);
    specsPushButton->setFixedWidth(200);
    specsPushButton->setFixedHeight(30);
    modelPushButton->setStyleSheet( "QPushButton {background-color: rgb(50,205,50);}"
                                    "QPushButton:hover{background-color:green;}");
    actorPushButton->setStyleSheet("QPushButton:hover{background-color:green;}");
    specsPushButton->setStyleSheet("QPushButton:hover{background-color:green;}");

    connect(modelPushButton,SIGNAL(clicked(bool)),this,SLOT(modelNaviClicked()));
    connect(actorPushButton,SIGNAL(clicked(bool)),this,SLOT(actorNaviClicked()));
    connect(specsPushButton,SIGNAL(clicked(bool)),this,SLOT(specsNaviClicked()));

    QHBoxLayout * navHBoxLayout = new QHBoxLayout;
    navHBoxLayout->addWidget(modelPushButton,0,Qt::AlignRight);
    navHBoxLayout->addWidget(actorPushButton,0,Qt::AlignCenter);
    navHBoxLayout->addWidget(specsPushButton,0,Qt::AlignLeft);
    navHBoxLayout->setContentsMargins(1,1,1,1);
    navigationFrame->setLayout(navHBoxLayout);

    centralFrameGridLayout->addWidget(sasStackedWidget,0,0);
    centralFrameGridLayout->addWidget(navigationFrame,1,0,Qt::AlignBottom);

    centralMainFrame->setLayout(centralFrameGridLayout);

    sasStackedWidget->setCurrentIndex(0);
}

void MainWindow::modelFrameInitialization()
{
    modelFrameObj= new ModelFrame(sasStackedWidget);
    sasStackedWidget->addWidget(modelFrameObj);
    modelListModel= new QStandardItemModel;
}

void MainWindow::actorFrameInitialization()
{
    actorFrameObj= new ActorFrame(sasStackedWidget);
    sasStackedWidget->addWidget(actorFrameObj);
    actorListModel= new QStandardItemModel;

    connect(this,SIGNAL(getActorDataModel()),actorFrameObj,SLOT(getDataModel()));
    connect(actorFrameObj,SIGNAL(dataModel(QStandardItemModel*,QStandardItemModel*,QStringList)),
            this,SLOT(dataAccModel(QStandardItemModel*,QStandardItemModel*,QStringList)));


}

void MainWindow::specsFrameInitialization()
{
    specsFrameObj= new SpecificationFrame(sasStackedWidget);
    sasStackedWidget->addWidget(specsFrameObj);
    connect(actorFrameObj,SIGNAL(actorAttributesAndSAS(QString)),
            specsFrameObj,SLOT(actorAtrributesSAS(QString)));
    connect(specsFrameObj,SIGNAL(getActorAttributeheaders()),actorFrameObj,SLOT(actorAtrributesHeaderList()));
    connect(actorFrameObj,SIGNAL(actorHeaderAttributes(QStringList,QStringList)),specsFrameObj
            ,SLOT(actorNameAttributes(QStringList,QStringList)));

    //add spec
    connect(modelFrameObj,SIGNAL(specificationNew(QString,QPair<DataValues,SpecsData>,int)),
            specsFrameObj,SLOT(actorModelSpecification(QString,QPair<DataValues,SpecsData>,int)));
    connect(actorFrameObj,SIGNAL(specificationNew(QString,QPair<DataValues,SpecsData>,int)),
            specsFrameObj,SLOT(actorModelSpecification(QString,QPair<DataValues,SpecsData>,int)));

    //remove spec
    connect(modelFrameObj,SIGNAL(removeSpecificationModel(int,int,QString)),specsFrameObj,SLOT(removeSpecModelActor(int,int,QString)));
    connect(actorFrameObj,SIGNAL(removeSpecificationActor(int,int,QString)),specsFrameObj,SLOT(removeSpecModelActor(int,int,QString)));

    //status Message
    connect(specsFrameObj,SIGNAL(statusMessage(QString)),SLOT(updateStatusMessage(QString)));

    filterListModel = new QStandardItemModel;
    crossProductListModel = new QStandardItemModel;

    //get final Spec List
    connect(this,SIGNAL(getFinalSpecs()),specsFrameObj,SLOT(getFinalSpecificationsList()));
    connect(specsFrameObj,SIGNAL(finalSpecListModel(DataValues,SpecsData,int)),
            this,SLOT(finalSpecificationsListModel(DataValues,SpecsData,int)));
    connect(specsFrameObj,SIGNAL(finalSpecListActor(DataValues,SpecsData,int)),
            this,SLOT(finalSpecificationsListActor(DataValues,SpecsData,int)));
    connect(specsFrameObj,SIGNAL(finalSpecListFilter(SpecsData,SpecificationVector,int)),
            this,SLOT(finalSpecificationsListFilter(SpecsData,SpecificationVector,int)));
    connect(specsFrameObj,SIGNAL(finalSpecListCrossProduct(SpecsData,SpecificationVector,int)),
            this,SLOT(finalSpecificationsListCrossProduct(SpecsData,SpecificationVector,int)));
}

void MainWindow::modelNaviClicked()
{
    modelPushButton->setStyleSheet( "QPushButton {background-color: rgb(50,205,50);}"
                                    "QPushButton:hover{background-color:green;}");
    actorPushButton->setStyleSheet("QPushButton:hover{background-color:green;}");
    specsPushButton->setStyleSheet("QPushButton:hover{background-color:green;}");

    sasStackedWidget->setCurrentIndex(0);//model window

}

void MainWindow::actorNaviClicked()
{
    actorPushButton->setStyleSheet( "QPushButton {background-color: rgb(50,205,50);}"
                                    "QPushButton:hover{background-color:green;}");
    modelPushButton->setStyleSheet("QPushButton:hover{background-color:green;}");
    specsPushButton->setStyleSheet("QPushButton:hover{background-color:green;}");

    sasStackedWidget->setCurrentIndex(1);//actor window

}

void MainWindow::specsNaviClicked()
{
    specsPushButton->setStyleSheet( "QPushButton {background-color: rgb(50,205,50);}"
                                    "QPushButton:hover{background-color:green;}");
    actorPushButton->setStyleSheet("QPushButton:hover{background-color:green;}");
    modelPushButton->setStyleSheet("QPushButton:hover{background-color:green;}");

    sasStackedWidget->setCurrentIndex(2);//specifiaction window

}

void MainWindow::setCsvItemModel(QStandardItemModel * model, QStringList scenarioList)
{
    emit setActorModel(model,scenarioList);
}

void MainWindow::createActions()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    QList <QKeySequence> seq;
    fileMenu->setToolTipsVisible(true);

    seq.clear();
    seq.append(Qt::Key_I | Qt::CTRL);
    const QIcon importIcon = QIcon::fromTheme("Import Data", QIcon("://images/import.png"));
    QAction *importFileAct = new QAction(importIcon, tr("Import data"), this);
    importFileAct->setShortcuts(seq);
    importFileAct->setStatusTip(tr("Import Data from CSV/XML File"));
    connect(importFileAct, SIGNAL(triggered(bool)), this,SLOT(importDataFileCSVXML(bool)));
    fileMenu->addAction(importFileAct);
    fileToolBar->addAction(importFileAct);

    seq.clear();
    seq.append(Qt::Key_X | Qt::CTRL);
    const QIcon clearIcon = QIcon::fromTheme("Clear Specifications", QIcon("://images/clear.png"));
    QAction *clearFileAct = new QAction(clearIcon, tr("Clear Specifications"), this);
    clearFileAct->setShortcuts(seq);
    clearFileAct->setStatusTip(tr("Clear Specifications"));
    connect(clearFileAct, SIGNAL(triggered(bool)), this,SLOT(clearSpecifications(bool)));
    fileMenu->addAction(clearFileAct);
    fileToolBar->addAction(clearFileAct);

    seq.clear();
    seq.append(Qt::Key_R | Qt::CTRL);
    const QIcon runIcon = QIcon::fromTheme("Run Model", QIcon("://images/run.png"));
    QAction *runFileAct = new QAction(runIcon, tr("Run "), this);
    runFileAct->setShortcuts(seq);
    runFileAct->setStatusTip(tr("Run Model"));
    connect(runFileAct, SIGNAL(triggered(bool)), this,SLOT(runSpecModel(bool)));
    fileMenu->addAction(runFileAct);
    fileToolBar->addAction(runFileAct);

    seq.clear();
    QCheckBox * logMinimumAct = new QCheckBox(" Log Minimum  ", this);
    logMinimumAct->setChecked(true);
    connect(logMinimumAct, SIGNAL(clicked(bool)), this,SLOT(logMinimumStatus(bool)));
    fileToolBar->addWidget(logMinimumAct);

    seedLineEdit=new QLineEdit(this);
    seedLineEdit->setPlaceholderText("Seed...");
    seedLineEdit->setMaximumWidth(200);
    fileToolBar->addWidget(seedLineEdit);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon("://images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("Save"), this);
    connect(saveAct, SIGNAL(triggered(bool)), this, SLOT(saveClicked(bool)));
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save"));
    fileMenu->addAction(saveAct);

    fileMenu->addSeparator();

    for (int i = 0; i < maxRecentFilesCount; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
    }

    separatorAct = fileMenu->addSeparator();
    for (int i = 0; i < maxRecentFilesCount; ++i)
    {
        fileMenu->addAction(recentFileActs[i]);
    }
    fileMenu->addSeparator();
    updateRecentFileActions();

    fileMenu->addSeparator();

    QAction *clearFileHistoryAct = new QAction(tr("Clear Recent File History"), this);
    clearFileHistoryAct->setToolTip("Clear Recently Open File History");
    clearFileHistoryAct->setStatusTip(tr("Clear Recently Open File History"));
    connect(clearFileHistoryAct, SIGNAL(triggered(bool)), this,SLOT(clearRecentFile(bool)));
    fileMenu->addAction(clearFileHistoryAct);
    fileMenu->addSeparator();

    fileMenu->addSeparator();

    QAction *changeHomeAct = new QAction(tr("Change Home Directory"), this);
    changeHomeAct->setToolTip("Change Home Directory");
    changeHomeAct->setStatusTip(tr("Change Home Directory"));
    connect(changeHomeAct, SIGNAL(triggered(bool)), this,SLOT(changeHomeDirectory(bool)));
    fileMenu->addAction(changeHomeAct);
    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("Quit ", QIcon("://images/exit.png"));
    QAction *quitAct = new QAction(exitIcon, tr("&Quit"), this);
    connect(quitAct, SIGNAL(triggered(bool)), this,SLOT(close()));
    fileMenu->addAction(quitAct);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    menuBar()->addSeparator();

    QMenu *optionMenu = menuBar()->addMenu(tr("&Log Options"));
    optionMenu->setToolTipsVisible(true);

    logActions = new QActionGroup(this);
    logActions->setExclusive(true);

    logDefaultAct =new QAction(tr("&Default"), this);
    logDefaultAct->setCheckable(true);
    optionMenu->addAction(logDefaultAct);
    logDefaultAct->setToolTip("Record the SMP model log in a timestamp-named file");
    logDefaultAct->setStatusTip(tr("Record the SMP model log in a timestamp-named file"));
    logActions->addAction(logDefaultAct);
    logDefaultAct->setChecked(true);

    logNewAct =new QAction(tr("&Custom"), this);
    logNewAct->setCheckable(true);
    optionMenu->addAction(logNewAct);
    logNewAct->setToolTip("Record the SMP model log in a specific file / location");
    logNewAct->setStatusTip(tr("Record the SMP model log in a specific file / location"));
    logActions->addAction(logNewAct);

    logNoneAct =new QAction(tr("&None"), this);
    logNoneAct->setCheckable(true);
    optionMenu->addAction(logNoneAct);
    logNoneAct->setToolTip(tr("Disable logging of the SMP model run"));
    logNoneAct->setStatusTip(tr("Disable logging of the SMP model run"));
    logActions->addAction(logNoneAct);

    menuBar()->addSeparator();
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct =new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    connect(aboutAct, SIGNAL(triggered(bool)),this,SLOT(about()));
    aboutAct->setStatusTip(tr("About SAS "));

}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        loadRecentFile(action->data().toString());
    }
}

void MainWindow::clearRecentFile(bool bl)
{
    recentFileSettings.remove("recentFileList");

    updateRecentFileActions();
}

void MainWindow::changeHomeDirectory(bool bl)
{
    QString dir = QFileDialog::getExistingDirectory(this,"Change Home Dir", QDir::homePath());

    if(!dir.isEmpty())
    {
        homeDirectory = dir;
        recentFileSettings.setValue("HomeDirectory",homeDirectory);
        defaultDirectory= homeDirectory;
    }
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    setWindowFilePath(fileName);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > maxRecentFilesCount)
        files.removeLast();

    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
        {
            mainWin->updateRecentFileActions();
        }
    }
}

void MainWindow::updateRecentFileActions()
{
    QStringList files = recentFileSettings.value("recentFileList").toStringList();
    recentFileSettings.value("recentFileList");

    int numRecentFiles = qMin(files.size(), (int)maxRecentFilesCount);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < maxRecentFilesCount; ++j)
    {
        recentFileActs[j]->setVisible(false);
    }

    separatorAct->setVisible(numRecentFiles > 0);

}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::loadRecentFile(const QString &fileName)
{
    QFileInfo checkFileValid(fileName);

    if (checkFileValid.exists() && checkFileValid.isFile())
    {
        QDir dir =QFileInfo(fileName).absoluteDir();
        homeDirectory = dir.absolutePath();
        //open the file by calling appropriate method

    }
    else
    {

        QMessageBox msgBox;
        msgBox.setText(QString(fileName+" not found !\n"
                               + "Do you want to remove from the recently accessed list ?"));
        QPushButton *yesButton = msgBox.addButton(tr("Yes"), QMessageBox::ActionRole);
        QPushButton *noButton = msgBox.addButton(QMessageBox::No);

        msgBox.exec();

        if (msgBox.clickedButton() == yesButton)
        {
            QStringList files = recentFileSettings.value("recentFileList").toStringList();
            recentFileSettings.remove("recentFileList");

            int index = files.indexOf(fileName);
            files.removeAt(index);

            recentFileSettings.setValue("recentFileList",files);

            updateRecentFileActions();
        }
        else if (msgBox.clickedButton() == noButton)
        {
            msgBox.close();
        }
    }

}

void MainWindow::intializeHomeDirectory()
{
    QString homeDir = recentFileSettings.value( "HomeDirectory" ).toString();
    if(!QDir(homeDir).exists() || homeDir.isEmpty())
    {
        homeDirectory=QDir::homePath().append(QDir::separator()).append("KTAB_SMP");

        if(!QDir(homeDirectory).exists())
        {
            if(!QDir(QDir::home()).mkdir("KTAB_SMP"))
            {
                displayMessage("Home Directory", "Unable to set Home directory as "+homeDirectory);
            }
        }
    }
    else if(!homeDir.isEmpty())
    {
        homeDirectory = homeDir;
    }

    recentFileSettings.setValue("HomeDirectory",homeDirectory);

    defaultDirectory= homeDirectory;
}

void MainWindow::displayMessage(QString cls, QString message)
{
    QMessageBox::warning(0,cls,message);
}

void MainWindow::logMinimumStatus(bool bl)
{
    logMin=bl;
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About KTAB SAS"),
                       tr("KTAB SAS\n\nVersion Beta 0.5\n\n"
                          "\n \n"
                          "More information can be found at http://kapsarc.github.io/KTAB/"
                          "\n \n"
                          "Copyright © KAPSARC"
                          "\n \n"
                          "KTAB is released as open source software under the terms of the MIT License (Expat)"
                          "\n \n"
                          "Please email comments, bug reports, and any feedback to ktab@kapsarc.org"));

}

void MainWindow::importDataFileCSVXML(bool bl)
{
    QString defaultFilter = tr("CSV (*.csv *.CSV)");
    QFileDialog *fd = new QFileDialog;
    QString fileName = fd->getOpenFileName(this,"Import File",homeDirectory,
                                           tr("CSV (*.csv *.CSV);;XML (*.xml *.XML)" ),&defaultFilter);
    if(!fileName.isEmpty())
    {
        QString ext = fd->selectedNameFilter();
        qDebug()<<fileName << ext;

        if(!(fileName.endsWith(".csv")||fileName.endsWith(".CSV")
             ||fileName.endsWith(".xml")||fileName.endsWith(".XML")))
        {
            displayMessage("Importing a File", "Please choose a File with extension as either .csv or .xml ");
        }
        else
        {
            fileNameM = fileName;
            if(fileName.endsWith(".csv")||fileName.endsWith(".CSV"))
            {
                emit csvFilePath(fileName);
            }
            else
            {
                emit openXMLFile(fileName);
            }
            actorNaviClicked();//Actor Attributes Window
        }
    }
}
void MainWindow::saveClicked(bool bl)
{

}

void MainWindow::runSpecModel(bool bl)
{
    runFileNamesList.clear();
    //Generate Seed
    using KBase::dSeed;
    uint64_t seed = dSeed;

    //Collect flags and Data from GUI
    if(seedLineEdit->text().length()>0 && !seedLineEdit->text().isNull())
    {
        seed = seedLineEdit->text().toULongLong();
    }
    else
    {
        seed=0;
    }

    if (0 == seed)
    {
        PRNG * rng = new PRNG();
        seed = rng->setSeed(seed); // 0 == get a random number
        delete rng;
        rng = nullptr;
    }
    seedVal = QString::number(seed);

    qDebug() <<  seed << "seed " << seedVal;


    //get final specs - Done
    emit getFinalSpecs();

    //get latest Data Model and Affinity Model from ActorObj
    emit getActorDataModel();

    //make a new local copy of thee model with changed spec values
    //Generate  XML files for the model
    updateModelwithSpecChanges();

    //DBfile location
    QDateTime UTC = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
    QString name (UTC.toString());

    name.replace(" ","_").replace(":","_");

    QFileDialog fileDialog;

    QString logFilePath="";
    if(logNoneAct->isChecked()==false)
    {
        logFilePath = fileDialog.getExistingDirectory(this, tr("Open Log Directory"),"/home",
                                                      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);


        fileDialog.close();
    }


    QString dbFilePath = fileDialog.getSaveFileName(this, tr("Save DB file as "),
                                                    QString(QDir::separator()+name),tr("DB File (*.db)"),
                                                    0,QFileDialog::DontConfirmOverwrite);

    QString logType;
    //log file type
    if(logDefaultAct->isChecked()==true)
    {
        logType="Default";
    }
    else if(logNewAct->isChecked()==true)
    {
        logType="New";
    }
    else
    {
        logType="None";
    }

    QApplication::processEvents();
    //Run SMP Model
    statusBar()->showMessage("Please wait ! SMP Model Run in progress This will take some time to complete !");
    runSmp->runSMPModel(runFileNamesList,logMin,seedVal,dbFilePath,logType,logFilePath);
    statusBar()->showMessage("Completed");
    qDebug()<<"runSmpModelXMLFiles";
    //clear
    dummy=0;
    //    runFileNamesList.clear();
    //    modelDimNames.clear();

    //    specType.clear(); //0 - ModelParameter, 1 - ActorData, 2 - ActorAccData

    //    modRowIndices.clear();
    //    modColIndices.clear();

    //    rowIndices.clear();
    //    columnIndices.clear();

    //    accRowIndices.clear();
    //    accColumnIndices.clear();



}
/*
void MainWindow::logSMPDataOptionsAnalysis()
{
    loggerConf.parseFromFile("./ktab-smp-logger.conf");
    // Disable all the logging to begin with
    loggerConf.set(el::Level::Global, el::ConfigurationType::Enabled, "false");
    el::Loggers::reconfigureAllLoggers(loggerConf);

    QDateTime UTC = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
    QString name("ktab-sas-");
    name.append(QString::number(UTC.date().year())).append("-");
    name.append(QString("%1").arg(UTC.date().month(), 2, 10, QLatin1Char('0'))).append("-");
    name.append(QString("%1").arg(UTC.date().day(), 2, 10, QLatin1Char('0'))).append("__");
    name.append(QString("%1").arg(UTC.time().hour(), 2, 10, QLatin1Char('0'))).append("-");
    name.append(QString("%1").arg(UTC.time().minute(), 2, 10, QLatin1Char('0'))).append("-");
    name.append(QString("%1").arg(UTC.time().second(), 2, 10, QLatin1Char('0')));
    name.append("_GMT");

    //    if(logDefaultAct->isChecked()==true)
    //    {
    //        if(logFileName.isEmpty())
    //        {
    QString logFileName = name.append("_log.txt");
    //        }

    loggerConf.setGlobally(el::ConfigurationType::Filename, logFileName.toStdString());
    // Enable all the logging
    loggerConf.set(el::Level::Global, el::ConfigurationType::Enabled, "true");
    el::Loggers::reconfigureAllLoggers(loggerConf);
    //    }
    //    else if(logNewAct->isChecked()==true)
    //    {
    //        fclose(stdout);
    //        fp_old = *stdout;
    //        QString logFileNewName = QString(homeDirectory+QDir::separator()+name);
    //        QString saveLogFilePath = QFileDialog::getSaveFileName(this, tr("Save Log File to "),logFileNewName,
    //                                                               tr("Text File (*.txt)"),0,QFileDialog::DontConfirmOverwrite);
    //        if(!saveLogFilePath.isEmpty())
    //        {
    //            if(saveLogFilePath.endsWith(".txt"))
    //            {
    //                saveLogFilePath.remove(".txt");
    //            }
    //            QDir dir =QFileInfo(saveLogFilePath).absoluteDir();
    //            homeDirectory = dir.absolutePath();

    //            logFileNewName=saveLogFilePath;

    //            std::string logFileName = logFileNewName.append("_log.txt").toStdString();
    //            loggerConf.setGlobally(el::ConfigurationType::Filename, logFileName);
    //            // Enable all the logging
    //            loggerConf.set(el::Level::Global, el::ConfigurationType::Enabled, "true");
    //            el::Loggers::reconfigureAllLoggers(loggerConf);
    //        }
    //    }
    //    else
    //    {
    //        if(logNoneAct->isChecked()==true)
    //        {
    //            // Disable all the logging
    //            loggerConf.set(el::Level::Global, el::ConfigurationType::Enabled, "false");

    //            el::Loggers::reconfigureAllLoggers(loggerConf);
    //        }
    //    }
}
*/



void MainWindow::clearSpecifications(bool bl)
{
    modelNaviClicked();//Model Attributes Window
    //clear Model Frame Specs
    modelFrameObj->listViewRemoveAllClicked();

    //clear Actor Frame Specs
    actorFrameObj->listViewRemoveAllClicked();
    //    actorFrameObj->clearModels();

    //clear Filter Specs
    specsFrameObj->filterListViewRemoveAllClicked();

    //clear CrossProduct Specs
    specsFrameObj->crossproductRemoveAllClicked();

    //clear Speceifications
    specsFrameObj->listViewSpecsRemoveAllClicked();

    actorListModel->clear();
    modelListModel->clear();
    filterListModel->clear();
    crossProductListModel->clear();
}

void MainWindow::openStatusXml(bool status)
{
    if(status)
    {
        emit readXMLFile();
    }
    else
    {
        displayMessage("Xml Parser", "Unable to open File");
    }
}

void MainWindow::xmlDataParsedFromFile(QStringList modelDesc, QStringList modpara,
                                       QStringList dims, QStandardItemModel * actModel,
                                       QVector<QStringList> idealAdjustmentList)
{
    actorScenarioList = modelDesc;
    modelParams = modpara;
    modelDimNames = dims;
    emit setAccomodationTableModel(actModel,idealAdjustmentList,dims,modelDesc);
}

void MainWindow::finalSpecificationsListModel(MainWindow::DataValues modelLhs, MainWindow::SpecsData modelRhs, int specCount)
{
    modelSpecificationsLHS=modelLhs;
    modelSpecificationsRHS=modelRhs ;
    specsIndexlist[0]=specCount;
}

void MainWindow::finalSpecificationsListActor(MainWindow::DataValues actorLhs, MainWindow::SpecsData actorRhs, int specCount)
{
    actorSpecificationsLHS=actorLhs ;
    actorSpecificationsRHS=actorRhs ;
    specsIndexlist[1]=specCount;
}

void MainWindow::finalSpecificationsListFilter(MainWindow::SpecsData filterLhs, MainWindow::SpecificationVector filterRhs, int specCount)
{
    filterSpecificationsLHS=filterLhs;
    filterSpecificationsRHS=filterRhs;
    specsIndexlist[2]=specCount;
}

void MainWindow::finalSpecificationsListCrossProduct(MainWindow::SpecsData crossProdLhs, MainWindow::SpecificationVector crossProdRhs, int specCount)
{
    crossProdSpecificationsLHS=crossProdLhs;
    crossProdSpecificationsRHS=crossProdRhs;
    specsIndexlist[3]=specCount;
}

void MainWindow::dataAccModel(QStandardItemModel *dataModel, QStandardItemModel *accModel, QStringList scenarioList)
{
    actorDataModel = dataModel;
    actorAccModel = accModel;
    actorScenarioList = scenarioList;

}

void MainWindow::updateModelwithSpecChanges()
{
    generateModelforScenZ();//scen 0

    for(int spec=0; spec < modelSpecificationsRHS.count(); ++spec)
    {
        QString lhsList = modelSpecificationsLHS.at(spec);
        QVector<QString> rhsList = modelSpecificationsRHS.at(spec);
        getRowColumnIndicesModelActor(lhsList);
        updateDataModelRowColumn(rhsList);
        saveSpecsToFile(0);
    }

    for(int spec=0; spec < actorSpecificationsRHS.count(); ++spec)
    {
        QString lhsList = actorSpecificationsLHS.at(spec);
        QVector<QString> rhsList = actorSpecificationsRHS.at(spec);
        getRowColumnIndicesModelActor(lhsList);
        updateDataModelRowColumn(rhsList);
        saveSpecsToFile(1);

    }

    for(int spec=0; spec < filterSpecificationsRHS.count(); ++spec)
    {
        QVector<QString>  lhsList = filterSpecificationsLHS.at(spec);
        QVector<QVector<QString>> rhsList = filterSpecificationsRHS.at(spec);
        getRowColumnIndicesFilterCrossProd(lhsList);
        for(int specsIndex =0 ; specsIndex < rhsList.count(); ++specsIndex)
        {
            updateFilterCrossProdRowColumn(rhsList.at(specsIndex));
        }
        saveSpecsToFile(2);
    }

    for(int spec=0; spec < crossProdSpecificationsRHS.count(); ++spec)
    {

        QVector<QString>  lhsList = crossProdSpecificationsLHS.at(spec);
        QVector<QVector<QString>> rhsList = crossProdSpecificationsRHS.at(spec);
        getRowColumnIndicesFilterCrossProd(lhsList);
        for(int specsIndex =0 ; specsIndex < rhsList.count(); ++specsIndex)
        {
            updateFilterCrossProdRowColumn(rhsList.at(specsIndex));
        }
        saveSpecsToFile(3);
    }
}

void MainWindow::getRowColumnIndicesModelActor(QString lhsList )
{
    modRowIndices.clear();
    modColIndices.clear();
    rowIndices.clear();
    columnIndices.clear();
    accRowIndices.clear();
    accColumnIndices.clear();
    specType.clear();

    // parse lhs list split spec for actor and get row id and column id
    QStringList rowColumnData = lhsList.split(".");
    if(rowColumnData.count()==1)
    {
        qDebug()<<lhsList <<"lhsList";
        //need to work on model parameters
        //model parameters

        int row = modelFrameObj->modelParametersList.indexOf(lhsList.trimmed());

        if(row != -1)
        {
            modRowIndices.append(row);
        }
    }
    else
    {
        QString accString = rowColumnData.at(1);
        if(accString.contains("<>"))
        {
            QStringList accStringList = accString.split(" <> ");
            qDebug()<<accStringList <<"accstringList" << accString;
            for(int rowIndex = 0; rowIndex < actorDataModel->rowCount(); ++ rowIndex)
            {
                if(accStringList.at(0)==actorDataModel->item(rowIndex)->text().trimmed())
                {
                    accRowIndices.append(rowIndex);
                }
                if(accStringList.at(1)==actorDataModel->item(rowIndex)->text().trimmed())
                {
                    accColumnIndices.append(rowIndex);
                }
            }
        }
        else
        {
            qDebug()<<rowColumnData.at(0) <<"lhsList" << rowColumnData.at(1);
            for(int rowIndex = 0; rowIndex < actorDataModel->rowCount(); ++ rowIndex)
            {
                if(rowColumnData.at(0)==actorDataModel->item(rowIndex)->text())
                {
                    rowIndices.append(rowIndex);
                }
            }
            for(int colIndex = 0; colIndex < actorDataModel->columnCount(); ++colIndex)
            {
                if(rowColumnData.at(1)==actorDataModel->horizontalHeaderItem(colIndex)->text())
                {
                    columnIndices.append(colIndex);
                }
            }
        }
    }
}

void MainWindow::getRowColumnIndicesFilterCrossProd(QVector<QString> lhsList)
{
    modRowIndices.clear();
    modColIndices.clear();
    rowIndices.clear();
    columnIndices.clear();
    accRowIndices.clear();
    accColumnIndices.clear();
    specType.clear();

    for(int lIndex = 0; lIndex < lhsList.length(); ++lIndex)
    {
        // parse lhs list split spec for actor and get row id and column id
        QStringList rowColumnData = lhsList.at(lIndex).split(".");
        if(rowColumnData.count()==1)
        {
            qDebug()<<lhsList.at(lIndex) <<"lhsList";
            //model parameters

            int row = modelFrameObj->modelParametersList.indexOf(lhsList.at(lIndex).trimmed());

            if(row != -1)
            {
                modRowIndices.append(row);
                specType.append(0);
            }
        }
        else
        {
            QString accString = rowColumnData.at(1);
            if(accString.contains("<>") || accString.contains(" <> "))
            {
                QStringList accStringList = accString.split("<>");
                if(accStringList.count()==1)
                {
                    accStringList.clear();
                    accStringList = accString.split(" <> ");
                }
                qDebug()<<accStringList <<"accstringList" << accString;
                for(int rowIndex = 0; rowIndex < actorDataModel->rowCount(); ++ rowIndex)
                {
                    QString actor =accStringList.at(0);
                    if(actor.trimmed()==actorDataModel->item(rowIndex)->text().trimmed())
                    {
                        accRowIndices.append(rowIndex);
                        specType.append(2);
                    }
                    actor =accStringList.at(1);
                    if(actor.trimmed()==actorDataModel->item(rowIndex)->text().trimmed())
                    {
                        accColumnIndices.append(rowIndex);
                    }
                }
            }
            else
            {
                qDebug()<<rowColumnData.at(0) <<"lhsList" << rowColumnData.at(1);
                for(int rowIndex = 0; rowIndex < actorDataModel->rowCount(); ++ rowIndex)
                {
                    if(rowColumnData.at(0)==actorDataModel->item(rowIndex)->text().simplified().trimmed())
                    {
                        rowIndices.append(rowIndex);
                        specType.append(1);
                    }
                }
                for(int colIndex = 0; colIndex < actorDataModel->columnCount(); ++colIndex)
                {
                    if(rowColumnData.at(1)==actorDataModel->horizontalHeaderItem(colIndex)->text().simplified().trimmed())
                    {
                        columnIndices.append(colIndex);
                    }
                }
            }
        }
    }
}

void MainWindow::generateModelforScenZ()
{

    QStandardItemModel * updatedDataModel= new QStandardItemModel;
    QStandardItemModel * updatedAccModel = new QStandardItemModel;

    //making a copy of the model to make modifications in it
    updatedDataModel->clear();
    updatedDataModel->insertColumns(0,actorDataModel->columnCount());
    updatedDataModel->insertRows(0,actorDataModel->rowCount());

    for(int col = 0; col <actorDataModel->columnCount();++col)
    {
        for (int row = 0 ; row < actorDataModel->rowCount() ; row++)
        {
            updatedDataModel->setItem(row,col,actorDataModel->item(row,col)->clone());
        }
    }

    updatedAccModel->clear();
    updatedAccModel->insertColumns(0,actorDataModel->rowCount());
    updatedAccModel->insertRows(0,actorDataModel->rowCount());

    if(actorAccModel==nullptr)
    {
        actorAccModel= new QStandardItemModel;
        actorAccModel->insertColumns(0,actorDataModel->rowCount());
        actorAccModel->insertRows(0,actorDataModel->rowCount());

        for(int col = 0; col <actorDataModel->rowCount(); ++col)
        {
            actorAccModel->setHorizontalHeaderItem(col,new QStandardItem(actorDataModel->item(col)->text()));
            actorAccModel->setVerticalHeaderItem(col,new QStandardItem(actorDataModel->item(col)->text()));

            for (int row = 0 ; row < actorDataModel->rowCount() ; row++)
            {
                actorAccModel->setItem(row,col,new QStandardItem("0"));
                if(row==col)
                {
                    actorAccModel->setItem(row,col,new QStandardItem("1"));
                }
            }
        }
    }

    for(int col = 0; col <actorAccModel->columnCount();++col)
    {
        for (int row = 0 ; row < actorAccModel->rowCount() ; row++)
        {
            updatedAccModel->setItem(row,col,actorAccModel->item(row,col)->clone());
        }
    }

    if(modelParams.count()<9)
    {
        //get default parameters from model and update
        for(int i=0; i <9; ++i)
        {
            modelParams.append(modelFrameObj->modelParameters.at(i).at(0));
        }
    }

    QStringList parameters;

    parameters.append(QString("Scen_"+QString::number(dummy)+";"+actorScenarioList.at(0)));//name
    parameters.append(QString("Scen_"+QString::number(dummy)+";"+actorScenarioList.at(1)));//desc
    parameters.append(seedVal);//seed

    parameters.append(modelParams); // Modify Parameters

    if(modelDimNames.length()==0)
    {
        //names of Dimensions
        for(int d=0; d< (actorDataModel->columnCount()-3)/2 ; ++d)
        {
            modelDimNames.append("Dim" + QString::number(d)); // dummy
        }

    }
    parameters.append(modelDimNames);

    QStringList fileNameSplit = fileNameM.split(".");
    QString file = fileNameSplit.at(0);
    file.append("_scen_" + QString::number(dummy)+ ".xml");


    for(int i=0; i < actorDataModel->rowCount(); ++i)
    {
        updatedAccModel->setHorizontalHeaderItem(i,new QStandardItem(updatedDataModel->item(i)->text()));
        updatedAccModel->setVerticalHeaderItem(i,new QStandardItem(updatedDataModel->item(i)->text()));
    }

    // emit model and save to xml file
    emit generateXMLFile(parameters,updatedDataModel,updatedAccModel,file);
    runFileNamesList.append(file);


    delete updatedDataModel;
    delete updatedAccModel;

    statusBar()->showMessage(" Done ...");
}


void MainWindow::updateDataModelRowColumn(QVector<QString> rhsList)
{
    QStandardItemModel * updatedDataModel= new QStandardItemModel;
    QStandardItemModel * updatedAccModel = new QStandardItemModel;

    for(int modelCount = 0 ; modelCount < rhsList.length(); ++modelCount)
    {
        //making a copy of the model to make modifications in it
        updatedDataModel->clear();
        updatedDataModel->insertColumns(0,actorDataModel->columnCount());
        updatedDataModel->insertRows(0,actorDataModel->rowCount());

        for(int col = 0; col <actorDataModel->columnCount();++col)
        {
            for (int row = 0 ; row < actorDataModel->rowCount() ; row++)
            {
                updatedDataModel->setItem(row,col,actorDataModel->item(row,col)->clone());
            }
        }

        updatedAccModel->clear();
        updatedAccModel->insertColumns(0,actorDataModel->rowCount());
        updatedAccModel->insertRows(0,actorDataModel->rowCount());

        if(actorAccModel==nullptr)
        {
            actorAccModel= new QStandardItemModel;
            actorAccModel->insertColumns(0,actorDataModel->rowCount());
            actorAccModel->insertRows(0,actorDataModel->rowCount());

            for(int col = 0; col <actorDataModel->rowCount(); ++col)
            {
                actorAccModel->setHorizontalHeaderItem(col,new QStandardItem(actorDataModel->item(col)->text()));
                actorAccModel->setVerticalHeaderItem(col,new QStandardItem(actorDataModel->item(col)->text()));

                for (int row = 0 ; row < actorDataModel->rowCount() ; row++)
                {
                    actorAccModel->setItem(row,col,new QStandardItem("0"));
                    if(row==col)
                    {
                        actorAccModel->setItem(row,col,new QStandardItem("1"));
                    }
                }
            }
        }

        for(int col = 0; col <actorAccModel->columnCount();++col)
        {
            for (int row = 0 ; row < actorAccModel->rowCount() ; row++)
            {
                updatedAccModel->setItem(row,col,actorAccModel->item(row,col)->clone());
            }
        }

        qDebug()<<actorDataModel->rowCount() << "RC" << updatedDataModel->rowCount();
        qDebug()<<actorDataModel->columnCount() << "CC" << updatedDataModel->columnCount();

        //updating Data Model with values
        for(int r=0; r<rowIndices.length(); ++r)
        {
            for(int c=0; c < columnIndices.length(); ++ c)
            {
                QStandardItem * item = new QStandardItem(rhsList.at(modelCount));

                updatedDataModel->setItem(rowIndices.at(r),columnIndices.at(c),item);
            }
        }

        //updating Acc Model with values
        for(int r=0; r<accRowIndices.length(); ++r)
        {
            for(int c=0; c < accColumnIndices.length(); ++ c)
            {
                QStandardItem * item = new QStandardItem(rhsList.at(modelCount));

                updatedAccModel->setItem(accRowIndices.at(r),accColumnIndices.at(c),item);
            }
        }

        if(modelParams.count()<9)
        {
            //get default parameters from model and update
            for(int i=0; i <9; ++i)
            {
                modelParams.append(modelFrameObj->modelParameters.at(i).at(0));
            }
            //updating Modelparameters with values
        }
        for(int r=0; r<modRowIndices.length(); ++r)
        {
            modelParams[modRowIndices.at(r)] = rhsList.at(modelCount);
        }

        QStringList parameters;

        dummy++;

        parameters.append(QString("Scen_"+QString::number(dummy)+";"+actorScenarioList.at(0)));//name
        parameters.append(QString("Scen_"+QString::number(dummy)+";"+actorScenarioList.at(1)));//desc
        parameters.append(seedVal);//seed

        parameters.append(modelParams); // Modify Parameters

        if(modelDimNames.length()==0)
        {
            //names of Dimensions
            for(int d=0; d< (actorDataModel->columnCount()-3)/2 ; ++d)
            {
                modelDimNames.append("Dim" + QString::number(d)); // dummy
            }

        }
        parameters.append(modelDimNames);

        QStringList fileNameSplit = fileNameM.split(".");
        QString file = fileNameSplit.at(0);
        file.append("_scen_" + QString::number(dummy)+ ".xml");

        for(int i=0; i < actorDataModel->rowCount(); ++i)
        {
            updatedAccModel->setHorizontalHeaderItem(i,new QStandardItem(updatedDataModel->item(i)->text()));
            updatedAccModel->setVerticalHeaderItem(i,new QStandardItem(updatedDataModel->item(i)->text()));
        }
        // emit model and save to xml file
        emit generateXMLFile(parameters,updatedDataModel,updatedAccModel,file);
        runFileNamesList.append(file);

    }
    delete updatedDataModel;
    delete updatedAccModel;

    statusBar()->showMessage(" Done ...");
}

void MainWindow::updateFilterCrossProdRowColumn(QVector<QString> rhsList)
{

    updatedDataModel->clear();
    updatedAccModel->clear();
    QStringList parameters;
    int in=0;
    int actIn=0;
    int accIn=0;

    //making a copy of the model to make modifications in it
    updatedDataModel->insertColumns(0,actorDataModel->columnCount());
    updatedDataModel->insertRows(0,actorDataModel->rowCount());

    for(int col = 0; col <actorDataModel->columnCount();++col)
    {
        for (int row = 0 ; row < actorDataModel->rowCount() ; row++)
        {
            updatedDataModel->setItem(row,col,actorDataModel->item(row,col)->clone());
        }
    }

    updatedAccModel->clear();
    updatedAccModel->insertColumns(0,actorDataModel->rowCount());
    updatedAccModel->insertRows(0,actorDataModel->rowCount());

    if(actorAccModel==nullptr)
    {
        actorAccModel= new QStandardItemModel;
        actorAccModel->insertColumns(0,actorDataModel->rowCount());
        actorAccModel->insertRows(0,actorDataModel->rowCount());

        for(int col = 0; col <actorDataModel->rowCount(); ++col)
        {
            for (int row = 0 ; row < actorDataModel->rowCount() ; row++)
            {
                actorAccModel->setItem(row,col,new QStandardItem("0"));
                if(row==col)
                {
                    actorAccModel->setItem(row,col,new QStandardItem("1"));
                }
            }
        }
    }

    for(int col = 0; col <actorAccModel->columnCount();++col)
    {
        for (int row = 0 ; row < actorAccModel->rowCount() ; row++)
        {
            updatedAccModel->setItem(row,col,actorAccModel->item(row,col)->clone());
        }
    }

    // checking the count
    if(modelParams.count()<9)
    {
        //get default parameters from model and update
        for(int i=0; i <9; ++i)
        {
            modelParams.append(modelFrameObj->modelParameters.at(i).at(0));
        }
    }

    for(int modelCount = 0 ; modelCount < rhsList.length(); ++modelCount)
    {
        if(specType.at(modelCount)==1)
        {
            QStandardItem * item = new QStandardItem(QString(rhsList.at(modelCount)));

            updatedDataModel->setItem(rowIndices.at(actIn),columnIndices.at(actIn),item);
            ++actIn;
        }
        if(specType.at(modelCount)==2)
        {
            QStandardItem * item = new QStandardItem(QString(rhsList.at(modelCount)));

            updatedAccModel->setItem(accRowIndices.at(accIn),accColumnIndices.at(accIn),item);
            ++accIn;
        }
        if(specType.at(modelCount)==0)
        {
            //updating Modelparameters with values
            {
                modelParams[modRowIndices.at(in++)] = rhsList.at(modelCount);
            }
        }
    }

    dummy++;

    parameters.append(QString("Scen_"+QString::number(dummy)+";"+actorScenarioList.at(0)));//name
    parameters.append(QString("Scen_"+QString::number(dummy)+";"+actorScenarioList.at(1)));//desc
    parameters.append(seedVal);//seed

    parameters.append(modelParams); // Modify Parameters

    if(modelDimNames.length()==0)
    {
        //names of Dimensions
        for(int d=0; d< (actorDataModel->columnCount()-3)/2 ; ++d)
        {
            modelDimNames.append("Dim" + QString::number(d)); // dummy
        }

    }
    parameters.append(modelDimNames);
    QStringList fileNameSplit = fileNameM.split(".");
    QString file = fileNameSplit.at(0);
    file.append("_scen_" + QString::number(dummy)+ ".xml");


    for(int i=0; i < actorDataModel->rowCount(); ++i)
    {
        updatedAccModel->setHorizontalHeaderItem(i,new QStandardItem(updatedDataModel->item(i)->text()));
        updatedAccModel->setVerticalHeaderItem(i,new QStandardItem(updatedDataModel->item(i)->text()));
    }

    // emit model and save to xml file
    emit generateXMLFile(parameters,updatedDataModel,updatedAccModel,file);
    runFileNamesList.append(file);

    statusBar()->showMessage(" Done ...");

}

void MainWindow::saveSpecsToFile(int specTypeIndex)
{
    QFile f("specListFile.txt");

    if (f.open(QFile::WriteOnly | QFile::Append))
    {
        QTextStream data( &f );
        QString scenIndex;
        scenIndex.append("Spec_");
        int val=1;

        //appending data
        if(specTypeIndex==0)
        {
            for(int indx = 0 ; indx < modelSpecificationsRHS.length();++indx)
            {
                QString spec;
                spec.append(scenIndex + QString::number(val++)).append(";");
                spec.append("(");
                spec.append(modelSpecificationsLHS.at(indx).at(0));
                spec.append(")=(");

                for(int parIndex=0; parIndex < modelSpecificationsRHS.at(indx).length(); ++parIndex)
                {
                    spec.append(modelSpecificationsRHS.at(indx).at(parIndex)).append(",");
                }
                spec.append("#");
                spec.remove(",#").append(")");
                data <<spec + "\n";
            }
        }
        if(specTypeIndex==1)
        {
            for(int indx = 0 ; indx < modelSpecificationsRHS.length();++indx)
            {
                QString spec;
                spec.append(scenIndex + QString::number(val++)).append(";");
                spec.append("(");
                spec.append(actorSpecificationsLHS.at(indx).at(0));
                spec.append(")=(");

                for(int parIndex=0; parIndex < actorSpecificationsRHS.at(indx).length(); ++parIndex)
                {
                    spec.append(actorSpecificationsRHS.at(indx).at(parIndex)).append(",");
                }
                spec.append("#");
                spec.remove(",#").append(")");
                data <<spec + "\n";
            }
        }
        if(specTypeIndex==2)
        {

            for(int specIndex = 0 ; specIndex < filterSpecificationsRHS.length(); ++ specIndex)
            {

                QString spec;
                spec.append(scenIndex + QString::number(val++)).append(";");
                spec.append("(");

                for(int indx= 0 ; indx < filterSpecificationsLHS.at(specIndex).length();++indx)
                {
                    spec.append(filterSpecificationsLHS.at(specIndex).at(indx));
                }
                spec.append(")=(");

                for(int parIndex=0; parIndex < filterSpecificationsRHS.at(specIndex).length(); ++parIndex)
                {
                    for(int valIndex = 0; valIndex < filterSpecificationsRHS.at(specIndex).at(parIndex).length() ; ++ valIndex)
                    {
                        spec.append(filterSpecificationsRHS.at(specIndex).at(parIndex).at(valIndex)).append(",");
                    }
                }
                spec.append("#");
                spec.remove(",#").append(")");

                data <<spec + "\n";
            }

        }
        if(specTypeIndex==3)
        {
            for(int specIndex = 0 ; specIndex <crossProdSpecificationsRHS.length(); ++ specIndex)
            {
                for(int parIndex=0; parIndex < crossProdSpecificationsRHS.at(specIndex).length(); ++parIndex)
                {
                    QString spec;
                    spec.append(scenIndex + QString::number(val++)).append(";");
                    spec.append("(");

                    for(int indx= 0 ; indx < crossProdSpecificationsLHS.at(specIndex).length();++indx)
                    {
                        spec.append(crossProdSpecificationsLHS.at(specIndex).at(indx)).append(",");
                    }
                    spec.append("#").remove(",#");
                    spec.append(")=(");

                    for(int valIndex = 0; valIndex < crossProdSpecificationsRHS.at(specIndex).at(parIndex).length() ; ++ valIndex)
                    {
                        spec.append(crossProdSpecificationsRHS.at(specIndex).at(parIndex).at(valIndex)).append(",");
                    }
                    spec.append("#");
                    spec.remove(",#").append(")");

                    data <<spec + "\n";
                }
            }
        }

        f.close();
    }
}

// preceding zeros in file name

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
