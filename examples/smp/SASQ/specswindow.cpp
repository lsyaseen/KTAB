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

#include "specswindow.h"


SpecificationFrame::SpecificationFrame(QObject *parent)
{
    frameMainLayout = new QGridLayout(this);
    initializeFrameLayout();
    setLayout(frameMainLayout);

}

SpecificationFrame::~SpecificationFrame()
{

}

void SpecificationFrame::initializeFrameLayout()
{
    QFrame * topFrame = new QFrame;
    QGridLayout * gridL = new QGridLayout;

    QSplitter *splitterV = new  QSplitter(Qt::Vertical,this);
    QSplitter *splitterH = new  QSplitter(Qt::Horizontal,this);

    filterFrame = new QFrame;
    crossProductFrame = new  QFrame;
    specsFrame = new QFrame;

    filterFrameGridLayout = new QGridLayout;
    crossProductFrameGridLayout = new QGridLayout;
    specsFrameGridLayout = new QGridLayout;

    filterFrame->setFrameStyle(QFrame::StyledPanel);
    crossProductFrame->setFrameStyle(QFrame::StyledPanel);
    specsFrame->setFrameStyle(QFrame::StyledPanel);

    initializeFiltersFrame();
    initializeCrossProductFrame();
    initializeSensAnalysisFrame();

    filterFrame->setLayout(filterFrameGridLayout);
    crossProductFrame->setLayout(crossProductFrameGridLayout);
    specsFrame->setLayout(specsFrameGridLayout);

    splitterV->addWidget(filterFrame);
    splitterV->addWidget(crossProductFrame);
    splitterV->setChildrenCollapsible(false);

    gridL->addWidget(splitterV);
    gridL->setContentsMargins(1,1,1,1);

    topFrame->setLayout(gridL);

    splitterH->addWidget(topFrame);
    splitterH->addWidget(specsFrame);
    splitterH->setChildrenCollapsible(false);

    frameMainLayout->addWidget(splitterH);
    setFrameStyle(QFrame::StyledPanel);
}

void SpecificationFrame::initializeFiltersFrame()
{
    QLabel * headingLabel = new QLabel("FILTERS");
    headingLabel->setAlignment(Qt::AlignHCenter);
    headingLabel->setStyleSheet("background-color: rgb(194, 194, 194);");
    headingLabel->setFrameStyle(6);

    QHBoxLayout * hBoxControlsLay = new QHBoxLayout;
    actorListComboBox = new QComboBox;
    attributeComboBox = new QComboBox;
    sasAttributeComboBox = new QComboBox;
    relationshipComboBox = new QComboBox;
    valueLineEdit = new QLineEdit;

    QPushButton * addSpecification = new QPushButton("Add Specification");
    valueLineEdit->setMaximumWidth(100);
    connect(addSpecification,SIGNAL(clicked(bool)),this,SLOT(filterAddSpecificationClicked(bool)));

    relationshipComboBox->addItem("==");
    relationshipComboBox->addItem("+");
    relationshipComboBox->addItem("-");
    relationshipComboBox->addItem("±");
    connect(relationshipComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(relationComboBoxChanged(int)));
    //    relationshipComboBox->setEditable(true);
    //    relationshipComboBox->lineEdit()->setAlignment(Qt::AlignHCenter);
    //    relationshipComboBox->lineEdit()->setReadOnly(true);

    //    for(int i =0 ; i < relationshipComboBox->count() ; ++i)
    //    {
    //        relationshipComboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    //    }
    valueLineEdit->setEnabled(false);
    valueLineEdit->setValidator( new QDoubleValidator(0,9999999, 2, this) );
    relationshipComboBox->setMaximumWidth(100);

    hBoxControlsLay->addWidget(actorListComboBox);
    hBoxControlsLay->addWidget(attributeComboBox);
    hBoxControlsLay->addWidget(sasAttributeComboBox);
    hBoxControlsLay->addWidget(relationshipComboBox);
    hBoxControlsLay->addWidget(valueLineEdit);
    hBoxControlsLay->setSpacing(0);

    filterListView = new QListView;
    filterListModel = new QStandardItemModel;
    filterListView->setModel(filterListModel);
    filterListView->setContextMenuPolicy(Qt::CustomContextMenu);

    filterFrameGridLayout->addWidget(headingLabel,0,0);
    filterFrameGridLayout->addLayout(hBoxControlsLay,1,0);
    filterFrameGridLayout->addWidget(filterListView,2,0);
    filterFrameGridLayout->addWidget(addSpecification,3,0);

    filterFrameGridLayout->setSpacing(0);
    connect(filterListView,SIGNAL(customContextMenuRequested(QPoint)),SLOT(filterListViewContextMenu(QPoint)));
    connect(actorListComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(actorIndexChangedComboBox(int)));

}

