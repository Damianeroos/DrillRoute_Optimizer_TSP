#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    MainWindow::setWindowTitle("PCB driller path finder");
    ui->statusbar->showMessage("No .brd file has been loaded");
    ui->boardFileName->setText("../test.brd");

    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(-10, 100);
    ui->customPlot->yAxis->setRange(-10, 100);
    ui->customPlot->replot();
    ui->customPlot->setInteraction(QCP::iRangeDrag, true);

    ui->comboBoxAlg->addItem("NN");


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
    else if(file.size() ==0){ //file is empty
        ui->statusbar->setStyleSheet("color: red");
        ui->statusbar->showMessage(file_name+" is empty.",5000);
        file.close();

    }
    else{
        ui->statusbar->setStyleSheet("color: green");
        ui->statusbar->showMessage(file_name+" has been loaded successfully.",5000);

        xml_file.setContent(&file);
        file.close();


        if(!ReadHolesPosition()){ //there is not elements to be drawn
            ui->statusbar->setStyleSheet("color: red");
            ui->statusbar->showMessage("Loaded board is empty.",5000);
        }
        else{
            //drawing
            for(int i=0;i < X.size(); i++){
                qDebug()<<QString::number(X[i]) + " " + QString::number(Y[i]);
            }

            // create graph and assign data to it:
            ui->customPlot->addGraph();
            ui->customPlot->graph(0)->setData(X, Y);
            ui->customPlot->graph(0)->setPen(QColor(0, 0, 0, 255));
            ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
            ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));

            ui->customPlot->replot();
        }
    }


}

int MainWindow::ReadHolesPosition()
{
    QDomElement doc = xml_file.documentElement();
    QDomElement elements = doc.firstChildElement("drawing").firstChildElement("board").firstChildElement("elements");
    QDomElement libraries = doc.firstChildElement("drawing").firstChildElement("board").firstChildElement("libraries");
    QDomElement atrr,pads;

    QString library_urn,package,rot;
    double x,y,xp,yp,rad,xpp,ypp;

    if(elements.isNull())
        return 0;



    for (QDomNode n = elements.firstChildElement(); !n.isNull(); n = n.nextSiblingElement()) {
        //here are elements
        //qDebug() << n.nodeName();
        atrr = n.toElement();
        library_urn = atrr.attribute("library_urn");
        package = atrr.attribute("package");
        rot = atrr.attribute("rot","R0");//if there is no rot atrr, then R = 0
        x = atrr.attribute("x").toDouble();
        y = atrr.attribute("y").toDouble();
        //here we are looking for suitable libraries

        for(QDomNode m = libraries.firstChildElement(); !m.isNull();m = m.nextSiblingElement()){
            //here are libraries
            //qDebug() << m.nodeName();
            atrr = m.toElement();
            if(atrr.attribute("urn")==library_urn){
                //this is suitable library, now looking for good pacakage

                for(QDomNode k = atrr.firstChildElement("packages");!k.isNull();k = k.nextSiblingElement()){

                    //here are packages and packages3d (idk why?!)

                    if(k.nodeName()=="packages"){
                        //qDebug()<<k.nodeName();
                        for(QDomNode i = k.firstChild();!i.isNull();i = i.nextSiblingElement()){
                            //hera are package
                            if(package == i.toElement().attribute("name")){
                                //good package
                                for(QDomNode j = i.firstChild();!j.isNull();j = j.nextSiblingElement()){
                                    //looking for pads
                                    if(j.nodeName()=="pad"){
                                        pads = j.toElement();
                                        //adding and computing pads position
                                        if(rot == "R0"){
                                            X.push_back(pads.attribute("x").toDouble()+x);
                                            Y.push_back(pads.attribute("y").toDouble()+y);
                                        }
                                        if(rot == "R90"){
                                            rad = qDegreesToRadians(90.00);
                                            xp = pads.attribute("x").toDouble();
                                            yp = pads.attribute("y").toDouble();
                                            xpp = xp*qCos(rad)-yp*qSin(rad);
                                            ypp = xp*qSin(rad)+yp*qCos(rad);
                                            X.push_back(xpp+x);
                                            Y.push_back(ypp+y);
                                        }
                                        if(rot == "R180"){
                                            rad = qDegreesToRadians(180.00);
                                            xp = pads.attribute("x").toDouble();
                                            yp = pads.attribute("y").toDouble();
                                            xpp = xp*qCos(rad)-yp*qSin(rad);
                                            ypp = xp*qSin(rad)+yp*qCos(rad);
                                            X.push_back(xpp+x);
                                            Y.push_back(ypp+y);
                                        }
                                        if(rot == "R270"){
                                            rad = qDegreesToRadians(270.00);
                                            xp = pads.attribute("x").toDouble();
                                            yp = pads.attribute("y").toDouble();
                                            xpp = xp*qCos(rad)-yp*qSin(rad);
                                            ypp = xp*qSin(rad)+yp*qCos(rad);
                                            X.push_back(xpp+x);
                                            Y.push_back(ypp+y);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    }

    return 1;

}

double MainWindow::NN_algorithm()
{
    QVector<double> XP,YP;
    XP = X;
    YP = Y;
    XP.push_front(0);//adding starting point (0,0)
    YP.push_front(0);

    int size = XP.size();
    double distance = 0;

    double ** distMat = new double* [size]; //creating distance matrix

    for(int i = 0;i < size ; i++){
        distMat[i] = new double [size];
    }

    //computing distance matrix
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            distMat[i][j]=qSqrt(qPow(XP[i]-XP[j],2)+qPow(YP[i]-YP[j],2));
        }
    }


    qDebug() << QString::number(size);

    for(int i = 0;i < size ; i++){ //clearing distance matrix
        delete [] distMat[i];
    }
    delete [] distMat;

    return distance;
}

void MainWindow::on_startButton_clicked()
{
    NN_algorithm();
}
