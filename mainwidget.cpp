  #include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(LCWidget *parent) :
    LCWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);



    cpus = new CPU(&sysctl);
    ui->groupBox->setTitle(cpus->Info.name);
    connect(cpus,&CPU::FrequencyChanged,this,[=](double Crest_Freq,double Bottom_Freq,double AVG_Freq)
    {
        ui->MaxFreq->setText("频峰\n" + QString::number(Crest_Freq,'f',3) + " Ghz");
        ui->MinFreq->setText("频谷\n" + QString::number(Bottom_Freq,'f',3) + " Ghz");
        ui->AvgFreq->setText("均频\n" + QString::number(AVG_Freq,'f',3) + " Ghz");
    });


    int index = sysctl.get_conf("governor").toUInt();
    ui->Governors->addItems(cpus->GetGovernors());
    ui->Governors->setCurrentIndex(index);

    foreach (auto var, FAN::search())
    {
        FAN *fan = new FAN(&sysctl);
        fan->setIndex(var);
        fans.append(fan);
        ui->FanBox->addItem(fan->label());
    }


    QFile QSS(":/MainWindow.qss");
    if(QSS.open(QFile::OpenModeFlag::ReadOnly))
        ui->frame->setStyleSheet(QSS.readAll());

}

MainWidget::~MainWidget()
{
    foreach (auto var, fans)
    {
        delete var;
    }

    delete cpus;
    delete ui;
}

void MainWidget::on_FanBox_currentIndexChanged(int index)
{
    if(oldfans != nullptr)disconnect(oldfans);
    connect(fans.at(index),&FAN::tempChanged,this,&MainWidget::FanTemp_Update);
    oldfans = fans.at(index);
    ui->Set_FanSpeed->setValue(sysctl.get_conf(oldfans->label()).toInt());
}

void MainWidget::on_Set_FanSpeed_valueChanged(int value)
{
    if(oldfans != nullptr)
    {
        oldfans->setSpeed(value);
//        qDebug() << "on_Set_FanSpeed_valueChanged" << value;
    }
}

void MainWidget::FanTemp_Update(double temp)
{
    ui->FanTemp->setText(QString::number(temp) +" °C");
}


void MainWidget::on_Governors_currentIndexChanged(int index)
{
    cpus->setGovernor(index);
}

void MainWidget::on_set_passwd_triggered()
{
    QString PassWord_Text = QInputDialog::getText(this,"Su权限","请输入root密码", QLineEdit::Normal,"");
    if (PassWord_Text.isEmpty())
    {
        QMessageBox::information(this,"Su权限","密码获取失败");
    }
    else
    {
        sysctl.set_passwd(PassWord_Text);
    }
}