void SpecificationFrame::initializeCrossProductFrame()
{
    QLabel * headingLabel = new QLabel("CROSS PRODUCT");
    headingLabel->setAlignment(Qt::AlignHCenter);
    headingLabel->setStyleSheet("background-color: rgb(194, 194, 194);");
    headingLabel->setFrameStyle(6);

    QHBoxLayout * hBoxControlsLay = new QHBoxLayout;
    att1ComboBox = new QComboBox;
    QLabel * valueLabel = new QLabel("*");
    valueLabel->setAlignment(Qt::AlignCenter);
    att2ComboBox = new QComboBox;
    valueLabel->setMaximumWidth(100);

    hBoxControlsLay->addWidget(att1ComboBox);
    hBoxControlsLay->addWidget(valueLabel);
    hBoxControlsLay->addWidget(att2ComboBox);
    hBoxControlsLay->setSpacing(0);

    QHBoxLayout * addSpecsLay = new QHBoxLayout;
    QPushButton * addSpecification = new QPushButton("Add Specification");
    QPushButton * crossAllSpecifications = new QPushButton("Cross All Specifications");

    connect(addSpecification,SIGNAL(clicked(bool)),this,SLOT(crossproductAddSpecificationClicked(bool)));
    connect(crossAllSpecifications,SIGNAL(clicked(bool)),this,SLOT(crossAllSpecs(bool)));

    addSpecsLay->addWidget(addSpecification);
    addSpecsLay->addWidget(crossAllSpecifications);

    crossProductListView = new QListView;
    crossProductListModel = new QStandardItemModel;
    crossProductListView->setModel(crossProductListModel);
    crossProductListView->setContextMenuPolicy(Qt::CustomContextMenu);

    crossProductFrameGridLayout->addWidget(headingLabel,0,0);
    crossProductFrameGridLayout->addLayout(hBoxControlsLay,1,0);
    crossProductFrameGridLayout->addWidget(crossProductListView,2,0);
    crossProductFrameGridLayout->addLayout(addSpecsLay,3,0);
    crossProductFrameGridLayout->setSpacing(0);

    connect(crossProductListView,SIGNAL(customContextMenuRequested(QPoint)),SLOT(crossproductContextMenu(QPoint)));

}

void SpecificationFrame::initializeSensAnalysisFrame()
{
    specsListView = new QListView;
    specsListView->setAutoScroll(true);
    connect(specsListView,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(specsListViewContextMenu(QPoint)));
    specsListView->setContextMenuPolicy(Qt::CustomContextMenu);

    specsListModel = new QStandardItemModel();
    specsListView->setModel(specsListModel);
    specsFrameGridLayout->addWidget(specsListView);
}

void SpecificationFrame::createCrossProductSpec(int att1, int att2)
{
    if(att1ComboBox->count() >0 && att2ComboBox->count() >0)
    {
        QApplication::setOverrideCursor(QCursor(QPixmap("://images/hourglass.png")));
        QApplication::processEvents();

        int attributeIndex1 = att1;
        int attributeIndex2 = att2;

        int att1SpecType;
        int att1SpecTypeIndex;
        int att2SpecType;
        int att2SpecTypeIndex;

        if(attributeIndex1 < specsIndexlist[0])
        {
            att1SpecType = 0;
            att1SpecTypeIndex = attributeIndex1;
        }
        else if(attributeIndex1 < (specsIndexlist[0]+specsIndexlist[1]))
        {
            att1SpecType = 1;
            att1SpecTypeIndex =attributeIndex1- specsIndexlist[0];
        }
        else if (attributeIndex1 < (specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]))
        {
            att1SpecType = 2;
            att1SpecTypeIndex =attributeIndex1- (specsIndexlist[0]+specsIndexlist[1]);
        }
        else if (attributeIndex1 < (specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]+specsIndexlist[3]))
        {
            att1SpecType = 3;
            att1SpecTypeIndex = attributeIndex1 - (specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]);
        }
        else
        {
            return;
        }

        if(attributeIndex2 < specsIndexlist[0])
        {
            att2SpecType = 0;
            att2SpecTypeIndex = attributeIndex2;
        }
        else if(attributeIndex2 < (specsIndexlist[0]+specsIndexlist[1]))
        {
            att2SpecType = 1;
            att2SpecTypeIndex = attributeIndex2-specsIndexlist[0];
        }
        else if (attributeIndex2 < (specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]))
        {
            att2SpecType = 2;
            att2SpecTypeIndex = attributeIndex2 - (specsIndexlist[0] + specsIndexlist[1]);
        }
        else if (attributeIndex2 < (specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]+specsIndexlist[3]))
        {
            att2SpecType = 3;
            att2SpecTypeIndex = attributeIndex2 -(specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]);
