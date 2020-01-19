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


    QSqlQueryModel *model = new QSqlQueryModel();


    QSqlQuery* qry = new QSqlQuery(mydb);

    qry->prepare("Select title FROM bazadanych");

    qry->exec();
    model->setQuery(*qry);

    ui->tableView->setModel(model);


    //autocompleter
    QStringList CompletionList;
    int counter = 0;

    if (qry->exec())
    {
        while (qry->next()) {
            CompletionList << qry->value(0).toString();
                counter++;
        }

    }

    qDebug()<<(counter);

    StringCompleter = new QCompleter(CompletionList, this);
    StringCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_2->setCompleter(StringCompleter);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

    QString Item = ui->lineEdit_2->text();
    float Masa = ui->lineEdit->text().toFloat();
    //qDebug()<<(Masa);

    QSqlQuery query;
    query.prepare( "SELECT * FROM bazadanych WHERE title= '"+Item+"'");

    float Kcal, Bialko, Tluszcze, Wegle;

    if (query.exec())
    {
        while(query.next())
        {

            Kcal = (query.value(1).toFloat())*Masa/100;
            Bialko = (query.value(2).toFloat())*Masa/100;
            Tluszcze = (query.value(3).toFloat())*Masa/100;
            Wegle = (query.value(4).toFloat())*Masa/100;

            ui->kcal->setNum(Kcal);
            ui->bialo->setNum(Bialko);
            ui->tluszcze->setNum(Tluszcze);
            ui->wegle->setNum(Wegle);
        }


    }

}
