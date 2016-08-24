/********************************************************************************
** Form generated from reading UI file 'ledtest.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEDTEST_H
#define UI_LEDTEST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ledtest
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QCheckBox *checkBox_LED2;
    QCheckBox *checkBox_LED1;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ledtest)
    {
        if (ledtest->objectName().isEmpty())
            ledtest->setObjectName(QString::fromUtf8("ledtest"));
        ledtest->resize(320, 240);
        centralWidget = new QWidget(ledtest);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        checkBox_LED2 = new QCheckBox(centralWidget);
        checkBox_LED2->setObjectName(QString::fromUtf8("checkBox_LED2"));

        gridLayout->addWidget(checkBox_LED2, 0, 1, 1, 1);

        checkBox_LED1 = new QCheckBox(centralWidget);
        checkBox_LED1->setObjectName(QString::fromUtf8("checkBox_LED1"));

        gridLayout->addWidget(checkBox_LED1, 0, 0, 1, 1);


        verticalLayout->addLayout(gridLayout);

        ledtest->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ledtest);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 320, 25));
        ledtest->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ledtest);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ledtest->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ledtest);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ledtest->setStatusBar(statusBar);

        retranslateUi(ledtest);

        QMetaObject::connectSlotsByName(ledtest);
    } // setupUi

    void retranslateUi(QMainWindow *ledtest)
    {
        ledtest->setWindowTitle(QApplication::translate("ledtest", "ledtest", 0, QApplication::UnicodeUTF8));
        checkBox_LED2->setText(QApplication::translate("ledtest", "LED2", 0, QApplication::UnicodeUTF8));
        checkBox_LED1->setText(QApplication::translate("ledtest", "LED1", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ledtest: public Ui_ledtest {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEDTEST_H