//            qDebug()<< att2SpecTypeIndex <<"att2SpecTypeIndex";
        }
        else
        {
            return;
        }


        QVector<QString> modActDataVal1;
        SpecificationVector filCrosDataVals1;

        QVector<QString> modActDataVal2;
        SpecificationVector filCrosDataVals2;

        QVector<QString> lhsVec;
        QString specsStr;
        specsStr.append("(");

        //LHS specification of cross product
        if(att1SpecType==0)
        {
//            qDebug()<< modelSpecificationsLHS <<"modelSpecificationsLHS";
            lhsVec.append(modelSpecificationsLHS.at(att1SpecTypeIndex));
            specsStr.append(modelSpecificationsLHS.at(att1SpecTypeIndex)).append(",");

            modActDataVal1=(modelSpecificationsRHS.at(att1SpecTypeIndex));
        }
        else if(att1SpecType==1)
        {
//            qDebug()<< actorSpecificationsLHS<< "actorSpecificationsLHS";

            lhsVec.append(actorSpecificationsLHS.at(att1SpecTypeIndex));
            specsStr.append(actorSpecificationsLHS.at(att1SpecTypeIndex)).append(",");

            modActDataVal1=(actorSpecificationsRHS.at(att1SpecTypeIndex));
        }
        else if(att1SpecType==2)
        {
//            qDebug()<< filterSpecificationsLHS<< "filterSpecificationsLHS";
            for(int i =0; i <filterSpecificationsLHS.at(att1SpecTypeIndex).length();++i)
            {
                lhsVec.append(filterSpecificationsLHS.at(att1SpecTypeIndex).at(i));
                specsStr.append(filterSpecificationsLHS.at(att1SpecTypeIndex).at(i)).append(",");
            }
            filCrosDataVals1.append(filterSpecificationsRHS.at(att1SpecTypeIndex));
        }
        else if(att1SpecType==3)
        {
//            qDebug()<< crossProdSpecificationsLHS<<"crossProdSpecificationsLHS";
            for(int i =0; i <crossProdSpecificationsLHS.at(att1SpecTypeIndex).length();++i)
            {
                lhsVec.append(crossProdSpecificationsLHS.at(att1SpecTypeIndex).at(i));
                specsStr.append(crossProdSpecificationsLHS.at(att1SpecTypeIndex).at(i)).append(",");
            }
            filCrosDataVals1.append(crossProdSpecificationsRHS.at(att1SpecTypeIndex));
        }

        //second specification of cross product
        if(att2SpecType==0)
        {
//            qDebug()<< modelSpecificationsLHS<<"modelSpecificationsLHS";
            lhsVec.append(modelSpecificationsLHS.at(att2SpecTypeIndex));
            specsStr.append(modelSpecificationsLHS.at(att2SpecTypeIndex)).append(",");
            specsStr.append("#").remove(",#").append(")=(");
            modActDataVal2=(modelSpecificationsRHS.at(att2SpecTypeIndex));
        }
        else if(att2SpecType==1)
        {
//            qDebug()<< actorSpecificationsLHS<<"actorSpecificationsLHS";

            lhsVec.append(actorSpecificationsLHS.at(att2SpecTypeIndex));
            specsStr.append(actorSpecificationsLHS.at(att2SpecTypeIndex)).append(",");

            specsStr.append("#").remove(",#").append(")=(");
            modActDataVal2=(actorSpecificationsRHS.at(att2SpecTypeIndex));
        }
        else if(att2SpecType==2)
        {
//            qDebug()<< filterSpecificationsLHS<<"filterSpecificationsLHS";
            for(int i =0; i <filterSpecificationsLHS.at(att2SpecTypeIndex).length();++i)
            {
                lhsVec.append(filterSpecificationsLHS.at(att2SpecTypeIndex).at(i));
                specsStr.append(filterSpecificationsLHS.at(att2SpecTypeIndex).at(i)).append(",");
            }
            specsStr.append("#").remove(",#").append(")=(");
            filCrosDataVals2.append(filterSpecificationsRHS.at(att2SpecTypeIndex));
        }
        else if(att2SpecType==3)
        {
//            qDebug()<< crossProdSpecificationsLHS<<"crossProdSpecificationsLHS";
            for(int i =0; i <crossProdSpecificationsLHS.at(att2SpecTypeIndex).length();++i)
            {
                lhsVec.append(crossProdSpecificationsLHS.at(att2SpecTypeIndex).at(i));
                specsStr.append(crossProdSpecificationsLHS.at(att2SpecTypeIndex).at(i)).append(",");
            }
            specsStr.append("#").remove(",#").append(")=(");
            filCrosDataVals2.append(crossProdSpecificationsRHS.at(att2SpecTypeIndex));
        }

        QVector<QVector<QString>> specification;
        if((att1SpecType==0||att1SpecType==1) && ((att2SpecType==0||att2SpecType==1)))
        {
            specification.clear();
            QVector<QString> newSpec;

            for(int i=0; i < modActDataVal1.length(); ++i)
            {
                for(int j=0; j < modActDataVal2.length(); ++j)
                {
                    specsStr.append("(").append(modActDataVal1.at(i)).append(":");
                    newSpec.append(modActDataVal1.at(i));
                    specsStr.append(modActDataVal2.at(j)).append("),");
                    newSpec.append(modActDataVal2.at(j));
                    specification.append(newSpec);
//                    qDebug()<< newSpec << "newSpec" ;
                    newSpec.clear();
                }
            }
            specsStr.append("#").remove(",#").append(")");
        }
        else if((att1SpecType==0||att1SpecType==1) && ((att2SpecType==2||att2SpecType==3)))
        {
            specification.clear();
            QVector<QString> newSpec;

            for(int i=0; i < modActDataVal1.length(); ++i)
            {
                for(int itr =0; itr < filCrosDataVals2.at(0).length();++itr)
                {
                    specsStr.append("(").append(modActDataVal1.at(i)).append(":");
                    newSpec.append(modActDataVal1.at(i));
                    for(int j=0; j < filCrosDataVals2.at(0).at(itr).length(); ++j)
                    {
                        specsStr.append(filCrosDataVals2.at(0).at(itr).at(j)).append(":");
                        newSpec.append(filCrosDataVals2.at(0).at(itr).at(j));
                    }
                    specsStr.append("#").remove(":#").append("),");
                    specification.append(newSpec);
                    newSpec.clear();
                }
            }
            specsStr.append("#").remove(",#").append(")");

        }
        else if((att1SpecType==2||att1SpecType==3) && ((att2SpecType==0||att2SpecType==1)))
        {

            specification.clear();
            QVector<QString> newSpec;

            for(int i=0; i < modActDataVal2.length(); ++i)
            {
                for(int itr =0; itr < filCrosDataVals1.at(0).length();++itr)
                {
                    specsStr.append("(");
                    for(int j=0; j < filCrosDataVals1.at(0).at(itr).length(); ++j)
                    {
                        specsStr.append(filCrosDataVals1.at(0).at(itr).at(j)).append(":");
                        newSpec.append(filCrosDataVals1.at(0).at(itr).at(j));
                    }
                    specsStr.append(modActDataVal2.at(i)).append("),");
                    newSpec.append(modActDataVal2.at(i));
                    specification.append(newSpec);
                    newSpec.clear();
                }
            }
            specsStr.append("#").remove(",#").append(")");
        }
        else if((att1SpecType==2||att1SpecType==3) && ((att2SpecType==2||att2SpecType==3)))
        {
            specification.clear();
            QVector<QString> newSpec;

            int strLimitSpecs =20;
            for(int itrI =0; itrI < filCrosDataVals1.at(0).length();++itrI)
            {
                for(int itrJ =0; itrJ < filCrosDataVals2.at(0).length();++itrJ)
                {
                    if(strLimitSpecs>0)
                    {
                        specsStr.append("(");
                    }
                    for(int i=0; i < filCrosDataVals1.at(0).at(itrI).length(); ++i)
                    {
                        if(strLimitSpecs>0)
                        {
                            specsStr.append(filCrosDataVals1.at(0).at(itrI).at(i)).append(":");
                        }
                        newSpec.append(filCrosDataVals1.at(0).at(itrI).at(i));
                    }
                    for(int j=0; j < filCrosDataVals2.at(0).at(itrJ).length(); ++j)
                    {
                        if(strLimitSpecs>0)
                        {
                            specsStr.append(filCrosDataVals2.at(0).at(itrJ).at(j)).append(":");
                        }
                        newSpec.append(filCrosDataVals2.at(0).at(itrJ).at(j));
                    }
                    if(strLimitSpecs>0)
                    {
                        specsStr.append("#").remove(":#").append("),");
                        --strLimitSpecs;
                    }
                    specification.append(newSpec);
                    newSpec.clear();
                }
            }
            if(strLimitSpecs<=0)
            {
                specsStr.append(".......))");
            }
            else
            {
                specsStr.append("#").remove(",#").append(")");
            }
        }
        else
        {
            return;
        }

        crossProductSpecsLHS.append(lhsVec);
