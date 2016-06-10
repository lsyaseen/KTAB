#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //action for read from file
    connect(ui->actionRead_from_file,SIGNAL(triggered(bool)),this,SLOT(csv_read_Action(bool)));
    model = new QStandardItemModel;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::csv_read_Action(bool action)
{

    QStringList tableHeaders;
    //getting the csv file path
    QString FilePath;
    FilePath = QFileDialog::getOpenFileName(this,tr("Open CSV File"), QDir::homePath() , tr("File Description (*.csv)"));

    if(!FilePath.isEmpty())
    {
        model->clear();

        QFile file(FilePath);
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
        ui->csv_tableView->setModel(model);
        ui->csv_tableView->showMaximized();
        ui->csv_tableView->setAlternatingRowColors(true);
        ui->csv_tableView->resizeRowsToContents();

        for (int c = 0; c < ui->csv_tableView->horizontalHeader()->count(); ++c)
            ui->csv_tableView->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);

        ui->csv_tableView->resizeColumnsToContents();

    }
}

