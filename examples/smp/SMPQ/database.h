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
    void getScenarioData(int turn, QString scenario);
    void getStateCount();

signals:
    void Message(QString , QString );
    void vectorPosition(QVector<double> &x, QVector<double> &y, QString actor);
    void dbModel(QSqlTableModel *);
    void statesCount(int value);
    void scenarios(QStringList *scenariosList);

private:
    QSqlDatabase db;
    QSqlTableModel * sqlmodel;

    int numActors =0;
    int numStates =0;
    QStringList * scenarioList;
    QString scenario;

    void getVectorPosition(int actor, int dim, int turn, QString scenario);

    //Default read Turn_t=0
    void readVectorPositionTable(int state, QString scenario);

    // number of actors
    void getNumActors();
    // number of states/turns in db
    void getNumStates();
    // Scenarios list in db
    void getScenarioList();

};

#endif // DATABASE_H
