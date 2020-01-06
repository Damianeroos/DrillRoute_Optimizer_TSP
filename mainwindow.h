#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QtXml>
#include <QDebug>
#include <QVector>
#include <QtMath>
#include <QFileDialog>
#include <sa_options.h>
#include <QRandomGenerator>

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
    void on_loadFileButton_clicked();

    void on_startButton_clicked();

    void on_optionButton_clicked();

    void on_comboBoxAlg_currentIndexChanged(int index);

    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;
    SA_options w_options;

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
