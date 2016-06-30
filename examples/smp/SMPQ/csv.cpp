#include "csv.h"

CSV::CSV()
{
    model = new QStandardItemModel;
}
void CSV::readCSVFile(QString path)
{
    QRegExp space("^\\s*$");
    //getting the csv file path
    QStringList scenarioName;
    if(!path.isEmpty())
    {
        // model->clear();

        QFile file(path);
        if (file.open(QIODevice::ReadOnly) && file.size() > 0)
        {
            int rowindex = 0;                     // file row counter
            QTextStream in(&file);                 // read to text stream

            while (!in.atEnd())
            {
                // read one line from textstream(separated by "\n")
                QString fileLine = in.readLine();

                // parse the read line into separate pieces(tokens) with "," as the delimiter
                QStringList lineToken = fileLine.split(",", QString::SkipEmptyParts);
                QString lastField = lineToken.at(lineToken.length()-1);
                if(lastField.contains(space))
                    lineToken.removeLast();


                if(rowindex >= 2 )
                {
                    // load parsed data to model accordingly
                    for (int j = 0; j < lineToken.size(); j++)
                    {
                        QString value = lineToken.at(j);
                        QStandardItem *item = new QStandardItem(value.trimmed());
                        model->setItem(rowindex-2, j, item);
                    }
                }
                else if(rowindex == 1)
                {
                     model->setHorizontalHeaderLabels(lineToken);
                }
                else if(rowindex == 0)
                {
                    scenarioName.append(lineToken.at(0));
                    scenarioName.append(lineToken.at(1));
                    lineToken.clear();
                }
                rowindex++;
            }

            file.close();
        }
        else
        {
            emit sendMessage("CSV","Empty File !");
            return;
        }
    }
    emit csvModel(model,scenarioName);
}
