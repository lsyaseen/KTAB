#ifndef SPECSWINDOW_H
#define SPECSWINDOW_H


#include <QFrame>
#include <QObject>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QtGui>
#include <QComboBox>
#include <QSplitter>
#include <QCheckBox>
#include <QPushButton>
#include <QListView>
#include <QMenu>

class SpecificationFrame: public QFrame
{
    Q_OBJECT

public:
    explicit SpecificationFrame(QObject *parent = Q_NULLPTR);
    ~SpecificationFrame();



};




#endif // SPECSWINDOW_H