//        qDebug()<< lhsVec << "\n" << specsStr<<"lhsVec";
        crossProductSpecsRHS.append(specification);

        QStandardItem * item = new QStandardItem(specsStr);
        item->setCheckable(true);
        item->setEditable(false);
        item->setCheckState(Qt::Unchecked);
        crossProductListModel->appendRow(item);
        crossProductListView->scrollToBottom();

        QPair<SpecsData,SpecificationVector> crossProdSpec;
        crossProdSpec.first.append(crossProductSpecsLHS.at(crossProductSpecsLHS.count()-1));
        crossProdSpec.second.append(crossProductSpecsRHS.at(crossProductSpecsRHS.count()-1));

        filterCrossProdSpecification(crossProductListModel->item(crossProductListModel->rowCount()-1)->text(),crossProdSpec,3);

        QApplication::restoreOverrideCursor();
    }
}

void SpecificationFrame::listViewClicked()
{
    for(int index=0; index < specsListModel->rowCount();++index)
    {
        QStandardItem * item = specsListModel->item(index);
        if (item->data(Qt::CheckStateRole).toBool() == true)   // Item checked, remove
        {

            int firstVal;
            int secondVal;

            if(index < specsIndexlist[0])
            {
                firstVal = 0;
                secondVal = index;
            }
            else if(index < (specsIndexlist[0]+specsIndexlist[1]))
            {
                firstVal = 1;
                secondVal =index- specsIndexlist[0];
            }
            else if (index < (specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]))
            {
                firstVal = 2;
                secondVal =index- (specsIndexlist[0]+specsIndexlist[1]);
            }
            else if (index < (specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]+specsIndexlist[3]))
            {
                firstVal = 3;
                secondVal = index - (specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]);
            }
            else
            {
                return;
            }

            if(firstVal==0)
            {
                modelSpecificationsLHS.removeAt(secondVal);
                modelSpecificationsRHS.removeAt(secondVal);
            }
            else if(firstVal==1)
            {
                actorSpecificationsLHS.removeAt(secondVal);
                actorSpecificationsRHS.removeAt(secondVal);
            }
            else if(firstVal==2)
            {
                filterSpecificationsLHS.removeAt(secondVal);
                filterSpecificationsRHS.removeAt(secondVal);
            }
            else if(firstVal==3)
            {
                crossProdSpecificationsLHS.removeAt(secondVal);
                crossProdSpecificationsRHS.removeAt(secondVal);
            }

            specsIndexlist[firstVal]= specsIndexlist[firstVal]-1;
            specsListModel->removeRow(index);
            index = index-1; // index changed to current row, deletion of item changes list index
        }
    }
    populateCrossProductComboBox();
}

