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

    void intializeFrameLayout();
    void initializeInputDataTable();
    void initializeAccomodationMatrix();
    void initializeAccomodationRowCol(int count, QString tableType);

public slots :
    void setActorTableModel(QStandardItemModel *model, QStringList scenarioList);


};


#endif // ACTORWINDOW_H
