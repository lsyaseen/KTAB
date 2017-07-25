#ifndef SPECSWINDOW_H
#define SPECSWINDOW_H

#include <QApplication>
#include <QFrame>
#include <QObject>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QtGui>
#include <QComboBox>
#include <QSplitter>
#include <QCheckBox>
#include <QPushButton>
#include <QListView>
#include <QMenu>
#include <QLineEdit>
#include <QPair>


class SpecificationFrame: public QFrame
{
    Q_OBJECT

public:
    explicit SpecificationFrame(QObject *parent = Q_NULLPTR);
    ~SpecificationFrame();

    QGridLayout * frameMainLayout;
private :

    QFrame *filterFrame;
    QFrame *crossProductFrame;
    QFrame *specsFrame;

    QListView *specsListView;
    QStandardItemModel *specsListModel;

    QListView *filterListView;
    QStandardItemModel *filterListModel;

    QListView *crossProductListView;
    QStandardItemModel *crossProductListModel;

    QGridLayout *filterFrameGridLayout;
    QGridLayout *crossProductFrameGridLayout;
    QGridLayout *specsFrameGridLayout;

    QComboBox * attributeComboBox;
    QComboBox * sasAttributeComboBox;
    QComboBox * relationshipComboBox;
    QLineEdit * valueLineEdit;

    QComboBox * att1ComboBox;
    QComboBox * att2ComboBox;

    using DataValues = QVector <QString>;
    using SpecsData = QVector <DataValues>;
    using SpecificationVector = QVector<SpecsData>;
    using Specifications = QVector<SpecificationVector>;

    SpecificationVector filterSpecsRHS;
    SpecsData filterSpecsLHS;

    SpecificationVector crossProductSpecsRHS;
    SpecsData crossProductSpecsLHS;
    SpecsData crossProductDataRHS;

    int specsIndexlist[4];

    DataValues modelSpecificationsLHS ;
    DataValues actorSpecificationsLHS ;
    SpecsData filterSpecificationsLHS;
    SpecsData crossProdSpecificationsLHS;

    SpecsData modelSpecificationsRHS ;
    SpecsData actorSpecificationsRHS ;
    SpecificationVector filterSpecificationsRHS;
    SpecificationVector crossProdSpecificationsRHS;

    void initializeFrameLayout();
    void initializeFiltersFrame();
    void initializeCrossProductFrame();
    void initializeSensAnalysisFrame();
    void updateSasSpecsList();

signals:
    void filterList(QStandardItemModel  *, QPair<SpecsData,SpecificationVector>);
    void crossProductList(QStandardItemModel *, QPair<SpecsData,SpecificationVector>);

private slots:
    void listViewClicked();
    void specsListViewContextMenu(QPoint pos);
    void specsListMainWindow(QStandardItemModel * modelList,  QPair<DataValues,SpecsData>,  QPair<DataValues,SpecsData>,
                             QPair<SpecsData,SpecificationVector>,  QPair<SpecsData,SpecificationVector> crossProdSpecs);
    void populateCrossProductComboBox();
    void actorAtrributesSAS(QStringList attributes, QStringList sas);
    void relationComboBoxChanged(int rel);
    void filterAddSpecificationClicked(bool bl);
    void crossproductAddSpecificationClicked(bool bl);
    void filterListViewContextMenu(QPoint pos);
    void filterListViewRemoveSelectedClicked();
    void crossproductContextMenu(QPoint pos);
    void crossproductRemoveSelectedClicked();

public slots:
    void filterListViewRemoveAllClicked();
    void crossproductRemoveAllClicked();
    void listViewSpecsRemoveAllClicked();

};




#endif // SPECSWINDOW_H