void SpecificationFrame::listViewSpecsRemoveAllClicked()
{
    specsListModel->clear();

    modelSpecificationsLHS.clear();
    modelSpecificationsRHS.clear();
    specsIndexlist[0]=0;

    actorSpecificationsLHS.clear();
    actorSpecificationsRHS.clear();
    specsIndexlist[1]=0;

    filterSpecificationsLHS.clear();
    filterSpecificationsRHS.clear();
    specsIndexlist[2]=0;

    crossProdSpecificationsLHS.clear();
    crossProdSpecificationsRHS.clear();
    specsIndexlist[3]=0;

    populateCrossProductComboBox();

}

void SpecificationFrame::actorModelSpecification(QString spec,QPair<SpecificationFrame::DataValues,
                                                 SpecificationFrame::SpecsData> specData, int type)
{
    if(0==type) //model
    {
        QStandardItem * item = new QStandardItem(spec);
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setEditable(false);

        specsListModel->insertRow(specsIndexlist[0],item);
//        qDebug()<< specsIndexlist[0]  << "apecs Index list" <<specsListModel->rowCount();
        specsListView->scrollToBottom();

        if(!specData.second.isEmpty())
        {
            modelSpecificationsLHS.insert(specsIndexlist[0],specData.first.at(0));
            modelSpecificationsRHS.insert(specsIndexlist[0],specData.second.at(0));

            //            specsIndexlist[0]=specData.second.length();
            specsIndexlist[0]=specsIndexlist[0]+1;

//            qDebug()<< specsIndexlist[0]  << "apecs Index list";
//            qDebug()<< specsListModel->item(specsListModel->rowCount()-1)->text();
        }
    }

    else if(1==type) //actor
    {
        QStandardItem * item = new QStandardItem(spec);
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setEditable(false);

        specsListModel->insertRow((specsIndexlist[0]+specsIndexlist[1]),item);
        specsListView->scrollToBottom();

        if(!specData.second.isEmpty())
        {
            actorSpecificationsLHS.insert(specsIndexlist[1],specData.first.at(0));
            actorSpecificationsRHS.insert(specsIndexlist[1],specData.second.at(0));

            //            specsIndexlist[1]=specData.second.length();
            specsIndexlist[1] = specsIndexlist[1]+1;
        }
    }
    populateCrossProductComboBox();

}

void SpecificationFrame::filterCrossProdSpecification(QString spec, QPair<SpecificationFrame::SpecsData,
                                                      SpecificationFrame::SpecificationVector> specData, int type)
{
    if(2==type) //filter
    {
        QStandardItem * item = new QStandardItem(spec);
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setEditable(false);

        specsListModel->insertRow((specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]),item);
        specsListView->scrollToBottom();

        if(!specData.second.isEmpty())
        {
            filterSpecificationsLHS.insert(specsIndexlist[2],specData.first.at(0));
            filterSpecificationsRHS.insert(specsIndexlist[2],specData.second.at(0));

            //            specsIndexlist[2]=specData.second.length();
            specsIndexlist[2]=specsIndexlist[2]+1;
        }
    }
    else if(3==type) //crossproduct
    {
        QStandardItem * item = new QStandardItem(spec);
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setEditable(false);

        specsListModel->insertRow((specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]+specsIndexlist[3]),item);
        specsListView->scrollToBottom();

        if(!specData.second.isEmpty())
        {
            crossProdSpecificationsLHS.insert(specsIndexlist[3],specData.first.at(0));
            crossProdSpecificationsRHS.insert(specsIndexlist[3],specData.second.at(0));

            //            specsIndexlist[3]=specData.second.length();
            specsIndexlist[3]=specsIndexlist[3]+1;

        }
    }
    populateCrossProductComboBox();

}

