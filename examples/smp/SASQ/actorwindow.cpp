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
    //    initializeModelControls();
    //    initializeModelSpecifications();

    actorInputTableFrame->setLayout(actorInputGridLayout);
    actorSensTableFrame->setLayout(actorSensGridLayout);
    actorSpecsFrame->setLayout(actorSpecsGridLayout);
    actorControlsFrame->setLayout(actorControlsGridLayout);

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

    inputDataTabWidget->addTab(actorDataTableView,"Actor Data");
    inputDataTabWidget->addTab(accomodationMatrixTableView,"Accomodation Matrix");

    actorInputGridLayout->addWidget(inputDataTabWidget);
}

void ActorFrame::initializeAccomodationMatrix()
{
    csvAccModel = new QStandardItemModel;
    accomodationMatrixTableView->setModel(csvAccModel);

    //Accomodation Matrix
    while(csvAccModel->rowCount() != csvActorDataModel->rowCount()
          && csvActorDataModel->rowCount()>csvAccModel->rowCount())
    {
        int rowCount = csvAccModel->rowCount();
        if(csvAccModel->rowCount() < csvActorDataModel->rowCount() ||
                csvAccModel->columnCount() < csvActorDataModel->rowCount())
        {
            csvAccModel->insertRows(csvAccModel->rowCount(),(csvActorDataModel->rowCount()
                                                             - csvAccModel->rowCount()));
            csvAccModel->insertColumns(csvAccModel->columnCount(),(csvActorDataModel->rowCount()
                                                                   - csvAccModel->columnCount()));
        }
        QString actorHeader;
        for(int rows = rowCount; rows < csvAccModel->rowCount();++ rows)
        {
            actorHeader=csvActorDataModel->item(rows,0)->text().trimmed();
            csvAccModel->setHorizontalHeaderItem(rows, new QStandardItem(actorHeader));
            csvAccModel->setVerticalHeaderItem(rows, new QStandardItem(actorHeader));
            initializeAccomodationRowCol(rows,"CSV");
            actorHeader.clear();
        }
    }

    //    csvTableAffinityView->setContextMenuPolicy(Qt::CustomContextMenu);
    //    csvTableAffinityView->setEditTriggers(QAbstractItemView::AllEditTriggers);
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
    csvActorDataModel = new QStandardItemModel;
    csvActorDataModel=model;
    actorDataTableView->setModel(csvActorDataModel);

    initializeAccomodationMatrix();

}
