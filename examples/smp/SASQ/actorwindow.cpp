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

#include "actorwindow.h"


ActorFrame::ActorFrame(QObject *parent)
{
    frameMainLayout = new QGridLayout(this);
    intializeFrameLayout();
    setLayout(frameMainLayout);
}

ActorFrame::~ActorFrame()
{

}

void ActorFrame::intializeFrameLayout()
{
    QFrame * topFrame = new QFrame;
    QGridLayout * topGridLayout = new QGridLayout;

    QFrame * bottomFrame = new QFrame;
    QGridLayout * bottomGridLayout = new QGridLayout;

    QSplitter *splitterTopH = new  QSplitter(Qt::Horizontal,this);
    QSplitter *splitterBottomH = new  QSplitter(Qt::Horizontal,this);
    QSplitter *splitterV = new  QSplitter(Qt::Vertical,this);

    actorInputTableFrame = new QFrame;
    actorSensTableFrame = new  QFrame;
    actorSpecsFrame = new QFrame;
    actorControlsFrame = new QFrame;

    actorInputGridLayout = new QGridLayout;
    actorSensGridLayout = new QGridLayout;
    actorSpecsGridLayout = new QGridLayout;
    actorControlsGridLayout = new QGridLayout;

    actorInputTableFrame->setFrameStyle(QFrame::StyledPanel);
    actorSensTableFrame->setFrameStyle(QFrame::StyledPanel);
    actorSpecsFrame->setFrameStyle(QFrame::StyledPanel);
    actorControlsFrame->setFrameStyle(QFrame::StyledPanel);

    initializeInputDataTable();
    initializeSASDataGrid();
    initializeSpecificationsTypeButtons();
    initializeSpecificationsList();

    actorInputTableFrame->setLayout(actorInputGridLayout);
    actorSensTableFrame->setLayout(actorSensGridLayout);
    actorControlsFrame->setLayout(actorControlsGridLayout);
    actorSpecsFrame->setLayout(actorSpecsGridLayout);

    splitterTopH->addWidget(actorInputTableFrame);
    splitterTopH->addWidget(actorSensTableFrame);
    splitterTopH->setChildrenCollapsible(false);

    splitterBottomH->addWidget(actorSpecsFrame);
    splitterBottomH->addWidget(actorControlsFrame);
    splitterBottomH->setChildrenCollapsible(false);

    topGridLayout->addWidget(splitterTopH);
    topFrame->setLayout(topGridLayout);

    bottomGridLayout->addWidget(splitterBottomH);
    bottomFrame->setLayout(bottomGridLayout);

    splitterV->addWidget(topFrame);
    splitterV->addWidget(bottomFrame);
    splitterV->setChildrenCollapsible(false);

    frameMainLayout->addWidget(splitterV);
    setFrameStyle(QFrame::StyledPanel);

}

void ActorFrame::initializeInputDataTable()
{
    inputDataTabWidget = new QTabWidget;
    actorDataTableView = new QTableView;
    accomodationMatrixTableView = new QTableView;
    csvActorDataModel = new QStandardItemModel;
    xmlActorDataModel = new QStandardItemModel;
    sasDataGridTableWidget = new QTableWidget;
    sasDataGridTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(sasDataGridTableWidget,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(sasDataGridContextMenuRequested(QPoint)));

    inputDataTabWidget->addTab(actorDataTableView,"Actor Data");
    inputDataTabWidget->addTab(accomodationMatrixTableView,"Accomodation Matrix");

    actorInputGridLayout->addWidget(inputDataTabWidget);

}