void SpecificationFrame::removeSpecModelActor(int index, int type,QString specLHS)
{
    if(type==0)
    {
        if(index < specsIndexlist[0])
        {
//            qDebug()<<modelSpecificationsLHS.at(index) << specLHS;

            if(modelSpecificationsLHS.at(index)==specLHS)
            {
                modelSpecificationsLHS.remove(index);
                modelSpecificationsRHS.remove(index);
                specsListModel->removeRow(index);
                specsIndexlist[0]-=1;
            }
        }
    }
    else if(type==1)
    {
        if(index < specsIndexlist[1])
        {
//            qDebug()<<actorSpecificationsLHS.at(index) << specLHS;

            if(actorSpecificationsLHS.at(index)==specLHS)
            {
                sasAttributeComboBox->removeItem(index);
                actorSpecificationsLHS.remove(index);
                actorSpecificationsRHS.remove(index);
                specsListModel->removeRow(specsIndexlist[0]+index);
                specsIndexlist[1]-=1;
            }
        }
    }
    populateCrossProductComboBox();
}

void SpecificationFrame::removeSpecFilterCrossProd(int index, int type, SpecificationFrame::DataValues specLHS)
{

    if(type==2)
    {
        if(index < specsIndexlist[2])
        {
//            qDebug()<<filterSpecificationsLHS.at(index) << specLHS;

            if(filterSpecificationsLHS.at(index).count()==specLHS.count())
            {
                for(int i=0; i < filterSpecificationsLHS.at(index).count(); ++i)
                {
                    if(filterSpecificationsLHS.at(index).at(i)!=specLHS.at(i))
                    {
                        return;
                    }
                }

                filterSpecificationsLHS.remove(index);
                filterSpecificationsRHS.remove(index);
                specsListModel->removeRow(specsIndexlist[0]+specsIndexlist[1]+index);
                specsIndexlist[2]-=1;
            }
        }
    }
    else if(type==3)
    {
        if(index < specsIndexlist[3])
        {
//            qDebug()<<crossProdSpecificationsLHS.at(index) << specLHS;

            if(crossProdSpecificationsLHS.at(index).count()==specLHS.count())
            {
                for(int i=0; i < crossProdSpecificationsLHS.at(index).count(); ++i)
                {
                    if(crossProdSpecificationsLHS.at(index).at(i)!=specLHS.at(i))
                    {
                        return;
                    }
                }
                crossProdSpecificationsLHS.remove(index);
                crossProdSpecificationsRHS.remove(index);
                specsListModel->removeRow(specsIndexlist[0]+specsIndexlist[1]+specsIndexlist[2]+index);
                specsIndexlist[3]-=1;
            }
        }
    }
    populateCrossProductComboBox();
}


void SpecificationFrame::specsListViewContextMenu(QPoint pos)
{
    if(specsListView->model()->rowCount()>0)
    {
        QMenu *menu = new QMenu(this);
        menu->addAction("Remove Selected Items", this, SLOT(listViewClicked()));
        menu->addAction("Remove All Items", this, SLOT(listViewSpecsRemoveAllClicked()));
        menu->popup(specsListView->mapToGlobal(pos));
    }
}

void SpecificationFrame::populateCrossProductComboBox()
{
    att1ComboBox->clear();
    att2ComboBox->clear();
    for(int i = 0 ; i < specsListModel->rowCount();++i)
    {
        att1ComboBox->addItem(specsListModel->item(i)->text());
        att2ComboBox->addItem(specsListModel->item(i)->text());
    }
}

void SpecificationFrame::actorAtrributesSAS(QString sas)
{
    sasAttributeComboBox->addItem(sas);
}

void SpecificationFrame::relationComboBoxChanged(int rel)
{
    if(0 == rel)
    {
        valueLineEdit->setEnabled(false);
        valueLineEdit->clear();
    }
    else
    {
        valueLineEdit->setEnabled(true);
    }
}

