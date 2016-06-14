#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow()
{
    central = new QFrame;
    QGridLayout *gLayout = new QGridLayout(central);

    csv_tableView = new QTableView(central);
    gLayout->addWidget(csv_tableView);
    central->setLayout(gLayout);

    setCentralWidget(central);

    central->setFrameShape(QFrame::StyledPanel);

    createActions();
    createStatusBar();

    createModuleParametersDockWindow();
    createGraph1DockWindows();
    createGraph2DockWindows();

    setWindowTitle(tr("SMPQ_Dock_GUI"));

    //CSV class obj
    csvObj = new CSV;
    //connect statement to pass the csv file path
    connect(this,&MainWindow::csvFilePath, csvObj, &CSV::readCSVFile);
    //connect statement to receive processed model
    connect(csvObj, &CSV::csvModel,this,&MainWindow::csvItemModel);

}

MainWindow::~MainWindow()
{
}


void MainWindow::csvGetFilePAth()
{
    //Get  *.csv file path
    QString FilePath;
    FilePath = QFileDialog::getOpenFileName(this,tr("Open CSV File"), QDir::homePath() , tr("CSV File (*.csv)"));
    statusBar()->showMessage(tr("csv path"));

    //emit path to csv class for processing
    emit csvFilePath(FilePath);
}

void MainWindow::dockWindowChanged()
{
    QWidget *TitleWidgetRec=new QWidget(this);
    if(graph2Dock->isFloating())
        graph2Dock->setTitleBarWidget(TitleWidgetRec);
    else
        graph2Dock->setTitleBarWidget(0);
}

void MainWindow::csvItemModel(QStandardItemModel *model)
{
    //update received model to widget
    csv_tableView->setModel(model);
    csv_tableView->showMaximized();
    csv_tableView->setAlternatingRowColors(true);
    csv_tableView->resizeRowsToContents();

    //    for (int c = 0; c < csv_tableView->horizontalHeader()->count(); ++c)
    //        csv_tableView->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);

    csv_tableView->resizeColumnsToContents();

    //Enable run button, which is disabled by default
    runButton->setEnabled(true);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About SMP "),
                       tr("Should update informtion regarding SMP here !! "));
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newDoc = new QAction(newIcon, tr("&New "), this);
    newDoc->setShortcuts(QKeySequence::New);
    newDoc->setStatusTip(tr("Create new "));
    fileMenu->addAction(newDoc);
    fileToolBar->addAction(newDoc);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save"));
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon csvIcon = QIcon::fromTheme("read-csv", QIcon(":/images/csv.png"));
    QAction *readCsvAct = new QAction(csvIcon, tr("&Read CSV"), this);
    readCsvAct->setShortcuts(QKeySequence::Save);
    readCsvAct->setStatusTip(tr("Save the current form letter"));
    connect(readCsvAct, &QAction::triggered, this, &MainWindow::csvGetFilePAth);
    fileMenu->addAction(readCsvAct);
    fileToolBar->addAction(readCsvAct);

    fileMenu->addSeparator();

    QAction *quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));


    viewMenu = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createGraph1DockWindows()
{
    graph1Dock = new QDockWidget(tr("Module 1"), this);
    //   graph1Dock->setAllowedAreas(Qt::LeftDockWidgetArea |Qt::RightDockWidgetArea |Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    listwidget = new QListWidget(graph1Dock);
    graph1Dock->setWidget(listwidget);
    addDockWidget(Qt::BottomDockWidgetArea, graph1Dock);
    viewMenu->addAction(graph1Dock->toggleViewAction());

    connect(graph1Dock,SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),this,SLOT(dockWindowChanged()));
}

void MainWindow::createGraph2DockWindows()
{
    graph2Dock = new QDockWidget(tr("Module2"), this);
    //  graph2Dock->setAllowedAreas(Qt::LeftDockWidgetArea |Qt::RightDockWidgetArea |Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    listwidget = new QListWidget(graph2Dock);
    graph2Dock->setWidget(listwidget);
    addDockWidget(Qt::BottomDockWidgetArea, graph2Dock);
    viewMenu->addAction(graph2Dock->toggleViewAction());

    connect(graph2Dock,SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),this,SLOT(dockWindowChanged()));

}

void MainWindow::createModuleParametersDockWindow()
{
    moduleParametersDock = new QDockWidget(tr(" Parameters"),this);

    moduleFrame = new QFrame(moduleParametersDock);
    VLayout  = new QGridLayout(moduleFrame);

    for (int i = 0; i < N; ++ i)
    {
        frames[i] = new QFrame(moduleParametersDock);
        frames[i]->setFrameShape(QFrame::StyledPanel);//change to flat if required
        VLayout->addWidget(frames[i]);
    }

    //RUN Button
    runButton = new QPushButton;
    runButton->setText("RUN");
    runButton->setEnabled(false);
    VLayout->addWidget(runButton);

    //Radio buttons //Section1
    QGridLayout *radioButtonLayout = new QGridLayout(frames[0]);
    rparam1 = new QRadioButton("Parameter 1");
    rparam2 = new QRadioButton("Parameter 2");
    rparam3 = new QRadioButton("Parameter 3");
    radioButtonLayout->addWidget(rparam1);
    radioButtonLayout->addWidget(rparam2);
    radioButtonLayout->addWidget(rparam3);
    frames[0]->setLayout(radioButtonLayout);

    //CheckBoxes //Section2
    QGridLayout * checkBoxLayout = new QGridLayout(frames[1]);
    cparam1 = new QCheckBox("Parameter 1");
    cparam2 = new QCheckBox("Parameter 2");
    cparam3 = new QCheckBox("Parameter 3");
    checkBoxLayout->addWidget(cparam1);
    checkBoxLayout->addWidget(cparam2);
    checkBoxLayout->addWidget(cparam3);
    frames[1]->setLayout(checkBoxLayout);

    moduleParametersDock->setWidget(moduleFrame);
    // moduleParametersDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
    addDockWidget(Qt::LeftDockWidgetArea, moduleParametersDock);
    viewMenu->addAction(moduleParametersDock->toggleViewAction());

}
