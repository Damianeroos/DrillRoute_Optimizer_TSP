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

private:
    Ui::MainWindow *ui;
    QString file_name;
    QFile file;
    QDomDocument xml_file;

    QVector<double> X,Y;
    QVector<int> Permutation;
    //returns  0 if there is no holes
    int ReadHolesPosition(void);
    double NN_algorithm(void);
    double NP_algorithm(void);
    double SA_algorithm(void);
    void DrawPermutation(void);
    double ComputeDistance(void);
    double ComputeDistance(QVector<int> permutation);

    SA_options w_options;
};



#endif // MAINWINDOW_H
