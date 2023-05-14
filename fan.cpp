#include "fan.h"

FAN::FAN(SYSCTL *ctl, QObject *parent)
    : QObject{parent}
{
    sysctl = ctl;
    connect(this,&FAN::setIndex,this,[=](QString fan_enable_path)
    {
        if(QFileInfo(fan_enable_path).isFile())
        {
            QString Base(QFileInfo(fan_enable_path).path() + '/');
            QString Index(QFileInfo(fan_enable_path).fileName().replace("pwm","").replace("_enable",""));

            fan_enable.setFileName(fan_enable_path);
            fan_pwm.setFileName(Base + "pwm" + Index);
            fan_temp.setFileName(Base + "temp" + Index + "_input");

            if(!fan_enable.isWritable()|| !fan_pwm.isWritable())
            {
                sysctl->Chmod(777,QStringList{fan_enable.fileName() , fan_pwm.fileName()});
            }

            QFile file;

            file.setFileName(Base + "fan" + Index + "_label");

            if(file.open(QFile::ReadOnly))
            {
                fan_label = file.readAll();
                fan_label.remove('\n');
                file.close();
            }

            file.setFileName(Base + "name");

            if(file.open(QFile::ReadOnly))
            {
               fan_makers = file.readAll();
               fan_makers.remove('\n');
               file.close();
            }

            TRD.start(fan_TRF);

//            qDebug() << fan_enable_path << fan_pwm_path << fan_temp.fileName() << fan_label << fan_makers;

        }

    },Qt::DirectConnection);

    connect(this,&FAN::setEnabled,this,[=](bool enable)
    {
        if(Active != enable && (fan_enable.isOpen() || fan_enable.open(QFile::WriteOnly)))
        {
            if(Active)
            {
                fan_enable.write("1");
                fan_enable.flush();
            }
            else
            {
                fan_enable.write("2");
                fan_enable.flush();
            }

            Active = enable;
        }

    },Qt::QueuedConnection);

    connect(this,&FAN::setSpeed,this,[=](uint8_t speed)
    {
        if(speed == 0)
            setEnabled(false);
        else if(!Active)
            setEnabled(true);

        sysctl->set_conf(fan_label,QString::number(speed));

        if(fan_pwm.isOpen() || fan_pwm.open(QFile::WriteOnly))
        {
            fan_pwm.write(QString::number(speed).toLatin1());
            fan_pwm.flush();
        }
        else
        {
            qDebug() << "pwm file open fail";
        }


    },Qt::DirectConnection);

    connect(this,&FAN::setFan_TRF,this,[=](uint16_t fan_TRF)
    {
        TRD.setInterval(fan_TRF);
    },Qt::QueuedConnection);

    connect(&TRD,&QTimer::timeout,this,[=]()
    {
        if(fan_temp.isOpen() || fan_temp.open(QFile::ReadOnly))
        {
            tempChanged(fan_temp.readAll().toDouble()/1000);
            fan_temp.reset();
        }
    });

}

FAN::~FAN()
{
    fan_enable.close();
    fan_pwm.close();
    fan_temp.close();
}

QStringList FAN::search()
{
    QStringList List;
    QDir dir("/sys/class/hwmon");
    dir.setFilter(QDir::Dirs);

    foreach (auto dir_list, dir.entryInfoList())
    {
        foreach(auto file, QDir(dir_list.absoluteFilePath()).entryInfoList(QStringList() << "pwm*_enable",QDir::Files))
        {
            List << file.absoluteFilePath();
        }
    }

    return List;
}

QString FAN::label()
{
    return fan_label;
}

QString FAN::makers()
{
    return fan_makers;
}
