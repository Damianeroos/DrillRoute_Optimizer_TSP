#include "sa_options.h"
#include "ui_sa_options.h"

SA_options::SA_options(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SA_options)
{
    ui->setupUi(this);
    this->setWindowTitle("Simulated annealing opions");
    iter = 10;
    initTemp = 100;
    finalTemp = 1;
    alpha = 0.99; //0.85 - 0.98
    repeat = 20;

    ui->alphaBox->setValue(alpha);
    ui->alphaBox->setRange(0.85,0.99);
    ui->initTempBox->setValue(initTemp);
    ui->initTempBox->setRange(1.0,10000);
    ui->finalTempBox->setValue(finalTemp);
    ui->finalTempBox->setRange(0.5,9999);
    ui->itBox->setValue(iter);
    ui->itBox->setRange(1,1000);
    ui->rpBox->setRange(1,1000);
    ui->rpBox->setValue(20);
}

SA_options::~SA_options()
{
    delete ui;
}

void SA_options::on_itBox_valueChanged(int arg1)
{
    iter = arg1;
}

void SA_options::on_alphaBox_valueChanged(double arg1)
{
    alpha = arg1;
}

void SA_options::on_finalTempBox_valueChanged(double arg1)
{

        finalTemp = arg1;

}

void SA_options::on_initTempBox_valueChanged(double arg1)
{

        initTemp = arg1;

}

void SA_options::on_initTempBox_editingFinished()
{
    if(initTemp <= finalTemp){
        initTemp = finalTemp+0.5;
        ui->initTempBox->setValue(initTemp);
    }
}

void SA_options::on_finalTempBox_editingFinished()
{
    if(initTemp <= finalTemp){
        finalTemp = initTemp-0.5;
        ui->finalTempBox->setValue(finalTemp);
    }
}

void SA_options::on_rpBox_valueChanged(int arg1)
{
    repeat = arg1;
}
