#ifndef ACTORWINDOW_H
#define ACTORWINDOW_H


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
#include <QTableView>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QInputDialog>
#include <QStandardItemModel>
#include <QStringList>
#include <QMessageBox>

class ActorFrame: public QFrame
{
    Q_OBJECT

public:
    explicit ActorFrame(QObject *parent = Q_NULLPTR);
    ~ActorFrame();

private :
    QGridLayout * frameMainLayout;

    QFrame * actorInputTableFrame;
    QFrame * actorSensTableFrame;
    QFrame * actorControlsFrame;
    QFrame * actorSpecsFrame;

    QGridLayout * actorInputGridLayout;
    QGridLayout * actorSensGridLayout;
    QGridLayout * actorControlsGridLayout;
    QGridLayout * actorSpecsGridLayout;

    QTabWidget * inputDataTabWidget;
    QTableView * actorDataTableView;
    QTableView * accomodationMatrixTableView;

    QStandardItemModel * csvAccModel = nullptr;
    QStandardItemModel * csvActorDataModel = nullptr;

    QStandardItemModel * xmlAccModel = nullptr;
    QStandardItemModel * xmlActorDataModel = nullptr;

    //sas Data Grid
    QTableWidget * sasDataGridTableWidget;

    QListView * specsListView = nullptr;
    QStandardItemModel *specsListModel = nullptr;

    QRadioButton * valueRadioButton;
    QRadioButton * minDeltaMaxRadioButton;
    QRadioButton * basePMRadioButton;
    QRadioButton * basePMPRadioButton;

    QComboBox * actorComboBox ;
    QLineEdit * scenarioName;
    QLineEdit * scenarioDescription;

    bool importedData;

    QStringList attributeList;

    using DataValues = QVector <QString>;
    using SpecsData = QVector <DataValues>;
    using SpecificationVector = QVector<SpecsData>;

    DataValues actorSpecsLHS;
    SpecsData actorSpecsRHS = SpecsData();


    void initializeFrameLayout();
    void initializeInputDataTable();
    void initializeAccomodationMatrix(QString type);
    void initializeAccomodationRowCol(int count, QString tableType);
    void populateAccomodationMatrix(QVector<QStringList> idealAdj, QVector<QString> actors);

    void initializeSASDataGrid();
    void intitalizeSasGridColumn();
    void initializeSpecificationsTypeButtons();
    void initializeSpecificationsList();
    void initializeAccMatrixRowCol(int count, QString table);
    void initializeBaseDataGrid();
    QString processMinDeltaMax(QVector<double> values, int row);
    QString processBasePM(QVector<double> values, int row);
    QString processBasePMP(QVector<double> values, int row);
    QString processValuesN(QVector<double> values, int row);
    void validateData();
    bool validateSalienceData();
//    void validateValues(QVector<double> values, int row);
   QPair<QVector<double>,QVector<int>> getConstantSalienceValues();


public slots:
    void listViewRemoveAllClicked();
    void clearModels();
    void actorAtrributesHeaderList();

private slots:
    void actorComboBoxChanged(QString index);
    void actorListViewContextMenu(QPoint pos);
    void listViewRemoveSelectedClicked();
    void clearSpecsList();

    void minDeltaMaxRadioButtonClicked(bool bl);
    void basePMRadioButtonClicked(bool bl);
    void basePMPRadioButtonClicked(bool bl);
    void valueRadioButtonClicked(bool bl);

    void addBasePushButtonClicked(bool bl);
    void addSpecClicked(bool bl);
    void sasDataGridContextMenuRequested(QPoint pos);
    void addValueColumn();
    void displayMenuTableView(QPoint pos);
    void cellSelected(QStandardItem *in);

signals:
    void modelList(QStandardItemModel *, QStringList, QPair<DataValues,SpecsData>);
    void actorHeaderAttributes(QStringList,QStringList);
    void specificationNew(QString,QPair<DataValues,SpecsData>,int);
    void actorAttributesAndSAS(QString sasValues);
    void removeSpecificationActor(int index, int type, QString specLHS);


public slots :
    void setActorTableModel(QStandardItemModel *model, QStringList scenarioList);
    void setAccTableModel(QStandardItemModel *model, QVector<QStringList> idealAdjustmentList,
                          QStringList dimensionsXml, QStringList desc);


};

#endif // ACTORWINDOW_H
