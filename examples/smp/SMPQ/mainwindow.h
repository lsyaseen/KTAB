#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "csv.h"
#include "database.h"
#include "qcustomplot.h"


#include <QtSql>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>


#include <QtWidgets>
#ifndef QT_NO_PRINTDIALOG
//#include <QtPrintSupport>
#endif

#include <QDebug>
#include <QMessageBox>

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
    void dockWindowChanged();
    //CSV
    void setCSVItemModel(QStandardItemModel * model);
    void setDBItemModel(QSqlTableModel * model);
    void csvGetFilePAth();
    //Database
    void DisplayMessage(QString cls, QString message);
    void VectorPositionsFromDB();
    void dbGetFilePAth();

signals:
    //CSV
    void csvFilePath(QString path);

    //Database
    void dbFilePath(QString path);
private:
    //MainWindow
    void createActions();
    void createStatusBar();
    void createGraph1DockWindows();
    void createGraph2DockWindows();
    void createModuleParametersDockWindow();

    // Central Main Frame
    QFrame *central;
    QFrame *tableControlsFrame;
    QComboBox * scenarioComboBox;
    QPushButton * actorsPushButton;
    QLineEdit * actorsLineEdit;
    QPushButton * dimensionsPushButton;
    QLineEdit * dimensionsLineEdit;
    QPushButton * donePushButton;
    QSlider * turnSlider;

    //Model parameters
    QFrame * frames[N];
    QPushButton * runButton;

    QRadioButton * rparam1;
    QRadioButton * rparam2;
    QRadioButton * rparam3;

    QCheckBox * cparam1;
    QCheckBox * cparam2;
    QCheckBox * cparam3;

    //csv window
    QTableView * csv_tableView;
    CSV *csvObj;

    QListWidget *listwidget2;

    QMenu *viewMenu;

    QDockWidget * moduleParametersDock;
    QDockWidget * graph1Dock;
    QDockWidget * graph2Dock;

    QGridLayout * VLayout;
    QFrame * moduleFrame;

    //Database Obj
    Database * dbObj ;
    QSqlDatabase db;

    //Graph 1 widget
    QFrame *graphWidget;
    QGridLayout * gridLayout;
    QCustomPlot * customGraph;

    void initializeGraphPlot1();
    void plotGraph();
    void initializeCentralViewFrame();


    //graph - customplot
private slots:
    void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void addGraphOnModule1(const QVector<double> &x, const QVector<double> &y);
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void graphClicked(QCPAbstractPlottable *plottable);
};

#endif // MAINWINDOW_H
