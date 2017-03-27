#include "popupwidget.h"
#include "ui_popupwidget.h"

PopupWidget::PopupWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PopupWidget)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(Qt::FramelessWindowHint);
}

PopupWidget::~PopupWidget()
{
    delete ui;
}

void PopupWidget::on_pushButton_clicked()
{
    this->close();
}

void PopupWidget::showText(QString info)
{
    ui->label->setText(info);
    ui->label->setWordWrap(true);

}

void PopupWidget::mousePressEvent ( QMouseEvent * event)
{
    b_mousePressed = true;
    QPoint qpMousePressedPoint = QCursor::pos();
    QPoint qpApploc = this->pos();
    iXdeffarace = qpMousePressedPoint.x() - qpApploc.x();
    iYdeffarance = qpMousePressedPoint.y() - qpApploc.y();
}

void PopupWidget::mouseReleaseEvent ( QMouseEvent * event )
{
    b_mousePressed = false;
}

void PopupWidget::mouseMoveEvent ( QMouseEvent * event )
{
    if(b_mousePressed)
    {
        QPoint qpAppNewLoc(  (QCursor::pos().x() - iXdeffarace) , (QCursor::pos().y() - iYdeffarance)  );
        this->setProperty("pos", qpAppNewLoc);
    }
}
