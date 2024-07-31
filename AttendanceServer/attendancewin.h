#ifndef ATTENDANCEWIN_H
#define ATTENDANCEWIN_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <opencv.hpp>
#include <qfaceobject.h>
#include <QSqlTableModel>
#include <QSqlRecord>
#include "registerwin.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AttendanceWin; }
QT_END_NAMESPACE

class AttendanceWin : public QMainWindow
{
    Q_OBJECT

public:
    AttendanceWin(QWidget *parent = nullptr);
    ~AttendanceWin();
signals:
    void query(cv::Mat &faceimage);
protected slots:
    void accept_client();
    void read_data();
    void ReceiveFaceId(int64_t faceid);

private slots:
    void on_pushButton_add_clicked();

private:
    Ui::AttendanceWin *ui;

    RegisterWin ww;

    QTcpServer mserver;
    QTcpSocket *msocket;
    quint64 receive_size;
    QFaceObject face_obj;
    QSqlTableModel model;
};
#endif // ATTENDANCEWIN_H
