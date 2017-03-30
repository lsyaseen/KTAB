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
    createConnections();
    intializeHomeDirectory();
}

MainWindow::~MainWindow()
{

}

void MainWindow::createConnections()
{

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
    clearFileAct->setStatusTip(tr("Import Data from CSV/XML File"));
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
    QAction *saveAct = new QAction(saveIcon, tr("&Save..."), this);
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

}

void MainWindow::saveClicked(bool bl)
{

}

void MainWindow::runModel(bool bl)
{

}
void MainWindow::clearSpecifications(bool bl)
{

}


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
