#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelPart = new ModelRo(this);
    ui->tableViewPart->setModel(modelPart);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->pushButtonOne,SIGNAL(clicked(bool)),this,SLOT(calc_one()));
    connect(ui->pushButtonAll,SIGNAL(clicked(bool)),this,SLOT(calc_all()));
}

MainWidget::~MainWidget()
{
    delete ui;
}

bool MainWidget::addRasx(int row)
{
    QSqlQuery query;
    query.prepare("select * from rasxod.proc_report(:id, :prod, :break, :wast, :grab, :dry)");
    query.bindValue(":id",modelPart->data(modelPart->index(row,0),Qt::EditRole).toInt());
    query.bindValue(":prod",modelPart->data(modelPart->index(row,5),Qt::EditRole).toDouble());
    query.bindValue(":break",modelPart->data(modelPart->index(row,6),Qt::EditRole).toDouble());
    query.bindValue(":wast",modelPart->data(modelPart->index(row,7),Qt::EditRole).toDouble());
    query.bindValue(":grab",modelPart->data(modelPart->index(row,8),Qt::EditRole).toDouble());
    query.bindValue(":dry",modelPart->data(modelPart->index(row,9),Qt::EditRole).toDouble());
    bool ok=query.exec();
    if (!ok){
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    return ok;
}

void MainWidget::upd()
{
    QString qu=QString("select p.id, p.n_s, p.dat_part, e.marka, p.diam, kvos.upack, kvos.brk, kvos.loss, kvos.garb, kvos.zames "
                       "from "
                       "(select parts.id_part, coalesce(pack.kvo,0.0) as upack, coalesce(break.kvo,0.0)  as brk, "
                       "coalesce(prod.loss,0.0) as loss,coalesce(garbage.garb,0.0) as garb, coalesce(zam.zames,0.0) as zames "
                       "from "
                       "( "
                       "select distinct id_part from "
                       "( "
                       "(select id_part from parti_pack pk inner join parti_nakl nk on nk.id=pk.id_nakl "
                       "where nk.tip=1 and nk.dat between :d1 and :d2) "
                       "union "
                       "(select id_part from parti_break br inner join parti_nakl nk on nk.id=br.id_nakl "
                       "where nk.tip=2 and nk.dat between :d1 and :d2) "
                       "union "
                       "(select id_part from part_prod where dat between :d1 and :d2) "
                       "union "
                       "(select id_part from parti_zam_break where dat between :d1 and :d2) "
                       ") as pids "
                       "where id_part>0 order by id_part "
                       ") as parts "
                       "left join "
                       "(select id_part, sum(kvo) as kvo from parti_pack pk inner join  parti_nakl nk on nk.id=pk.id_nakl "
                       "where nk.tip=1 and nk.dat between :d1 and :d2 "
                       "group by id_part) as pack on pack.id_part=parts.id_part "
                       "left join "
                       "(select id_part, sum(kvo) as kvo from parti_break br inner join parti_nakl nk on nk.id=br.id_nakl "
                       "where nk.tip=2 and nk.dat between :d1 and :d2 "
                       "group by id_part) as break on break.id_part=parts.id_part "
                       "left join "
                       "(select id_part, sum(loss) as loss from part_prod "
                       "where dat between :d1 and :d2 "
                       "group by id_part) as prod on prod.id_part=parts.id_part "
                       "left join "
                       "(select id_part, sum(garb) as garb from part_prod "
                       "where dat between :d1 and :d2 "
                       "group by id_part) as garbage on garbage.id_part=parts.id_part "
                       "left join "
                       "(select id_part, sum(kvo) as zames from parti_zam_break "
                       "where dat between :d1 and :d2 "
                       "group by id_part) as zam on zam.id_part=parts.id_part "
                       ") as kvos "
                       "inner join parti p on p.id=kvos.id_part "
                       "inner join elrtr e on e.id=p.id_el "
                       "order by n_s ");
    qu.replace(":d1","'"+ui->dateEditBeg->date().toString("yyyy-MM-dd")+"'");
    qu.replace(":d2","'"+ui->dateEditEnd->date().toString("yyyy-MM-dd")+"'");
    QSqlQuery query;
    query.prepare(qu);
    if (modelPart->execQuery(query)){
        modelPart->setHeaderData(1,Qt::Horizontal,tr("Партия"));
        modelPart->setHeaderData(2,Qt::Horizontal,tr("Дата"));
        modelPart->setHeaderData(3,Qt::Horizontal,tr("Марка"));
        modelPart->setHeaderData(4,Qt::Horizontal,tr("ф"));
        modelPart->setHeaderData(5,Qt::Horizontal,tr("Упаков."));
        modelPart->setHeaderData(6,Qt::Horizontal,tr("В брак"));
        modelPart->setHeaderData(7,Qt::Horizontal,tr("Потери прес."));
        modelPart->setHeaderData(8,Qt::Horizontal,tr("Отх. мусор"));
        modelPart->setHeaderData(9,Qt::Horizontal,tr("Шихта брак"));
        ui->tableViewPart->setColumnHidden(0,true);
        ui->tableViewPart->resizeToContents();
    }
    calc_sum();
}

void MainWidget::calc_one()
{
    clear_rasx();
    bool ok=true;
    for (int i=0; i<ui->tableViewPart->model()->rowCount();i++){
        if (ui->tableViewPart->selectionModel()->isRowSelected(i,QModelIndex())){
            ok = ok && addRasx(i);
            if (!ok){
                break;
            }
        }
    }
    if (ok && ui->tableViewPart->model()->rowCount()){
        showReport();
    }
}

void MainWidget::calc_all()
{
    clear_rasx();
    bool ok=true;
    for (int i=0; i<modelPart->rowCount(); i++){
        ok = ok && addRasx(i);
        if (!ok){
            break;
        }
    }
    if (ok && modelPart->rowCount()){
        showReport();
    }
}

void MainWidget::calc_sum()
{
    double sum=0;
    for (int i=0; i<modelPart->rowCount();i++){
        sum+=modelPart->data(modelPart->index(i,5),Qt::EditRole).toDouble();
    }
    ui->labelSum->setText(tr("Упаковано за период ")+QLocale().toString(sum,'f',1)+tr(" кг"));
}

void MainWidget::clear_rasx()
{
    QSqlQuery query;
    query.prepare("delete from rasxod.report_new");
    if (!query.exec()){
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
    query.prepare("delete from rasxod.part_diff");
    if (!query.exec()){
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

void MainWidget::showReport()
{

}
