
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MainWindow::setWindowTitle("PCB driller path finder");
    ui->statusbar->showMessage("No .brd file has been loaded");


    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(-10, 100);
    ui->customPlot->yAxis->setRange(-10, 100);
    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, "Empty plot", QFont("sans", 12, QFont::Bold)));
    ui->customPlot->replot();
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->algorithmBox->addItem("NP"); //natural permutation
    ui->algorithmBox->addItem("NN"); //nearest neighbour
    ui->algorithmBox->addItem("SA"); //simulated annealing
    ui->algorithmBox->addItem("2-opt");//2-opt algrithm
    ui->optionBtn->setEnabled(false);
    ui->progressBar->setVisible(false);
    ui->progressBar->setAlignment(Qt::AlignCenter);
    ui->progressBar->setRange(0,w_options.repeat);
    ui->progressBar->setValue(0);
    ui->progressBar->setFormat("%v/%m");
    ui->animationBtn->setEnabled(false);
    ui->startBtn->setEnabled(false);
    ui->algorithmBox->setEnabled(false);
    ui->saveBtn->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_startBtn_clicked()
{
    ui->loadBtn->setEnabled(false);
    ui->startBtn->setEnabled(false);
    ui->algorithmBox->setEnabled(false);
    ui->saveBtn->setEnabled(false);
    w_options.setEnabled(false);
    ui->statusbar->setStyleSheet("color: green");
    ui->statusbar->showMessage("busy...");

    switch (ui->algorithmBox->currentIndex()) {
    case 0 :
        ui->finalDistance->setText(QString::number(NP_algorithm()));
        DrawPermutation();
        break;
    case 1 :
        ui->finalDistance->setText(QString::number(NN_algorithm()));
        DrawPermutation();
        break;
    case 2 :
        ui->progressBar->setRange(0,w_options.repeat);

        for(int i = 0 ; i < w_options.repeat;i++){
            ui->progressBar->setFormat("%v/%m");
            ui->progressBar->setValue(i+1);
            ui->finalDistance->setText(QString::number(SA_algorithm()));
            DrawPermutation();
        }
        ui->progressBar->setFormat("Complete!!!");
        break;
    case 3 :
        ui->finalDistance->setText(QString::number(opt2_algorithm()));
        DrawPermutation();
        break;
    }

    ui->loadBtn->setEnabled(true);
    ui->startBtn->setEnabled(true);
    ui->algorithmBox->setEnabled(true);
    w_options.setEnabled(true);
    ui->statusbar->showMessage("complete");
    ui->saveBtn->setEnabled(true);

}


void MainWindow::on_loadBtn_clicked()
{
    QVector<double> tempX,tempY,point0;
    int size;
    point0.push_back(0);

    tsp_.clear();

    file_name = QFileDialog::getOpenFileName(this,"Open .brd file","../","Eagle files(*.brd *.xml *.txt)");
    ui->customPlot->setWindowTitle(file_name);
    ui->customPlot->plotLayout()->removeAt(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot, file_name, QFont("sans", 12, QFont::Bold)));

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
            //delete previuos plot
            ui->customPlot->clearGraphs();
            // create graph and assign data to it:
            ui->customPlot->addGraph();
            ui->customPlot->graph(0)->setData(tsp_.pointX(), tsp_.pointY());
            ui->customPlot->graph(0)->setPen(QColor(0, 0, 0, 255));
            ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
            ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));

            ui->customPlot->addGraph();
            ui->customPlot->graph(1)->setData({tsp_.startPoint().x()}, {tsp_.startPoint().y()});
            ui->customPlot->graph(1)->setPen(QColor(255, 0, 0, 255));
            ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
            ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 12));

            ui->customPlot->replot();

            //making distance matrix
            tempX = tsp_.pointX();
            tempY = tsp_.pointY();
            tempX.push_front(tsp_.startPoint().x()); //add starting point (0,0)
            tempY.push_front(tsp_.startPoint().y());
            size = tempX.size();

            auto& dist = tsp_.dist();
            dist.resize(size);
            for(int i = 0;i < size ; i++){
                dist[i].resize(size);
            }

            //computing distance matrix
            for(int i=0;i<size;i++){
                for(int j=0;j<size;j++){
                    dist[i][j]=qSqrt(qPow(tempX[i]-tempX[j],2)+qPow(tempY[i]-tempY[j],2));
                }
            }
            ui->startBtn->setEnabled(true);
            ui->algorithmBox->setEnabled(true);
        }
    }
}


