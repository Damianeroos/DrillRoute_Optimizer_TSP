
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QtXml>

#include "sa_options.h"
#include "tsp_problem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startBtn_clicked();

    void on_loadBtn_clicked();

    void on_optionBtn_clicked();

    void on_animationBtn_clicked(bool checked);

    void on_saveBtn_clicked();

    void on_algorithmBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    SA_options w_options;

    TspProblem tsp_;

    QString file_name;
    QFile file;
    QDomDocument xml_file;
    double **DistanceMatrix;

    QVector<double> X,Y;
    QVector<int> Permutation;
    //returns  0 if there is no holes
    int ReadHolesPosition(void);
    double NN_algorithm(void);
    double NP_algorithm(void);
    double SA_algorithm(void);
    double opt2_algorithm(void);
    QVector<int> opt2_swap(QVector<int> permutation,int a,int b);
    void DrawPermutation(void);
    void DrawPermutation(QVector<int> permutation);
    double ComputeDistance(void);
    double ComputeDistance(QVector<int> permutation);
};

#endif // MAINWINDOW_H
