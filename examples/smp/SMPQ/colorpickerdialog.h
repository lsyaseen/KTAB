#ifndef COLORPICKERDIALOG_H
#define COLORPICKERDIALOG_H

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QLineEdit>
#include <QColorDialog>
#include <QDebug>
#include <QMenu>

namespace Ui {
class colorPickerDialog;
}

class ColorPickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorPickerDialog(QWidget *parent = 0);
    ~ColorPickerDialog();
    void intializeActors(QList<QString> actorsNames, QList<QColor> colors);

private:
    void initializeDialogWindow();
    QFrame *  initializeActorFrame(QString actName, QList<QColor> colors, int index);

    QVBoxLayout * vLayout;
    QList<QColor> pickerColors;
    QList<QPushButton *> colorButtons;
    QList<QString> actorNamesList;

    QColor copyColorCode;

private slots:
    void showColorPickerWidget(bool bl);
    void donePushButtonClicked(bool bl);
    void showContextMenuForActorPB(QPoint pos);
signals:
    void changedColors(QList<QColor> colors);
};

#endif // COLORPICKERDIALOG_H
