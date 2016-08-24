#include <QtGui/QApplication>
#include "ledtest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ledtest w;
    w.setWindowFlags(w.windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
    w.showMaximized();
    return a.exec();
}
