#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    ui->comboBoxAlg->addItem("NP"); //natural permutation
    ui->comboBoxAlg->addItem("NN"); //nearest neighbour
    ui->comboBoxAlg->addItem("SA"); //simulated annealing
    ui->comboBoxAlg->addItem("2-opt");//2-opt algrithm
    ui->optionButton->setEnabled(false);
    ui->progressBar->setVisible(false);
    ui->progressBar->setRange(0,w_options.repeat);
    ui->progressBar->setValue(0);
    ui->progressBar->setFormat("%v/%m");


}

MainWindow::~MainWindow()
{
    if(!X.isEmpty()){
        for(int i=0;i<=X.size();i++){
            delete [] DistanceMatrix[i];
        }
        delete [] DistanceMatrix;
    }

    delete ui;
}



void MainWindow::on_loadFileButton_clicked()
{
    QVector<double> tempX,tempY;
    int size;
    //clearing previuos data
    if(!X.isEmpty()){
        for(int i=0;i<=X.size();i++){
            delete [] DistanceMatrix[i];
        }
        delete [] DistanceMatrix;
    }

    X.clear();
    Y.clear();
    Permutation.clear();

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
            ui->customPlot->graph(0)->setData(X, Y);
            ui->customPlot->graph(0)->setPen(QColor(0, 0, 0, 255));
            ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
            ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));

            ui->customPlot->replot();

            //making distance matrix
            tempX = X;
            tempY = Y;
            tempX.push_front(0); //add starting point (0,0)
            tempY.push_front(0);
            size = tempX.size();

            DistanceMatrix = new double* [size]; //creating distance matrix

            for(int i = 0;i < size ; i++){
                DistanceMatrix[i] = new double [size];
            }

            //computing distance matrix
            for(int i=0;i<size;i++){
                for(int j=0;j<size;j++){
                    DistanceMatrix[i][j]=qSqrt(qPow(tempX[i]-tempX[j],2)+qPow(tempY[i]-tempY[j],2));
                }
            }
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
    //clearing previous points
    X.clear();
    Y.clear();

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

    QVector<int> permutation;


    double distance = 0;



    //starting algorithm
    bool pointUsed = false;
    double bestDist = 99999999999;
    int bestPoint,i;
    bestPoint = -1;
    // permutation.push_back(0);//starting point is always at (0,0)

    //first nearest hole
    for(int i = 1 ; i <= X.size() ; i++){
        if(bestDist > DistanceMatrix[0][i] ){
            bestPoint=i;
            bestDist = DistanceMatrix[0][i];
        }
    }
    permutation.push_back(bestPoint);

    distance += bestDist;
    bestDist = 9999999999999;

    while(permutation.size()!=X.size()+1){


        for( i = 1; i <= X.size() ;i++){

            //can not pointing on the same element
            for(int j = 0; j <permutation.size();j++){
                if( i == permutation[j]){
                    pointUsed = true;
                }
            }
            if(bestDist > DistanceMatrix[permutation.back()][i] && !pointUsed){
                bestDist = DistanceMatrix[permutation.back()][i];
                bestPoint = i;

            }
            pointUsed = false;

        }
        distance += bestDist;
        bestDist = 99999999999;
        permutation.push_back(bestPoint);

    }
    distance += DistanceMatrix[0][permutation.back()]; // returning route to point starting (0,0)


    Permutation = permutation;

    return ComputeDistance();
}

double MainWindow::NP_algorithm()
{
    QVector<int> permutation;

    //starting algorithm
    for(int i=1;i<=X.size();i++){
        permutation.push_back(i);
    }


    Permutation = permutation;

    return ComputeDistance();

}

