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


    QSqlQuery* qry = new QSqlQuery(mydb);

    qry->prepare("Select title FROM bazadanych");


    //autocompleter
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

    QSqlQuery qry2, qry3, qry4;

    //tabela do zapisu posilkow
    QString tablename = QDate::currentDate().toString("dd_MM_yyyy");
    QString query = "CREATE TABLE IF NOT EXISTS '"+tablename+"'(item_id INTEGER UNIQUE PRIMARY KEY AUTOINCREMENT, posilek TEXT, produkt TEXT, kalorie REAL, bial REAL, tlusz REAL, wegle REAL)";

    if(!qry2.exec(query))
        qDebug() << "error creating table";
    else
    {
        qDebug() << "Table created!";
    }

    //tabela z sumami
    QString tablename2 = QDate::currentDate().toString("suma_dd_MM_yyyy");
    QString query2 = "CREATE TABLE IF NOT EXISTS '"+tablename2+"'(id INTEGER UNIQUE PRIMARY KEY, posilek TEXT, kalorie REAL, bial REAL, tlusz REAL, wegle REAL)";


    if(!qry3.exec(query2))
        qDebug() << "error creating table(sum)";
    else
    {
        qDebug() << "Table(sum) created!";
    }

    //tabela z dniami
    QString tablename3 = "dni";
    QString query3 = "CREATE TABLE IF NOT EXISTS '"+tablename3+"'(id INTEGER UNIQUE PRIMARY KEY, kalorie REAL, bial REAL, tlusz REAL, wegle REAL)";


    if(!qry4.exec(query3))
        qDebug() << "error creating table(dni)";
    else
    {
        qDebug() << "Table(dni) created!";
    }


    show_table("Sniadanie", tablename);
    show_table("Obiad", tablename);
    show_table("Kolacja", tablename);

    suma(tablename, tablename2, tablename3);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show_table(QString posilek, QString nazwa_tablicy)
{

    QSqlQuery* qry = new QSqlQuery();
    QSqlQueryModel *model = new QSqlQueryModel();
    qry->prepare("SELECT produkt FROM'"+nazwa_tablicy+"'WHERE posilek IN ('"+posilek+"')");
    qry->exec();
    model->setQuery(*qry);
    if (posilek == "Sniadanie")
        ui->breakfast->setModel(model);
    if (posilek == "Obiad")
         ui->lunch->setModel(model);
    if (posilek == "Kolacja")
         ui->dinner->setModel(model);

}


void MainWindow::suma(QString nazwa_tabeli, QString nazwa_tabeli2, QString nazwa_tabeli3)
{
    QSqlQuery* qry = new QSqlQuery();
    QSqlQueryModel *model = new QSqlQueryModel();
    qry->prepare("SELECT posilek, SUM(kalorie), SUM(bial), SUM(tlusz), SUM(wegle)  FROM '"+nazwa_tabeli+"' GROUP BY posilek ORDER BY posilek DESC");
    qry->exec();
    model->setQuery(*qry);
    ui->tableView->setModel(model);


    QSqlQuery query;
    QString posilek;
    float kcal, bialko, tluszcze, wegle;

     if (qry->exec())
     {
         int counter = 1;
         while(qry->next())
         {
             posilek =  qry->value(0).toString();
             kcal = qry->value(1).toFloat();
             bialko = qry->value(2).toFloat();
             tluszcze = qry->value(3).toFloat();
             wegle = qry->value(4).toFloat();

             query.prepare("INSERT OR REPLACE INTO'"+nazwa_tabeli2+"'("
                           "id,"
                           "posilek,"
                           "kalorie,"
                           "bial,"
                           "tlusz,"
                           "wegle)"
                           "VALUES (?,?, ?, ?, ?, ?);");


             query.addBindValue(counter);
             query.addBindValue(posilek);
             query.addBindValue(kcal);
             query.addBindValue(bialko);
             query.addBindValue(tluszcze);
             query.addBindValue(wegle);

             if(!query.exec())
                 qDebug() << "dasz rade";
             counter ++;
         }
     }

     QSqlQuery qry2;


     QSqlQuery* qry1 = new QSqlQuery();
     QSqlQueryModel *model1 = new QSqlQueryModel();

     qry1->prepare("SELECT SUM(kalorie), SUM(bial), SUM(tlusz), SUM(wegle)  FROM '"+nazwa_tabeli2+"'");
     qry1->exec();
     model1->setQuery(*qry1);
     ui->tableview1->setModel(model1);


     if (qry1->exec())
     {
         while(qry1->next())
         {
             qry2.prepare("INSERT OR REPLACE INTO'"+nazwa_tabeli3+"'("
                                                                   "id,"
                                                                  "kalorie,"
                                                                  "bial,"
                                                                  "tlusz,"
                                                                  "wegle)"
                                                                  "VALUES (?, ?, ?, ?, ?);");

                kcal = qry1->value(0).toFloat();
                bialko = qry1->value(1).toFloat();
                tluszcze = qry1->value(2).toFloat();
                wegle = qry1->value(3).toFloat();

                qry2.addBindValue(1);
                qry2.addBindValue(kcal);
                qry2.addBindValue(bialko);
                qry2.addBindValue(tluszcze);
                qry2.addBindValue(wegle);
         }
     }



     if(!qry2.exec())
         qDebug() << "dasz rade";


}



void MainWindow::on_pushButton_clicked()
{

    QString item = ui->lineEdit_2->text();
    float masa = ui->lineEdit->text().toFloat();
    QString meal = ui->comboBox_meal->currentText();
    QString tablename = QDate::currentDate().toString("dd_MM_yyyy");
    QString tablename2 = QDate::currentDate().toString("suma_dd_MM_yyyy");
    QString tablename3 = "dni";

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

            qry.prepare("INSERT INTO'"+tablename+"'("
                          "posilek,"
                          "produkt,"
                          "kalorie,"
                          "bial,"
                          "tlusz,"
                          "wegle)"
                          "VALUES (?, ?, ?, ?, ?, ?);");


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

    show_table("Sniadanie", tablename);
    show_table("Obiad", tablename);
    show_table("Kolacja", tablename);

    suma(tablename, tablename2, tablename3);



}
