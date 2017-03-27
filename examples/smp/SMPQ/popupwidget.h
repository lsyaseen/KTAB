#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#include <QDialog>

namespace Ui {
class PopupWidget;
}

class PopupWidget : public QDialog
{
    Q_OBJECT

public:
    explicit PopupWidget(QWidget *parent = 0);
    ~PopupWidget();
    void showText(QString info);

protected slots:
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void on_pushButton_clicked();

private:
    int iXdeffarace = -1;
    int iYdeffarance  = -1;
    bool b_mousePressed;

private:
    Ui::PopupWidget *ui;
};

#endif // POPUPWIDGET_H
