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

    QStandardItemModel * csvAccModel;
    QStandardItemModel * csvActorDataModel;

    QStandardItemModel * xmlAccModel;
    QStandardItemModel * xmlActorDataModel;

    //sas Data Grid
    QTableWidget * sasDataGridTableWidget;

    QListView * specsListView;
    QStandardItemModel *specsListModel;

    QRadioButton * valueRadioButton;
    QRadioButton * minDeltaMaxRadioButton;
    QRadioButton * basePMRadioButton;
    QRadioButton * basePMPRadioButton;

    QComboBox * actorComboBox ;
    QLineEdit * scenarioName;
    QLineEdit * scenarioDescription;

    bool importedData;

    void intializeFrameLayout();
    void initializeInputDataTable();
    void initializeAccomodationMatrix(QString type);
    void initializeAccomodationRowCol(int count, QString tableType);
    void populateAccomodationMatrix(QList<QStringList> idealAdj, QVector<QString> actors);

    void initializeSASDataGrid();
    void intitalizeSasGridColumn();
    void initializeSpecificationsTypeButtons();
    void initializeSpecificationsList();
    void initializeAffinityMatrixRowCol(int count, QString table);

private slots:
    void actorComboBoxChanged(QString index);
    void actorListViewContextMenu(QPoint pos);
    void listViewClicked();
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

public slots :
    void setActorTableModel(QStandardItemModel *model, QStringList scenarioList);
    void setAccTableModel(QStandardItemModel *model, QList<QStringList> idealAdjustmentList, QStringList dimensionsXml, QStringList desc);


};

#endif // ACTORWINDOW_H
