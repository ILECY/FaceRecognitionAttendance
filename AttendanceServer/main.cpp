#include "attendancewin.h"
#include "registerwin.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <opencv.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<cv::Mat>("cv::Mat&");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<int64_t>("int64_t");


    //连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("server.db");
    if(!db.open()){
        qDebug() << db.lastError().text();
        return -1;
    }
    QString createsql = "create table if not exists employee(employeeID integer primary key autoincrement,name varchar(256), sex varchar(32),"
                        "birthday text, address text, phone text, faceID integer unique, headfile text)";
    QSqlQuery query;
    if(!query.exec(createsql))
    {
       qDebug()<<query.lastError().text();
       return -1;
    }

    //考勤表格
    createsql = "create table if not exists attendance(attendaceID integer primary key autoincrement, employeeID integer,"
                "attendaceTime TimeStamp NOT NULL DEFAULT(datetime('now','localtime')))";
    if(!query.exec(createsql))
    {
       qDebug()<<query.lastError().text();
       return -1;
    }


    AttendanceWin w;
    w.show();
    return a.exec();
}
