#include "databasedialog.h"
#include "ui_databasedialog.h"
#include <QDebug>

DatabaseDialog::DatabaseDialog( QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("KTAB_SMP:Configure Database");

    connect(ui->nextPushButton, SIGNAL(clicked(bool)),this,SLOT(nextClicked(bool)));
    ui->label_5->setVisible(false);
    ui->postgreDBLineEdit->setVisible(false);

}

DatabaseDialog::~DatabaseDialog()
{
    delete ui;
}

bool DatabaseDialog::showDialog(quint8 indx, QStringList paraMem)
{
    this->show();
    if(!paraMem.isEmpty())
    {
        if(!QString(paraMem.at(0)).isEmpty())
        {
            ui->usernameLineEdit->setText(paraMem.at(0));
        }

        if(!QString(paraMem.at(1)).isEmpty())
        {
            ui->postgreDBLineEdit->setText(paraMem.at(1));
        }

        if(!QString(paraMem.at(2)).isEmpty())
        {
            ui->ipLineEdit->setText(paraMem.at(2));
        }

        if(!QString(paraMem.at(3)).isEmpty())
        {
            ui->portLineEdit->setText(paraMem.at(3));
        }
    }
    ui->stackedWidget->setCurrentIndex(0);
    ui->sqlComboBox->setCurrentIndex(indx);
    return false;
}

void DatabaseDialog::nextClicked(bool)
{
    if(ui->sqlComboBox->currentIndex()==1)
    {
        ui->stackedWidget->setCurrentIndex(ui->sqlComboBox->currentIndex()+1);
    }
    else if(ui->sqlComboBox->currentIndex()==0)
    {
        on_sqliteDonePushButton_clicked();
    }
}

void DatabaseDialog::on_sqliteDonePushButton_clicked()
{
    QString connectionSring;
    connectionSring.append("Driver=QSQLITE;");//connectionType
    connectionSring.append("Server=localhost;");//host address
    connectionSring.append("Database=").append("None").append(";");

    this->close();
    emit connectionStringPath(connectionSring);

}

void DatabaseDialog::on_postgrePushButton_clicked()
{
    QStringList parametersMemory;
    bool status = true;
    QString connectionSring;
    connectionSring.append("Driver=QPSQL;");//connectionType

    connectionSring.append("Database=").append("postgres").append(";");

    if(!ui->usernameLineEdit->text().trimmed().isEmpty())
    {
        parametersMemory.append(ui->usernameLineEdit->text().trimmed());
        connectionSring.append("Uid=").append(ui->usernameLineEdit->text()).append(";");
    }
    else
    {
        parametersMemory.clear();
        status=false;
        QMessageBox::warning(this,"Database Configuration","Please enter the Username "); // username
    }
    parametersMemory.append(ui->postgreDBLineEdit->text().trimmed());

    if(!ui->passwordLineEdit->text().trimmed().isEmpty())
    {
        connectionSring.append("Pwd=").append(ui->passwordLineEdit->text()).append(";"); // password
    }
    else
    {
        parametersMemory.clear();
        status=false;
        QMessageBox::warning(this,"Database Configuration","Please enter the Password ");
    }

    if(!ui->ipLineEdit->text().trimmed().isEmpty())
    {
        connectionSring.append("Server=").append(ui->ipLineEdit->text()).append(";");  //host address
        parametersMemory.append(ui->ipLineEdit->text().trimmed());

    }
    else
    {
        parametersMemory.clear();
        status=false;
        QMessageBox::warning(this,"Database Configuration","Please fill Database Server Address");
    }
    if(!ui->portLineEdit->text().trimmed().isEmpty())
    {
        connectionSring.append("Port=").append(ui->portLineEdit->text()).append(";");  //port address, optional
        parametersMemory.append(ui->portLineEdit->text().trimmed());
    }
    else
    {
        parametersMemory.append("5432");
    }

    if(status==true)
    {
        this->close();
        emit connectionStringPath(connectionSring,parametersMemory);
    }
}

void DatabaseDialog::on_pushButton_clicked()
{
    //    QDateTime UTC = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
    //    QString name (UTC.toString());
    //    name.replace(" ","_").replace(":","_");

    //    QFileDialog fileDialog(this);
    //    dbFilePath = fileDialog.getSaveFileName(this, tr("Save DB file as "),
    //                                            QString(/*homeDirectory+*/QDir::separator()+name),tr("DB File (*.db)"),
    //                                            0,QFileDialog::DontConfirmOverwrite);

    //    ui->sqliteDBLineEdit->setText(dbFilePath);
}

void DatabaseDialog::on_sqlComboBox_currentIndexChanged(int index)
{
    if(index==0)
    {
        ui->nextPushButton->setText("DONE");
    }
    else if (index == 1)
    {
        ui->nextPushButton->setText("NEXT");
    }
}
