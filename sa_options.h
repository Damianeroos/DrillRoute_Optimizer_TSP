#ifndef SA_OPTIONS_H
#define SA_OPTIONS_H

#include <QWidget>

namespace Ui {
class SA_options;
}

class SA_options : public QWidget
{
    Q_OBJECT



public:
    explicit SA_options(QWidget *parent = nullptr);
    ~SA_options();

    double initTemp,finalTemp,alpha;
    int iter,repeat;

private slots:
    void on_itBox_valueChanged(int arg1);

    void on_alphaBox_valueChanged(double arg1);

    void on_finalTempBox_valueChanged(double arg1);

    void on_initTempBox_valueChanged(double arg1);

    void on_initTempBox_editingFinished();

    void on_finalTempBox_editingFinished();

    void on_rpBox_valueChanged(int arg1);

private:
    Ui::SA_options *ui;


};

#endif // SA_OPTIONS_H
