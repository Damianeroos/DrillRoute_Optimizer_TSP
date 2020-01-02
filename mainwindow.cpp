#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    MainWindow::setWindowTitle("PCB driller path finder");
    ui->statusbar->showMessage("No .brd file has been loaded");


}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_loadFileButton_clicked()
{
    file_name = ui->boardFileName->text();
    file.setFileName(file_name);

    //cannot open file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
           ui->statusbar->setStyleSheet("color: red");
           ui->statusbar->showMessage("Can not open file.",5000);
    }
    else if(file.pos()==0){ //plik jest pusty
        ui->statusbar->setStyleSheet("color: red");
        ui->statusbar->showMessage(file_name+" is empty.",5000);
    }
    else{
        ui->statusbar->setStyleSheet("color: green");
        ui->statusbar->showMessage(file_name+" has been loaded successfully.",5000);
    }


}
