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
    void readCSVFile(QString path);
    QStandardItemModel * model;

signals:
    void csvModel(QStandardItemModel * ,QStringList ScenarioName);

};

#endif // CSV_H
