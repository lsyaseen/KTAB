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
    initializeFrameLayout();
    setLayout(frameMainLayout);
    importedData = true; //csv
}

ActorFrame::~ActorFrame()
{

}

void ActorFrame::initializeFrameLayout()
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
    connect(csvActorDataModel,SIGNAL(itemChanged(QStandardItem*)),
            this,SLOT(cellSelected(QStandardItem*)));
    xmlActorDataModel = new QStandardItemModel;

    sasDataGridTableWidget = new QTableWidget;
    sasDataGridTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(sasDataGridTableWidget,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(sasDataGridContextMenuRequested(QPoint)));
    inputDataTabWidget->addTab(actorDataTableView,"Actor Data");
    inputDataTabWidget->addTab(accomodationMatrixTableView,"Accomodation Matrix");

    actorInputGridLayout->addWidget(inputDataTabWidget);

    scenarioName = new QLineEdit;
    scenarioDescription = new QLineEdit;
    actorDataTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    actorDataTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    connect(actorDataTableView,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(displayMenuTableView(QPoint)));


}

void ActorFrame::initializeAccomodationMatrix(QString type)
{
    csvAccModel = new QStandardItemModel;
    csvAccModel->setRowCount(0);
    csvAccModel->setColumnCount(0);
    accomodationMatrixTableView->setModel(csvAccModel);

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
    while(csvAccModel->rowCount() != rowCount && rowCount > csvAccModel->rowCount())
    {
        int rowCount1 = csvAccModel->rowCount();
        if(csvAccModel->rowCount() < rowCount || csvAccModel->columnCount() < rowCount)
        {
            csvAccModel->insertRows(csvAccModel->rowCount(),(rowCount - csvAccModel->rowCount()));
            csvAccModel->insertColumns(csvAccModel->columnCount(),(rowCount - csvAccModel->columnCount()));
        }
        QString actorHeader;
        for(int rows = rowCount1; rows < csvAccModel->rowCount();++ rows)
        {
            if(type == "CSV")
            {
                actorHeader=csvActorDataModel->item(rows,0)->text().trimmed();
            }
            else if (type == "XML")
            {
                actorHeader=xmlActorDataModel->item(rows,0)->text().trimmed();
            }

            csvAccModel->setHorizontalHeaderItem(rows, new QStandardItem(actorHeader));
            csvAccModel->setVerticalHeaderItem(rows, new QStandardItem(actorHeader));
            initializeAccomodationRowCol(rows,"CSV");
            actorHeader.clear();
        }
    }
}

void ActorFrame::initializeAccomodationRowCol(int count, QString tableType)
{
    if ("CSV"==tableType)
    {
        for( int col = 0; col < csvAccModel->columnCount(); ++col)
        {
            if(count==col)
                csvAccModel->setItem(count,col,new QStandardItem("1"));
            else
                csvAccModel->setItem(count,col,new QStandardItem("0"));
        }

        for(int row =0; row < csvAccModel->rowCount() ; ++row)
        {
            if(count==row)
                csvAccModel->setItem(row,count,new QStandardItem("1"));
            else
                csvAccModel->setItem(row,count,new QStandardItem("0"));
        }
    }
}

void ActorFrame::setActorTableModel(QStandardItemModel *model, QStringList scenarioList)
{
    scenarioName->setText(scenarioList.at(0));
    scenarioDescription->setText(scenarioList.at(1));
    csvActorDataModel=model;
    actorDataTableView->setModel(csvActorDataModel);
    connect(csvActorDataModel,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(cellSelected(QStandardItem*)));

    importedData=true;
    initializeAccomodationMatrix("CSV");
    intitalizeSasGridColumn();

}

void ActorFrame::setAccTableModel(QStandardItemModel *model,
                                  QVector<QStringList> idealAdjustmentList,
                                  QStringList dimensionsXml,QStringList desc)
{
    scenarioName->setText(desc.at(0));
    scenarioDescription->setText(desc.at(1));
    xmlActorDataModel=model;
    actorDataTableView->setModel(xmlActorDataModel);
    connect(xmlActorDataModel,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(cellSelected(QStandardItem*)));
    importedData=false;

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


void ActorFrame::populateAccomodationMatrix(QVector<QStringList> idealAdj, QVector<QString> actors)
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
}

