#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow()
{
    initializeCentralViewFrame();

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
    connect(csvObj, &CSV::csvModel,this,&MainWindow::setCSVItemModel);

    //Database
    dbObj = new Database();
    //To open database by passing the path
    connect(this,&MainWindow::dbFilePath,dbObj, &Database::openDB);
    //To get Database item Model to show on GUI
    connect(dbObj,&Database::dbModel,this,&MainWindow::setDBItemModel);
    //To display any message to user
    connect(dbObj,&Database::Message,this,&MainWindow::displayMessage);
    //To get vector positions of actors to plot a graph on GUI
    connect(dbObj,&Database::vectorPosition,this,&MainWindow::addGraphOnModule1);
    //To get number of states to set the max values of slider
    connect(dbObj,&Database::statesCount,this,&MainWindow::updateStateCount_SliderRange);
    //To get all scenarios from Database to update ScenarioComboBox
    connect(dbObj,&Database::scenarios,this,&MainWindow::updateScenarioList_ComboBox);
    //To get initial scenario vector positions and Item model to update graph and Table view
    connect(this, &MainWindow::getScenarioRunValues,dbObj,&Database::getScenarioData);
    //To get state count
    connect(this, &MainWindow::getStateCountfromDB,dbObj,&Database::getStateCount);
    //To get dimensions count
    connect(this, &MainWindow::getDimensionCountfromDB,dbObj,&Database::getDimensionCount);
    //received dimension Values
    connect(dbObj,SIGNAL(dimensionsCount(int)),this,SLOT(updateDimensionCount(int)));


    //headers of TableWidget
    header_editor = 0;

}

MainWindow::~MainWindow()
{
}

void MainWindow::csvGetFilePAth()
{
    //Get  *.csv file path
    QString csvPath;
    csvPath = QFileDialog::getOpenFileName(this,tr("Open CSV File"), QDir::homePath() , tr("CSV File (*.csv)"));
    statusBar()->showMessage(tr("csv path"));

    modeltoCSV->clear();

    //emit path to csv class for processing
    if(!csvPath.isEmpty())
        emit csvFilePath(csvPath);
}

void MainWindow::dbGetFilePAth()
{
    //Get  *.db file path
    QString dbPath;
    dbPath = QFileDialog::getOpenFileName(this,tr("Database File"), QDir::homePath() , tr("Database File (*.db)"));
    statusBar()->showMessage(tr("Looking for Database file ..."));

    //emit path to db class for processing
    if(!dbPath.isEmpty())
        emit dbFilePath(dbPath);
}

void MainWindow::updateStateCount_SliderRange(int states)
{
    turnSlider->setRange(0,states);
}

void MainWindow::updateScenarioList_ComboBox(QStringList *scenarios)
{
    turnSlider->setValue(0);
    scenarioComboBox->clear();
    for(int index=0;index<scenarios->length();++index)
        scenarioComboBox->addItem(scenarios->at(index));

    //    sliderStateValueToQryDB(0);//when new database is opened, start from zero
}

void MainWindow::updateDimensionCount(int dim)
{
    dimensions = dim;
}

void MainWindow::sliderStateValueToQryDB(int value)
{
    removeAllGraphs();
    emit getScenarioRunValues(value,scenario_box);
}

void MainWindow::scenarioComboBoxValue(QString scenario)
{
    removeAllGraphs();
    scenario_box = scenario;

    if(tableType=="Database")
        emit getScenarioRunValues(turnSlider->value(),scenario_box); // to keep the same state when scenario changes

}

void MainWindow::cellSelected(int row, int column)
{

}

void MainWindow::insertNewRowCSV()
{
    if(tableType=="NewCSV")
    {
        while(csv_tableWidget->rowCount()!=actorsLineEdit->text().toInt() && actorsLineEdit->text().toInt()>=csv_tableWidget->rowCount())
            csv_tableWidget->insertRow(csv_tableWidget->rowCount());

    }
    else if(tableType=="CSV")
    {
        if(modeltoCSV->rowCount() < actorsLineEdit->text().toInt())
            modeltoCSV->insertRows(modeltoCSV->rowCount(),(actorsLineEdit->text().toInt() - modeltoCSV->rowCount()));
    }
}

