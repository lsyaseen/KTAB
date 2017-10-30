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
    importedDataCSV = true; //csv
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

    importedDataCSV=true;
    initializeAccomodationMatrix("CSV");
    intitalizeSasGridColumn();
    actorAtrributesHeaderList();
    validateData();
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
    importedDataCSV=false;

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
    actorAtrributesHeaderList();
    validateData();
}

void ActorFrame::getDataModel()
{
    QStandardItemModel * accDataModel;
    QStandardItemModel * accModel;

    if(importedDataCSV== true)// csv
    {
        accDataModel = csvActorDataModel;
        accModel = csvAccModel;

    }
    else // xml
    {
        accDataModel = xmlActorDataModel;
        accModel = csvAccModel;
    }

    QStringList scenarioData;
    scenarioData.append(scenarioName->text());
    scenarioData.append(scenarioDescription->text());
    scenarioData.append("0"); //seed
    emit dataModel(accDataModel,accModel,scenarioData);
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
            removeSpecificationActor(index,1,actorSpecsLHS.at(index)); // 1 == actor type
            specsListModel->removeRow(index);
            actorSpecsRHS.removeAt(index);
            actorSpecsLHS.removeAt(index);
            index = index -1; // index changed to current row, deletion of item changes list index
        }
    }
}