void ActorFrame::initializeSASDataGrid()
{
    sasDataGridTableWidget->setShowGrid(true);
    //    sasDataGridTableWidget->verticalHeader()->hide();

    QFrame * inputFrame = new QFrame;
    QHBoxLayout * hLay = new QHBoxLayout;
    actorComboBox = new QComboBox;

    hLay->addWidget(actorComboBox);
    hLay->addWidget(scenarioName);
    hLay->addWidget(scenarioDescription);
    inputFrame->setLayout(hLay);

    actorSensGridLayout->addWidget(inputFrame,0,0,Qt::AlignTop);
    actorSensGridLayout->addWidget(sasDataGridTableWidget,1,0);
    hLay->setContentsMargins(1,1,1,1);
    sasDataGridTableWidget->setContentsMargins(1,1,1,1);
}

void ActorFrame::intitalizeSasGridColumn()
{
    clearSpecsList();
    sasDataGridTableWidget->setRowCount(0);
    sasDataGridTableWidget->setColumnCount(0);
    sasDataGridTableWidget->insertColumn(0);

    for(int act =0; act < actorDataTableView->model()->rowCount(); ++act)
    {
        actorComboBox->addItem(actorDataTableView->model()->index(act,0).data().toString());
    }
    connect(actorComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(actorComboBoxChanged(QString)));

    int row=0;
    for( int col =2; col < actorDataTableView->model()->columnCount(); ++col)
    {
        sasDataGridTableWidget->insertRow(row);
        QTableWidgetItem * item = new QTableWidgetItem(
                    actorDataTableView->model()->headerData(col,Qt::Horizontal)
                    .toString().remove("\n"));
        sasDataGridTableWidget->setVerticalHeaderItem(row,item);
        attributeList.append(actorComboBox->currentText().append(".").append(item->text()));
        ++row;
    }

    row = sasDataGridTableWidget->rowCount();
    for(int actIndex = 0 ; actIndex < accomodationMatrixTableView->model()->rowCount(); ++actIndex)
    {
        QString headerItem = actorDataTableView->model()->index(actIndex,0).data().toString();
        headerItem.prepend(QString(actorDataTableView->model()->index(0,0).data().toString() + " <> "));

        QTableWidgetItem * item = new QTableWidgetItem(headerItem);
        sasDataGridTableWidget->insertRow(row);
        sasDataGridTableWidget->setVerticalHeaderItem(row,item);
        attributeList.append(actorComboBox->currentText().append(".").append(item->text()));
        ++row;
    }

    minDeltaMaxRadioButton->click();
}

void ActorFrame::initializeSpecificationsTypeButtons()
{
    QGroupBox * specsTypeBox = new QGroupBox("Specifications Type");
    QVBoxLayout * specsTypeLayout = new QVBoxLayout;

    minDeltaMaxRadioButton = new QRadioButton(" (Min, Delta, Max) ");
    basePMRadioButton = new QRadioButton(" (Base, ±) ");
    basePMPRadioButton = new QRadioButton(" (Base, ±%) ");
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
    attributeList.clear();
    sasDataGridTableWidget->clearContents();
    int row=0;
    for( int col =2; col < actorDataTableView->model()->columnCount(); ++col)
    {
        QTableWidgetItem * item = new QTableWidgetItem(
                    actorDataTableView->model()->headerData(col,Qt::Horizontal)
                    .toString().remove("\n"));
        sasDataGridTableWidget->setVerticalHeaderItem(row,item);
        attributeList.append(actorComboBox->currentText().append(".").append(item->text()));

        ++row;
    }

    row = actorDataTableView->model()->columnCount()-2;

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
        sasDataGridTableWidget->setVerticalHeaderItem(row,item);
        attributeList.append(actorComboBox->currentText().append(".").append(item->text()));

        row++;
    }

    if(true == basePMPRadioButton->isChecked() || true == basePMRadioButton->isChecked())
    {
        initializeBaseDataGrid();
    }
}

void ActorFrame::actorListViewContextMenu(QPoint pos)
{
    if(specsListView->model()->rowCount()>0)
    {
        QMenu *menu = new QMenu(this);
        menu->addAction("Remove Selected Items", this, SLOT(listViewRemoveSelectedClicked()));
        menu->addAction("Remove All Items", this, SLOT(listViewRemoveAllClicked()));
        menu->popup(specsListView->mapToGlobal(pos));
    }
}

