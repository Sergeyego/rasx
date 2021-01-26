#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "modelro.h"

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

private slots:
    void upd();
    void calc_one();
    void calc_all();
};

#endif // MAINWIDGET_H