void ActorFrame::initializeAccomodationMatrix(QString type)
{
    accModel = new QStandardItemModel;
    accModel->setRowCount(0);
    accModel->setColumnCount(0);
    accomodationMatrixTableView->setModel(accModel);

    int rowCount =0 ;
    int colCount =0 ;
    if(type == "CSV")
    {
        rowCount=csvActorDataModel->rowCount();
        colCount=csvActorDataModel->columnCount();
    }
    else if (type == "XML")
    {
        rowCount=xmlActorDataModel->rowCount();
        colCount=xmlActorDataModel->columnCount();

    }
    //Accomodation Matrix
    while(accModel->rowCount() != rowCount && rowCount > accModel->rowCount())
    {
        int rowCount1 = accModel->rowCount();
        if(accModel->rowCount() < rowCount || accModel->columnCount() < rowCount)
        {
            accModel->insertRows(accModel->rowCount(),(rowCount - accModel->rowCount()));
            accModel->insertColumns(accModel->columnCount(),(rowCount - accModel->columnCount()));
        }
        QString actorHeader;
        for(int rows = rowCount1; rows < accModel->rowCount();++ rows)
        {
            if(type == "CSV")
            {
                actorHeader=csvActorDataModel->item(rows,0)->text().trimmed();
            }
            else if (type == "XML")
            {
                actorHeader=xmlActorDataModel->item(rows,0)->text().trimmed();
            }

            accModel->setHorizontalHeaderItem(rows, new QStandardItem(actorHeader));
            accModel->setVerticalHeaderItem(rows, new QStandardItem(actorHeader));
            initializeAccomodationRowCol(rows,"CSV");
            actorHeader.clear();
        }
    }
}

void ActorFrame::initializeAccomodationRowCol(int count, QString tableType)
{
    if ("CSV"==tableType)
    {
        for( int col = 0; col < accModel->columnCount(); ++col)
        {
            if(count==col)
                accModel->setItem(count,col,new QStandardItem("1"));
            else
                accModel->setItem(count,col,new QStandardItem("0"));
        }

        for(int row =0; row < accModel->rowCount() ; ++row)
        {
            if(count==row)
                accModel->setItem(row,count,new QStandardItem("1"));
            else
                accModel->setItem(row,count,new QStandardItem("0"));
        }
    }
}

void ActorFrame::setActorTableModel(QStandardItemModel *model, QStringList scenarioList)
{
    csvActorDataModel=model;
    actorDataTableView->setModel(csvActorDataModel);

    initializeAccomodationMatrix("CSV");
    intitalizeSasGridColumn();
}

void ActorFrame::setAccTableModel(QStandardItemModel *model,
                                  QList<QStringList> idealAdjustmentList,QStringList dimensionsXml)
{
    xmlActorDataModel=model;
    actorDataTableView->setModel(xmlActorDataModel);

    QVector <QString> actors ;

    for(int act =0 ; act < xmlActorDataModel->rowCount(); ++act)
    {
        actors.append(xmlActorDataModel->item(act)->text());
    }

    if(idealAdjustmentList.isEmpty())
    {
        initializeAccomodationMatrix("XML");
    }
    else
    {
        populateAccomodationMatrix(idealAdjustmentList,actors);
    }

    QStringList xmlTableHeaders;
    xmlTableHeaders<< "Actor" << "Description" << "Power"
                   << "Position " << "Salience ";

    for(int col =0 ; col <3;++col)
    {
        xmlActorDataModel->setHeaderData(col,Qt::Horizontal,xmlTableHeaders.at(col));
    }

    int dim=0;
    for(int col = 3 ; col <xmlActorDataModel->columnCount(); col+=2)
    {
        QString pos(" \n Position ");
        QString sal(" \n Salience ");

        pos.prepend(dimensionsXml.at(dim));
        sal.prepend(dimensionsXml.at(dim));

        xmlActorDataModel->setHeaderData(col,Qt::Horizontal,pos);
        xmlActorDataModel->setHeaderData(col+1,Qt::Horizontal,sal);

        ++dim;
    }
    intitalizeSasGridColumn();
}


void ActorFrame::populateAccomodationMatrix(QList<QStringList> idealAdj, QVector<QString> actors)
{
    QStandardItemModel * accModel = new QStandardItemModel;

    for(int act = 0 ; act < actors.length() ; ++act)
    {
        accModel->setHorizontalHeaderItem(act,new QStandardItem(actors.at(act)));
        accModel->setVerticalHeaderItem(act,new QStandardItem(actors.at(act)));

        for(int actH =0 ; actH <actors.length(); ++actH)
        {
            if(act==actH)
                accModel->setItem(act,actH, new QStandardItem("1"));
            else
                accModel->setItem(act,actH, new QStandardItem("0"));
        }
    }

    for(int i=0; i< idealAdj.length();++i)
    {
        accModel->setItem(actors.indexOf(idealAdj.at(i).at(1)),
                          actors.indexOf(idealAdj.at(i).at(0)),new QStandardItem(idealAdj.at(i).at(2)));
    }

    accomodationMatrixTableView->setModel(accModel);
    accomodationMatrixTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    accomodationMatrixTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    //    connect(xmlSmpDataModel,SIGNAL(itemChanged(QStandardItem*)),this, SLOT(xmlCellSelected(QStandardItem*)));
    //    connect(xmlAffinityMatrixTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayMenuXmlAffTableView(QPoint)));

}

