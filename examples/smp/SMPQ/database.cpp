#include "database.h"

Database::Database()
{
}

Database::~Database()
{
    // Database Initiaization
    QStringList driverList;
    driverList = QSqlDatabase::drivers();

    if (!driverList.contains("QSQLITE", Qt::CaseInsensitive))
        emit Message("Database Error", "No QSQLITE support! Check all needed dll-files!");



}

void Database::openDB(QString dbPath)
{
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

        scenario_m =  scenarioList->at(0);
        readVectorPositionTable(0, scenario_m);//turn
    }
}

void Database::openDBEdit(QString dbPath)
{
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

        readVectorPositionTableEdit(scenario_m);//turn

        // Scenarios list in db
        //  getScenarioList();

        // readVectorPositionTableEdit(scenario_m);//turn
    }
}

void Database::getScenarioData(int turn, QString scenario)
{
    scenario_m=scenario;
    readVectorPositionTable(turn,scenario_m);//turn
}

void Database::getScenarioDataEdit(QString scenario)
{
    scenario_m=scenario;
}

void Database::getStateCount()
{
    getNumStates();
}

void Database::getDimensionCount()
{
    QSqlQuery qry;
    QString query= QString("select DISTINCT Dim_k from VectorPosition ");

    qry.exec(query);

    while(qry.next())
    {
        numDimension = qry.value(0).toInt();
    }
    emit dimensionsCount(numDimension);
}

void Database::getActors_DescriptionDB()
{
    actorNameList.clear();
    actorDescList.clear();

    QSqlQuery qry;
    QString query= QString("select Name,DESC from ActorDescription order by Act_i ASC ");

    qry.exec(query);

    while(qry.next())
    {
        actorNameList.append(qry.value(0).toString());
        actorDescList.append(qry.value(1).toString());
    }

    emit actorsNameDesc(actorNameList , actorDescList);
}


void Database::getInfluenceDB()
{
    actorInfluence.clear();

    qDebug()<<scenario_m;
    QSqlQuery qry;
    QString query= QString(" select SpatialCapability.Cap from SpatialCapability,ActorDescription where"
                           " ActorDescription.Act_i = SpatialCapability.Act_i"
                           " and SpatialCapability.Scenario='%1' and Turn_t=0").arg(scenario_m);

    qry.exec(query);

    while(qry.next())
    {
        actorInfluence.append(qry.value(0).toString());
    }

    emit actorsInflu(actorInfluence);
}

void Database::getPositionDB(int dim)
{
    actorPosition.clear();
    qDebug()<<scenario_m;

    QSqlQuery qry;
    QString query= QString(" select VectorPosition.Coord from VectorPosition,ActorDescription where"
                           " ActorDescription.Act_i = VectorPosition.Act_i"
                           " and VectorPosition.Scenario='%2' and Turn_t=0 and dim_k='%1'").arg(dim).arg(scenario_m);

    qry.exec(query);

    while(qry.next())
    {
        actorPosition.append(qry.value(0).toString());
    }
    emit actors_Pos(actorPosition,dim);
}

void Database::getSalienceDB(int dim)
{
    actorSalience.clear();
    qDebug()<<scenario_m;

    QSqlQuery qry;
    QString query= QString(" select SpatialSalience.Sal from SpatialSalience,ActorDescription where"
                           " ActorDescription.Act_i = SpatialSalience.Act_i"
                           " and  SpatialSalience.Scenario='%2' and Turn_t=0 and dim_k='%1'").arg(dim).arg(scenario_m);

    qry.exec(query);

    while(qry.next())
    {
        actorSalience.append(qry.value(0).toString());
    }
    emit actors_Sal(actorSalience,dim);
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
    }

    emit vectorPosition(x,y,actor_name);
}

void Database::readVectorPositionTable(int turn, QString scenario)
{
    //TO-DO add scenario as a constraint and populate the scenario dropdown cum edit box
    //choose the first scenario as default

    sqlmodel = new QSqlTableModel(this);
    sqlmodel->setTable("VectorPosition");
    sqlmodel->setFilter(QString("Turn_t='%1' and  Dim_k='%2' and Scenario='%3'")
                        .arg(turn).arg(QString::number(0)).arg(scenario));
    sqlmodel->select();

    emit dbModel(sqlmodel);

    //To plot graph
    for(int actors=0; actors <= numActors; ++actors)
        getVectorPosition(actors,0,turn,scenario);//actors, dimension, turn
}


void Database::readVectorPositionTableEdit(QString scenario)
{
    //TO-DO add scenario as a constraint and populate the scenario dropdown cum edit box
    //choose the first scenario as default

    sqlmodelEdit = new QSqlTableModel(this);
    sqlmodelEdit->setTable("VectorPosition");
    sqlmodelEdit->setFilter(QString("Turn_t=0 and Scenario='%1'").arg(scenario));
    sqlmodelEdit->select();

    emit dbModelEdit(sqlmodelEdit);
}

void Database::getNumActors()
{
    QSqlQuery qry;
    QString query= QString("select DISTINCT Act_i from VectorPosition ");

    qry.exec(query);

    while(qry.next())
    {
        numActors = qry.value(0).toInt();
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
    }
    emit scenarios(scenarioList);
}
