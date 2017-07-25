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
}

MainWindow::~MainWindow()
{

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
    connect(modelFrameObj,SIGNAL(modelList(QStandardItemModel*, QPair<DataValues,SpecsData> )),
            this,SLOT(modelparamListModel(QStandardItemModel*, QPair<DataValues,SpecsData> )));
    modelListModel= new QStandardItemModel;
}

void MainWindow::actorFrameInitialization()
{
    actorFrameObj= new ActorFrame(sasStackedWidget);
    sasStackedWidget->addWidget(actorFrameObj);
    connect(actorFrameObj,SIGNAL(modelList(QStandardItemModel*,QStringList, QPair<DataValues,SpecsData>)),
            this,SLOT(actorsItemListModel(QStandardItemModel*,QStringList, QPair<DataValues,SpecsData>)));
    actorListModel= new QStandardItemModel;

}

void MainWindow::specsFrameInitialization()
{
    specsFrameObj= new SpecificationFrame(sasStackedWidget);
    sasStackedWidget->addWidget(specsFrameObj);
    connect(this,SIGNAL(specsItemListModel(QStandardItemModel*, QPair<DataValues,SpecsData>,QPair<DataValues,SpecsData>,
                                            QPair<SpecsData,SpecificationVector>, QPair<SpecsData,SpecificationVector>)),
            specsFrameObj,SLOT(specsListMainWindow(QStandardItemModel*, QPair<DataValues,SpecsData>,QPair<DataValues,SpecsData>,
                                                    QPair<SpecsData,SpecificationVector>, QPair<SpecsData,SpecificationVector>)));
    connect(this,SIGNAL(actorAttributesAndSAS(QStringList,QStringList)),
            specsFrameObj,SLOT(actorAtrributesSAS(QStringList,QStringList)));
    connect(specsFrameObj,SIGNAL(filterList(QStandardItemModel*,  QPair<SpecsData,SpecificationVector>)),
            this,SLOT(filtersListModel(QStandardItemModel*,  QPair<SpecsData,SpecificationVector>)));
    connect(specsFrameObj,SIGNAL(crossProductList(QStandardItemModel*, QPair<SpecsData,SpecificationVector>)),
            this,SLOT(crossProdListModel(QStandardItemModel*, QPair<SpecsData,SpecificationVector>)));

    filterListModel = new QStandardItemModel;
    crossProductListModel = new QStandardItemModel;
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
    connect(runFileAct, SIGNAL(triggered(bool)), this,SLOT(runModel(bool)));
    fileMenu->addAction(runFileAct);
    fileToolBar->addAction(runFileAct);

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

void MainWindow::about()
{
    QMessageBox::about(this, tr("About KTAB SAS"),
                       tr("KTAB SAS\n\nVersion 1.0\n\n"
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
            if(fileName.endsWith(".csv")||fileName.endsWith(".CSV"))
            {
                emit csvFilePath(fileName);
                actorNaviClicked();//Actor Attributes Window
            }
            else
            {
                emit openXMLFile(fileName);
                actorNaviClicked();//Actor Attributes Window
            }
        }
    }
}
void MainWindow::saveClicked(bool bl)
{

}

void MainWindow::runModel(bool bl)
{

}

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
       combinedSASModel->clear();
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
    emit setAccomodationTableModel(actModel,idealAdjustmentList,dims,modelDesc);
}

void MainWindow::actorsItemListModel(QStandardItemModel * specsList, QStringList attributes,
                                     QPair<DataValues,SpecsData> specsVec)
{
    combinedSASModel = new QStandardItemModel;

    actorSpecs = (specsVec);

    actorListModel= new QStandardItemModel;
    QStringList sasValues;
    for(int i = 0 ; i < specsList->rowCount(); ++i)
    {
        actorListModel->appendRow(new QStandardItem(specsList->item(i)->text()));
        sasValues.append(specsList->item(i)->text());
    }
    emit actorAttributesAndSAS(attributes,sasValues);
    combinedModel();
}

void MainWindow::modelparamListModel(QStandardItemModel * specsList,  QPair<DataValues,SpecsData> spec)
{
    combinedSASModel = new QStandardItemModel;

    modelSpecs = spec;
    qDebug()<<"modelparamListModel";
    modelListModel= new QStandardItemModel;
    for(int i = 0 ; i < specsList->rowCount(); ++i)
    {
        modelListModel->appendRow(new QStandardItem(specsList->item(i)->text()));
    }
    combinedModel();
}

void MainWindow::filtersListModel(QStandardItemModel *specsList,  QPair<SpecsData,SpecificationVector> specsVec)
{
    combinedSASModel = new QStandardItemModel;
    filterSpecs = (specsVec);
    filterListModel= new QStandardItemModel;

    for(int i = 0 ; i < specsList->rowCount(); ++i)
    {
        filterListModel->appendRow(new QStandardItem(specsList->item(i)->text()));
    }
    combinedModel();
}

void MainWindow::crossProdListModel(QStandardItemModel *specsList, QPair<SpecsData, SpecificationVector> specsVec)
{
    combinedSASModel = new QStandardItemModel;
    crossProductSpecs = specsVec;
    crossProductListModel= new QStandardItemModel;
    for(int i = 0 ; i < specsList->rowCount(); ++i)
    {
        crossProductListModel->appendRow(new QStandardItem(specsList->item(i)->text()));
    }
    combinedModel();
}

void MainWindow::combinedModel()
{
    qDebug()<<"combinedModel";
    //combined
    for(int i = 0 ; i < modelListModel->rowCount(); ++i)
    {
        combinedSASModel->appendRow(new QStandardItem(modelListModel->item(i)->text()));
    }
    for(int i = 0 ; i < actorListModel->rowCount(); ++i)
    {
        combinedSASModel->appendRow(new QStandardItem(actorListModel->item(i)->text()));
    }
    for(int i = 0 ; i < filterListModel->rowCount(); ++i)
    {
        combinedSASModel->appendRow(new QStandardItem(filterListModel->item(i)->text()));
    }
    for(int i = 0 ; i < crossProductListModel->rowCount(); ++i)
    {
        combinedSASModel->appendRow(new QStandardItem(crossProductListModel->item(i)->text()));
    }
    emit specsItemListModel(combinedSASModel,modelSpecs,actorSpecs,filterSpecs,crossProductSpecs);

}

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
