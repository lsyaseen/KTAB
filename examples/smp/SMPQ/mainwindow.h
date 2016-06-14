#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "csv.h"

//! [0]
#include <QtWidgets>
#ifndef QT_NO_PRINTDIALOG
//#include <QtPrintSupport>
#endif

#include <QDebug>
class QAction;
class QListWidget;
class QMenu;
class QTextEdit;


namespace Ui {
class MainWindow;
}

static const int N = 2;

class MainWindow : public QMainWindow
{

    Q_OBJECT

public:
    //explicit MainWindow(QWidget *parent = 0);
    MainWindow();
    ~MainWindow();

private slots:
    void about();

    void csvItemModel(QStandardItemModel * model);
    void csvGetFilePAth();
    void dockWindowChanged();
 signals:

    void csvFilePath(QString path);
private:
    void createActions();
    void createStatusBar();
    void createGraph1DockWindows();
    void createGraph2DockWindows();
    void createModuleParametersDockWindow();

    //Model parameters
    QFrame *central;

    QFrame * frames[N];
    QPushButton * runButton;


    QRadioButton * rparam1;
    QRadioButton * rparam2;
    QRadioButton * rparam3;

    QCheckBox * cparam1;
    QCheckBox * cparam2;
    QCheckBox * cparam3;

    //csv
    QTableView * csv_tableView;
    CSV *csvObj;

    QListWidget *listwidget;
    QListWidget *listwidget2;

    QMenu *viewMenu;


    QDockWidget * moduleParametersDock;
    QDockWidget * graph1Dock;
    QDockWidget * graph2Dock;

    QGridLayout * VLayout;
    QFrame * moduleFrame;





    //private:
    //  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
