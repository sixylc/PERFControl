#ifndef SYSCTL_H
#define SYSCTL_H

#include <QObject>
#include <QInputDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

class SYSCTL : public QObject
{
    Q_OBJECT
public:
    explicit SYSCTL(QObject *parent = nullptr);
    ~SYSCTL();

    void Chmod(uint16_t power,QStringList path);
    void set_conf(QString index,QString value);
    QString get_conf(QString index);

    void set_passwd(QString passwd);


signals:



private:
    QJsonObject conf;
    bool TestPasswd();

};

#endif // SYSCTL_H