void ActorFrame::listViewRemoveAllClicked()
{
    for(int index=0; index < specsListModel->rowCount();++index)
    {
        removeSpecificationActor(index,1,actorSpecsLHS.at(index)); // 1 == actor type
        specsListModel->removeRow(index);
        actorSpecsRHS.removeAt(index);
        actorSpecsLHS.removeAt(index);
        index = index -1; // index changed to current row, deletion of item changes list index
    }
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

void ActorFrame::actorAtrributesHeaderList()
{
    QStringList headerAttribs;
    QStringList actorNames;

    for(int act = 0 ; act < actorComboBox->count();++act)
    {
        actorNames.append(actorComboBox->itemText(act));
    }

    for( int col =2; col < actorDataTableView->model()->columnCount(); ++col)
    {
        headerAttribs.append(actorDataTableView->model()->headerData(col,Qt::Horizontal).toString().remove("\n"));
    }

    emit actorHeaderAttributes(headerAttribs,actorNames);

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
        //        validateSalienceData();

        bool scaleSum = false;

        QList <int> salIndex;
        QList <int> posIndex;
        QString specification;
        QList <QString> specList;
        QList <QPair<DataValues,SpecsData>> specPairList;

        for(int row =0; row < sasDataGridTableWidget->rowCount()-1; ++row)
        {
            QString actorValue;
            actorValue.append(actorComboBox->currentText()).append(".")
                    .append(sasDataGridTableWidget->verticalHeaderItem(row)->text());
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
                        double val = sasDataGridTableWidget->item(row,col)->text().toDouble();
                        if(val>0.0)
                        {
                            values.append(val);
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
            {
                QString procStr = processMinDeltaMax(values,row);
                if(!procStr.isEmpty())
                {
                    str.append(procStr);
                    actorSpecsLHS.append(actorValue);
                    //                    qDebug()<<actorSpecsLHS << " LHS";

                    if(row >0 && row%2==0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
                    {
                        salIndex.append(actorSpecsLHS.length()-1);
                    }
                    else
                    {
                        posIndex.append(actorSpecsLHS.length()-1);
                    }
                }
                else
                {
                    return;
                }
            }
            else if (true == basePMRadioButton->isChecked() && 2==values.count())
            {

                QString procStr = processBasePM(values,row);
                if(!procStr.isEmpty())
                {
                    actorSpecsLHS.append(actorValue);
                    //                    qDebug()<<actorSpecsLHS << " LHS";
                    str.append(procStr);

                    if(row >0 && row%2==0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
                    {
                        salIndex.append(actorSpecsLHS.length()-1);
                    }
                    else
                    {
                        posIndex.append(actorSpecsLHS.length()-1);
                    }
                }
                else
                {
                    return;
                }
            }
            else if (true == basePMPRadioButton->isChecked()&& 2==values.count())
            {

                QString procStr = processBasePMP(values,row);
                if(!procStr.isEmpty())
                {
                    actorSpecsLHS.append(actorValue);
                    //                    qDebug()<<actorSpecsLHS << " LHS";
                    str.append(procStr);

                    if(row >0 && row%2==0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
                    {
                        salIndex.append(actorSpecsLHS.length()-1);
                    }
                    else
                    {
                        posIndex.append(actorSpecsLHS.length()-1);
                    }
                }
                else
                {
                    return;
                }
            }
            else if (true == valueRadioButton->isChecked() &&
                     sasDataGridTableWidget->columnCount()==values.count())
            {

                QString procStr = processValuesN(values,row);
                if(!procStr.isEmpty())
                {
                    actorSpecsLHS.append(actorValue);
                    //                    qDebug()<<actorSpecsLHS << "LHS";
                    str.append(procStr);

                    if(row >0 && row%2==0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
                    {
                        salIndex.append(actorSpecsLHS.length()-1);
                    }
                    else
                    {
                        posIndex.append(actorSpecsLHS.length()-1);
                    }
                }
                else
                {
                    return;
                }
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
                    specList.append(specification);
                    specification.clear();

                    //emit singleSpecification
                    QPair<DataValues,SpecsData> spec;
                    spec.first.append(actorSpecsLHS.at(actorSpecsLHS.count()-1));
                    spec.second.append(actorSpecsRHS.at(actorSpecsRHS.count()-1));
                    specPairList.append(spec);

                }
            }
            else
            {
                specification.clear();
                values.clear();
                str.clear();
            }
        }

        int dimensionCount = (actorDataTableView->model()->columnCount()-3/2);

        //getting the user specified salience values
        QVector<QVector<QString>> salValues;

        for(int indx = 0; indx < salIndex.length(); ++indx)
        {
            QVector<QString> sal;
            sal.append(actorSpecsRHS.at(salIndex.at(indx)));
            salValues.append(sal);
        }

        QVector <int> salValLenVec;
        double salLimit;

        //validate sal against other dimensions
        if(salIndex.length()>1)
        {
            if(true==minDeltaMaxRadioButton->isChecked())
            {
                int maxLen=0;
                int index=0;
                for(int i=0; i < salValues.length();++i)
                {
                    salValLenVec.append(salValues.at(i).length());
                    if(maxLen<salValues.at(i).length())
                    {
                        maxLen=salValues.at(i).length();
                        index=i;
                    }
                }

                for(int salValIndex = 0; salValIndex < salValues.at(index).length() ;++salValIndex)
                {
                    salLimit = 0.0;
                    int salListIndex;

                    for(salListIndex = 0; salListIndex < salValues.length(); ++salListIndex)
                    {
                        if(salValues.at(salListIndex).length()>salValIndex)
                        {
                            salLimit+= salValues.at(salListIndex).at(salValIndex).toDouble();
                        }
                    }


                    if(salLimit>100.0)
                    {
                        scaleSum=true;
                        for(int salIndex = 0; salIndex < salValues.length();++salIndex)
                        {
                            if(salValues.length()==dimensionCount)// if salience for all dimensions are specified
                            {
                                if(salValues.at(salIndex).length()>salValIndex)
                                {
                                    salValues[salIndex][salValIndex]=
                                            QString::number((salValues[salIndex][salValIndex].toDouble()/salLimit)*100); // scaling the values to 100
                                }
                            }
                            else // if salience is not specified for all dimensions
                            {
                                if(salValues.at(salIndex).length()>salValIndex)
                                {

                                    70 - (70 + 48 - 100);
                                    //subtract values

                                    salValues[salIndex][salValIndex]=
                                            QString::number((salValues[salIndex][salValIndex].toDouble()/salLimit)*100); // scaling the values to 100
                                }
                            }
                        }

                        for(int indx = 0; indx < salIndex.length(); ++indx)
                        {
                            actorSpecsRHS[salIndex[indx]]=salValues[indx];
                        }

                    }

                    for(int i = 0 ; i < salIndex.length(); ++i)
                    {

                        QStringList str = specList.at(salIndex.at(i)-(actorSpecsRHS.length()-specList.length())).split("=");
                        QString newSpec = str.at(0);

                        //                        qDebug()<<newSpec << "newSpec";
                        newSpec.append("=(");
                        for(int j =0 ; j < actorSpecsRHS.at(salIndex.at(i)).length(); ++j)
                        {
                            newSpec.append(actorSpecsRHS.at(salIndex.at(i)).at(j)).append(",");
                        }
                        newSpec.append("#").remove(",#").append(")");
                        specList[salIndex.at(i)-(actorSpecsRHS.length()-specList.length())]=newSpec;
                    }
                    for(int i = 0 ; i < posIndex.length(); ++i)
                    {

                        QStringList str = specList.at(posIndex.at(i)-(actorSpecsRHS.length()-specList.length())).split("=");
                        QString newSpec = str.at(0);

                        //                        qDebug()<<newSpec << "newSpec";
                        newSpec.append("=(");
                        for(int j =0 ; j < actorSpecsRHS.at(posIndex.at(i)).length(); ++j)
                        {
                            newSpec.append(actorSpecsRHS.at(posIndex.at(i)).at(j)).append(",");
                        }
                        newSpec.append("#").remove(",#").append(")");
                        specList[posIndex.at(i)-(actorSpecsRHS.length()-specList.length())]=newSpec;
                    }
                }
            }
            else
            {
                for(int salValIndex = 0; salValIndex < salValues.at(0).length();++salValIndex)
                {
                    salLimit = 0.0;
                    int salListIndex;
                    for(salListIndex = 0; salListIndex < salValues.length(); ++salListIndex)
                    {
                        salLimit+= salValues.at(salListIndex).at(salValIndex).toDouble();
                    }

                    if(salLimit>100.0)
                    {
                        scaleSum=true;
                        for(int salIndex = 0; salIndex < salValues.length();++salIndex)
                        {
                            //                            qDebug() << salValues[salIndex][salValIndex] << "Before ";

                            salValues[salIndex][salValIndex]=
                                    QString::number(salValues[salIndex][salValIndex].toDouble()/salLimit);

                            //                            qDebug() << salValues[salIndex][salValIndex] << "After ";
                            //                            qDebug() << salIndex << salValIndex << "Indices ";
                        }

                        for(int indx = 0; indx < salIndex.length(); ++indx)
                        {
                            actorSpecsRHS[salIndex[indx]]=salValues[indx];
                        }
                    }

                    for(int i = 0 ; i < salIndex.length(); ++i)
                    {

                        QStringList str = specList.at(salIndex.at(i)-(actorSpecsRHS.length()-specList.length())).split("=");
                        QString newSpec = str.at(0);

                        //                        qDebug()<<newSpec << "newSpec";
                        newSpec.append("=(");
                        for(int j =0 ; j < actorSpecsRHS.at(salIndex.at(i)).length(); ++j)
                        {
                            newSpec.append(actorSpecsRHS.at(salIndex.at(i)).at(j)).append(",");
                        }
                        newSpec.append("#").remove(",#").append(")");
                        specList[salIndex.at(i)-(actorSpecsRHS.length()-specList.length())]=newSpec;

                    }

                    for(int i = 0 ; i < posIndex.length(); ++i)
                    {

                        QStringList str = specList.at(posIndex.at(i)-(actorSpecsRHS.length()-specList.length())).split("=");
                        QString newSpec = str.at(0);

                        //                        qDebug()<<newSpec << "newSpec";
                        newSpec.append("=(");
                        for(int j =0 ; j < actorSpecsRHS.at(posIndex.at(i)).length(); ++j)
                        {
                            newSpec.append(actorSpecsRHS.at(posIndex.at(i)).at(j)).append(",");
                        }
                        newSpec.append("#").remove(",#").append(")");
                        specList[posIndex.at(i)-(actorSpecsRHS.length()-specList.length())]=newSpec;

                    }
                }
            }
            if(scaleSum==true)
            {
                QMessageBox::information(this,"Salience Values modified",
                                         "Salience values against multiple dimensions cross limit hence values are scaled to 100");
                scaleSum=false;
            }
        }

        //salience calculation against unspecied dimensions (partial)

        //        else
        //        {

        //            //get salience values for unspecified saliences
        //            QPair<QVector<double>, QVector<int> > salConstData;
        //            // first - allSalienceData, second - salConstIndices
        //            salConstData = getConstantSalienceValues();



        //            for (int iSal=0; iSal < dimensionCount; ++iSal)
        //            {
        //                // get constant Sal values for all actors
        //            }



        //            //for spec other than mid delta max
        //            for(int salValIndex = 0; salValIndex < salValues.at(0).length();++salValIndex)
        //            {
        //                salLimit = 0.0;
        //                int salListIndex;
        //                for(salListIndex = 0; salListIndex < salValues.length(); ++salListIndex)
        //                {
        //                    salLimit+= salValues.at(salListIndex).at(salValIndex).toDouble();
        //                }


        //                if(salValues.count()>=1 && salValues.count()<dimensionCount)
        //                {



        //                }



        //                if(salLimit>100.0)
        //                {
        //                    scaleSum=true;
        //                    for(int salIndex = 0; salIndex < salValues.length();++salIndex)
        //                    {
        //                        //                            qDebug() << salValues[salIndex][salValIndex] << "Before ";

        //                        salValues[salIndex][salValIndex]=
        //                                QString::number(salValues[salIndex][salValIndex].toDouble()/salLimit);

        //                        //                            qDebug() << salValues[salIndex][salValIndex] << "After ";
        //                        //                            qDebug() << salIndex << salValIndex << "Indices ";
        //                    }

        //                    for(int indx = 0; indx < salIndex.length(); ++indx)
        //                    {
        //                        actorSpecsRHS[salIndex[indx]]=salValues[indx];
        //                    }
        //                }

        //                for(int i = 0 ; i < salIndex.length(); ++i)
        //                {

        //                    QStringList str = specList.at(salIndex.at(i)-(actorSpecsRHS.length()-specList.length())).split("=");
        //                    QString newSpec = str.at(0);

        //                    //                        qDebug()<<newSpec << "newSpec";
        //                    newSpec.append("=(");
        //                    for(int j =0 ; j < actorSpecsRHS.at(salIndex.at(i)).length(); ++j)
        //                    {
        //                        newSpec.append(actorSpecsRHS.at(salIndex.at(i)).at(j)).append(",");
        //                    }
        //                    newSpec.append("#").remove(",#").append(")");
        //                    specList[salIndex.at(i)-(actorSpecsRHS.length()-specList.length())]=newSpec;

        //                }

        //                for(int i = 0 ; i < posIndex.length(); ++i)
        //                {

        //                    QStringList str = specList.at(posIndex.at(i)-(actorSpecsRHS.length()-specList.length())).split("=");
        //                    QString newSpec = str.at(0);

        //                    //                        qDebug()<<newSpec << "newSpec";
        //                    newSpec.append("=(");
        //                    for(int j =0 ; j < actorSpecsRHS.at(posIndex.at(i)).length(); ++j)
        //                    {
        //                        newSpec.append(actorSpecsRHS.at(posIndex.at(i)).at(j)).append(",");
        //                    }
        //                    newSpec.append("#").remove(",#").append(")");
        //                    specList[posIndex.at(i)-(actorSpecsRHS.length()-specList.length())]=newSpec;

        //                }
        //            }


        //        }


        // final spec code

        for(int specIndx=0; specIndx < specList.length(); ++specIndx )
        {
            QStandardItem *item = new QStandardItem(specList.at(specIndx));
            item->setCheckable(true);
            item->setCheckState(Qt::Unchecked);
            item->setEditable(false);
            specsListModel->setItem(specsListModel->rowCount(),item);
            specsListView->scrollToBottom();
            //            qDebug()<<specList.at(specIndx);

            emit actorAttributesAndSAS(specList.at(specIndx));
            emit specificationNew(specsListModel->item(specsListModel->rowCount()-1)->text(),specPairList.at(specIndx),1);//1 == Actor

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

bool ActorFrame::validateSalienceData()
{
    int dimensions = (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()-1)/2;
    QVector<QVector<double>> salVec;
    int dim=0;

    for(int row=2; row < sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount(); row+=2)
    {
        for(int col=0; col < sasDataGridTableWidget->model()->columnCount();++col)
        {
            auto ret = sasDataGridTableWidget->item(row,col);

            double val ;
            QVector<double> v;
            if(ret != 0 && (!ret->text().isEmpty()))
            {
                val = sasDataGridTableWidget->item(row,col)->text().toDouble();
                if(val > 100.0 || val <0.0)
                {
                    return false;
                }
                v.append(val);
            }
            else
            {
                val =0.0;
                v.append(val);
            }
            salVec.append(v);
        }
        ++dim;
    }

    double limit = 0.0;

    for(int j=0; j < salVec.at(0).length(); ++j)
    {
        for(int dimensn=0; dimensn < salVec.length();++dimensn)
        {
            limit += salVec.at(j).at(dimensn);
        }
        if(limit > 100.00)
        {
            QMessageBox::information(this,"Limit Crossed","Salience limit has been "
                                                          "crossed hence values are modified to scale the sum to 100");

            for(int row = 0; row < dimensions; ++row)
            {
                auto ret = sasDataGridTableWidget->item(row,j);

                if(ret != 0 && (!ret->text().isEmpty()))
                {
                    double val = sasDataGridTableWidget->item(row,j)->text().toDouble();

                    val = (val/limit);

                    QTableWidgetItem *item = new QTableWidgetItem(val);
                    sasDataGridTableWidget->setItem(row+2,j,item);
                    //                    qDebug()<<"here";
                }
                else
                {
                    continue;
                }
            }
        }
    }
}

//salience calculation against unspecied dimensions (partial)
//QPair<QVector<double>, QVector<int> > ActorFrame::getConstantSalienceValues()
//{
//    QPair<QVector<double>, QVector<int> > salConstData;
//    QVector<double> salConstValues;
//    QVector<int> salConstIndices;
//    //salience rows start from 2
//    for(int row = 2; row < (sasDataGridTableWidget->rowCount()-accomodationMatrixTableView->model()->rowCount()); row +=2)
//    {
//        double val = sasDataGridTableWidget->item(row,1)->text().toDouble(); // col is 1 always
//        if(val>0.0)
//        {
//            salConstValues.append(sasDataGridTableWidget->item(row,1)->text().toDouble());

//        }
//        else
//        {
//            salConstValues.append(actorDataTableView->model()
//                                  ->index(actorComboBox->currentIndex(),row).data().toString());
//            //here row index of sas Datagrid is the column index of actorData table

//            salConstIndices.append(row);

//        }

//    }
//    salConstData.first.append(salConstValues);
//    salConstData.second.append(salConstIndices);\

//    return salConstData;


//}

//void ActorFrame::validateValues(QVector<double> values, int row)
//{
//    if(values.at(0)<values.at(2)) // min < max
//    {
//        QVector <QString> actorValues;
//        for(double min = values.at(0) ; min <= values.at(2) ; min +=  values.at(1))
//        {
//            if(row > 0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
//            {
//                if(row%2!=0) //position
//                {
//                    if(min>100.0 || min <0.0)
//                    {
//                        QMessageBox::information(this,"Limit Crossed","Limit [0,100] of Position values has Crossed");
//                        return 0;
//                    }
//                }
//                else // salience
//                {
//                    if(min>100.0 || min <0.0)
//                    {
//                        QMessageBox::information(this,"Limit Crossed","Limit [0,100] of Salience values has Crossed");
//                        return 0;
//                    }
//                }
//            }
//            actorValues.append(QString::number(min));
//        }

//        if(row%2==0  && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
//        {
//            for()


//        }




//    }
//}

void ActorFrame::addBasePushButtonClicked(bool bl)
{
    if(basePMPRadioButton->isChecked()==true || basePMRadioButton->isChecked())
    {

        if(sasDataGridTableWidget->model()->rowCount()>0)
        {

            bool scaleSum=false;

            QString currentAct = actorComboBox->currentText();
            for(int act =0; act < actorComboBox->count(); ++act)
            {
                //                QString specification;

                QList <int> salIndex;
                QList <int> posIndex;
                QString specification;
                QList <QString> specList;
                QList <QPair<DataValues,SpecsData>> specPairList;

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
                        QString procStr = processBasePM(values,row);
                        if(!procStr.isEmpty())
                        {
                            actorSpecsLHS.append(actorValue);
                            //                            qDebug()<<actorSpecsLHS << " LHS";
                            str.append(procStr);

                            if(row >0 && row%2==0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
                            {
                                salIndex.append(actorSpecsLHS.length()-1);
                            }
                            else
                            {
                                posIndex.append(actorSpecsLHS.length()-1);
                            }
                        }
                        else
                        {
                            return;
                        }
                    }
                    else if (true == basePMPRadioButton->isChecked()&& 2==values.count())
                    {
                        QString procStr = processBasePMP(values,row);
                        if(!procStr.isEmpty())
                        {
                            actorSpecsLHS.append(actorValue);
                            //                            qDebug()<<actorSpecsLHS << " LHS";
                            str.append(procStr);

                            if(row >0 && row%2==0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
                            {
                                salIndex.append(actorSpecsLHS.length()-1);
                            }
                            else
                            {
                                posIndex.append(actorSpecsLHS.length()-1);
                            }
                        }
                        else
                        {
                            return;
                        }

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
                            //                            specification.replace(QString(currentAct+" <"),QString(actorComboBox->itemText(act)+" <"));
                            specList.append(specification);
                            specification.clear();

                            //emit singleSpecification
                            QPair<DataValues,SpecsData> spec;
                            spec.first.append(actorSpecsLHS.at(actorSpecsLHS.count()-1));
                            spec.second.append(actorSpecsRHS.at(actorSpecsRHS.count()-1));
                            specPairList.append(spec);

                            //                            specification.replace(QString(currentAct+" <"),QString(actorComboBox->itemText(act)+" <"));
                            //                            QStandardItem *item = new QStandardItem(specification);
                            //                            item->setCheckable(true);
                            //                            item->setCheckState(Qt::Unchecked);
                            //                            item->setEditable(false);
                            //                            specsListModel->setItem(specsListModel->rowCount(),item);
                            //                            specsListView->scrollToBottom();

                            //                            //emit singleSpecification
                            //                            QPair<DataValues,SpecsData> spec;
                            //                            spec.first.append(actorSpecsLHS.at(actorSpecsLHS.count()-1));
                            //                            spec.second.append(actorSpecsRHS.at(actorSpecsRHS.count()-1));

                            //                            emit actorAttributesAndSAS(specification);
                            //                            emit specificationNew(specsListModel->item(specsListModel->rowCount()-1)->text(),spec,1);//1 == Actor

                            //                            specification.clear();
                        }
                    }
                    else
                    {
                        specification.clear();
                        values.clear();
                        str.clear();
                    }
                }


                //validate sal against other dimensions
                if(salIndex.length()>1)
                {
                    QVector<QVector<QString>> salValues;

                    for(int indx = 0; indx < salIndex.length(); ++indx)
                    {
                        QVector<QString> sal;
                        sal.append(actorSpecsRHS.at(salIndex.at(indx)));
                        salValues.append(sal);
                    }

                    double salLimit;

                    for(int salValIndex = 0; salValIndex < salValues.at(0).length();++salValIndex)
                    {
                        salLimit = 0.0;
                        int salListIndex;
                        for(salListIndex = 0; salListIndex < salValues.length(); ++salListIndex)
                        {
                            salLimit+= salValues.at(salListIndex).at(salValIndex).toDouble();
                        }

                        if(salLimit>100.0)
                        {
                            scaleSum=true;
                            for(int salIndex = 0; salIndex < salValues.length();++salIndex)
                            {
                                //                                qDebug() << salValues[salIndex][salValIndex] << "Before ";

                                salValues[salIndex][salValIndex]=
                                        QString::number(salValues[salIndex][salValIndex].toDouble()/salLimit);

                                //                                qDebug() << salValues[salIndex][salValIndex] << "After ";
                                //                                qDebug() << salIndex << salValIndex << "Indices ";
                            }

                            for(int indx = 0; indx < salIndex.length(); ++indx)
                            {
                                actorSpecsRHS[salIndex[indx]]=salValues[indx];
                            }

                        }

                        for(int i = 0 ; i < salIndex.length(); ++i)
                        {

                            QStringList str = specList.at(salIndex.at(i)-(actorSpecsRHS.length()-specList.length())).split("=");
                            QString newSpec = str.at(0);

                            //                            qDebug()<<newSpec << "newSpec";
                            newSpec.append("=(");
                            for(int j =0 ; j < actorSpecsRHS.at(salIndex.at(i)).length(); ++j)
                            {
                                newSpec.append(actorSpecsRHS.at(salIndex.at(i)).at(j)).append(",");
                            }
                            newSpec.append("#").remove(",#").append(")");
                            specList[salIndex.at(i)-(actorSpecsRHS.length()-specList.length())]=newSpec;

                        }

                        for(int i = 0 ; i < posIndex.length(); ++i)
                        {

                            QStringList str = specList.at(posIndex.at(i)-(actorSpecsRHS.length()-specList.length())).split("=");
                            QString newSpec = str.at(0);

                            //                            qDebug()<<newSpec << "newSpec";
                            newSpec.append("=(");
                            for(int j =0 ; j < actorSpecsRHS.at(posIndex.at(i)).length(); ++j)
                            {
                                newSpec.append(actorSpecsRHS.at(posIndex.at(i)).at(j)).append(",");
                            }
                            newSpec.append("#").remove(",#").append(")");
                            specList[posIndex.at(i)-(actorSpecsRHS.length()-specList.length())]=newSpec;

                        }

                    }
                }

                if(scaleSum==true)
                {
                    scaleSum=false;

                }
                for(int specIndx=0; specIndx < specList.length(); ++specIndx )
                {
                    QStandardItem *item = new QStandardItem(specList.at(specIndx));
                    item->setCheckable(true);
                    item->setCheckState(Qt::Unchecked);
                    item->setEditable(false);
                    specsListModel->setItem(specsListModel->rowCount(),item);
                    specsListView->scrollToBottom();
                    //                    qDebug()<<specList.at(specIndx);

                    emit actorAttributesAndSAS(specList.at(specIndx));
                    emit specificationNew(specsListModel->item(specsListModel->rowCount()-1)->text(),specPairList.at(specIndx),1);//1 == Actor

                }
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
            if(importedDataCSV==true)//csv
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
        if(importedDataCSV==true)
        {
            csvAccModel->removeColumn(actorDataTableView->currentIndex().row());
            csvAccModel->removeRow(actorDataTableView->currentIndex().row());
        }
        else
        {
            xmlAccModel->removeColumn(actorDataTableView->currentIndex().row());
            xmlAccModel->removeRow(actorDataTableView->currentIndex().row());
        }


        if(importedDataCSV==true)//csv
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
            if(importedDataCSV==true)//csv
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

                    if(importedDataCSV==true)//csv
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
                    if(importedDataCSV==true)//csv
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

                if(importedDataCSV==true)//csv
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
            if(importedDataCSV==true)//csv
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
            if(importedDataCSV==true)//csv
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
        if(importedDataCSV==true)
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
        if(importedDataCSV==true)
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

        if(importedDataCSV==true)//csv
        {
            initializeAccMatrixRowCol(actorDataTableView->currentIndex().row(),"CSV");
        }
        else // xml
        {
            initializeAccMatrixRowCol(actorDataTableView->currentIndex().row(),"XML");
        }
        actorHeader.clear();

        if(importedDataCSV==true)//csv
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
        if(importedDataCSV == true)//csv
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

QString ActorFrame::processMinDeltaMax(QVector<double> values, int row)
{
    //    salIndex(values,row);

    QString processedString;
    if(values.at(0)<values.at(2)) // min < max
    {
        QVector <QString> actorValues;
        for(double min = values.at(0) ; min <= values.at(2) ; min +=  values.at(1))
        {
            if(row > 0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
            {
                if(row%2!=0) //position
                {
                    if(min>100.0 || min <0.0)
                    {
                        QMessageBox::information(this,"Limit Crossed","Limit [0,100] of Position values has Crossed at Row "+ QString::number(row+1));
                        return 0;
                    }
                }
                else // salience
                {
                    if(min>100.0 || min <0.0)
                    {
                        QMessageBox::information(this,"Limit Crossed","Limit [0,100] of Salience values has Crossed at Row "+ QString::number(row+1));
                        return 0;
                    }
                }
            }
            actorValues.append(QString::number(min));
            //  qDebug()<<min << "val";
            processedString.append(QString::number(min)).append(",");
            //qDebug()<<processedString;
        }

        if(!actorValues.isEmpty())
        {
            actorSpecsRHS.append(actorValues);
            //            qDebug()<<actorSpecsRHS <<"RHS";
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

QString ActorFrame::processBasePM(QVector<double> values, int row)
{
    QString processedString;

    QVector <QString> actorValues;

    double base = values.at(0) - values.at(1);
    actorValues.append(QString::number(base));
    processedString.append(QString::number(base)).append(",");

    actorValues.append(QString::number(values.at(0)));
    processedString.append(QString::number(values.at(0))).append(",");

    base = values.at(0) + values.at(1);
    actorValues.append(QString::number(base));
    processedString.append(QString::number(base));

    for(int i = 0 ; i  < actorValues.length(); ++ i)
    {
        if(row > 0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
        {
            if(row%2!=0) //position
            {
                if(actorValues.at(i).toDouble()>100.0 || actorValues.at(i).toDouble()<0.0)
                {
                    QMessageBox::information(this,"Limit Crossed","Limit [0,100] of Position values has Crossed at Row "+ QString::number(row+1));
                    return 0;
                }
            }
            else // salience
            {
                if(actorValues.at(i).toDouble()>100.0 || actorValues.at(i).toDouble()<0.0)
                {
                    QMessageBox::information(this,"Limit Crossed","Limit [0,100] of Salience values has Crossed at Row "+ QString::number(row+1));
                    return 0;
                }
            }
        }
    }
    actorSpecsRHS.append(actorValues);
    //    qDebug()<<actorSpecsRHS <<"RHS";
    return processedString;

}

QString ActorFrame::processBasePMP(QVector<double> values, int row)
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


    for(int i = 0 ; i  < actorValues.length(); ++ i)
    {
        if(row > 0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
        {
            if(row%2!=0) //position
            {
                if(actorValues.at(i).toDouble()>100.0 || actorValues.at(i).toDouble()<0.0)
                {
                    QMessageBox::information(this,"Limit Crossed","Limit [0,100] of Position values has Crossed at Row "+ QString::number(row+1));
                    return 0;
                }
            }
            else // salience
            {
                if(actorValues.at(i).toDouble()>100.0 || actorValues.at(i).toDouble()<0.0)
                {
                    QMessageBox::information(this,"Limit Crossed","Limit [0,100] of Salience values has Crossed at Row "+ QString::number(row+1));
                    return 0;
                }
            }
        }
    }
    actorSpecsRHS.append(actorValues);
    //    qDebug()<<actorSpecsRHS <<"RHS";
    return processedString;
}

QString ActorFrame::processValuesN(QVector<double> values,int row)
{
    QString processedString;
    QVector <QString> actorValues;

    //    qDebug()<<values;
    for(int val = 0 ; val < values.count();++val)
    {
        actorValues.append(QString::number(values.at(val)));
        processedString.append(QString::number(values.at(val))).append(",");

        if(row > 0 && row < (sasDataGridTableWidget->rowCount()-actorDataTableView->model()->rowCount()))
        {
            if(row%2!=0) //position
            {
                if(actorValues.at(val).toDouble()>100.0 || actorValues.at(val).toDouble()<0.0)
                {
                    QMessageBox::information(this,"Limit Crossed","Limit [0,100] of Position values has Crossed at Row "+ QString::number(row+1));
                    return 0;
                }
            }
            else // salience
            {
                if(actorValues.at(val).toDouble()>100.0 || actorValues.at(val).toDouble()<0.0)
                {
                    QMessageBox::information(this,"Limit Crossed","Limit [0,100] of Salience values has Crossed at Row "+ QString::number(row+1));
                    return 0;
                }
            }
        }
    }
    processedString.remove(processedString.length()-1,1);

    if(!actorValues.isEmpty())
    {
        actorSpecsRHS.append(actorValues);
        //        qDebug()<<actorSpecsRHS <<"RHS";

    }
    return processedString;
}

void ActorFrame::validateData()
{
    int actors = actorDataTableView->model()->rowCount();
    bool vio = false;
    QString violations;
    violations.append("Position Values have crossed the limit [0,100] for \n");

    //validate position values
    for(int row = 0; row < actors; ++row)
    {
        for( int col =3 ; col < actorDataTableView->model()->columnCount();col+=2)
        {
            if((actorDataTableView->model()->data(actorDataTableView->model()->index(row,col)).toDouble()>100.00))
            {
                vio=true;
                violations.append(actorDataTableView->model()->data(actorDataTableView->model()->index(row,0)).toString()+" at ");
                violations.append("Row: "+QString::number(row+1)+ " and Column: ");
                violations.append(actorDataTableView->model()->headerData(col,Qt::Horizontal).toString()+" \n");
            }
        }
    }
    if(vio==true)
    {
        vio=false;
        QMessageBox::warning(this,"Position Limit Crossed",violations);
    }

    //validate salience values
    violations.clear();
    double sal;
    violations.append("Salience Values have crossed the limit [0,1] for \n");
    for(int row = 0; row < actors; ++row)
    {
        sal = 0.0;
        for( int col=4 ; col < actorDataTableView->model()->columnCount(); col+=2)
        {
            sal += (actorDataTableView->model()->data(actorDataTableView->model()->index(row,col)).toDouble())/100.00;
        }
        if(sal>1.0)
        {
            vio=true;
            violations.append(actorDataTableView->model()->data(actorDataTableView->model()->index(row,0)).toString()+" at ");
            violations.append("Row: "+QString::number(row+1) + " \n");
        }
    }
    if(vio==true)
    {
        vio=false;
        QMessageBox::warning(this,"Position Limit Crossed",violations);
    }
}

