#include "attendancewin.h"
#include "ui_attendancewin.h"
#include <QDebug>
#include <QDateTime>
#include <QThread>
#include <opencv.hpp>
#include <QSqlQuery>

AttendanceWin::AttendanceWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AttendanceWin)
{
    ui->setupUi(this);
    receive_size = 0;

    connect(&mserver, &QTcpServer::newConnection, this, &AttendanceWin::accept_client);
    mserver.listen(QHostAddress::Any, 9999);

    model.setTable("employee");

    //创建线程
    QThread *thread = new QThread();
    face_obj.moveToThread(thread);
    thread->start();

    connect(this, &AttendanceWin::query, &face_obj, &QFaceObject::face_query);
    connect(&face_obj, &QFaceObject::return_id, this, &AttendanceWin::ReceiveFaceId);
}

AttendanceWin::~AttendanceWin()
{
    delete ui;
}

void AttendanceWin::accept_client()
{
    //获取与客户端通信的套接字
    msocket = mserver.nextPendingConnection();

    connect(msocket, &QTcpSocket::readyRead, this, &AttendanceWin::read_data);
}

void AttendanceWin::read_data(){
    QDataStream stream(msocket);
    stream.setVersion(QDataStream::Qt_5_14);

    if(receive_size == 0){
        if(msocket->bytesAvailable() < (qint64)sizeof(receive_size)){return;}
        stream >> receive_size;
    }
    if(msocket->bytesAvailable() < receive_size){
        return;
    }

    QByteArray data;
    stream >> data;
    receive_size = 0;
    if(data.size() == 0) {return;}

    QPixmap qmp;
    qmp.loadFromData(data, "jpg");
    qmp = qmp.scaled(ui->label_pic->size());
    ui->label_pic->setPixmap(qmp);

    //人脸识别
    cv::Mat face_image;
    std::vector<uchar> decode;
    decode.resize(data.size());
    memcpy(decode.data(), data.data(), data.size());
    face_image = cv::imdecode(decode, cv::IMREAD_COLOR);

    //auto faceid = face_obj.face_query(face_image);
    emit query(face_image);

}

void AttendanceWin::ReceiveFaceId(int64_t face_id){
    qDebug() << "faceid: " << face_id;
    QString send_msg;

    if(face_id == -1){
        send_msg = "Unrecognizable";
        msocket->write(send_msg.toUtf8());
        return ;
    }

    model.setFilter(QString("faceID=%1").arg(face_id));
    model.select();
    //传输查询到的信息到客户端
    if(model.rowCount() == 1){
        QSqlRecord record = model.record(0);
        send_msg = QString("{\"employeeID\":\"%1\",\"name\":\"%2\",\"department\":\"软件\",\"time\":\"%3\"}")
                        .arg(record.value("employeeID").toString()).arg(record.value("name").toString())
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));



        //undo 把数据写入数据库--考勤表
        QString insertSql = QString("insert into attendance(employeeID) values('%1')").arg(record.value("employeeID").toString());
        QSqlQuery query;
        if(!query.exec(insertSql)){
            send_msg = "Attendance failed";
            //qDebug()<<query.lastError().text();
        }
        msocket->write(send_msg.toUtf8());
    }
}

void AttendanceWin::on_pushButton_add_clicked()
{
    ww.show();
}