void ActorFrame::initializeSASDataGrid()
{
    sasDataGridTableWidget->setShowGrid(true);
    sasDataGridTableWidget->verticalHeader()->hide();
    actorSensGridLayout->addWidget(sasDataGridTableWidget);
}

void ActorFrame::intitalizeSasGridColumn()
{
    QComboBox * actorComboBox = new QComboBox;
    sasDataGridTableWidget->setRowCount(0);
    sasDataGridTableWidget->setColumnCount(0);
    sasDataGridTableWidget->insertColumn(0);
    sasDataGridTableWidget->insertRow(0);
    sasDataGridTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem(" "));
    sasDataGridTableWidget->horizontalHeader()->setMaximumHeight(5);


    for(int act =0; act < actorDataTableView->model()->rowCount(); ++act)
    {
        actorComboBox->addItem(actorDataTableView->model()->index(act,0).data().toString());
    }
    sasDataGridTableWidget->setCellWidget(0,0,actorComboBox);
    connect(actorComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(actorComboBoxChanged(QString)));

    int row =1;
    for( int col =2; col < actorDataTableView->model()->columnCount(); ++col)
    {
        QTableWidgetItem * item = new QTableWidgetItem( actorDataTableView->model()
                                                        ->headerData(col,Qt::Horizontal).toString());
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        sasDataGridTableWidget->insertRow(row);
        sasDataGridTableWidget->setItem(row,0,item);
        row++;
    }

    row = sasDataGridTableWidget->rowCount();
    for(int actIndex = 0 ; actIndex < accomodationMatrixTableView->model()->rowCount(); ++actIndex)
    {
        QString headerItem = actorDataTableView->model()->index(actIndex,0).data().toString();
        headerItem.prepend(QString(actorDataTableView->model()->index(0,0).data().toString() + " <> "));

        QTableWidgetItem * item = new QTableWidgetItem(headerItem);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        sasDataGridTableWidget->insertRow(row);
        sasDataGridTableWidget->setItem(row,0,item);
        row++;
    }

    minDeltaMaxRadioButton->click();
//    minDeltaMaxRadioButtonClicked(true);
}

void ActorFrame::initializeSpecificationsTypeButtons()
{
    QGroupBox * specsTypeBox = new QGroupBox("Specifications Type");
    QVBoxLayout * specsTypeLayout = new QVBoxLayout;

    minDeltaMaxRadioButton = new QRadioButton(" (Min, Delta, Max) ");
    QRadioButton * basePMRadioButton = new QRadioButton(" (Base, ±) ");
    QRadioButton * basePMPRadioButton = new QRadioButton(" (Base, ±%) ");
    valueRadioButton = new QRadioButton(" (Value1, Value2, ...) ");

    minDeltaMaxRadioButton->setChecked(true);
    connect(minDeltaMaxRadioButton,SIGNAL(clicked(bool)),this,SLOT(minDeltaMaxRadioButtonClicked(bool)));
    connect(basePMRadioButton,SIGNAL(clicked(bool)),this,SLOT(basePMRadioButtonClicked(bool)));
    connect(basePMPRadioButton,SIGNAL(clicked(bool)),this,SLOT(basePMPRadioButtonClicked(bool)));
    connect(valueRadioButton,SIGNAL(clicked(bool)),this,SLOT(valueRadioButtonClicked(bool)));

    QPushButton * addBasePushButton = new QPushButton("Add (Base, ±) for all Actors");
    connect(addBasePushButton,SIGNAL(clicked(bool)),this,SLOT(addBasePushButtonClicked(bool)));

    QPushButton * addSpecsPushButton = new QPushButton("Add Specification(s)");
    connect(addSpecsPushButton,SIGNAL(clicked(bool)),this,SLOT(addSpecClicked(bool)));

    specsTypeLayout->addWidget(minDeltaMaxRadioButton);
    specsTypeLayout->addWidget(basePMRadioButton);
    specsTypeLayout->addWidget(basePMPRadioButton);
    specsTypeLayout->addWidget(valueRadioButton);
    specsTypeBox->setLayout(specsTypeLayout);

    actorControlsGridLayout->addWidget(specsTypeBox,0,0);
    actorControlsGridLayout->addWidget(addBasePushButton,1,0);
    actorControlsGridLayout->addWidget(addSpecsPushButton,2,0);

}