double MainWindow::SA_algorithm()
{

    double Temperature = w_options.initTemp;
    QRandomGenerator generator;
    int a,b,temp;
    double diff;
    QVector<int> sPermutation,bPermutation,nPermutation;
    if(Permutation.empty()){
        NN_algorithm(); //base permutation
    }
    sPermutation=bPermutation=nPermutation=Permutation;

    while(Temperature > w_options.finalTemp){
        nPermutation = sPermutation;
        for(int i = 0; i < w_options.iter ;i++){

            //generating two random numbers
            a = rand() % nPermutation.size();
            b = rand() % nPermutation.size();
            while(a==b){
                b = rand() % nPermutation.size(); // a and b shouldnt be the same
            }
            // qDebug()<<QString::number(a) + " " + QString::number(b);
            //making similar but different solution
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
        // qDebug()<<QString::number(Temperature);
    }
    Permutation = bPermutation;


    return ComputeDistance();
}

double MainWindow::opt2_algorithm()
{
    bool NoImprovement = false;
    double best_distance,new_distance;
    QVector<int> NewPermutation,ExistingPermutation;


    if(Permutation.empty()){
        NN_algorithm();
    }

    ExistingPermutation = Permutation;

    while(!NoImprovement){
        NoImprovement = true;
        best_distance = ComputeDistance(ExistingPermutation);
        for(int i=0; i<Permutation.size();i++){
            for(int k = i + 1; k <Permutation.size()-1;k++){

                NewPermutation = opt2_swap(ExistingPermutation,i,k);

               new_distance = ComputeDistance(NewPermutation);
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

    Permutation = ExistingPermutation;
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

    //clear previous permutation
    ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(X, Y);
    ui->customPlot->graph(0)->setPen(QColor(0, 0, 0, 255));
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));


    //no permutation
    if(Permutation.size()==0)
        return;

    for(int i = 0; i < Permutation.size()-1 ;i++){
        x[0] = X[Permutation[i]-1]; //vextor X and Y dont have starting point (0,0) so then -1
        x[1] = X[Permutation[i+1]-1];
        y[0] = Y[Permutation[i]-1];
        y[1] = Y[Permutation[i+1]-1];

        ui->customPlot->addGraph();
        ui->customPlot->graph(ui->customPlot->graphCount()-1)->setData(x,y);
    }
    //adding first and last route
    x[0] = 0;
    x[1] = X[Permutation[0]-1];
    y[0] = 0;
    y[1] = Y[Permutation[0]-1];
    ui->customPlot->addGraph();
    ui->customPlot->graph(ui->customPlot->graphCount()-1)->setData(x,y);

    x[0] = 0;
    x[1] = X[Permutation.back()-1];
    y[0] = 0;
    y[1] = Y[Permutation.back()-1];
    ui->customPlot->addGraph();
    ui->customPlot->graph(ui->customPlot->graphCount()-1)->setData(x,y);


    //ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}

double MainWindow::ComputeDistance()
{

    double distance = 0;



    for(int i=0;i<Permutation.size()-1;i++){
        distance += DistanceMatrix[Permutation[i]][Permutation[i+1]];
    }

    distance+=DistanceMatrix[X.size()][0];



    return distance;
}

double MainWindow::ComputeDistance(QVector<int> permutation)
{

    double distance = 0;



    for(int i=0;i<permutation.size()-1;i++){
        distance += DistanceMatrix[permutation[i]][permutation[i+1]];
    }

    distance+=DistanceMatrix[X.size()][0];


    return distance;
}

void MainWindow::on_startButton_clicked()
{

    switch (ui->comboBoxAlg->currentIndex()) {
    case 0 :
        ui->lineDistance->setText(QString::number(NP_algorithm()));
        DrawPermutation();
        break;
    case 1 :
        ui->lineDistance->setText(QString::number(NN_algorithm()));
        DrawPermutation();
        break;
    case 2 :
        ui->progressBar->setRange(0,w_options.repeat);

        for(int i = 0 ; i < w_options.repeat;i++){
            ui->progressBar->setFormat("%v/%m");
            ui->progressBar->setValue(i+1);
            ui->lineDistance->setText(QString::number(SA_algorithm()));
            DrawPermutation();
        }
        ui->progressBar->setFormat("Complete!!!");
        break;
    case 3 :
        ui->lineDistance->setText(QString::number(opt2_algorithm()));
        DrawPermutation();
        break;
    }

}

void MainWindow::on_optionButton_clicked()
{
    w_options.show();
}

void MainWindow::on_comboBoxAlg_currentIndexChanged(int index)
{
    if(index == 2){
        ui->optionButton->setEnabled(true);
        ui->progressBar->setVisible(true);
        ui->progressBar->setFormat("%v/%m");
    }
    else{
        ui->optionButton->setEnabled(false);
        ui->progressBar->setVisible(false);
    }
}

//double MainWindow::NN_algorithm()
//{
//    QVector<double> XP,YP;
//    QVector<int> permutation;
//    XP = X;
//    YP = Y;
//    XP.push_front(0);//adding starting point (0,0)
//    YP.push_front(0);

//    int size = XP.size();
//    double distance = 0;

//    double ** distMat = new double* [size]; //creating distance matrix

//    for(int i = 0;i < size ; i++){
//        distMat[i] = new double [size];
//    }

//    //computing distance matrix
//    for(int i=0;i<size;i++){
//        for(int j=0;j<size;j++){
//            distMat[i][j]=qSqrt(qPow(XP[i]-XP[j],2)+qPow(YP[i]-YP[j],2));
//        }
//    }

//    //starting algorithm
//    bool pointUsed = false;
//    double bestDist = 99999999999;
//    int bestPoint,i;
//    bestPoint = -1;
//    // permutation.push_back(0);//starting point is always at (0,0)

//    //first nearest hole
//    for(int i = 1 ; i < size ; i++){
//        if(bestDist > distMat[0][i] ){
//            bestPoint=i;
//            bestDist = distMat[0][i];
//        }
//    }
//    permutation.push_back(bestPoint);

//    distance += bestDist;
//    bestDist = 9999999999999;

//    while(permutation.size()!=size){


//        for( i = 1; i < size ;i++){

//            //can not pointing on the same element
//            for(int j = 0; j <permutation.size();j++){
//                if( i == permutation[j]){
//                    pointUsed = true;
//                }
//            }
//            if(bestDist > distMat[permutation.back()][i] && !pointUsed){
//                bestDist = distMat[permutation.back()][i];
//                bestPoint = i;

//            }
//            pointUsed = false;

//        }
//        distance += bestDist;
//        bestDist = 99999999999;
//        permutation.push_back(bestPoint);

//    }
//    distance += distMat[0][permutation.back()]; // returning route to point starting (0,0)


//    for(int i = 0;i < size ; i++){ //clearing distance matrix
//        delete [] distMat[i];
//    }
//    delete [] distMat;

//    Permutation = permutation;

//    return ComputeDistance();
//}
