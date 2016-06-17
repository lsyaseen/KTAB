#include "database.h"

Database::Database()
{
    qDebug()<< "Test";
}

Database::~Database()
{

}

void Database::openDB(QString dbPath)
{
    // Database Initiaization
    QStringList driverList;
    driverList = QSqlDatabase::drivers();

    if (!driverList.contains("QSQLITE", Qt::CaseInsensitive))
        emit Message("Database Error", "No QSQLITE support! Check all needed dll-files!");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    db.setHostName("localhost");
    db.setUserName("root");

    if(!db.open())
    {
        emit Message("Database Error", db.lastError().text());
    }
    else
    {
        // to update numActors in db
        getNumActors();

        // number of states/turns in db
        getNumStates();

        // Scenarios list in db
        getScenarioList();

        scenario =  scenarioList->at(0);
        readVectorPositionTable(0, scenario);//turn
    }
}

void Database::getScenarioData(int turn, QString scenario)
{
    readVectorPositionTable(turn,scenario);//turn
}

void Database::getStateCount()
{
    getNumStates();
}

void Database::getVectorPosition(int actor, int dim, int turn, QString scenario)
{
    QSqlQuery qry;
    QString query;

    int i =0;
    QVector<double> x(numStates+1), y(numStates+1);

    query= QString("select * from VectorPosition where Act_i='%1' and Dim_k='%2' and Turn_t<='%3' and  Scenario = '%4' ")
            .arg(actor).arg(dim).arg(turn).arg(scenario);

    qry.exec(query);
    while(qry.next())
    {
        x[i]=qry.value(1).toDouble();
        y[i]=qry.value(4).toDouble()*100;// y scales from 0 to 100
        ++i;
    }

    QSqlQuery qry2;
    QString actor_name;

    QString query2 = QString("select * from ActorDescription where Act_i='%1'").arg(actor);
    qry2.exec(query2);
    while(qry2.next())
    {
        actor_name = qry2.value(2).toString();
        qDebug()<<actor_name <<"NAME";
    }

    emit vectorPosition(x,y,actor_name);

}

void Database::readVectorPositionTable(int state, QString scenario)
{
    //TO-DO add scenario as a constraint and populate the scenario dropdown cum edit box
    //choose the first scenario as default

    sqlmodel = new QSqlTableModel(this);
    sqlmodel->setTable("VectorPosition");
    sqlmodel->setFilter(QString("Turn_t='%1' and  Dim_k='%2' and Scenario='%3'")
                        .arg(state).arg(QString::number(0)).arg(scenario));
    sqlmodel->select ();

    emit dbModel(sqlmodel);

    //To plot Initial graph
    for(int actors=0; actors <= numActors; ++actors)
        getVectorPosition(actors,0,state,scenario);//actors, dimension, turn
}

void Database::getNumActors()
{
    QSqlQuery qry;
    QString query= QString("select DISTINCT Act_i from VectorPosition ");

    qry.exec(query);

    while(qry.next())
    {
        numActors = qry.value(0).toInt();
        qDebug()<<"numActors" << numActors;
    }

}

void Database::getNumStates()
{
    QSqlQuery qry;
    QString query= QString("select DISTINCT Turn_t from VectorPosition ");

    qry.exec(query);

    while(qry.next())
    {
        numStates = qry.value(0).toInt();
        qDebug()<<"numStates" << numStates;
    }

    emit statesCount(numStates);
}

void Database::getScenarioList()
{
    scenarioList = new QStringList;
    QSqlQuery qry;
    QString query= QString("select DISTINCT Scenario, Scenario from VectorPosition ");

    qry.exec(query);

    while(qry.next())
    {
        scenarioList->append(qry.value(1).toString());
        qDebug()<<"scenarioList" << scenarioList ;
    }
    emit scenarios(scenarioList);
}
