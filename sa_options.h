#ifndef SA_OPTIONS_H
#define SA_OPTIONS_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Sa_options; }
QT_END_NAMESPACE

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
    Ui::Sa_options *ui;


};

#endif // SA_OPTIONS_H