void SpecificationFrame::filterAddSpecificationClicked(bool bl)
{
    if(attributeComboBox->count()>0 && sasAttributeComboBox->count()>0)
    {
        QString attributeValue = attributeComboBox->currentText();
        QString sasAttributeValue = sasAttributeComboBox->currentText();
        SpecsData filterDataRHS;

        if(!attributeValue.isEmpty() && !sasAttributeValue.isEmpty())
        {
            if(0==relationshipComboBox->currentIndex()) //==
            {
                QVector<QString> dataLhs;
                QString sasValue;
                QStringList values = sasAttributeValue.split("=");
                dataLhs.append(attributeValue);
                dataLhs.append(values.at(0));
                sasValue.append("(").append(attributeValue).append(",").append(values.at(0)).append(")=(");
                QString numericValues = values.at(1);
                numericValues.remove("(").remove(")");
                values = numericValues.split(",");
                filterSpecsLHS.append(dataLhs);

                for(int val = 0 ; val < values.count(); ++val)
                {
                    QVector<QString> dataVals;
                    dataVals.append(values.at(val));
                    dataVals.append(values.at(val));
                    sasValue.append("(").append(values.at(val)).append(",").append(values.at(val)).append("),");// == hence same
                    filterDataRHS.append(dataVals);
                }
                sasValue.append(")").remove(",)").append(")");

                QStandardItem * item = new QStandardItem(sasValue);
                item->setCheckable(true);
                item->setEditable(false);
                item->setCheckState(Qt::Unchecked);
                filterListModel->appendRow(item);

            }
            else if(1==relationshipComboBox->currentIndex()) //+
            {
                if(!valueLineEdit->text().isEmpty())
                {
                    QVector<QString> dataLhs;
                    QString sasValue;
                    QStringList values = sasAttributeValue.split("=");
                    sasValue.append("(").append(attributeValue).append(",").append(values.at(0)).append(")=(");
                    dataLhs.append(attributeValue);
                    dataLhs.append(values.at(0));
                    QString numericValues = values.at(1);
                    numericValues.remove("(").remove(")");
                    values = numericValues.split(",");
                    filterSpecsLHS.append(dataLhs);

                    for(int val = 0 ; val < values.count(); ++val)
                    {
                        QVector<QString> dataVals;
                        QString doubVal = values.at(val);
                        double num = doubVal.toDouble();
                        num += valueLineEdit->text().toDouble();
                        dataVals.append(QString::number(num));
                        dataVals.append(values.at(val));
                        sasValue.append("(").append(QString::number(num))
                                .append(",").append(values.at(val)).append("),");// +
                        filterDataRHS.append(dataVals);
                    }
                    sasValue.append(")").remove(",)").append(")");

                    QStandardItem * item = new QStandardItem(sasValue);
                    item->setCheckable(true);
                    item->setEditable(false);
                    item->setCheckState(Qt::Unchecked);
                    filterListModel->appendRow(item);
                }
            }
            else if(2==relationshipComboBox->currentIndex()) //-
            {
                if(!valueLineEdit->text().isEmpty())
                {
                    QVector<QString> dataLhs;
                    QString sasValue;
                    QStringList values = sasAttributeValue.split("=");
                    sasValue.append("(").append(attributeValue).append(",").append(values.at(0)).append(")=(");
                    dataLhs.append(attributeValue);
                    dataLhs.append(values.at(0));
                    QString numericValues = values.at(1);
                    numericValues.remove("(").remove(")");
                    values = numericValues.split(",");
                    filterSpecsLHS.append(dataLhs);

                    for(int val = 0 ; val < values.count(); ++val)
                    {
                        QVector<QString> dataVals;
                        QString doubVal = values.at(val);
                        double num = doubVal.toDouble();
                        num -= valueLineEdit->text().toDouble();
                        dataVals.append(QString::number(num));
                        dataVals.append(values.at(val));
                        sasValue.append("(").append(QString::number(num))
                                .append(",").append(values.at(val)).append("),");// -
                        filterDataRHS.append(dataVals);
                    }
                    sasValue.append(")").remove(",)").append(")");

                    QStandardItem * item = new QStandardItem(sasValue);
                    item->setCheckable(true);
                    item->setEditable(false);
                    item->setCheckState(Qt::Unchecked);
                    filterListModel->appendRow(item);
                }
            }
            else if(3==relationshipComboBox->currentIndex()) //+-
            {
                if(!valueLineEdit->text().isEmpty())
                {
                    QVector<QString> dataLhs;
                    QString sasValue;
                    QStringList values = sasAttributeValue.split("=");
                    sasValue.append("(").append(attributeValue).append(",").append(values.at(0)).append(")=(");
                    dataLhs.append(attributeValue);
                    dataLhs.append(values.at(0));
                    QString numericValues = values.at(1);
                    numericValues.remove("(").remove(")");
                    values = numericValues.split(",");
                    filterSpecsLHS.append(dataLhs);

                    for(int val = 0 ; val < values.count(); ++val)
                    {
                        QVector<QString> dataVals;
                        QString doubVal = values.at(val);
                        double numP = doubVal.toDouble();
                        numP += valueLineEdit->text().toDouble();
                        dataVals.append(QString::number(numP));
                        dataVals.append(values.at(val));

                        sasValue.append("(").append(QString::number(numP))
                                .append(",").append(values.at(val)).append("),");// +
                        filterDataRHS.append(dataVals);
                        dataVals.clear();
                        doubVal = values.at(val);
                        double numM = doubVal.toDouble();
                        numM -= valueLineEdit->text().toDouble();
                        dataVals.append(QString::number(numM));
                        dataVals.append(values.at(val));
                        sasValue.append("(").append(QString::number(numM))
                                .append(",").append(values.at(val)).append("),");// -

                        filterDataRHS.append(dataVals);
                    }
                    sasValue.append(")").remove(",)").append(")");

                    QStandardItem * item = new QStandardItem(sasValue);
                    item->setCheckable(true);
                    item->setEditable(false);
                    item->setCheckState(Qt::Unchecked);
                    filterListModel->appendRow(item);
                }
            }
            else
            {
                return;
            }
            filterSpecsRHS.append(filterDataRHS);
//            qDebug()<<filterSpecsRHS <<"filterSpecsRHS"<<filterSpecsLHS;
            filterDataRHS.clear();
        }
        filterListView->scrollToBottom();

        QPair<SpecsData,SpecificationVector> filterSpec;
        filterSpec.first.append(filterSpecsLHS.at(filterSpecsLHS.count()-1));
        filterSpec.second.append(filterSpecsRHS.at(filterSpecsRHS.count()-1));
        filterCrossProdSpecification(filterListModel->item(filterListModel->rowCount()-1)->text(),filterSpec,2);

    }
}