void ActorFrame::listViewRemoveSelectedClicked()
{

    for(int index=0; index < specsListModel->rowCount();++index)
    {
        QStandardItem * item = specsListModel->item(index);
        if (item->data(Qt::CheckStateRole).toBool() == true)   // Item checked, remove
        {
            specsListModel->removeRow(index);
            actorSpecsRHS.removeAt(index);
            actorSpecsLHS.removeAt(index);
            index = index -1; // index changed to current row, deletion of item changes list index
        }
    }
    QPair<DataValues,SpecsData> spec;
    spec.first = actorSpecsLHS;
    spec.second = actorSpecsRHS;
    emit modelList(specsListModel,attributeList,spec);

}

void ActorFrame::listViewRemoveAllClicked()
{
    for(int index=0; index < specsListModel->rowCount();++index)
    {
        specsListModel->removeRow(index);
        actorSpecsRHS.removeAt(index);
        actorSpecsLHS.removeAt(index);
        index = index -1; // index changed to current row, deletion of item changes list index
    }
    QPair<DataValues,SpecsData> spec;
    spec.first = actorSpecsLHS;
    spec.second = actorSpecsRHS;
    emit modelList(specsListModel,attributeList,spec);

}

void ActorFrame::clearModels()
{
    if(csvAccModel != nullptr)
    {
        csvAccModel->clear();
        csvActorDataModel->clear();
    }
    if(xmlAccModel != nullptr)
    {
        xmlAccModel->clear();
        xmlActorDataModel->clear();
    }

    inputDataTabWidget->clear();
    actorComboBox->clear();
    scenarioName->clear();
    scenarioDescription->clear();
}


void ActorFrame::clearSpecsList()
{
    for(int index=0; index < specsListModel->rowCount();++index)
    {
        specsListModel->removeRow(index);
        index = index -1; // index changed to current row, deletion of item changes list index
    }
    disconnect(actorComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(actorComboBoxChanged(QString)));
    actorComboBox->clear();
    attributeList.clear();

}

void ActorFrame::addSpecClicked(bool bl)
{
    if(sasDataGridTableWidget->model()->rowCount()>0)
    {
        int specsCount = specsListModel->rowCount();

        QString specification;
        for(int row =0; row < sasDataGridTableWidget->rowCount()-1; ++row)
        {
            QString actorValue;
            actorValue.append(actorComboBox->currentText()).append(".")
                    .append(sasDataGridTableWidget->verticalHeaderItem(row)->text());
            //            specification.append(actorComboBox->currentText());
            //            specification.append(".");
            specification.append(actorValue).append("=(");
            specification.remove("\n");

            QString str;
            QVector<double> values;
            for(int col = 0 ; col < sasDataGridTableWidget->columnCount() ; ++col)
            {
                auto ret = sasDataGridTableWidget->item(row,col);

                if(ret != 0)
                {
                    if(col>0)
                    {
                        double v = sasDataGridTableWidget->item(row,col)->text().toDouble();
                        if(v>0.0)
                        {
                            values.append(v);
                        }
                    }
                    else
                    {
                        values.append(sasDataGridTableWidget->item(row,col)->text().toDouble());
                    }
                }
                else
                {
                    specification.clear();
                    values.clear();
                    str.clear();
                    break;
                }
            }
            if(true==minDeltaMaxRadioButton->isChecked() && 3==values.count())
            {   actorSpecsLHS.append(actorValue);
                qDebug()<<actorSpecsLHS << " LHS";
                str.append(processMinDeltaMax(values));
            }
            else if (true == basePMRadioButton->isChecked()&& 2==values.count())
            {
                actorSpecsLHS.append(actorValue);
                qDebug()<<actorSpecsLHS << " LHS";
                str.append(processBasePM(values));
            }
            else if (true == basePMPRadioButton->isChecked()&& 2==values.count())
            {
                actorSpecsLHS.append(actorValue);
                qDebug()<<actorSpecsLHS << " LHS";
                str.append(processBasePMP(values));
            }
            else if (true == valueRadioButton->isChecked() &&
                     sasDataGridTableWidget->columnCount()==values.count())
            {
                actorSpecsLHS.append(actorValue);
                qDebug()<<actorSpecsLHS << " LHS";
                str.append(processValuesN(values));
            }
            else
            {
                specification.clear();
                values.clear();
                str.clear();
            }
            specification.append(str);
            if(!specification.isEmpty())
            {
                specification.append(")");

                if(specification.contains("()") || specification.contains("))")
                        || specification.contains(",)"))
                {
                    specification.clear();
                }
                else
                {
                    QStandardItem *item = new QStandardItem(specification);
                    item->setCheckable(true);
                    item->setCheckState(Qt::Unchecked);
                    item->setEditable(false);
                    specsListModel->setItem(specsListModel->rowCount(),item);
                    specsListView->scrollToBottom();
                    qDebug()<<specification;
                    specification.clear();
                }
            }
            else
            {
                specification.clear();
                values.clear();
                str.clear();
            }
        }
        if(specsCount != specsListModel->rowCount())
        {
            QPair<DataValues,SpecsData> spec;
            spec.first = actorSpecsLHS;
            spec.second = actorSpecsRHS;
            emit modelList(specsListModel,attributeList,spec);
        }
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
    sasDataGridTableWidget->setHorizontalHeaderItem(sasDataGridTableWidget->columnCount()-1
                                                    ,new QTableWidgetItem(
                                                        QString("Val "+QString::number(sasDataGridTableWidget->columnCount()))));
}

void ActorFrame::minDeltaMaxRadioButtonClicked(bool bl)
{
    if(true==bl)
    {
        if(csvActorDataModel->hasIndex(0,0) || xmlActorDataModel->hasIndex(0,0))
        {
            while(sasDataGridTableWidget->columnCount()>0)
            {
                sasDataGridTableWidget->removeColumn(sasDataGridTableWidget->columnCount()-1);
            }

            sasDataGridTableWidget->insertColumn(0);
            sasDataGridTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Min"));
            sasDataGridTableWidget->insertColumn(1);
            sasDataGridTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Delta"));
            sasDataGridTableWidget->insertColumn(2);
            sasDataGridTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Max"));
        }
    }
}

void ActorFrame::basePMRadioButtonClicked(bool bl)
{
    if(true==bl)
    {
        if(csvActorDataModel->hasIndex(0,0) || xmlActorDataModel->hasIndex(0,0))
        {
            while(sasDataGridTableWidget->columnCount()>0)
            {
                sasDataGridTableWidget->removeColumn(sasDataGridTableWidget->columnCount()-1);
            }

            sasDataGridTableWidget->insertColumn(0);
            sasDataGridTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Base"));

            initializeBaseDataGrid();

            sasDataGridTableWidget->insertColumn(1);
            sasDataGridTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("±"));
        }
    }
}

