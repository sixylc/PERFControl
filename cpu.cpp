#include "cpu.h"

CPU::CPU(SYSCTL *ctl,QObject *parent)
    : QObject{parent}
{
    sysctl = ctl;

    GetCpuInfo();

    connect(this,&CPU::setGovernor,this,[=](uint8_t index)
    {
        sysctl->set_conf("governor",QString::number(index));

        foreach (auto var, cores)
        {
            if(!QFileInfo(var->scaling_governor.fileName()).isWritable())
            {
                sysctl->Chmod(777,QStringList(var->scaling_governor.fileName()));
            }

            if(var->scaling_governor.isOpen() || var->scaling_governor.open(QFile::WriteOnly))
            {
                var->scaling_governor.write(var->scaling_available_governors.at(index).toLatin1());
                var->scaling_governor.flush();
            }
        }
    });

    connect(&FRD,&QTimer::timeout,this,[=]()
    {
        foreach (auto var, cores)
        {
            if(var->scaling_cur_freq.isOpen() || var->scaling_cur_freq.open(QFile::ReadOnly))
            {
                Freq_List << var->scaling_cur_freq.readAll().toUInt();
                var->scaling_cur_freq.reset();
            }
        }

        if(Freq_List.count() > cores.count() *20)
        {
            std::sort(Freq_List.begin(),Freq_List.end());
            AVG_Freq = std::accumulate(Freq_List.begin(), Freq_List.end(), 0)/Freq_List.count()/1000000.0;
            Crest_Freq = Freq_List.last()/1000000.0;
            Bottom_Freq = Freq_List.first()/1000000.0;
            Freq_List.clear();
            emit FrequencyChanged(Crest_Freq,Bottom_Freq,AVG_Freq);
        }

    });

    search();

    FRD.start(100);

}

CPU::~CPU()
{
    foreach (auto var, cores)
    {
        delete var;
    }
}

void CPU::search()
{
    cores.clear();
    QDir dir("/sys/devices/system/cpu/cpufreq");
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (auto dir_list, dir.entryInfoList(QStringList()<< "policy*"))
    {
        foreach(auto file, QDir(dir_list.absoluteFilePath()).entryInfoList(QStringList() << "affected_cpus",QDir::Files))
        {
            CORE* core = new CORE();
            QFile core_file;

            core_file.setFileName(file.absolutePath() + '/' + "affected_cpus");
            if(core_file.open(QFile::ReadOnly)) {core->affected_cpus = core_file.readAll().replace("\n","");core_file.close();}

            core_file.setFileName(file.absolutePath() + '/' + "scaling_driver");
            if(core_file.open(QFile::ReadOnly)) {core->scaling_driver = core_file.readAll().replace("\n","");core_file.close();}

            core_file.setFileName(file.absolutePath() + '/' + "cpuinfo_max_freq");
            if(core_file.open(QFile::ReadOnly)) {core->cpuinfo_max_freq = core_file.readAll().replace("\n","");core_file.close();}

            core_file.setFileName(file.absolutePath() + '/' + "cpuinfo_min_freq");
            if(core_file.open(QFile::ReadOnly)) {core->cpuinfo_min_freq = core_file.readAll().replace("\n","");core_file.close();}

            core_file.setFileName(file.absolutePath() + '/' + "cpuinfo_transition_latency");
            if(core_file.open(QFile::ReadOnly)) {core->cpuinfo_transition_latency = core_file.readAll().replace("\n","");core_file.close();}

            core_file.setFileName(file.absolutePath() + '/' + "scaling_available_governors");
            if(core_file.open(QFile::ReadOnly)) {core->scaling_available_governors = QString::fromUtf8(core_file.readAll().replace(" \n","")).split(' ');core_file.close();}

            core->scaling_cur_freq.setFileName(file.absolutePath() + '/' + "scaling_cur_freq");
            core->scaling_governor.setFileName(file.absolutePath() + '/' + "scaling_governor");
            core->scaling_max_freq.setFileName(file.absolutePath() + '/' + "scaling_max_freq");
            core->scaling_min_freq.setFileName(file.absolutePath() + '/' + "scaling_min_freq");
            core->scaling_setspeed.setFileName(file.absolutePath() + '/' + "scaling_setspeed");

            cores << core;
        }
    }

}

QStringList CPU::GetGovernors()
{
    if(!cores.isEmpty())
        return cores.first()->scaling_available_governors;
    else
        return QStringList("ERR");
}

void CPU::GetCpuInfo()
{
    QFile CPUINFO_File("/proc/cpuinfo");

    if(CPUINFO_File.open(QFile::ReadOnly))
    {
        QRegularExpression regex;

        regex.setPattern("model name\\s*:\\s*(.*)");
        Info.name = regex.match(CPUINFO_File.readAll()).captured(1);

        regex.setPattern("cpu cores\\s*:\\s*(.*)");
        Info.cores = regex.match(CPUINFO_File.readAll()).captured(1);

        regex.setPattern("siblings\\s*:\\s*(.*)");
        Info.logic_cores = regex.match(CPUINFO_File.readAll()).captured(1);
    }

}
