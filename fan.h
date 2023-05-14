#ifndef FAN_H
#define FAN_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <stdlib.h>
#include "sysctl.h"

class FAN : public QObject
{
    Q_OBJECT
public:
    explicit FAN(SYSCTL *ctl, QObject *parent = nullptr);
    ~FAN();
    static QStringList search();
    QString label();
    QString makers();
    bool isActive();
    void set_sysctl(SYSCTL *ctl);


signals:
    void setIndex(QString pwm_enable);
    void setEnabled(bool fan_enable);
    void setSpeed(uint8_t speed);
    void setFan_TRF(uint16_t fan_TRF);
    void tempChanged(double temp);


private:
    QFile fan_enable;//使能
    QFile fan_pwm;//调速
    QFile fan_temp;//温度
    QString fan_label;//标签
    QString fan_makers;//厂商

    QTimer TRD;
    uint16_t fan_TRF = 500;
    bool Active;
    SYSCTL *sysctl;
};

#endif // FAN_H
