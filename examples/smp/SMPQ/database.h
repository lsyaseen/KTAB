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
// Declare the functions and classes unique to the Priority case.
// -------------------------------------------------

#ifndef DATABASE_H
#define DATABASE_H

#include <QtCore>
#include <QtSql>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QStandardItemModel>

class Database : public QObject
{
    Q_OBJECT
public:
    Database();
    ~Database();

public slots :
    void openDB(QString dbPath);
    void openDBEdit(QString dbPath);
    void getScenarioData(int turn, QString scenario);
    void getScenarioDataEdit(QString scenario);
    void getStateCount();
    void getDimensionCount();
    //DB to CSV
    void getActors_DescriptionDB();
    void getInfluenceDB(int turn =0);
    void getPositionDB(int dim,int turn =0);
    void getSalienceDB(int dim,int turn =0);

signals:
    void Message(QString , QString );
    void vectorPosition(QVector<double> x, QVector<double> y, QString actor);
    void dbModel(QStandardItemModel  *);
    void dbModelEdit(QSqlTableModel *);
    void statesCount(int value);
    void dimensionsCount(int value);
    void scenarios(QStringList *scenariosList);

    //DB to CSV
    void actorsNameDesc(QList <QString>, QList <QString>);
    void actorsInflu(QList <QString>);
    void actors_Pos(QList<QString>,int);
    void actors_Sal(QList<QString>,int);

private:
    QSqlDatabase db;
//    QSqlTableModel * sqlmodel;
    QSqlTableModel * sqlmodelEdit;
    QStandardItemModel * sqlmodel;

    int numActors =0;
    int numStates =0;
    int numDimension =0;
    QStringList * scenarioList;
    QString scenario_m;

    //DB to CSV
    QList <QString> actorNameList;
    QList <QString> actorDescList;
    QList <QString> actorInfluence;
    QList <QString> actorPosition;
    QList <QString> actorSalience;


    void getVectorPosition(int actor, int dim, int turn, QString scenario);

    //Default read Turn_t=0
    void readVectorPositionTable(int state, QString scenario);

    // number of actors
    void getNumActors();
    // number of states/turns in db
    void getNumStates();
    // Scenarios list in db
    void getScenarioList();

    void readVectorPositionTableEdit(QString scenario);
};

#endif // DATABASE_H



// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
