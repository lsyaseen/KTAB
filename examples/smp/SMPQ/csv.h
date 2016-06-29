#ifndef CSV_H
#define CSV_H

#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>


class CSV : public QObject
{
    Q_OBJECT
public:
    CSV();
    QStandardItemModel * model;
public slots:
    void readCSVFile(QString path);

signals:
    void csvModel(QStandardItemModel * ,QStringList ScenarioName);
    void sendMessage(QString className, QString Message);

};

#endif // CSV_H
