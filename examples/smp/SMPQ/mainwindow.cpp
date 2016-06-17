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
    connect(dbObj,&Database::Message,this,&MainWindow::DisplayMessage);
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
    scenarioComboBox->clear();
    for(int index=0;index<scenarios->length();++index)
        scenarioComboBox->addItem(scenarios->at(index));
}

void MainWindow::sliderStateValueToQryDB(int value)
{
    removeAllGraphs();
    emit getScenarioRunValues(value,scenario_box);
}

void MainWindow::scenarioComboBoxValue(QString scenario)
{
    removeAllGraphs();
    turnSlider->setValue(0);
    scenario_box = scenario;
}

void MainWindow::DisplayMessage(QString cls, QString message)
{
    QMessageBox::warning(0,cls,message);
}

void MainWindow::VectorPositionsFromDB()
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

void MainWindow::setCSVItemModel(QStandardItemModel *model)
{
    //update received model to widget
    csv_tableView->setModel(model);
    csv_tableView->showMaximized();
    csv_tableView->setAlternatingRowColors(true);
    csv_tableView->resizeRowsToContents();

    //    for (int c = 0; c < csv_tableView->horizontalHeader()->count(); ++c)
    //        csv_tableView->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);

    csv_tableView->resizeColumnsToContents();
    csv_tableView->hideColumn(0);

    //Enable run button, which is disabled by default
    runButton->setEnabled(true);
}

void MainWindow::setDBItemModel(QSqlTableModel *model)
{
    //update received model to widget

    csv_tableView->setModel(model);
    csv_tableView->showMaximized();
    csv_tableView->setAlternatingRowColors(true);
    csv_tableView->resizeRowsToContents();

    //    for (int c = 0; c < csv_tableView->horizontalHeader()->count(); ++c)
    //        csv_tableView->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);

    csv_tableView->resizeColumnsToContents();
    csv_tableView->hideColumn(0);

    //Enable run button, which is disabled by default
    runButton->setEnabled(true);
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

    fileMenu->addSeparator();

    const QIcon csvIcon = QIcon::fromTheme("Read CSV File", QIcon(":/images/csv.png"));
    QAction *readCsvAct = new QAction(csvIcon, tr("&Read CSV"), this);
    readCsvAct->setShortcuts(QKeySequence::Open);
    readCsvAct->setStatusTip(tr("Read a CSV File"));
    connect(readCsvAct, &QAction::triggered, this, &MainWindow::csvGetFilePAth);
    fileMenu->addAction(readCsvAct);
    fileToolBar->addAction(readCsvAct);

    const QIcon dbIcon = QIcon::fromTheme("Import Database ", QIcon(":/images/csv.png"));
    QAction *importDBAct = new QAction(dbIcon, tr("&Import Database"), this);
    //    importDBAct->setShortcuts(QKeySequence::);
    importDBAct->setStatusTip(tr("Import a database"));
    connect(importDBAct, &QAction::triggered, this, &MainWindow::dbGetFilePAth);
    fileMenu->addAction(importDBAct);
    fileToolBar->addAction(importDBAct);

    fileMenu->addSeparator();

    QAction *quitAct = new QAction(dbIcon, tr("&Quit"), this);
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

void MainWindow::initializeCentralViewFrame()
{

    central = new QFrame;
    central->setFrameShape(QFrame::StyledPanel);

    QGridLayout *gLayout = new QGridLayout;
    central->setLayout(gLayout);

    tableControlsFrame = new QFrame;
    tableControlsFrame->setFrameShape(QFrame::StyledPanel);

    QGridLayout *gCLayout = new QGridLayout(tableControlsFrame);

    scenarioComboBox = new QComboBox(tableControlsFrame);
    //    scenarioComboBox->setMinimumWidth(40);
    //    scenarioComboBox->setMaximumWidth(70);
    //    scenarioComboBox->setFixedWidth(60);
    gCLayout->addWidget(scenarioComboBox,0,2);
    connect(scenarioComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(scenarioComboBoxValue(QString)));


    turnSlider = new QSlider(Qt::Horizontal,tableControlsFrame);
    //    turnSlider->setMinimumWidth(40);
    //    turnSlider->setMaximumWidth(70);
    //    turnSlider->setFixedWidth(60);
    //    turnSlider->setTickInterval(1);
    turnSlider->setPageStep(1);
    gCLayout->addWidget(turnSlider,1,2);

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

    scenarioNewLineEdit = new QLineEdit(tableControlsFrame);
    scenarioNewLineEdit->setMaximumWidth(120);
    scenarioNewLineEdit->setFixedWidth(90);
    scenarioNewLineEdit->setHidden(true);
    gCLayout->addWidget(scenarioNewLineEdit,0,3);

    donePushButton = new QPushButton("Done",tableControlsFrame);
    donePushButton->setMaximumWidth(120);
    donePushButton->setFixedWidth(90);
    gCLayout->addWidget(donePushButton,1,3);

    csv_tableView = new QTableView(central);
    csv_tableView->setShowGrid(true);

    gLayout->addWidget(tableControlsFrame);
    gLayout->addWidget(csv_tableView);

    setCentralWidget(central);
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

