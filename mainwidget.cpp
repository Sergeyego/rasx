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

void MainWidget::upd()
{
    QSqlQuery query;
    query.prepare("select p.n_s, p.id, e.marka, p.diam, kvos.upack, kvos.brk, kvos.loss,kvos.garb, kvos.zames,p.dat_part "
                  "from "
                      "(select parts.id_part, coalesce(pack.kvo,0.0) as upack, coalesce(break.kvo,0.0)  as brk, "
                      "coalesce(prod.loss,0.0) as loss,coalesce(garbage.garb,0.0) as garb, coalesce(zam.zames,0.0) as zames "
                      "from"
                      "("
                       "   select distinct id_part from "
                       "   ( "
                       "   (select id_part from parti_pack pk inner join parti_nakl nk on nk.id=pk.id_nakl "
                       "   where nk.tip=1 and nk.dat between '2020-12-01' and '2020-12-31') "
                       "   union "
                          "(select id_part from parti_break br inner join parti_nakl nk on nk.id=br.id_nakl "
                          "where nk.tip=2 and nk.dat between '2020-12-01' and '2020-12-31') "
                          "union "
                          "(select id_part from part_prod where dat between '2020-12-01' and '2020-12-31') "
                          "union "
                          "(select id_part from parti_zam_break where dat between '2020-12-01' and '2020-12-31') "
                        ") as pids "
                        "where id_part>0 order by id_part "
                        ") as parts "
                        "left join "
                        "(select id_part, sum(kvo) as kvo from parti_pack pk inner join  parti_nakl nk on nk.id=pk.id_nakl "
                        "where nk.tip=1 and nk.dat between '2020-12-01' and '2020-12-31' "
                        "group by id_part) as pack on pack.id_part=parts.id_part "
                        "left join "
                        "(select id_part, sum(kvo) as kvo from parti_break br inner join parti_nakl nk on nk.id=br.id_nakl "
                        "where nk.tip=2 and nk.dat between '2020-12-01' and '2020-12-31' "
                        "group by id_part) as break on break.id_part=parts.id_part "
                        "left join "
                        "(select id_part, sum(loss) as loss from part_prod "
                        "where dat between '2020-12-01' and '2020-12-31' "
                        "group by id_part) as prod on prod.id_part=parts.id_part "
                        "left join "
                        "(select id_part, sum(garb) as garb from part_prod "
                        "where dat between '2020-12-01' and '2020-12-31' "
                        "group by id_part) as garbage on garbage.id_part=parts.id_part "
                        "left join "
                        "(select id_part, sum(kvo) as zames from parti_zam_break "
                        "where dat between '2020-12-01' and '2020-12-31' "
                        "group by id_part) as zam on zam.id_part=parts.id_part "
                  ") as kvos "
                  "inner join parti p on p.id=kvos.id_part "
                  "inner join elrtr e on e.id=p.id_el "
                  "order by n_s ");
    if (modelPart->execQuery(query)){

    }
}

void MainWidget::calc_one()
{

}

void MainWidget::calc_all()
{

}