void MainWindow::insertNewColumnCSV()
{
    if(tableType=="NewCSV")
    {
        while(((csv_tableWidget->columnCount()-3)/2)!=dimensionsLineEdit->text().toInt()
              && ((csv_tableWidget->columnCount()-3)/2) <= dimensionsLineEdit->text().toInt())
        {
            createSeperateColumn();
        }
    }
    else if(tableType=="CSV")
    {
        if(((modeltoCSV->columnCount()-3)/2) < dimensionsLineEdit->text().toInt())
            modeltoCSV->insertColumns(modeltoCSV->columnCount(),((modeltoCSV->columnCount()-3))-dimensionsLineEdit->text().toInt());
    }
}


void MainWindow::createSeperateColumn()
{
    QTableWidgetItem * hdr = new QTableWidgetItem("HEADER");

    csv_tableWidget->insertColumn(csv_tableWidget->columnCount());
    csv_tableWidget->setHorizontalHeaderItem(csv_tableWidget->columnCount()-1,hdr);
}

void MainWindow::donePushButtonClicked()
{
    if(tableType=="CSV")
        saveTableViewToCSV();
    else if (tableType=="NewCSV")
        saveTableWidgetToCSV();
}

void MainWindow::displayMessage(QString cls, QString message)
{
    QMessageBox::warning(0,cls,message);
}

void MainWindow::vectorPositionsFromDB()
{
}

void MainWindow::dockWindowChanged()
{
    QWidget *TitleWidgetRec=new QWidget(this);
    if(graph2Dock->isFloating())
        graph2Dock->setTitleBarWidget(TitleWidgetRec);
    else
        graph2Dock->setTitleBarWidget(0);
}

void MainWindow::setCSVItemModel(QStandardItemModel *model, QStringList scenarioName)
{
    tableType="CSV";

    stackWidget->setCurrentIndex(0);
    csv_tableView->horizontalHeader()->viewport()->installEventFilter(this);
    csv_tableView->verticalHeader()->viewport()->installEventFilter(this);

    //Enable Editing
    csv_tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    turnSlider->hide();
    tableControlsFrame->show();

    scenarioComboBox->setEditable(true);
    turnSlider->setVisible(false);
    scenarioDescriptionLineEdit->setVisible(true);

    actorsLineEdit->setEnabled(true);
    dimensionsLineEdit->setEnabled(true);
    actorsPushButton->setEnabled(true);
    dimensionsPushButton->setEnabled(true);
    donePushButton->setEnabled(true);

    modeltoCSV = model;

    //update received model to widget
    csv_tableView->setModel(modeltoCSV);
    csv_tableView->showMaximized();
    //    csv_tableView->setAlternatingRowColors(true);
    //    csv_tableView->resizeRowsToContents();
    //    for (int c = 0; c < csv_tableView->horizontalHeader()->count(); ++c)
    //        csv_tableView->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);

    csv_tableView->resizeColumnsToContents();

    //Enable run button, which is disabled by default
    runButton->setEnabled(true);

    //update scenario name
    scenarioComboBox->clear();
    scenarioComboBox->addItem(scenarioName.at(0));
    scenarioDescriptionLineEdit->clear();
    scenarioDescriptionLineEdit->setText(scenarioName.at(1));

    actorsLineEdit->setText(QString::number(modeltoCSV->rowCount()));
    dimensionsLineEdit->setText(QString::number((modeltoCSV->columnCount()-3)/2));
}

void MainWindow::setDBItemModel(QSqlTableModel *model)
{
    tableType="Database";

    //Disable Editing
    csv_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    stackWidget->setCurrentIndex(0);
    csv_tableView->horizontalHeader()->viewport()->removeEventFilter(this);
    csv_tableView->verticalHeader()->viewport()->removeEventFilter(this);

    turnSlider->hide();
    tableControlsFrame->show();

    scenarioComboBox->setEditable(false);
    turnSlider->setVisible(true);
    scenarioDescriptionLineEdit->setVisible(false);

    actorsLineEdit->setEnabled(false);
    dimensionsLineEdit->setEnabled(false);
    actorsPushButton->setEnabled(false);
    dimensionsPushButton->setEnabled(false);
    donePushButton->setEnabled(false);

    csv_tableView->setModel(model);
    csv_tableView->showMaximized();
    //    csv_tableView->setAlternatingRowColors(true);
    //    csv_tableView->resizeRowsToContents();

    //    for (int c = 0; c < csv_tableView->horizontalHeader()->count(); ++c)
    //        csv_tableView->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);

    csv_tableView->resizeColumnsToContents();
    csv_tableView->hideColumn(0);

    //Enable run button, which is disabled by default
    runButton->setEnabled(true);

    emit getDimensionCountfromDB();

    actorsLineEdit->setText(QString::number(model->rowCount()));
    dimensionsLineEdit->setText(QString::number(dimensions+1));
}

