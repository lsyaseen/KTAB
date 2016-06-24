#ifndef DATABASE_H
#define DATABASE_H

#include <QtCore>
#include <QtSql>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>

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

signals:
    void Message(QString , QString );
    void vectorPosition(QVector<double> &x, QVector<double> &y, QString actor);
    void dbModel(QSqlTableModel *);
    void dbModelEdit(QSqlTableModel *);
    void statesCount(int value);
    void dimensionsCount(int value);
    void scenarios(QStringList *scenariosList);

    //DB to CSV
    void actorsNameDesc(QList <QString>, QList <QString>);

private:
    QSqlDatabase db;
    QSqlTableModel * sqlmodel;
    QSqlTableModel * sqlmodelEdit;

    int numActors =0;
    int numStates =0;
    int numDimension =0;
    QStringList * scenarioList;
    QString scenario_m;

    //DB to CSV
    QList <QString> actorNameList;
    QList <QString> actorDescList;

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
