#include "colorpickerdialog.h"

ColorPickerDialog::ColorPickerDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Actor Color Picker");
    setModal(true);

    int nWidth = 200;
    int nHeight = 500;
    if (parent != NULL)
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
                    parent->y() + parent->height()/2 - nHeight/2,
                    nWidth, nHeight);
    else
        resize(nWidth, nHeight);

    initializeDialogWindow();

}

ColorPickerDialog::~ColorPickerDialog()
{
    delete vLayout;
}

void ColorPickerDialog::initializeDialogWindow()
{
    QGridLayout * mainLayout = new QGridLayout;
    setLayout(mainLayout);

    QFrame * headerFrame = new QFrame;
    headerFrame->setFrameStyle(QFrame::Plain);
    headerFrame->setFrameShape(QFrame::Box);

    QHBoxLayout * headerLayout = new QHBoxLayout(headerFrame);

    QLabel *actorHLabel= new QLabel("Actor");
    headerLayout->addWidget(actorHLabel,0,Qt::AlignHCenter);

    QScrollArea * actorsSA = new QScrollArea;

    QWidget* widget = new QWidget;
    vLayout = new QVBoxLayout(widget);

    mainLayout->addWidget(headerFrame);
    mainLayout->addWidget(actorsSA);
    QHBoxLayout * controlsLayout = new QHBoxLayout;

    QPushButton * donePB = new QPushButton("Done");
    QPushButton * cancelPB = new QPushButton("Cancel");
    controlsLayout->addWidget(donePB,0,Qt::AlignRight);
    controlsLayout->addWidget(cancelPB,0,Qt::AlignLeft);
    connect(donePB,SIGNAL(clicked(bool)),this,SLOT(donePushButtonClicked(bool)));
    connect(cancelPB,SIGNAL(clicked(bool)),this,SLOT(close()));

    mainLayout->addLayout(controlsLayout,2,0,Qt::AlignBottom);

    actorsSA->setWidget(widget);
    actorsSA->setWidgetResizable(true);
}

void ColorPickerDialog::intializeActors(QList<QString> actorsNames, QList<QColor> colors)
{
    pickerColors = colors;
    actorNamesList = actorsNames;
    colorButtons.clear();
    copyColorCode=colors.at(0);
    for( int i = 0; i < actorsNames.length() ; ++i)
    {
        vLayout->addWidget(initializeActorFrame(actorsNames.at(i),pickerColors,i));
    }
}

QFrame * ColorPickerDialog::initializeActorFrame(QString actName, QList<QColor> colors,int index)
{
    QFrame * actorFrame= new QFrame;

    QHBoxLayout * hLayout= new QHBoxLayout(actorFrame);

    QColor mycolor = colors.at(index);

    QString style = "background: rgb(%1, %2, %3);";
    style = style.arg(mycolor.red()).arg(mycolor.green()).arg(mycolor.blue());

    QPushButton * chooseColorPB = new QPushButton(actName);
    chooseColorPB->setMaximumHeight(20);
    chooseColorPB->setMaximumWidth(80);
    chooseColorPB->setStyleSheet(style);
    chooseColorPB->setObjectName(actName+mycolor.name());
    chooseColorPB->setContextMenuPolicy(Qt::CustomContextMenu);
    colorButtons.append(chooseColorPB);

    connect(chooseColorPB,SIGNAL(clicked(bool)),this,SLOT(showColorPickerWidget(bool)));
    connect(chooseColorPB,SIGNAL(customContextMenuRequested(QPoint))
            ,this,SLOT(showContextMenuForActorPB(QPoint)));

    hLayout->addWidget(chooseColorPB,0,Qt::AlignHCenter);

    return actorFrame;
}

void ColorPickerDialog::showColorPickerWidget(bool bl)
{
    QPushButton * actorColorChooser = qobject_cast<QPushButton *>(sender());
    QString str = actorColorChooser->objectName();
    QStringList strlist = str.split("#");
    str = strlist.at(0);

    int actorId = actorNamesList.indexOf(str);
    qDebug()<< "actorColorChooser" << actorColorChooser->objectName() <<str <<actorId;

    QColor color = QColorDialog::
            getColor(pickerColors.at(actorId),
                     this, "Pick a color",  QColorDialog::DontUseNativeDialog);

    if(color.isValid())
    {
        qDebug()<<color << "here ";
        pickerColors[actorId]=color;
        QColor mycolor =color;

        QString style = "background: rgb(%1, %2, %3);";
        style = style.arg(mycolor.red()).arg(mycolor.green()).arg(mycolor.blue());

        colorButtons.at(actorId)->setStyleSheet(style);
        colorButtons.at(actorId)->setObjectName(str+color.name());

    }
}

void ColorPickerDialog::donePushButtonClicked(bool bl)
{
    emit changedColors(pickerColors);
    this->close();
}
//NOTE: USE SPLIT ON STRING
void ColorPickerDialog::showContextMenuForActorPB(QPoint pos)
{
    QPushButton * actorColorButton = qobject_cast<QPushButton *>(sender());

    QMenu menu(this);
    QAction *copyColor = menu.addAction("Copy");
    QAction *pasteColor = menu.addAction("Paste");

    QAction *act = menu.exec(actorColorButton->mapToGlobal(pos));

    if (act == copyColor)
    {
        QString str = actorColorButton->objectName();
        QStringList strlist = str.split("#");
        str = strlist.at(1);
        str.prepend("#");

        copyColorCode=str;
        qDebug()<< "copyColor" <<  actorColorButton->styleSheet()  << actorColorButton->objectName() <<str;
    }
    else if (act == pasteColor)
    {
        QString str = actorColorButton->objectName();
        QStringList strlist = str.split("#");
        str = strlist.at(0);

        int actorId = actorNamesList.indexOf(str);

        QColor mycolor =copyColorCode;
        QString style = "background: rgb(%1, %2, %3);";
        style = style.arg(mycolor.red()).arg(mycolor.green()).arg(mycolor.blue());
        colorButtons.at(actorId)->setStyleSheet(style);
        colorButtons.at(actorId)->setObjectName(str+copyColorCode.name());
        pickerColors[actorId]=copyColorCode;
        qDebug()<<actorId << actorColorButton->objectName()<<"paste" <<colorButtons.at(actorId)->objectName();
    }
}