void MainWindow::on_optionBtn_clicked()
{
    w_options.show();
}


void MainWindow::on_animationBtn_clicked(bool checked)
{

}


void MainWindow::on_saveBtn_clicked()
{
    QString filename="out_";
    filename += ui->finalDistance->text();
    QFile file( filename );
    if ( file.open(QIODevice::WriteOnly) )
    {
        QTextStream stream( &file );
        for(int i = 0 ; i <  tsp_.pointX().size(); i++){
            stream << QString::number(tsp_.pointX()[tsp_.permutation()[i]-1]) + " " +  QString::number(tsp_.pointY()[tsp_.permutation()[i]-1]) << Qt::endl;
        }
        ui->statusbar->setStyleSheet("color: blue");
        ui->statusbar->showMessage("Results saved in " + filename + " file",5000);
        file.close();
    }
    else{
        ui->statusbar->setStyleSheet("color: red");
        ui->statusbar->showMessage("Unable to saved file.",5000);
    }
}


void MainWindow::on_algorithmBox_currentIndexChanged(int index)
{
    if(index == 2){
        ui->optionBtn->setEnabled(true);
        ui->progressBar->setVisible(true);
        ui->progressBar->setFormat("%v/%m");
    }
    else{
        ui->optionBtn->setEnabled(false);
        ui->progressBar->setVisible(false);
    }

    if(index == 3){
        ui->animationBtn->setEnabled(true);
    }
    else{
        ui->animationBtn->setEnabled(false);
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

    auto& X = tsp_.pointX();
    auto& Y = tsp_.pointY();

    for (QDomNode n = elements.firstChildElement(); !n.isNull(); n = n.nextSiblingElement()) {
        atrr = n.toElement();
        library_urn = atrr.attribute("library_urn");
        package = atrr.attribute("package");
        rot = atrr.attribute("rot","R0");
        x = atrr.attribute("x").toDouble();
        y = atrr.attribute("y").toDouble();

        for(QDomNode m = libraries.firstChildElement(); !m.isNull();m = m.nextSiblingElement()){
            atrr = m.toElement();
            if(atrr.attribute("urn")==library_urn){
                for(QDomNode k = atrr.firstChildElement("packages");!k.isNull();k = k.nextSiblingElement()){
                    if(k.nodeName()=="packages"){
                        for(QDomNode i = k.firstChild();!i.isNull();i = i.nextSiblingElement()){
                            if(package == i.toElement().attribute("name")){
                                for(QDomNode j = i.firstChild();!j.isNull();j = j.nextSiblingElement()){
                                    if(j.nodeName()=="pad"){
                                        pads = j.toElement();
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
    QVector<int> permutation;
    double distance = 0;

    //starting algorithm
    bool pointUsed = false;
    double bestDist = 99999999999;
    int bestPoint,i;
    bestPoint = -1;

    size_t n = tsp_.pointX().size();

    auto& dist = tsp_.dist();
    for(int i = 1 ; i <= n ; i++){
        if(bestDist > dist[0][i] ){
            bestPoint=i;
            bestDist = dist[0][i];
        }
    }
    permutation.push_back(bestPoint);

    distance += bestDist;
    bestDist = 9999999999999;

    while(permutation.size()!=n+1){


        for( i = 1; i <= n ;i++){

            //can not pointing on the same element
            for(int j = 0; j <permutation.size();j++){
                if( i == permutation[j]){
                    pointUsed = true;
                }
            }
            if(bestDist > dist[permutation.back()][i] && !pointUsed){
                bestDist = dist[permutation.back()][i];
                bestPoint = i;

            }
            pointUsed = false;

        }
        distance += bestDist;
        bestDist = 99999999999;
        permutation.push_back(bestPoint);

    }
    distance += dist[0][permutation.back()]; // returning route to point starting (0,0)

    tsp_.permutation() = permutation;

    return ComputeDistance();
}

double MainWindow::NP_algorithm()
{
    QVector<int> permutation;

    //starting algorithm
    for(int i=1;i<=tsp_.pointX().size();i++){
        permutation.push_back(i);
    }


    tsp_.permutation() = permutation;

    return ComputeDistance();

}

double MainWindow::SA_algorithm()
{

    double Temperature = w_options.initTemp;
    QRandomGenerator generator;
    int a,b,temp;
    double diff;
    QVector<int> sPermutation,bPermutation,nPermutation;
    if(tsp_.permutation().empty()){
        NN_algorithm(); //base permutation
    }
    sPermutation=bPermutation=nPermutation=tsp_.permutation();

    while(Temperature > w_options.finalTemp){
        nPermutation = sPermutation;
        for(int i = 0; i < w_options.iter ;i++){
            QCoreApplication::processEvents(); //keeping gui responsive

            a = rand() % nPermutation.size();
            b = rand() % nPermutation.size();
            while(a==b){
                b = rand() % nPermutation.size();
            }

            temp = nPermutation[a];
            nPermutation[a]=nPermutation[b];
            nPermutation[b] = temp;

            if(ComputeDistance(nPermutation)<ComputeDistance(bPermutation)){
                bPermutation = nPermutation;
            }
            diff = ComputeDistance(nPermutation) - ComputeDistance(sPermutation);
            if(diff < 0){
                sPermutation = nPermutation;
            }
            else{

                if(rand() < qExp((-diff)/Temperature)){
                    sPermutation = nPermutation;
                }
            }
        }
        Temperature *= w_options.alpha;
    }
    tsp_.permutation() = bPermutation;

    return ComputeDistance();
}

double MainWindow::opt2_algorithm()
{
    bool NoImprovement = false;
    double best_distance,new_distance;
    QVector<int> NewPermutation,ExistingPermutation;

    QVector<int>& permutation = tsp_.permutation();

    if(permutation.empty()){
        NN_algorithm();
    }

    ExistingPermutation = permutation;

    while(!NoImprovement){
        NoImprovement = true;
        best_distance = ComputeDistance(ExistingPermutation);
        for(int i=0; i<permutation.size();i++){
            for(int k = i + 1; k <permutation.size()-1;k++){

                NewPermutation = opt2_swap(ExistingPermutation,i,k);
                QCoreApplication::processEvents(); //keeping gui responsive

                new_distance = ComputeDistance(NewPermutation);

                if(ui->animationBtn->isChecked()){
                    DrawPermutation(NewPermutation);
                    ui->finalDistance->setText(QString::number(best_distance));
                }

                if(new_distance < best_distance){
                    ExistingPermutation=NewPermutation;
                    NoImprovement = false;
                }
                if(!NoImprovement)
                    break;
            }
            if(!NoImprovement)
                break;
        }
    }

    permutation = ExistingPermutation;
    return ComputeDistance();
}

QVector<int> MainWindow::opt2_swap(QVector<int> permutation, int a, int b)
{
    int temp;
    QVector<int> NewPermutation;

    if(a > b){
        temp = a;
        a = b;
        b = temp;
    }

    for(int i=0;i<a;i++){
        NewPermutation.push_back(permutation[i]);
    }

    if(b < permutation.size()){
        for(int i = b ; i >= a ; i--){
            NewPermutation.push_back(permutation[i]);
        }
    }
    for(int i = b+1; i < permutation.size();i++){

        NewPermutation.push_back(permutation[i]);
    }

    return NewPermutation;
}

void MainWindow::DrawPermutation()
{
    QVector<double> x(2),y(2);

    QVector<double>& X = tsp_.pointX();
    QVector<double>& Y = tsp_.pointY();

    //clear previous permutation
    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(X, Y);
    ui->customPlot->graph(0)->setPen(QColor(0, 0, 0, 255));
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setData({tsp_.startPoint().x()}, {tsp_.startPoint().y()});
    ui->customPlot->graph(1)->setPen(QColor(255, 0, 0, 255));
    ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 12));


    auto& permutation = tsp_.permutation();
    if(permutation.size()==0)
        return;

    for(int i = 0; i < permutation.size()-1 ;i++){
        x[0] = X[permutation[i]-1]; //vextor X and Y dont have starting point (0,0) so then -1
        x[1] = X[permutation[i+1]-1];
        y[0] = Y[permutation[i]-1];
        y[1] = Y[permutation[i+1]-1];

        ui->customPlot->addGraph();
        ui->customPlot->graph(ui->customPlot->graphCount()-1)->setData(x,y);
    }
    //adding first and last route
    x[0] = 0;
    x[1] = X[permutation[0]-1];
    y[0] = 0;
    y[1] = Y[permutation[0]-1];
    ui->customPlot->addGraph();
    ui->customPlot->graph(ui->customPlot->graphCount()-1)->setData(x,y);

    x[0] = 0;
    x[1] = X[permutation.back()-1];
    y[0] = 0;
    y[1] = Y[permutation.back()-1];
    ui->customPlot->addGraph();
    ui->customPlot->graph(ui->customPlot->graphCount()-1)->setData(x,y);

    ui->customPlot->replot();
}

void MainWindow::DrawPermutation(QVector<int> permutation)
{
    QVector<double> x(2),y(2);

    QVector<double>& X = tsp_.pointX();
    QVector<double>& Y = tsp_.pointY();

    //clear previous permutation
    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(X, Y);
    ui->customPlot->graph(0)->setPen(QColor(0, 0, 0, 255));
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setData({tsp_.startPoint().x()}, {tsp_.startPoint().y()});
    ui->customPlot->graph(1)->setPen(QColor(255, 0, 0, 255));
    ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 12));

    //no permutation
    if(permutation.size()==0)
        return;

    for(int i = 0; i < permutation.size()-1 ;i++){
        x[0] = X[permutation[i]-1]; //vextor X and Y dont have starting point (0,0) so then -1
        x[1] = X[permutation[i+1]-1];
        y[0] = Y[permutation[i]-1];
        y[1] = Y[permutation[i+1]-1];

        ui->customPlot->addGraph();
        ui->customPlot->graph(ui->customPlot->graphCount()-1)->setData(x,y);
    }
    //adding first and last route
    x[0] = 0;
    x[1] = X[permutation[0]-1];
    y[0] = 0;
    y[1] = Y[permutation[0]-1];
    ui->customPlot->addGraph();
    ui->customPlot->graph(ui->customPlot->graphCount()-1)->setData(x,y);

    x[0] = 0;
    x[1] = X[permutation.back()-1];
    y[0] = 0;
    y[1] = Y[permutation.back()-1];
    ui->customPlot->addGraph();
    ui->customPlot->graph(ui->customPlot->graphCount()-1)->setData(x,y);

    ui->customPlot->replot();
}

double MainWindow::ComputeDistance()
{
    double distance = 0;

    for(int i=0;i<tsp_.pointX().size()-1;i++){
        distance += tsp_.dist()[tsp_.permutation()[i]][tsp_.permutation()[i+1]];
    }

    distance+=tsp_.dist()[X.size()][0];

    return distance;
}

double MainWindow::ComputeDistance(QVector<int> permutation)
{

    double distance = 0;

    for(int i=0;i<permutation.size()-1;i++){
        distance += tsp_.dist()[permutation[i]][permutation[i+1]];
    }

    distance+=tsp_.dist()[X.size()][0];

    return distance;
}
