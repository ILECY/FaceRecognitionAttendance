#include "faceattendance.h"
#include "ui_faceattendance.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

FaceAttendance::FaceAttendance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FaceAttendance)
{
    ui->setupUi(this);

    cap.open(0);
    startTimer(20);

    //导入级联分类器文件
    cascade.load("D:\\FaceRecognition\\opencv452\\etc\\haarcascades\\haarcascade_frontalface_alt2.xml");

    //定时器连接服务器
    connect(&msocket, &QTcpSocket::disconnected, this, &FaceAttendance::start_connect);
    connect(&msocket, &QTcpSocket::connected, this, &FaceAttendance::stop_connect);
    connect(&mtimer, &QTimer::timeout, this, &FaceAttendance::timer_connect);
    //接收数据
    connect(&msocket, &QTcpSocket::readyRead, this, &FaceAttendance::receive_data);
    mtimer.start(5000);

}

FaceAttendance::~FaceAttendance()
{
    delete ui;
}

void FaceAttendance::timerEvent(QTimerEvent *e)
{
    Mat srcimage;
    if(cap.grab()){
        cap.read(srcimage);
    }

    Mat grayimage;
    cvtColor(srcimage, grayimage, COLOR_BGR2GRAY);
    std::vector<Rect> face_rects;
    //检测人脸
    cascade.detectMultiScale(grayimage, face_rects);
    if(face_rects.size() > 0){
        Rect rect = face_rects.at(0);
        rectangle(srcimage, rect, Scalar(0, 0, 255));
        if(sameface_count > 2){
            //把Mat数据转化为QbyteArray
            std::vector<uchar> buf;
            cv::imencode(".jpg", srcimage, buf);
            QByteArray byte((const char*)buf.data(), buf.size());

            //发送数据
            quint64 backsize = byte.size();
            QByteArray sendData;
            QDataStream stream(&sendData, QIODevice::WriteOnly);
            stream.setVersion(QDataStream::Qt_5_14);
            stream << backsize << byte;
            msocket.write(sendData);
            sameface_count = -2;
        }
        sameface_count ++;
    }
    if(face_rects.size() == 0){
        sameface_count = 0;
    }
    if(srcimage.data == nullptr) {return ;}
    cvtColor(srcimage, srcimage, COLOR_BGR2RGB);
    QImage image(srcimage.data, srcimage.cols, srcimage.rows, srcimage.step1(), QImage::Format_RGB888);
    QPixmap qmp =QPixmap::fromImage(image);
    ui->label_face->setPixmap(qmp);
}

void FaceAttendance::receive_data()
{
    //QString msg = msocket.readAll();
    //qDebug() << msg;

    QByteArray array = msocket.readAll();
    //Json解析
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(array, &err);
    if(err.error != QJsonParseError::NoError){
        qDebug() << "json error";
        return ;
    }
    QJsonObject json_obj = doc.object();
    QString employeeid = json_obj.value("employeeID").toString();
    QString name = json_obj.value("name").toString();
    QString department = json_obj.value("department").toString();
    QString timestr = json_obj.value("time").toString();

    ui->lineEdit_id->setText(employeeid);
    ui->lineEdit_name->setText(name);
    ui->lineEdit_sector->setText(department);
    ui->lineEdit_time->setText(timestr);
}

void FaceAttendance::timer_connect()
{
    //连接服务器
    msocket.connectToHost("192.168.31.162",9999);
    qDebug() << "正在连接服务器...";
}

void FaceAttendance::stop_connect()
{
    mtimer.stop();
    qDebug() << "连接成功";
}

void FaceAttendance::start_connect()
{
    mtimer.start(5000);
    qDebug() << "断开连接";
}