void SpecificationFrame::crossproductAddSpecificationClicked(bool bl)
{
    if(att1ComboBox->count() >0 && att2ComboBox->count() >0)
    {
        QString msg;
        msg="Please wait ! Computing cross product for all specifications, this might take a while !!";
        emit statusMessage(msg);

        int attributeIndex1 = att1ComboBox->currentIndex();
        int attributeIndex2 = att2ComboBox->currentIndex();

        createCrossProductSpec(attributeIndex1,attributeIndex2);

        msg="completed";
        emit statusMessage(msg);
    }
}

void SpecificationFrame::crossAllSpecs(bool bl)
{
    if(att1ComboBox->count()>=1)
    {
        //Cross Product Spec for 1st two specs
        createCrossProductSpec(0,1); // specs 1 and 2
    }

    if(att1ComboBox->count()>3)
    {
        int specsCount = att1ComboBox->count()-1;

        //Cross Product Spec for the rest of specs
        for(int specIndex=2; specIndex < specsCount;++specIndex)
        {
            createCrossProductSpec(specIndex,att1ComboBox->count()-1);
        }
    }
}

void SpecificationFrame::filterListViewContextMenu(QPoint pos)
{
    if(filterListView->model()->rowCount()>0)
    {
        QMenu *menu = new QMenu(this);
        menu->addAction("Remove Selected Items", this, SLOT(filterListViewRemoveSelectedClicked()));
        menu->addAction("Remove All Items", this, SLOT(filterListViewRemoveAllClicked()));
        menu->popup(filterListView->mapToGlobal(pos));
    }
}

void SpecificationFrame::filterListViewRemoveSelectedClicked()
{
    for(int index=0; index < filterListModel->rowCount();++index)
    {
        QStandardItem * item = filterListModel->item(index);
        if (item->data(Qt::CheckStateRole).toBool() == true)   // Item checked, remove
        {
            removeSpecFilterCrossProd(index,2,filterSpecsLHS.at(index));//2 is type filter
            filterSpecsRHS.removeAt(index);
            filterSpecsLHS.removeAt(index);
            filterListModel->removeRow(index);
            index = index -1; // index changed to current row, deletion of item changes list index
        }
    }
}

void SpecificationFrame::filterListViewRemoveAllClicked()
{
    for(int index=0; index < filterListModel->rowCount();++index)
    {
        removeSpecFilterCrossProd(index,2,filterSpecsLHS.at(index));//2 is type filter
        filterSpecsRHS.removeAt(index);
        filterSpecsLHS.removeAt(index);
        filterListModel->removeRow(index);
        index = index -1; // index changed to current row, deletion of item changes list index
    }
}

void SpecificationFrame::crossproductContextMenu(QPoint pos)
{
    if(crossProductListView->model()->rowCount()>0)
    {
        QMenu *menu = new QMenu(this);
        menu->addAction("Remove Selected Items", this, SLOT(crossproductRemoveSelectedClicked()));
        menu->addAction("Remove All Items", this, SLOT(crossproductRemoveAllClicked()));
        menu->popup(crossProductListView->mapToGlobal(pos));
    }
}

void SpecificationFrame::crossproductRemoveSelectedClicked()
{
    for(int index=0; index < crossProductListModel->rowCount();++index)
    {
        QStandardItem * item = crossProductListModel->item(index);
        if (item->data(Qt::CheckStateRole).toBool() == true)   // Item checked, remove
        {
            removeSpecFilterCrossProd(index,3,crossProductSpecsLHS.at(index));//3 is type cross prod
            crossProductSpecsLHS.removeAt(index);
            crossProductSpecsRHS.removeAt(index);
            crossProductListModel->removeRow(index);
            index = index -1; // index changed to current row, deletion of item changes list index
        }
    }
}

void SpecificationFrame::actorNameAttributes(QStringList headers, QStringList actors)
{
    actorListComboBox->clear();
    actorHeaders= headers;
    actorListComboBox->addItems(actors);
}

void SpecificationFrame::actorIndexChangedComboBox(int indx)
{
    attributeComboBox->clear();

    for(int hIndx = 0; hIndx < actorHeaders.length();++hIndx)
    {
        QString itemStr;
        itemStr.append(actorListComboBox->currentText().trimmed()).append(".").append(actorHeaders.at(hIndx));
        attributeComboBox->addItem(itemStr);
    }

    for(int actIndx = 0 ; actIndx < actorListComboBox->count();++actIndx)
    {
        QString itemStr;
        itemStr.append(actorListComboBox->currentText().trimmed()).append(".").
                append(actorListComboBox->currentText().trimmed()).append("<>").
                append(actorListComboBox->itemText(actIndx));
        attributeComboBox->addItem(itemStr);

    }
}

void SpecificationFrame::crossproductRemoveAllClicked()
{
    for(int index=0; index < crossProductListModel->rowCount();++index)
    {
        removeSpecFilterCrossProd(index,3,crossProductSpecsLHS.at(index));//3 is type cross prod
        crossProductSpecsLHS.removeAt(index);
        crossProductSpecsRHS.removeAt(index);
        crossProductListModel->removeRow(index);
        index = index -1; // index changed to current row, deletion of item changes list index
    }
}
