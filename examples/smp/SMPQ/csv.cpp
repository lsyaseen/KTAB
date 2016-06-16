#include "csv.h"

CSV::CSV()
{
    model = new QStandardItemModel;
}
void CSV::readCSVFile(QString path)
{
    //getting the csv file path

    if(!path.isEmpty())
    {
        // model->clear();

        QFile file(path);
        if (file.open(QIODevice::ReadOnly))
        {
            int rowindex = 0;                     // file row counter
            QTextStream in(&file);                 // read to text stream

            while (!in.atEnd())
            {
                // read one line from textstream(separated by "\n")
                QString fileLine = in.readLine();

                // parse the read line into separate pieces(tokens) with "," as the delimiter
                QStringList lineToken = fileLine.split(",", QString::SkipEmptyParts);

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
                else
                {
                    lineToken.clear();
                }
                rowindex++;
            }

            file.close();
        }

    }
    emit csvModel(model);
}