void ActorFrame::basePMPRadioButtonClicked(bool bl)
{
    if(true==bl)
    {
        if(csvActorDataModel->hasIndex(0,0) || xmlActorDataModel->hasIndex(0,0))
        {
            while(sasDataGridTableWidget->columnCount()>0)
            {
                sasDataGridTableWidget->removeColumn(sasDataGridTableWidget->columnCount()-1);
            }

            sasDataGridTableWidget->insertColumn(0);
            sasDataGridTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Base"));

            initializeBaseDataGrid();

            sasDataGridTableWidget->insertColumn(1);
            sasDataGridTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("±%"));
        }
    }
}

void ActorFrame::valueRadioButtonClicked(bool bl)
{
    if(true==bl)
    {
        if(csvActorDataModel->hasIndex(0,0) || xmlActorDataModel->hasIndex(0,0))
        {
            while(sasDataGridTableWidget->columnCount()>0)
            {
                sasDataGridTableWidget->removeColumn(sasDataGridTableWidget->columnCount()-1);
            }

            sasDataGridTableWidget->insertColumn(0);
            sasDataGridTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Val 1"));
            //            sasDataGridTableWidget->item(0,1)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
            //            sasDataGridTableWidget->item(0,1)->setTextAlignment(Qt::AlignCenter);
            sasDataGridTableWidget->insertColumn(1);
            sasDataGridTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Val 2"));
            //            sasDataGridTableWidget->item(0,2)->setTextAlignment(Qt::AlignCenter);
            //            sasDataGridTableWidget->item(0,2)->setBackgroundColor(QColor::fromRgb(192,192,192,135));
        }
    }
}

