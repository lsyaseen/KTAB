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
#include <QMenu>

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
    void setCSVItemModel(QStandardItemModel * model, QStringList scenarioName);
    void csvGetFilePAth();
    //Database
    void setDBItemModel(QSqlTableModel * model);
    void setDBItemModelEdit();
    void displayMessage(QString cls, QString message);
    void vectorPositionsFromDB();
    void dbGetFilePAth();
    void dbEditGetFilePAth();
    void updateStateCount_SliderRange(int states);
    void updateScenarioList_ComboBox(QStringList * scenarios);
    void updateDimensionCount(int dim);

    //Central-  Controls Frame
    void sliderStateValueToQryDB(int value);
    void scenarioComboBoxValue(QString scenario_box);
    void createNewCSV();
    void cellSelected(int row, int column);
    void insertNewRowCSV();
    void insertNewColumnCSV();
    void donePushButtonClicked();

    bool eventFilter(QObject*, QEvent*);

    //DB to CSV
    void actorsName_Description(QList<QString> actorName, QList<QString> actorDescription);
    void actors_Influence(QList<QString> ActorInfluence);
    void actors_Position(QList<QString> actorPosition, int dim);
    void actors_Salience(QList<QString> actorSalience, int dim);


signals:
    //CSV
    void csvFilePath(QString path);

    //Database
    void dbFilePath(QString path);
    void dbEditFilePath(QString path);

    void getScenarioRunValues(int state, QString scenario_box);
    void getScenarioRunValuesEdit(QString scenario_box);
    void getStateCountfromDB();
    void getDimensionCountfromDB();

    //save DB to csv
    void getActorsDesc();
    void getInfluence();
    void getPosition(int dim);
    void getSalience(int dim);

private:
    //MainWindow
    void createActions();
    void createStatusBar();
    void createGraph1DockWindows();
    void createGraph2DockWindows();
    void createModuleParametersDockWindow();
    void saveTableViewToCSV();
    void saveTableWidgetToCSV();
    void convertDBtoCSVFormat();

    // Central Main Frame
    QFrame *central;
    QFrame *tableControlsFrame;
    QGridLayout *gLayout;
    QStackedWidget * stackWidget;

    QComboBox * scenarioComboBox;
    QPushButton * actorsPushButton;
    QLineEdit * actorsLineEdit;
    QPushButton * dimensionsPushButton;
    QLineEdit * dimensionsLineEdit;
    QPushButton * donePushButton;
    QLineEdit * scenarioDescriptionLineEdit;
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

    QTableWidget * csv_tableWidget;

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
    int dimensions;
    QString dbPath;

    //Graph 1 widget
    QFrame *graphWidget;
    QGridLayout * gridLayout;
    QCustomPlot * customGraph;

    void initializeGraphPlot1();
    void plotGraph();
    void initializeCentralViewFrame();

    QString scenario_box;
    //graph - customplot

    //    to  edit headers

    QLineEdit* header_editor;
    int editor_index;

    QString  tableType; // CSV, Database, NewCSV
    QStandardItemModel *modeltoCSV;

    QString inputCSV;
    void createSeperateColumn();

    //DB to CSV
    int dim;
    QList <QString> actorsName;
    QList <QString> actorsDescription;
    QList <QString> actorsInfluence;
    QList <QString> actorsPosition[3];
    QList <QString> actorsSalience[3];


private slots:
    void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void addGraphOnModule1(const QVector<double> &x, const QVector<double> &y, QString Actor);
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void graphClicked(QCPAbstractPlottable *plottable);
};

#endif // MAINWINDOW_H