void MainWindow::createNewCSV()
{
    tableType="NewCSV";

    if(stackWidget->count()>1) // 1 is csv_table view
        stackWidget->removeWidget(csv_tableWidget);

    csv_tableWidget = new QTableWidget(central); // new CSV File
    csv_tableWidget->horizontalHeader()->viewport()->installEventFilter(this);
    csv_tableWidget->verticalHeader()->viewport()->installEventFilter(this);

    stackWidget->addWidget(csv_tableWidget);
    stackWidget->setCurrentIndex(1);

    removeAllGraphs();

    actorsLineEdit->setEnabled(true);
    dimensionsLineEdit->setEnabled(true);
    actorsPushButton->setEnabled(true);
    dimensionsPushButton->setEnabled(true);
    donePushButton->setEnabled(true);


    scenarioComboBox->setEditable(true);
    turnSlider->setVisible(false);
    scenarioDescriptionLineEdit->setVisible(true);
    scenarioComboBox->lineEdit()->setPlaceholderText("Enter Scenario...");
    scenarioDescriptionLineEdit->setPlaceholderText("Enter Scenario Description here ... ");

    turnSlider->hide();
    tableControlsFrame->show();

    //    for (int i = 0 ; i < csv_tableWidget->columnCount(); ++i)
    //        csv_tableWidget->removeColumn(i);

    csv_tableWidget->setShowGrid(true);

    // csv_tableWidget->setItem(0, 1, new QTableWidgetItem("Test"));
    // connect(csv_tableWidget, SIGNAL(cellDoubleClicked (int, int)),
    //      this, SLOT( cellSelected( int, int )));

    //csv_tableWidget->setHorizontalHeaderLabels(QString("HEADER;HEADER;HEADER;HEADER;HEADER").split(";"));

    actorsLineEdit->clear();
    dimensionsLineEdit->clear();
    scenarioComboBox->clear();
    scenarioDescriptionLineEdit->clear();

    actorsLineEdit->setText(QString::number(3));
    dimensionsLineEdit->setText(QString::number(1));

    //    csv_tableWidget->horizontalHeader()->hide();

    insertNewRowCSV();
    insertNewColumnCSV();

}

