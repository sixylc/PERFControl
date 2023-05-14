#include "sysctl.h"

SYSCTL::SYSCTL(QObject *parent)
    : QObject{parent}
{
    if(QFileInfo(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/perfcontrol/perfcontrol.conf").exists())
    {
        QFile file(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/perfcontrol/perfcontrol.conf");
        if(file.open(QFile::ReadOnly))
        {
            conf = QJsonDocument::fromJson(file.readAll()).object();
        }

        file.close();
    }
    else
    {
        QDir dir;
        dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/perfcontrol");
        QFile file(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/perfcontrol/perfcontrol.conf");
        file.open(QFile::WriteOnly);
        file.close();
    }
}

SYSCTL::~SYSCTL()
{
    QFile file(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/perfcontrol/perfcontrol.conf");
    if(file.open(QFile::WriteOnly))
    {
        file.write(QJsonDocument(conf).toJson());
    }
    file.close();
}


void SYSCTL::Chmod(uint16_t power, QStringList path)
{
    QString paths;

    foreach (auto var, path)
    {
        paths += var + ' ';
    }

    if(TestPasswd())system(QString("echo '" + conf.value("passwd").toString() + "'|sudo -S chmod "+ QString::number(power) + ' '  +  paths).toLatin1().data());

}

void SYSCTL::set_conf(QString index,QString value)
{
    conf[index] = value;
}

QString SYSCTL::get_conf(QString index)
{
    return conf.value(index).toString();
}

void SYSCTL::set_passwd(QString passwd)
{
    set_conf("passwd",passwd);
}

bool SYSCTL::TestPasswd()
{
    if(conf.value("passwd").toString().isEmpty())
    {
        QString PassWord_Text = QInputDialog::getText(nullptr,"Su权限","请输入root密码", QLineEdit::Normal,"");

        if (PassWord_Text.isEmpty())
        {
            return false;
        }
        else
        {
            ;
            if(WEXITSTATUS(system(QString("echo '" + PassWord_Text + "'|sudo -S echo  'This is SU Passwd Test'").toLatin1().data())))
                QMessageBox::information(NULL,"SU密码测试","密码错误");
            else
                conf["passwd"] = PassWord_Text;


            return true;
        }
    }
    else
        return true;
}


