#include "ledtest.h"
#include "ui_ledtest.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>

#include <QDirectPainter>

volatile int LED1=0,LED2=0,LED3=0,LED4=0;

ledtest::ledtest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ledtest)
{
    ui->setupUi(this);
    int screenWidth=QDirectPainter::screenWidth();
    int screenHeight=QDirectPainter::screenHeight();
    this->resize(screenWidth,screenHeight);

    connect(ui->checkBox_LED1,SIGNAL(toggled(bool)),this,SLOT(LED1_Toggle()));
    connect(ui->checkBox_LED2,SIGNAL(toggled(bool)),this,SLOT(LED2_Toggle()));
}

ledtest::~ledtest()
{
    delete ui;
}

void ledtest::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ledtest::LED1_Toggle()
{
    LED1=!LED1;
    if(LED1==1)
    {
        ::system("echo 1 > /sys/class/leds/led1/brightness");
    }
    else
    {
        ::system("echo 0 > /sys/class/leds/led1/brightness");
    }
}

void ledtest::LED2_Toggle()
{
    LED2=!LED2;
    if(LED2==1)
    {
       ::system("echo 1 > /sys/class/leds/led2/brightness");
    }
    else
    {
        ::system("echo 0 > /sys/class/leds/led2/brightness");
    }
}


void ledtest::moveEvent(QMoveEvent *)
{
    this->move(QPoint(0,0));
}

void ledtest::resizeEvent(QResizeEvent *)
{
    this->showMaximized();
}

