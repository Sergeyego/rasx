#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "modelro.h"
#include <QProgressDialog>
#include "olap/cubewidget.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
    ModelRo *modelPart;
    bool addRasx(int row);

private slots:
    void upd();
    void calc_one();
    void calc_all();
    void calc_sum();
    void clear_rasx();
    void showReport();
    void save();
};

#endif // MAINWIDGET_H
