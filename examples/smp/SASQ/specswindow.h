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
#include <QStatusBar>


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

    QComboBox * actorListComboBox;
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

    int specsIndexlist[4]= {0};

    DataValues modelSpecificationsLHS ;
    DataValues actorSpecificationsLHS ;
    SpecsData filterSpecificationsLHS;
    SpecsData crossProdSpecificationsLHS;

    SpecsData modelSpecificationsRHS ;
    SpecsData actorSpecificationsRHS ;
    SpecificationVector filterSpecificationsRHS;
    SpecificationVector crossProdSpecificationsRHS;

    QStringList actorHeaders;

    void initializeFrameLayout();
    void initializeFiltersFrame();
    void initializeCrossProductFrame();
    void initializeSensAnalysisFrame();
    void createCrossProductSpec(int att1, int att2);

signals:
    void filterList(QStandardItemModel  *, QPair<SpecsData,SpecificationVector>);
    void crossProductList(QStandardItemModel *, QPair<SpecsData,SpecificationVector>);
    void getActorAttribQuteheaders();
    void statusMessage(QString);
    void finalSpecListModel(DataValues,SpecsData,int);
    void finalSpecListActor(DataValues,SpecsData,int);
    void finalSpecListFilter(SpecsData,SpecificationVector,int);
    void finalSpecListCrossProduct(SpecsData,SpecificationVector,int);

private slots:
    void listViewClicked();
    void specsListViewContextMenu(QPoint pos);
    void populateCrossProductComboBox();
    void actorAtrributesSAS(QString sas);
    void relationComboBoxChanged(int rel);
    void filterAddSpecificationClicked(bool bl);
    void crossproductAddSpecificationClicked(bool bl);
    void crossAllSpecs(bool bl);
    void filterListViewContextMenu(QPoint pos);
    void filterListViewRemoveSelectedClicked();
    void crossproductContextMenu(QPoint pos);
    void crossproductRemoveSelectedClicked();
    void actorNameAttributes(QStringList headers, QStringList actors);
    void actorIndexChangedComboBox(int indx);

public slots:
    void filterListViewRemoveAllClicked();
    void crossproductRemoveAllClicked();
    void listViewSpecsRemoveAllClicked();
    void actorModelSpecification(QString spec, QPair<DataValues,SpecsData> specData, int type);
    void filterCrossProdSpecification(QString spec, QPair<SpecsData,SpecificationVector> specData, int type);
    void removeSpecModelActor(int index, int type, QString specLHS);
    void removeSpecFilterCrossProd(int index, int type, DataValues specLHS);
    void getFinalSpecificationsList();

};




#endif // SPECSWINDOW_H
