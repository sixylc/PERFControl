#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "lcwidget.h"
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include "fan.h"
#include "cpu.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public LCWidget
{
    Q_OBJECT

public:
    explicit MainWidget(LCWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
    SYSCTL sysctl;
    QList<FAN*> fans;
    FAN* oldfans = nullptr;
    CPU *cpus = nullptr;

private slots:
    void on_FanBox_currentIndexChanged(int index);
    void on_Set_FanSpeed_valueChanged(int value);
    void FanTemp_Update(double temp);
    void on_Governors_currentIndexChanged(int index);
    void on_set_passwd_triggered();
};

#endif // MAINWIDGET_H