void MainWindow::initializeCentralViewFrame()
{
    central = new QFrame;
    central->setFrameShape(QFrame::StyledPanel);

    gLayout = new QGridLayout;
    central->setLayout(gLayout);

    tableControlsFrame = new QFrame;
    tableControlsFrame->setFrameShape(QFrame::StyledPanel);

    QGridLayout *gCLayout = new QGridLayout(tableControlsFrame);

    stackWidget = new QStackedWidget(central);

    scenarioComboBox = new QComboBox(tableControlsFrame);
    //    scenarioComboBox->setMinimumWidth(40);
    scenarioComboBox->setMaximumWidth(200);
    scenarioComboBox->setFixedWidth(150);
    gCLayout->addWidget(scenarioComboBox,0,2);
    scenarioComboBox->setEditable(true);
    connect(scenarioComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(scenarioComboBoxValue(QString)));

    turnSlider = new QSlider(Qt::Horizontal,central);
    //    turnSlider->setMinimumWidth(40);
    //    turnSlider->setMaximumWidth(150);
    //    turnSlider->setFixedWidth(130);
    //    turnSlider->setTickInterval(1);
    turnSlider->setPageStep(1);
    //gCLayout->addWidget(turnSlider,1,3);

    connect(turnSlider,&QSlider::valueChanged,this,&MainWindow::sliderStateValueToQryDB);

    actorsPushButton = new QPushButton("Actors",tableControlsFrame);
    actorsPushButton->setMaximumWidth(120);
    actorsPushButton->setFixedWidth(90);
    gCLayout->addWidget(actorsPushButton,0,0);

    actorsLineEdit = new QLineEdit ("3",tableControlsFrame);
    actorsLineEdit->setMaximumWidth(70);
    actorsLineEdit->setFixedWidth(60);
    gCLayout->addWidget(actorsLineEdit,0,1);

    dimensionsPushButton = new QPushButton("Dimensions",tableControlsFrame);
    dimensionsPushButton->setMaximumWidth(120);
    dimensionsPushButton->setFixedWidth(90);
    gCLayout->addWidget(dimensionsPushButton,1,0);

    dimensionsLineEdit = new QLineEdit ("1",tableControlsFrame);
    dimensionsLineEdit->setMaximumWidth(70);
    dimensionsLineEdit->setFixedWidth(60);
    gCLayout->addWidget(dimensionsLineEdit,1,1);

    scenarioDescriptionLineEdit = new QLineEdit(tableControlsFrame);
    //    scenarioDescriptionLineEdit->setMaximumWidth(150);
    //    scenarioDescriptionLineEdit->setFixedWidth(130);
    gCLayout->addWidget(scenarioDescriptionLineEdit,0,3);

    donePushButton = new QPushButton("Done",tableControlsFrame);
    donePushButton->setMaximumWidth(150);
    donePushButton->setFixedWidth(130);
    gCLayout->addWidget(donePushButton,1,2);

    setCentralWidget(central);

    modeltoCSV = new QStandardItemModel;

    csv_tableView = new QTableView(central); // CSV and DB
    csv_tableView->setShowGrid(true);
    csv_tableView->horizontalHeader()->viewport()->installEventFilter(this);
    csv_tableView->verticalHeader()->viewport()->installEventFilter(this);

    csv_tableWidget = new QTableWidget(central); // new CSV File
    csv_tableWidget->horizontalHeader()->viewport()->installEventFilter(this);
    csv_tableWidget->verticalHeader()->viewport()->installEventFilter(this);

    gLayout->addWidget(tableControlsFrame);
    gLayout->addWidget(turnSlider);
    gLayout->addWidget(stackWidget);

    stackWidget->addWidget(csv_tableView);
    stackWidget->addWidget(csv_tableWidget);

    tableControlsFrame->hide();
    turnSlider->hide();

    connect(actorsPushButton,&QPushButton::clicked,this, &MainWindow::insertNewRowCSV);
    connect(dimensionsPushButton,&QPushButton::clicked,this, &MainWindow::insertNewColumnCSV);
    connect(donePushButton,&QPushButton::clicked,this,&MainWindow::donePushButtonClicked);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About SMP "),
                       tr("SMP !! "));
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    //    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    //    QAction *newDoc = new QAction(newIcon, tr("&New "), this);
    //    newDoc->setShortcuts(QKeySequence::New);
    //    newDoc->setStatusTip(tr("Create new "));
    //    fileMenu->addAction(newDoc);
    //    fileToolBar->addAction(newDoc);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save..."), this);
    connect(saveAct, &QAction::triggered, this, &MainWindow::donePushButtonClicked);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save"));
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    fileMenu->addSeparator();

    const QIcon newCSVIcon = QIcon::fromTheme("Create New CSV File", QIcon(":/images/csv.png"));
    QAction *newCsvAct = new QAction(newCSVIcon, tr("&Create New CSV File"), this);
    //newCsvAct->setShortcuts(QKeySequence::Open);
    newCsvAct->setStatusTip(tr("Create New CSV File"));
    connect(newCsvAct, &QAction::triggered, this, &MainWindow::createNewCSV);
    fileMenu->addAction(newCsvAct);
    fileToolBar->addAction(newCsvAct);

    const QIcon csvIcon = QIcon::fromTheme("View/Modify Exisiting CSV File", QIcon(":/images/csv.png"));
    QAction *readCsvAct = new QAction(csvIcon, tr("&View/Modify Exisiting CSV File"), this);
    readCsvAct->setShortcuts(QKeySequence::Open);
    readCsvAct->setStatusTip(tr("View/Modify Exisiting CSV File"));
    connect(readCsvAct, &QAction::triggered, this, &MainWindow::csvGetFilePAth);
    fileMenu->addAction(readCsvAct);
    fileToolBar->addAction(readCsvAct);

    const QIcon dbIcon = QIcon::fromTheme("Import Database ", QIcon(":/images/csv.png"));
    QAction *importDBAct = new QAction(dbIcon, tr("&Import Database"), this);
    //    importDBAct->setShortcuts(QKeySequence::);
    importDBAct->setStatusTip(tr("Import Database"));
    connect(importDBAct, &QAction::triggered, this, &MainWindow::dbGetFilePAth);
    fileMenu->addAction(importDBAct);
    fileToolBar->addAction(importDBAct);

    const QIcon modDBIcon = QIcon::fromTheme("Modify Exisiting Database", QIcon(":/images/csv.png"));
    QAction *modifyDBAct = new QAction(modDBIcon, tr("&Modify Exisiting Database"), this);
    modifyDBAct->setStatusTip(tr("Modify Exisiting Database"));
    connect(modifyDBAct, &QAction::triggered, this, &MainWindow::dbGetFilePAth);
    fileMenu->addAction(modifyDBAct);
    fileToolBar->addAction(modifyDBAct);

    fileMenu->addSeparator();

    QAction *quitAct = new QAction(modDBIcon, tr("&Quit"), this);
    connect(quitAct,SIGNAL(triggered(bool)),this,SLOT(close()));
    fileMenu->addAction(quitAct);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    viewMenu = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct =new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    connect(aboutAct,SIGNAL(triggered(bool)),this,SLOT(about()));
    aboutAct->setStatusTip(tr("About SMPQ "));

}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createGraph1DockWindows()
{
    graph1Dock = new QDockWidget(tr("Graph 1"), this);
    //   graph1Dock->setAllowedAreas(Qt::LeftDockWidgetArea |Qt::RightDockWidgetArea |Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    graphWidget = new QFrame(graph1Dock);
    gridLayout = new QGridLayout(graphWidget);
    customGraph = new QCustomPlot(graphWidget);
    gridLayout->addWidget(customGraph);

    //draw a graph
    initializeGraphPlot1();
    plotGraph();

    graph1Dock->setWidget(graphWidget);
    addDockWidget(Qt::RightDockWidgetArea, graph1Dock);
    viewMenu->addAction(graph1Dock->toggleViewAction());

    connect(graph1Dock,SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),this,SLOT(dockWindowChanged()));
}

