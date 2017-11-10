#include "runmodel.h"

RunModel::RunModel()
{
    loggerConf.parseFromFile("./ktab-smp-logger.conf");
    // Disable all the logging to begin with
    loggerConf.set(el::Level::Global, el::ConfigurationType::Enabled, "false");
    el::Loggers::reconfigureAllLoggers(loggerConf);
}

RunModel::~RunModel()
{

}

void RunModel::runSMPModel(QStringList fileNames, bool logStatus, QString seedVal, QString dbFilePath, QString logType, QString logFileLoc)
{
    logFileName.clear();
    logFileLocation = logFileLoc;

    QString con = configureDbRun(dbFilePath);

    for( int fileIndex = 0 ; fileIndex < fileNames.length() ; ++ fileIndex)
    {
        logSMPDataOptionsAnalysis(logType,QString("_spec_"+ QString::number(fileIndex)));
        runModel(con,fileNames.at(fileIndex),logStatus,seedVal);
        qDebug()<<"runModel" << fileIndex;
    }

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

void RunModel::logSMPDataOptionsAnalysis(QString logType, QString specCount)
{
    QDateTime UTC = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
    QString name("ktab-smp-");
    name.append(QString::number(UTC.date().year())).append("-");
    name.append(QString("%1").arg(UTC.date().month(), 2, 10, QLatin1Char('0'))).append("-");
    name.append(QString("%1").arg(UTC.date().day(), 2, 10, QLatin1Char('0'))).append("__");
    name.append(QString("%1").arg(UTC.time().hour(), 2, 10, QLatin1Char('0'))).append("-");
    name.append(QString("%1").arg(UTC.time().minute(), 2, 10, QLatin1Char('0'))).append("-");
    name.append(QString("%1").arg(UTC.time().second(), 2, 10, QLatin1Char('0')));
    name.append("_GMT");

    if(logType=="Default")
    {
        if(logFileName.isEmpty())
        {
            logFileName = logFileLocation + QDir::separator() + name;
            logFileName.append("_log.txt");
        }
        qDebug()<<logFileName <<"logFileName";

        loggerConf.setGlobally(el::ConfigurationType::Filename, logFileName.toStdString());
        // Enable all the logging
        loggerConf.set(el::Level::Global, el::ConfigurationType::Enabled, "true");
        el::Loggers::reconfigureAllLoggers(loggerConf);
    }
    else if(logType=="New")
    {
        fclose(stdout);
        fp_old = *stdout;

        QString logFileNewName = logFileLocation + QDir::separator() +name;
        qDebug()<<logFileNewName <<"logFileNewName";
        if(!logFileNewName.isEmpty())
        {
            logFileNewName.append(specCount).append("_log.txt");

            std::string logFileName = logFileNewName.toStdString();
            loggerConf.setGlobally(el::ConfigurationType::Filename, logFileName);
            // Enable all the logging
            loggerConf.set(el::Level::Global, el::ConfigurationType::Enabled, "true");
            el::Loggers::reconfigureAllLoggers(loggerConf);
        }
    }
    else
    {
        if(logType=="None")
        {
            // Disable all the logging
            loggerConf.set(el::Level::Global, el::ConfigurationType::Enabled, "false");

            el::Loggers::reconfigureAllLoggers(loggerConf);
        }
    }
}