void ActorFrame::addBasePushButtonClicked(bool bl)
{
    if(basePMPRadioButton->isChecked()==true || basePMRadioButton->isChecked())
    {

        if(sasDataGridTableWidget->model()->rowCount()>0)
        {
            int specsCount = specsListModel->rowCount();

            QString currentAct = actorComboBox->currentText();
            for(int act =0; act < actorComboBox->count(); ++act)
            {
                QString specification;

                for(int row =0; row < sasDataGridTableWidget->rowCount()-1; ++row)
                {
                    QString actorValue;
                    actorValue.append(actorComboBox->itemText(act)).append(".")
                            .append(sasDataGridTableWidget->verticalHeaderItem(row)->text());
                    specification.append(actorValue).append("=(");
                    specification.remove("\n");

                    QString str;
                    QVector<double> values;
                    if(row <= actorDataTableView->model()->columnCount()-2)
                    {
                        auto ret = sasDataGridTableWidget->item(row,1); // col 1

                        if(ret != 0 && (!ret->text().isEmpty()))
                        {
                            double v = sasDataGridTableWidget->item(row,1)->text().toDouble();
                            if(v>0.0)
                            {
                                int c = row+2; // 2 is offset
                                values.append(actorDataTableView->model()->data(
                                                  actorDataTableView->model()->index(act,c)).toDouble());
                                values.append(v);// +- / +-% , 2nd value
                            }
                        }
                        else
                        {
                            specification.clear();
                            values.clear();
                            str.clear();
                        }
                    }
                    else if (row > actorDataTableView->model()->columnCount()-2)
                    {

                        auto ret = sasDataGridTableWidget->item(row,1); // col 1

                        if(ret != 0 && (!ret->text().isEmpty()))
                        {
                            double v = sasDataGridTableWidget->item(row,1)->text().toDouble();
                            if(v>0.0)
                            {
                                int c = row-actorDataTableView->model()->rowCount()-2;
                                values.append(accomodationMatrixTableView->model()->data(
                                                  accomodationMatrixTableView->model()->index(act,c)).toDouble());
                                values.append(v);// +- / +-% , 2nd value
                            }
                        }
                        else
                        {
                            specification.clear();
                            values.clear();
                            str.clear();
                        }
                    }

                    if (true == basePMRadioButton->isChecked()&& 2==values.count())
                    {
                        actorSpecsLHS.append(actorValue);
                        qDebug()<<actorSpecsLHS << " LHS";
                        str.append(processBasePM(values));
                    }
                    else if (true == basePMPRadioButton->isChecked()&& 2==values.count())
                    {
                        actorSpecsLHS.append(actorValue);
                        qDebug()<<actorSpecsLHS << " LHS";
                        str.append(processBasePMP(values));
                    }
                    else
                    {
                        specification.clear();
                        values.clear();
                        str.clear();
                    }
                    specification.append(str);
                    if(!specification.isEmpty())
                    {
                        specification.append(")");


                        if(specification.contains("()") || specification.contains("))")
                                || specification.contains(",)"))
                        {
                            specification.clear();
                        }
                        else
                        {
                            specification.replace(QString(currentAct+" <"),QString(actorComboBox->itemText(act)+" <"));
                            QStandardItem *item = new QStandardItem(specification);
                            item->setCheckable(true);
                            item->setCheckState(Qt::Unchecked);
                            item->setEditable(false);
                            specsListModel->setItem(specsListModel->rowCount(),item);
                            specsListView->scrollToBottom();
                            specification.clear();
                        }
                    }
                    else
                    {
                        specification.clear();
                        values.clear();
                        str.clear();
                    }
                }
            }
            if(specsCount != specsListModel->rowCount())
            {
                QPair<DataValues,SpecsData> spec;
                spec.first = actorSpecsLHS;
                spec.second = actorSpecsRHS;
                emit modelList(specsListModel,attributeList,spec);
            }
        }
    }
}

