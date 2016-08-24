#ifndef LEDTEST_H
#define LEDTEST_H

#include <QMainWindow>

namespace Ui {
    class ledtest;
}

class ledtest : public QMainWindow {
    Q_OBJECT
public:
    ledtest(QWidget *parent = 0);
    ~ledtest();

protected:
    void changeEvent(QEvent *e);
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);

public slots:
     void LED1_Toggle();
     void LED2_Toggle();

private:
    Ui::ledtest *ui;
};

#endif // LEDTEST_H
