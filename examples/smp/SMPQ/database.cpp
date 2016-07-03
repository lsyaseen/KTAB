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

        readVectorPositionTable(0, scenario_m);//turn
    }
}

void Database::openDBEdit(QString dbPath)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

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


void Database::getInfluenceDB(int turn)
{
    actorInfluence.clear();

    //qDebug()<<scenario_m << "turn" << turn ;
    QSqlQuery qry;
    QString query= QString(" select SpatialCapability.Cap from SpatialCapability,ActorDescription where "
                           " ActorDescription.Act_i = SpatialCapability.Act_i "
                           " and SpatialCapability.Scenario='%1' and SpatialCapability.Turn_t='%2' "
                           ).arg(scenario_m).arg(turn);

    qry.exec(query);

    while(qry.next())
    {
        actorInfluence.append(qry.value(0).toString());
        //qDebug()<<actorInfluence << "Influence";
    }

    emit actorsInflu(actorInfluence);
}

void Database::getPositionDB(int dim, int turn)
{
    actorPosition.clear();
    //qDebug()<<scenario_m;

    QSqlQuery qry;
    QString query= QString(" select VectorPosition.Coord from VectorPosition,ActorDescription where"
                           " ActorDescription.Act_i = VectorPosition.Act_i"
                           " and VectorPosition.Scenario='%2' and VectorPosition.Turn_t='%3'"
                           " and VectorPosition.dim_k='%1'")
            .arg(dim).arg(scenario_m).arg(turn);

    qry.exec(query);

    while(qry.next())
    {
        actorPosition.append(qry.value(0).toString());
    }
    emit actors_Pos(actorPosition,dim);
}

void Database::getSalienceDB(int dim, int turn)
{
    actorSalience.clear();
    //qDebug()<<scenario_m;

    QSqlQuery qry;
    QString query= QString(" select SpatialSalience.Sal from SpatialSalience,ActorDescription where"
                           " ActorDescription.Act_i = SpatialSalience.Act_i"
                           " and SpatialSalience.Scenario='%2' and SpatialSalience.Turn_t='%3'"
                           " and SpatialSalience.dim_k='%1'")
            .arg(dim).arg(scenario_m).arg(turn);

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

    //    sqlmodel = new QSqlTableModel(this);
    //    sqlmodel->setTable("VectorPosition");
    //    sqlmodel->setFilter(QString("Turn_t='%1' and  Dim_k='%2' and Scenario='%3'")
    //                        .arg(turn).arg(QString::number(0)).arg(scenario));
    //    sqlmodel->select();

    sqlmodel = new QStandardItemModel(this);
    QSqlQuery qry;
    QString query;

    query= QString("select * from VectorPosition where Turn_t='%1' and Dim_k='%2' and Scenario='%3'")
            .arg(turn).arg(QString::number(0)).arg(scenario);

    qry.exec(query);

    int rowindex =0;
    while(qry.next())
    {
        QString value = qry.value(0).toString();
        QString value1 = qry.value(1).toString();

        QStandardItem *item = new QStandardItem(value.trimmed());
        QStandardItem *item1 = new QStandardItem(value1.trimmed());

        sqlmodel->setItem(rowindex,0,item);
        sqlmodel->setItem(rowindex,1,item1);

        ++rowindex;
    }
    // load parsed data to model accordingly
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

    if(scenarioList->length()>0)
        scenario_m =  scenarioList->at(0);
    else
        Message("Database","there are no Scenario's");

    emit scenarios(scenarioList);
}
// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