void ActorFrame::displayMenuTableView(QPoint pos)
{
    QMenu menu(this);
    QAction *posCol = menu.addAction("Insert Position Column");
    QAction *salCol = menu.addAction("Insert Salience Column");
    menu.addSeparator();
    QAction *newRow = menu.addAction("Insert Row");
    menu.addSeparator();
    QAction *col = menu.addAction("Remove Column");
    QAction *row = menu.addAction("Remove Row");
    menu.addSeparator();
    QAction *rename = menu.addAction("Rename Column Header");

    QAction *act = menu.exec(actorDataTableView->viewport()->mapToGlobal(pos));

    if (act == col)
    {
        if(actorDataTableView->currentIndex().column()>2)
        {
            if(importedData==true)//csv
            {
                csvActorDataModel->removeColumn(actorDataTableView->currentIndex().column());
            }
            else // xml
            {
                xmlActorDataModel->removeColumn(actorDataTableView->currentIndex().column());
            }
        }
        else
        {
            //            statusBar()->showMessage("No Permission ! You cannot delete Actor,"
            //                                     " Description and Influence Columns");
        }
    }
    if (act == row)
    {
        if(importedData==true)
        {
            csvAccModel->removeColumn(actorDataTableView->currentIndex().row());
            csvAccModel->removeRow(actorDataTableView->currentIndex().row());
        }
        else
        {
            xmlAccModel->removeColumn(actorDataTableView->currentIndex().row());
            xmlAccModel->removeRow(actorDataTableView->currentIndex().row());
        }


        if(importedData==true)//csv
        {
            csvActorDataModel->removeRow(actorDataTableView->currentIndex().row());
        }
        else // xml
        {
            xmlActorDataModel->removeRow(actorDataTableView->currentIndex().row());
        }
    }
    if (act == rename)
    {
        if(actorDataTableView->currentIndex().column()>2)
        {
            bool ok;
            QString text;
            if(importedData==true)//csv
            {
                text = QInputDialog::getText(this, tr("Plesase Enter the Header Name"),
                                             tr("Header Name"), QLineEdit::Normal,
                                             csvActorDataModel->headerData(actorDataTableView->currentIndex().column(),Qt::Horizontal).toString(), &ok);
            }
            else // xml
            {
                text = QInputDialog::getText(this, tr("Plesase Enter the Header Name"),
                                             tr("Header Name"), QLineEdit::Normal,
                                             xmlActorDataModel->headerData(actorDataTableView->currentIndex().column(),Qt::Horizontal).toString(), &ok);
            }


            if (ok && !text.isEmpty())
            {
                if(actorDataTableView->currentIndex().column()%2!=0)
                {
                    if(!(text.contains("Position") || text.contains("position")))
                    {
                        if(!text.contains("\n"))
                            text = text.append(" \n Position");
                        else
                            text = text.append(" Position");
                    }
                    else
                    {
                        QString header = text;
                        header.remove("Position").remove("position");
                        header.remove("\n");
                        header.append("\n Position");
                        text = header;
                    }

                    if(importedData==true)//csv
                    {
                        csvActorDataModel->horizontalHeaderItem(actorDataTableView->currentIndex().column())->
                                setToolTip("The stated position, or advocacy, of the actor");
                    }
                    else // xml
                    {
                        xmlActorDataModel->horizontalHeaderItem(actorDataTableView->currentIndex().column())->
                                setToolTip("The stated position, or advocacy, of the actor");
                    }
                }
                else
                {
                    if(!(text.contains("Salience")|| text.contains("salience")))
                    {
                        if(!text.contains("\n"))
                            text = text.append(" \n Salience");
                        else
                            text = text.append(" Salience");
                    }
                    else
                    {
                        QString header = text;
                        header.remove("Salience").remove("salience");
                        header.remove("\n");
                        header.append("\n Salience");
                        text = header;
                    }
                    if(importedData==true)//csv
                    {
                        csvActorDataModel->horizontalHeaderItem(actorDataTableView->currentIndex().column())->
                                setToolTip("The relative importance, or priority, for the actor");
                    }
                    else // xml
                    {
                        xmlActorDataModel->horizontalHeaderItem(actorDataTableView->currentIndex().column())->
                                setToolTip("The relative importance, or priority, for the actor");
                    }
                }

                if(importedData==true)//csv
                {
                    csvActorDataModel->setHeaderData(actorDataTableView->currentIndex().column(),Qt::Horizontal,text);
                }
                else // xml
                {
                    xmlActorDataModel->setHeaderData(actorDataTableView->currentIndex().column(),Qt::Horizontal,text);
                }
                //                statusBar()->showMessage("Header changed");
            }
        }
        else
        {
            //            statusBar()->showMessage("No Permission !  You cannot Edit Headers of"
            //                                     " Actor, Description and Influence Columns");
        }
    }
    if (act == posCol)
    {
        if(actorDataTableView->currentIndex().column()>2)
        {
            if(importedData==true)//csv
            {
                csvActorDataModel->insertColumn(actorDataTableView->currentIndex().column());
                csvActorDataModel->setHeaderData(actorDataTableView->currentIndex().column()-1,Qt::Horizontal,"Position");
                csvActorDataModel->horizontalHeaderItem(actorDataTableView->currentIndex().column()-1)->
                        setToolTip("The stated position, or advocacy, of the actor");
                //                statusBar()->showMessage("Column Inserted, Header changed");
            }
            else // xml
            {
                xmlActorDataModel->insertColumn(actorDataTableView->currentIndex().column());
                xmlActorDataModel->setHeaderData(actorDataTableView->currentIndex().column()-1,Qt::Horizontal,"Position");
                xmlActorDataModel->horizontalHeaderItem(actorDataTableView->currentIndex().column()-1)->
                        setToolTip("The stated position, or advocacy, of the actor");
                //                statusBar()->showMessage("Column Inserted, Header changed");
            }
        }
        else
        {
            //            statusBar()->showMessage("No Permission !  You cannot Edit Headers of"
            //                                     " Actor, Description and Influence Columns");
        }
    }
    if (act == salCol)
    {
        if(actorDataTableView->currentIndex().column()>2)
        {
            if(importedData==true)//csv
            {
                csvActorDataModel->insertColumn(actorDataTableView->currentIndex().column());
                csvActorDataModel->setHeaderData(actorDataTableView->currentIndex().column()-1,Qt::Horizontal,"Salience");
                csvActorDataModel->horizontalHeaderItem(actorDataTableView->currentIndex().column()-1)->
                        setToolTip("The relative importance, or priority, for the actor");

            }
            else // xml
            {
                xmlActorDataModel->insertColumn(actorDataTableView->currentIndex().column());
                xmlActorDataModel->setHeaderData(actorDataTableView->currentIndex().column()-1,Qt::Horizontal,"Salience");
                xmlActorDataModel->horizontalHeaderItem(actorDataTableView->currentIndex().column()-1)->
                        setToolTip("The relative importance, or priority, for the actor");
            }
            //            statusBar()->showMessage("Column Inserted, Header changed");
        }
        else
        {
            //            statusBar()->showMessage("No Permission !  You cannot Edit Headers of"
            //                                     " Actor, Description and Influence Columns");
        }
    }

    if(act == newRow)
    {
        if(importedData==true)
        {
            csvAccModel->insertColumn(actorDataTableView->currentIndex().row());
            csvAccModel->insertRow(actorDataTableView->currentIndex().row());
        }
        else
        {
            xmlAccModel->insertColumn(actorDataTableView->currentIndex().row());
            xmlAccModel->insertRow(actorDataTableView->currentIndex().row());
        }

        QString actorHeader;
        actorHeader.append(" Actor ")/*.append(QString::number(rows+1))*/;
        if(importedData==true)
        {
            csvAccModel->setHorizontalHeaderItem(actorDataTableView->currentIndex().row(),
                                                 new QStandardItem(actorHeader));
            csvAccModel->setVerticalHeaderItem(actorDataTableView->currentIndex().row(),
                                               new QStandardItem(actorHeader));
        }
        else
        {
            xmlAccModel->setHorizontalHeaderItem(actorDataTableView->currentIndex().row(),
                                                 new QStandardItem(actorHeader));
            xmlAccModel->setVerticalHeaderItem(actorDataTableView->currentIndex().row(),
                                               new QStandardItem(actorHeader));
        }

        if(importedData==true)//csv
        {
            initializeAccMatrixRowCol(actorDataTableView->currentIndex().row(),"CSV");
        }
        else // xml
        {
            initializeAccMatrixRowCol(actorDataTableView->currentIndex().row(),"XML");
        }
        actorHeader.clear();

        if(importedData==true)//csv
        {
            csvActorDataModel->insertRow(actorDataTableView->currentIndex().row());
        }
        else // xml
        {
            xmlActorDataModel->insertRow(actorDataTableView->currentIndex().row());
        }
    }
}

