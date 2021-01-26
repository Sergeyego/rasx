#include "mainwidget.h"
#include <QApplication>
#include "db/dblogin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/ico.ico"));

    QPixmap logo(":/images/simb_labl.png");
    DbLogin d(QObject::tr("Расход компонентов по упаковке"),logo);
    if (d.exec()!=QDialog::Accepted) exit(1);

    MainWidget w;
    w.show();

    return a.exec();
}
