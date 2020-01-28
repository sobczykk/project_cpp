#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcompleter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connOpen();

    if(!mydb.open())
        ui->label->setText("Failed to open the database");
    else
        ui->label->setText("Connected");


    //autocompleter
    QSqlQuery* qry = new QSqlQuery(mydb);
    qry->prepare("Select title FROM bazadanych");
    QStringList CompletionList;

    if (qry->exec())
    {
        while (qry->next()) {
            CompletionList << qry->value(0).toString();
        }

    }

    StringCompleter = new QCompleter(CompletionList, this);
    StringCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_2->setCompleter(StringCompleter);


    //combobox z posilkami do wyboru
    ui->comboBox_meal->insertItems(0, QStringList() << "Sniadanie" << "Obiad" << "Kolacja");



    //tabela do zapisu posilkow
    QSqlQuery qry2;
    QString query = "CREATE TABLE IF NOT EXISTS tablica_produkty(item_id INTEGER UNIQUE PRIMARY KEY AUTOINCREMENT, data TEXT, posilek TEXT, produkt TEXT, kalorie REAL, bial REAL, tlusz REAL, wegle REAL)";

    if(!qry2.exec(query))
        qDebug() << "error creating table";
    else
    {
        qDebug() << "Table created!";
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show_table(QString posilek, QString wybrany_dzien)
{

    QSqlQuery* qry = new QSqlQuery();
    QSqlQueryModel *model = new QSqlQueryModel();
    qry->prepare("SELECT produkt FROM tablica_produkty WHERE (posilek IN ('"+posilek+"')) AND (data='"+wybrany_dzien+"')");

    qry->exec();
    model->setQuery(*qry);
    if (posilek == "Sniadanie")
        ui->breakfast->setModel(model);
    if (posilek == "Obiad")
         ui->lunch->setModel(model);
    if (posilek == "Kolacja")
         ui->dinner->setModel(model);
    if(!qry->exec())
        qDebug() << "error";

}


void MainWindow::suma(QString wybrany_dzien)
{
    QSqlQuery* qry = new QSqlQuery();
    QSqlQueryModel *model = new QSqlQueryModel();
    qry->prepare("SELECT posilek, SUM(kalorie), SUM(bial), SUM(tlusz), SUM(wegle) FROM tablica_produkty WHERE data='"+wybrany_dzien+"' GROUP BY posilek ORDER BY posilek DESC");
    qry->exec();
    model->setQuery(*qry);
    ui->tableView->setModel(model);


     QSqlQuery* qry1 = new QSqlQuery();
     QSqlQueryModel *model1 = new QSqlQueryModel();

     qry1->prepare("SELECT data, SUM(kalorie), SUM(bial), SUM(tlusz), SUM(wegle) FROM tablica_produkty GROUP BY data ORDER BY data");
     qry1->exec();
     model1->setQuery(*qry1);
     ui->tableview1->setModel(model1);

}

void MainWindow::on_calendarWidget_clicked()
{
    QString data = ui->calendarWidget->selectedDate().toString("yyyy-MM-dd");
    show_table("Sniadanie", data);
    show_table("Obiad", data);
    show_table("Kolacja", data);
    suma(data);
}



void MainWindow::on_pushButton_clicked()
{

    QString item = ui->lineEdit_2->text();
    float masa = ui->lineEdit->text().toFloat();
    QString meal = ui->comboBox_meal->currentText();
    QString wybrany_dzien = ui->calendarWidget->selectedDate().toString("yyyy-MM-dd");
    qDebug() << wybrany_dzien;


    QSqlQuery query, qry;
    query.prepare( "SELECT * FROM bazadanych WHERE title= '"+item+"'");

    float kcal, bialko, tluszcze, wegle;

    if (query.exec())
    {
        while(query.next())
        {

            kcal = (query.value(1).toFloat())*masa/100;
            bialko = (query.value(2).toFloat())*masa/100;
            tluszcze = (query.value(3).toFloat())*masa/100;
            wegle = (query.value(4).toFloat())*masa/100;

            ui->kcal->setNum(kcal);
            ui->bialo->setNum(bialko);
            ui->tluszcze->setNum(tluszcze);
            ui->wegle->setNum(wegle);

            qry.prepare("INSERT INTO tablica_produkty("
                          "data,"
                          "posilek,"
                          "produkt,"
                          "kalorie,"
                          "bial,"
                          "tlusz,"
                          "wegle)"
                          "VALUES (?, ?, ?, ?, ?, ?, ?);");

            qry.addBindValue(wybrany_dzien);
            qry.addBindValue(meal);
            qry.addBindValue(item);
            qry.addBindValue(kcal);
            qry.addBindValue(bialko);
            qry.addBindValue(tluszcze);
            qry.addBindValue(wegle);

            if(!qry.exec())
                qDebug() << "error adding values";

        }

    }

    show_table("Sniadanie", wybrany_dzien);
    show_table("Obiad", wybrany_dzien);
    show_table("Kolacja", wybrany_dzien);
    suma(wybrany_dzien);



}