void ActorFrame::initializeAccMatrixRowCol(int count, QString table)
{
    if ("XML"==table)
    {
        for( int col = 0; col < xmlAccModel->columnCount(); ++col)
        {
            if(count==col)
                xmlAccModel->setItem(count,col,new QStandardItem("1"));
            else
                xmlAccModel->setItem(count,col,new QStandardItem("0"));
        }

        for(int row =0; row < xmlAccModel->rowCount() ; ++row)
        {
            if(count==row)
                xmlAccModel->setItem(row,count,new QStandardItem("1"));
            else
                xmlAccModel->setItem(row,count,new QStandardItem("0"));
        }
    }
    else if ("CSV"==table)
    {
        for( int col = 0; col < csvAccModel->columnCount(); ++col)
        {
            if(count==col)
                csvAccModel->setItem(count,col,new QStandardItem("1"));
            else
                csvAccModel->setItem(count,col,new QStandardItem("0"));
        }

        for(int row =0; row < csvAccModel->rowCount() ; ++row)
        {
            if(count==row)
                csvAccModel->setItem(row,count,new QStandardItem("1"));
            else
                csvAccModel->setItem(row,count,new QStandardItem("0"));
        }
    }
}

void ActorFrame::cellSelected(QStandardItem* in)
{
    if(0==in->column())
    {
        if(importedData == true)//csv
        {
            csvAccModel->setHorizontalHeaderItem(in->row(),new QStandardItem(in->text()));
            csvAccModel->setVerticalHeaderItem(in->row(),new QStandardItem(in->text()));
        }
        else
        {
            xmlAccModel->setHorizontalHeaderItem(in->row(),new QStandardItem(in->text()));
            xmlAccModel->setVerticalHeaderItem(in->row(),new QStandardItem(in->text()));
        }
    }
}

