#include "runmodel.h"

RunModel::RunModel()
{
}

RunModel::~RunModel()
{

}

void RunModel::runSMPModel(QStringList fileNames,bool logStatus, QString seedVal, QString dbFilePath)
{
    qDebug()<<"inside";
    //    configureDbRun();
    qDebug()<<"configureDbRun"  ;

    QString con = configureDbRun(dbFilePath);
    //    if(!connectionString.isEmpty())
    //    {

    for( int fileIndex = 0 ; fileIndex < fileNames.length() ; ++ fileIndex)
    {
        runModel(con,fileNames.at(fileIndex),logStatus,seedVal);
        qDebug()<<"runModel" << fileIndex;

    }

    //    }
    QMessageBox::information(0,"Done", "Model run completed");
    qDebug()<<"here";
}


QString RunModel::configureDbRun(QString dbFilePath)
{
     if(!dbFilePath.isEmpty())
    {
        QString connectionStr;
        connectionStr.append("Driver=QSQLITE;");//connectionType
        connectionStr.append("Database=").append(dbFilePath.remove(".db").trimmed()).append(";");

        qDebug() <<connectionStr;
        //        dbPath = dbFilePath;

        //Configure DB
        SMPLib::SMPModel::loginCredentials(connectionStr.toStdString());

        return connectionStr;
    }

    return "";

}

void RunModel::runModel(QString conStr, QString fileName, bool logStatus, QString seedVal)
{

    auto sTime = KBase::displayProgramStart(DemoSMP::appName, DemoSMP::appVersion);

    std::vector<int> parameters;

    //    parameters={0,0,0,0,0,0,0,0,0}; // need to update

    QString currentScenarioId;
    QString connectDBString;
    connectDBString = conStr;
    //    qDebug()<<connectDBString << "db";

    if(!connectDBString.isEmpty())
    {
        // A code snippet from Demosmp.cpp to run the model


        uint64_t seed = seedVal.toULongLong();

        // JAH 20160730 vector of SQL logging flags for 5 groups of tables:
        // 0 = Information Tables, 1 = Position Tables, 2 = Challenge Tables,
        // 3 = Bargain Resolution Tables, 4 = VectorPosition table
        // JAH 20161010 added group 4 for only VectorPos so it can be logged alone
        std::vector<bool> sqlFlags = {true,true,true,true,true};

        //log minimum
        if(true==logStatus)
        {
            sqlFlags = {true,false,false,false,true};
        }

        // Notice that we NEVER use anything but the default seed.
        //        printf("Using PRNG seed:  %020llu \n", seed);
        //        printf("Same seed in hex:   0x%016llX \n", seed);


        currentScenarioId =QString::fromStdString(SMPLib::SMPModel::runModel
                                                  (sqlFlags,
                                                   fileName.toStdString(),seed,false,parameters));

        KBase::displayProgramEnd(sTime);

    }

}


