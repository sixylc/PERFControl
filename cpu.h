#ifndef CPU_H
#define CPU_H

#include <QObject>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include "sysctl.h"
class CPU : public QObject
{
    Q_OBJECT
public:
    explicit CPU(SYSCTL *ctl,QObject *parent = nullptr);
    ~CPU();
    void search();
    QStringList GetGovernors();
    typedef struct CPUINFO
    {
        QString name;
        QString cores;
        QString logic_cores;
    }CPUINFO;
    CPUINFO Info;


signals:
    void setGovernor(uint8_t index);
    void setCpu_TRF(uint16_t fan_TRF);
    void setIndex(QString pwm_enable);
    void FrequencyChanged(double Crest_Freq,double Bottom_Freq,double AVG_Freq);

private:
    void GetCpuInfo();
    QTimer FRD;
    typedef struct CORE
    {
        QString affected_cpus;
        QString scaling_driver;
        QString cpuinfo_max_freq;
        QString cpuinfo_min_freq;
        QString cpuinfo_transition_latency;
        QStringList scaling_available_governors;
        QFile scaling_cur_freq;
        QFile scaling_governor;
        QFile scaling_max_freq;
        QFile scaling_min_freq;
        QFile scaling_setspeed;
    }CORE;



    QList<CORE*> cores;
    QList <uint32_t> Freq_List;
    double AVG_Freq;
    double Crest_Freq;
    double Bottom_Freq;
    SYSCTL *sysctl;
};

#endif // CPU_H