void ActorFrame::initializeBaseDataGrid()
{
    int row =0;
    for(int actCol = 2 ; actCol < actorDataTableView->model()->columnCount(); ++actCol)
    {
        sasDataGridTableWidget->setItem(row,0,new QTableWidgetItem(
                                            actorDataTableView->model()->data
                                            (actorDataTableView->model()->index(
                                                 actorComboBox->currentIndex(),actCol)).toString()));
        row++;
    }

    for(int accIndex = 0 ; accIndex < accomodationMatrixTableView->model()->columnCount(); ++accIndex)
    {
        sasDataGridTableWidget->setItem(row,0,new QTableWidgetItem(
                                            accomodationMatrixTableView->model()->data
                                            (accomodationMatrixTableView->model()->index(
                                                 actorComboBox->currentIndex(),accIndex)).toString()));

        row++;
    }
}

QString ActorFrame::processMinDeltaMax( QVector<double> values)
{
    QString processedString;
    if(values.at(0)<values.at(2)) // min < max
    {
        QVector <QString> actorValues;
        for(double min = values.at(0) ; min <= values.at(2) ; min +=  values.at(1))
        {
            actorValues.append(QString::number(min));
            //  qDebug()<<min << "val";
            processedString.append(QString::number(min)).append(",");
            //qDebug()<<processedString;
        }
        if(!actorValues.isEmpty())
        {
            actorSpecsRHS.append(actorValues);
            qDebug()<<actorSpecsRHS <<"RHS";
        }
        processedString.remove(processedString.length()-1,1);
        //        qDebug()<<processedString;
        return processedString;
    }
    else // min > max
    {
        processedString.append(QString::number(values.at(0)));
    }
    return processedString;
}

QString ActorFrame::processBasePM(QVector<double> values)
{
    QString processedString;

    QVector <QString> actorValues;

    double base = values.at(0) - values.at(1);
    actorValues.append(QString::number(base));
    processedString.append(QString::number(base)).append(",");
    actorValues.append(QString::number(values.at(0)));
    processedString.append(QString::number(values.at(0))).append(",");
    base = values.at(0)+values.at(1);
    actorValues.append(QString::number(base));
    processedString.append(QString::number(base));
    actorSpecsRHS.append(actorValues);
    qDebug()<<actorSpecsRHS <<"RHS";
    return processedString;

}

QString ActorFrame::processBasePMP(QVector<double> values)
{
    QString processedString;
    QVector <QString> actorValues;

    //    qDebug()<<values;
    double base = values.at(0)-(values.at(0)*values.at(1))/100;
    actorValues.append(QString::number(base));
    processedString.append(QString::number(base)).append(",");
    actorValues.append(QString::number(values.at(0)));
    processedString.append(QString::number(values.at(0))).append(",");
    base = values.at(0)+(values.at(0)*values.at(1))/100;
    actorValues.append(QString::number(base));
    processedString.append(QString::number(base));

    actorSpecsRHS.append(actorValues);
    qDebug()<<actorSpecsRHS <<"RHS";

    return processedString;
}

QString ActorFrame::processValuesN(QVector<double> values)
{
    QString processedString;
    QVector <QString> actorValues;

    //    qDebug()<<values;
    for(int val = 0 ; val < values.count();++val)
    {
        actorValues.append(QString::number(values.at(val)));
        processedString.append(QString::number(values.at(val))).append(",");
    }
    processedString.remove(processedString.length()-1,1);

    if(!actorValues.isEmpty())
    {
        actorSpecsRHS.append(actorValues);
        qDebug()<<actorSpecsRHS <<"RHS";

    }
    return processedString;
}