void ActorFrame::initializeSpecificationsList()
{
    specsListView = new QListView;
    specsListView->setAutoScroll(true);
    connect(specsListView,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(actorListViewContextMenu(QPoint)));
    specsListView->setContextMenuPolicy(Qt::CustomContextMenu);

    specsListModel = new QStandardItemModel();

    specsListView->setModel(specsListModel);

    actorSpecsGridLayout->addWidget(specsListView);

}

void ActorFrame::actorComboBoxChanged(QString  index)
{
    int row = actorDataTableView->model()->columnCount()-1;
    while(sasDataGridTableWidget->rowCount()!=row)
    {
        sasDataGridTableWidget->removeRow(sasDataGridTableWidget->rowCount()-1);
    }

    for(int actIndex = 0 ; actIndex < accomodationMatrixTableView->model()->rowCount(); ++actIndex)
    {
        QString headerItem = actorDataTableView->model()->index(actIndex,0).data().toString();
        headerItem.prepend(QString(index + " <> "));
        QTableWidgetItem * item = new QTableWidgetItem(headerItem);

        sasDataGridTableWidget->insertRow(row);
        sasDataGridTableWidget->setItem(row,0,item);
        row++;
    }
}

void ActorFrame::actorListViewContextMenu(QPoint pos)
{
    if(specsListView->model()->rowCount()>0)
    {
        QMenu *menu = new QMenu(this);
        menu->addAction("Remove Selected Items", this, SLOT(listViewClicked()));
        menu->popup(specsListView->mapToGlobal(pos));
    }
}

void ActorFrame::listViewClicked()
{
    for(int index=0; index < specsListModel->rowCount();++index)
    {
        QStandardItem * item = specsListModel->item(index);
        if (item->data(Qt::CheckStateRole).toBool() == true)   // Item checked, remove
        {
            specsListModel->removeRow(index);
            index = index -1; // index changed to current row, deletion of item changes list index
        }
    }
}

void ActorFrame::addSpecClicked(bool bl)
{
    int count = 0;
    QString specification;
    //    specification.append(parameterName->currentText()).append("=(");
    //    for(int para = 0 ; para < parametersCheckBoxList.length(); ++ para)
    //    {
    //        if(true==parametersCheckBoxList.at(para)->isChecked())
    //        {
    //            specification.append(parametersCheckBoxList.at(para)->text()).append(",");
    count++;
    //        }
    //    }
    specification.append(")");
    specification.remove(",)").append(")");

    if(count>0)
    {
        QStandardItem *item = new QStandardItem(specification);
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setEditable(false);
        specsListModel->setItem(specsListModel->rowCount(),item);
        specsListView->scrollToBottom();
    }
}

void ActorFrame::sasDataGridContextMenuRequested(QPoint pos)
{
    if(true==valueRadioButton->isChecked())
    {
        QMenu *menu = new QMenu(this);
        menu->addAction("Add a new Column", this, SLOT(addValueColumn()));
        menu->popup(sasDataGridTableWidget->mapToGlobal(pos));
    }
}

void ActorFrame::addValueColumn()
{
    sasDataGridTableWidget->insertColumn(sasDataGridTableWidget->columnCount());
    sasDataGridTableWidget->setItem(0,sasDataGridTableWidget->columnCount()-1
                                    ,new QTableWidgetItem(
                                        QString("Val "+QString::number(sasDataGridTableWidget->columnCount()-1))));
    sasDataGridTableWidget->item(0,sasDataGridTableWidget->columnCount()-1)
            ->setBackgroundColor(QColor::fromRgb(192,192,192,135));
    sasDataGridTableWidget->item(0,sasDataGridTableWidget->columnCount()-1)
            ->setTextAlignment(Qt::AlignCenter);
}

