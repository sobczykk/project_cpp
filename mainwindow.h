#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QtDebug>
#include<QFileInfo>
#include <QListView>
#include <QComboBox>
#include <QCompleter>
#include <QFileSystemModel>
#include<QLineEdit>
#include<QWidget>
#include<iostream>
#include<string>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QSqlDatabase mydb;
    QSqlDatabase db_days;


    void connClose()
    {
        mydb.close();
        mydb.removeDatabase(QSqlDatabase::defaultConnection);
    }

    bool connOpen()
    {
        mydb = QSqlDatabase ::addDatabase("QSQLITE");
        mydb.setDatabaseName("/home/kamila/fit/bazadanych.db");

        if(!mydb.open()){
             qDebug()<<("Failed to open the database");
             return false;
        }
        else{
             qDebug()<<("Connected");
             return true;
             }
    }

    void show_table(QString posilek, QString wybrany_dzien);
    void suma(QString wybrany_dzien);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_pushButton_clicked();

    void on_calendarWidget_clicked();

private:
    Ui::MainWindow *ui;

    QCompleter *StringCompleter;

};

#endif // MAINWINDOW_H