void MainWindow::createGraph2DockWindows()
{
    graph2Dock = new QDockWidget(tr("Module2"), this);
    //  graph2Dock->setAllowedAreas(Qt::LeftDockWidgetArea |Qt::RightDockWidgetArea |Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    listwidget2 = new QListWidget(graph2Dock);
    graph2Dock->setWidget(listwidget2);
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

void MainWindow::saveTableViewToCSV()
{
    QString saveFilePath = QFileDialog::getSaveFileName(this,tr("Save File to "), QDir::homePath(),tr("CSV File (*.csv)"));
    if( __linux__ )
        saveFilePath.append(".csv");

    QFile f(saveFilePath);

    if (f.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream data( &f );
        QStringList strList;

        strList <<scenarioComboBox->currentText();
        strList <<scenarioDescriptionLineEdit->text().trimmed();
        strList <<QString::number(modeltoCSV->rowCount());
        strList <<QString::number((modeltoCSV->columnCount()-3)/2);

        data << strList.join(",") << ","<< "\n";
        strList.clear();

        //Appending a header
        for( int col = 0; col < modeltoCSV->columnCount(); ++col )
        {
            strList <<modeltoCSV->horizontalHeaderItem(col)->data(Qt::DisplayRole).toString();
        }
        data << strList.join(",") << "," <<"\n";

        //appending data
        for (int row=0; row<modeltoCSV->rowCount(); row++)
        {
            strList.clear();

            for (int col=0; col<modeltoCSV->columnCount(); col++)
            {

                //NOTE : uncomment if strict checking is required
                //                if(!modeltoCSV->data(modeltoCSV->index(row,col)).toString().isEmpty())
                strList << modeltoCSV->data(modeltoCSV->index(row,col)).toString();
                //                else
                //                {    QMessageBox::about(0,"No Data Entered","Enter Data at Row : "
                //                                        + QString::number(row+1) + ", Column : " + QString::number(col+1));
                //                    return;
                //                }
            }
            data << strList.join(",") + "\n";
        }
        f.close();
    }
}

void MainWindow::saveTableWidgetToCSV()
{
    QString saveFilePath = QFileDialog::getSaveFileName(this,tr("Save File to "), QDir::homePath(),tr("CSV File (*.csv)"));
    if( __linux__ )
        saveFilePath.append(".csv");
    QFile f(saveFilePath);

    if (f.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream data( &f );
        QStringList strList;

        strList <<scenarioComboBox->currentText();
        strList <<scenarioDescriptionLineEdit->text();
        strList <<QString::number(csv_tableWidget->rowCount());
        strList <<QString::number((csv_tableWidget->columnCount()-3)/2);

        data << strList.join(",") << ","<< "\n";
        strList.clear();

        //Appending a header
        for( int col = 0; col < csv_tableWidget->columnCount(); ++col )
        {
            strList << csv_tableWidget->horizontalHeaderItem(col)->data(Qt::DisplayRole).toString();
        }
        data << strList.join(",") << "," <<"\n";

        for( int row = 0; row < csv_tableWidget->rowCount(); ++row )
        {
            strList.clear();
            for( int column = 0; column < csv_tableWidget->columnCount(); ++column )
            {
                QTableWidgetItem* item = csv_tableWidget->item(row,column);

                if( !item  || item->text().isEmpty() )
                {
                    QMessageBox::about(0,"No Data Entered","Enter Data at Row : " + QString::number(row+1) + ", Column : " + QString::number(column+1));
                    return;
                }
                else
                    strList << " "+csv_tableWidget->item(row,column)->text()+" ";
            }
            data << strList.join( "," ) << ","  << "\n";
        }
        f.close();
    }
}

void MainWindow::initializeGraphPlot1()
{
    customGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                 QCP::iSelectLegend | QCP::iSelectPlottables);
    customGraph->xAxis->setRange(0, 10);
    customGraph->yAxis->setRange(0,100);
    customGraph->axisRect()->setupFullAxesBox();

    customGraph->plotLayout()->insertRow(0);
    customGraph->plotLayout()->addElement(0, 0, new QCPPlotTitle(customGraph, "Competitive vs Time "));

    customGraph->xAxis->setLabel("Time");
    customGraph->yAxis->setLabel("Competitive");
    customGraph->legend->setVisible(true);

    QFont legendFont = font();
    legendFont.setPointSize(10);
    customGraph->legend->setFont(legendFont);
    customGraph->legend->setSelectedFont(legendFont);
    customGraph->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    connect(customGraph, SIGNAL(legendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(selectionChanged()));
    // connect slot that ties some axis selections together (especially opposite axes):
    connect(customGraph, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(customGraph, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(customGraph, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(customGraph->xAxis, SIGNAL(rangeChanged(QCPRange)), customGraph->xAxis2, SLOT(setRange(QCPRange)));
    connect(customGraph->yAxis, SIGNAL(rangeChanged(QCPRange)), customGraph->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(customGraph, SIGNAL(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)), this, SLOT(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)));
    connect(customGraph, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(customGraph, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(customGraph, SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*)));

    // setup policy and connect slot for context menu popup:
    customGraph->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(customGraph, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
}

void MainWindow::plotGraph()
{
    //    addGraphOnModule1(0,0);
    //    addGraphOnModule1(1,0);
    //    addGraphOnModule1(2,0);
    //    addGraphOnModule1(3,0);
    //    addGraphOnModule1(4,0);
    //    addGraphOnModule1(5,0);
    //    addGraphOnModule1(6,0);
    //    addGraphOnModule1(7,0);
}


bool MainWindow::eventFilter(QObject* object, QEvent* event)
{

    if ((object == csv_tableView->horizontalHeader()->viewport() ||
         object == csv_tableView->verticalHeader()->viewport()) &&
            event->type() == QEvent::MouseButtonDblClick)
    {
        if (header_editor)
        { //delete previous editor just in case
            header_editor->deleteLater();
            header_editor = 0;
        }
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        QHeaderView* header = static_cast<QHeaderView*>(object->parent());
        int mouse_pos = header->orientation() == Qt::Horizontal ? e->x() : e->y();
        int logical_index = header->logicalIndex(header->visualIndexAt(mouse_pos));

        if (logical_index >= 0)
        {
            // if mouse is over an item
            QRect rect; // line edit rect in header's viewport's coordinates

            if (header->orientation() == Qt::Horizontal)
            {
                rect.setLeft(header->sectionPosition(logical_index));
                rect.setWidth(header->sectionSize(logical_index));
                rect.setTop(0);
                rect.setHeight(header->height());
            }
            else
            {
                rect.setTop(header->sectionPosition(logical_index));
                rect.setHeight(header->sectionSize(logical_index));
                rect.setLeft(0);
                rect.setWidth(header->width());
            }
            rect.adjust(1, 1, -1, -1);

            header_editor = new QLineEdit(header->viewport());
            header_editor->move(rect.topLeft());
            header_editor->resize(rect.size());
            header_editor->setFrame(false);//get current item text

            QString text = header->model()->headerData(logical_index, header->orientation()).toString();

            header_editor->setText(text);
            header_editor->setFocus();
            editor_index = logical_index; //save for future use
            header_editor->installEventFilter(this); //catch focus out event

            //if user presses Enter it should close editor
            connect(header_editor, SIGNAL(returnPressed()),csv_tableView, SLOT(setFocus()));
            header_editor->show();
        }
        return true; // filter out event
    }
    else if ((object == csv_tableWidget->horizontalHeader()->viewport() ||
              object == csv_tableWidget->verticalHeader()->viewport()) &&
             event->type() == QEvent::MouseButtonDblClick)
    {
        if (header_editor)
        { //delete previous editor just in case
            header_editor->deleteLater();
            header_editor = 0;
        }
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        QHeaderView* header = static_cast<QHeaderView*>(object->parent());
        int mouse_pos = header->orientation() == Qt::Horizontal ? e->x() : e->y();
        int logical_index = header->logicalIndex(header->visualIndexAt(mouse_pos));

        if (logical_index >= 0)
        {
            // if mouse is over an item
            QRect rect; // line edit rect in header's viewport's coordinates

            if (header->orientation() == Qt::Horizontal)
            {
                rect.setLeft(header->sectionPosition(logical_index));
                rect.setWidth(header->sectionSize(logical_index));
                rect.setTop(0);
                rect.setHeight(header->height());
            }
            else
            {
                rect.setTop(header->sectionPosition(logical_index));
                rect.setHeight(header->sectionSize(logical_index));
                rect.setLeft(0);
                rect.setWidth(header->width());
            }
            rect.adjust(1, 1, -1, -1);

            header_editor = new QLineEdit(header->viewport());
            header_editor->move(rect.topLeft());
            header_editor->resize(rect.size());
            header_editor->setFrame(false);//get current item text

            QString text = header->model()->headerData(logical_index, header->orientation()).toString();

            header_editor->setText(text);
            header_editor->setFocus();
            editor_index = logical_index; //save for future use
            header_editor->installEventFilter(this); //catch focus out event

            //if user presses Enter it should close editor
            connect(header_editor, SIGNAL(returnPressed()),csv_tableWidget, SLOT(setFocus()));
            header_editor->show();
        }
        return true; // filter out event
    }

    else if (object == header_editor && event->type() == QEvent::FocusOut)
    {
        QHeaderView* header = static_cast<QHeaderView*>(header_editor->parentWidget()->parentWidget());

        //save item text
        header->model()->setHeaderData(editor_index, header->orientation(),header_editor->text());
        header_editor->deleteLater(); //safely delete editor
        header_editor = 0;
    }
    return false;
}



void MainWindow::titleDoubleClick(QMouseEvent* event, QCPPlotTitle* title)
{
    Q_UNUSED(event)
    // Set the plot title by double clicking on it
    bool ok;
    QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
    if (ok)
    {
        title->setText(newTitle);
        customGraph->replot();
    }
}

void MainWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
    // Set an axis label by double clicking on it
    if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
    {
        bool ok;
        QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
        if (ok)
        {
            axis->setLabel(newLabel);
            customGraph->replot();
        }
    }
}

void MainWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
    // Rename a graph by double clicking on its legend item

    Q_UNUSED(legend)
    if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
    {
        QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
        bool ok;
        QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
        if (ok)
        {
            plItem->plottable()->setName(newName);
            customGraph->replot();
        }
    }
}

void MainWindow::selectionChanged()
{
    // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (customGraph->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || customGraph->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            customGraph->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customGraph->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        customGraph->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        customGraph->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (customGraph->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || customGraph->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            customGraph->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customGraph->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        customGraph->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        customGraph->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<customGraph->graphCount(); ++i)
    {
        QCPGraph *graph = customGraph->graph(i);
        QCPPlottableLegendItem *item = customGraph->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            graph->setSelected(true);
        }
    }
}

void MainWindow::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (customGraph->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        customGraph->axisRect()->setRangeDrag(customGraph->xAxis->orientation());
    else if (customGraph->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        customGraph->axisRect()->setRangeDrag(customGraph->yAxis->orientation());
    else
        customGraph->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (customGraph->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        customGraph->axisRect()->setRangeZoom(customGraph->xAxis->orientation());
    else if (customGraph->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        customGraph->axisRect()->setRangeZoom(customGraph->yAxis->orientation());
    else
        customGraph->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::addGraphOnModule1(const QVector<double> &x, const QVector<double> &y,QString Actor)
{
    {//    QSqlQuery qry;
        //    QString query= QString("select * from VectorPosition where Act_i='%1' and Dim_k='%2' ").arg(act).arg(dim);

        //    int n=0;
        //    qry.exec(query);

        //    while(qry.next())
        //    {
        //        n++;
        //        qDebug()<<"n" << n;
        //    }

        //    QVector<double> x(n), y(n);
        //    int actornumber=0;
        //    int i =0;
        //    query= QString("select * from VectorPosition where Act_i='%1' and Dim_k='%2' ").arg(act).arg(dim);
        //    qry.exec(query);
        //    while(qry.next())
        //    {
        //        qDebug()<<qry.value(1).toDouble() << "    " <<qry.value(4).toDouble();
        //        x[i]=qry.value(1).toDouble();
        //        y[i]=qry.value(4).toDouble()*100;
        //        actornumber=qry.value(2).toInt();
        //        ++i;
        //    }

        //    customGraph->addGraph();
        //    customGraph->graph()->setPen(QPen(Qt::blue));
        //    customGraph->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));
        //    customGraph->addGraph();
        //    customGraph->graph()->setPen(QPen(Qt::red));
        //    QVector<double> x(500), y0(500), y1(500);

        //    for (int i=0; i<n; ++i)
        //    {
        //        //     x[i] = (i/n-0.5)*10;
        //        //     y[i] = qExp(-x[i]*x[i]*0.25)*qSin(x[i]*5)*5;
        //        //      y[i] = qExp(-x[i]*x[i]*0.25)*5;
        //    }
        //    customGraph->graph(0)->setData(x, y);
        //    //  customGraph->graph(1)->setData(x, y1);
        //    customGraph->axisRect()->setupFullAxesBox(true);
        //    customGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

        //    QVector<double> x(500), y0(500), y1(500);


        //    for (int i=0; i<n; ++i)
        //    {
        //      x[i] = (i/n-1-0.5)*10;
        //      y[i] = qExp(-x[i]*x[i]*0.25)*qSin(x[i]*5)*5;
        //    }
        //    customGraph->graph()->setData(x, y);
        //   // customGraph->graph(1)->setData(x, y);
        //    customGraph->axisRect()->setupFullAxesBox(true);
        //    customGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

        //        int n = 50; // number of points in graph
        //    double xScale = (rand()/(double)RAND_MAX + 0.5)*2;
        //    double yScale = (rand()/(double)RAND_MAX + 0.5)*2;
        //    double xOffset = (rand()/(double)RAND_MAX - 0.5)*4;
        //    double yOffset = (rand()/(double)RAND_MAX - 0.5)*5;
        //    double r1 = (rand()/(double)RAND_MAX - 0.5)*2;
        //    double r2 = (rand()/(double)RAND_MAX - 0.5)*2;
        //    double r3 = (rand()/(double)RAND_MAX - 0.5)*2;
        //    double r4 = (rand()/(double)RAND_MAX - 0.5)*2;
        //        QVector<double> x(n), y(n);

        //        for (int i=0; i<n; i++)
        //        {
        //            x[i] = (i/(double)n-0.5)*10.0*xScale + xOffset;
        //            y[i] = (qSin(x[i]*r1*5)*qSin(qCos(x[i]*r2)*r4*3)+r3*qCos(qSin(x[i])*r4*2))*yScale + yOffset;
        //        }
    }

    customGraph->addGraph();
    customGraph->graph()->setName(Actor);
    customGraph->graph()->setData(x, y);
    customGraph->graph()->setLineStyle(((QCPGraph::LineStyle)(1)));//upto 5

    //  if (rand()%100 > 50)
    customGraph->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(9)));

    QPen graphPen;
    graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
    graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);

    customGraph->graph()->setPen(graphPen);
    customGraph->replot();

}

void MainWindow::removeSelectedGraph()
{
    if (customGraph->selectedGraphs().size() > 0)
    {
        customGraph->removeGraph(customGraph->selectedGraphs().first());
        customGraph->replot();
    }
}

void MainWindow::removeAllGraphs()
{
    customGraph->clearGraphs();
    customGraph->replot();
}

void MainWindow::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (customGraph->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
        menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
        menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
        menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
        menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
        menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
    } else  // general context menu on graphs requested
    {
        menu->addAction("Add random graph", this, SLOT(addGraphOnModule1()));
        if (customGraph->selectedGraphs().size() > 0)
            menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
        if (customGraph->graphCount() > 0)
            menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
    }

    menu->popup(customGraph->mapToGlobal(pos));
}

void MainWindow::moveLegend()
{
    if (QAction * contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok)
        {
            customGraph->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
            customGraph->replot();
        }
    }
}

void MainWindow::graphClicked(QCPAbstractPlottable *plottable)
{
    //statusBar->showMessage(QString("Clicked on graph '%1'.").arg(plottable->name()), 1000);
}