void ActorFrame::minDeltaMaxRadioButtonClicked(bool bl)
{

    while(sasDataGridTableWidget->columnCount()>1)
    {
        sasDataGridTableWidget->removeColumn(sasDataGridTableWidget->columnCount()-1);
    }

    sasDataGridTableWidget->insertColumn(1);
    sasDataGridTableWidget->setItem(0,1,new QTableWidgetItem("Min"));
    sasDataGridTableWidget->item(0,1)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
    sasDataGridTableWidget->item(0,1)->setTextAlignment(Qt::AlignCenter);
    sasDataGridTableWidget->insertColumn(2);
    sasDataGridTableWidget->setItem(0,2,new QTableWidgetItem("Delta"));
    sasDataGridTableWidget->item(0,2)->setTextAlignment(Qt::AlignCenter);
    sasDataGridTableWidget->item(0,2)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
    sasDataGridTableWidget->insertColumn(3);
    sasDataGridTableWidget->setItem(0,3,new QTableWidgetItem("Max"));
    sasDataGridTableWidget->item(0,3)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
    sasDataGridTableWidget->item(0,3)->setTextAlignment(Qt::AlignCenter);

}

void ActorFrame::basePMRadioButtonClicked(bool bl)
{
    if(true==bl)
    {
        while(sasDataGridTableWidget->columnCount()>1)
        {
            sasDataGridTableWidget->removeColumn(sasDataGridTableWidget->columnCount()-1);
        }

        sasDataGridTableWidget->insertColumn(1);
        sasDataGridTableWidget->setItem(0,1,new QTableWidgetItem("Base"));
        sasDataGridTableWidget->item(0,1)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
        sasDataGridTableWidget->item(0,1)->setTextAlignment(Qt::AlignCenter);
        sasDataGridTableWidget->insertColumn(2);
        sasDataGridTableWidget->setItem(0,2,new QTableWidgetItem("±"));
        sasDataGridTableWidget->item(0,2)->setTextAlignment(Qt::AlignCenter);
        sasDataGridTableWidget->item(0,2)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
    }
}

void ActorFrame::basePMPRadioButtonClicked(bool bl)
{
    if(true==bl)
    {
        while(sasDataGridTableWidget->columnCount()>1)
        {
            sasDataGridTableWidget->removeColumn(sasDataGridTableWidget->columnCount()-1);
        }

        sasDataGridTableWidget->insertColumn(1);
        sasDataGridTableWidget->setItem(0,1,new QTableWidgetItem("Base"));
        sasDataGridTableWidget->item(0,1)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
        sasDataGridTableWidget->item(0,1)->setTextAlignment(Qt::AlignCenter);
        sasDataGridTableWidget->insertColumn(2);
        sasDataGridTableWidget->setItem(0,2,new QTableWidgetItem("±%"));
        sasDataGridTableWidget->item(0,2)->setTextAlignment(Qt::AlignCenter);
        sasDataGridTableWidget->item(0,2)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
    }
}

void ActorFrame::valueRadioButtonClicked(bool bl)
{
    if(true==bl)
    {
        while(sasDataGridTableWidget->columnCount()>1)
        {
            sasDataGridTableWidget->removeColumn(sasDataGridTableWidget->columnCount()-1);
        }

        sasDataGridTableWidget->insertColumn(1);
        sasDataGridTableWidget->setItem(0,1,new QTableWidgetItem("Val 1"));
        sasDataGridTableWidget->item(0,1)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
        sasDataGridTableWidget->item(0,1)->setTextAlignment(Qt::AlignCenter);
        sasDataGridTableWidget->insertColumn(2);
        sasDataGridTableWidget->setItem(0,2,new QTableWidgetItem("Val 2"));
        sasDataGridTableWidget->item(0,2)->setTextAlignment(Qt::AlignCenter);
        sasDataGridTableWidget->item(0,2)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
    }
}

void ActorFrame::addBasePushButtonClicked(bool bl)
{

}
