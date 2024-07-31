#ifndef FACEATTENDANCE_H
#define FACEATTENDANCE_H

#include <QMainWindow>
#include <opencv.hpp>
#include <QTcpSocket>
#include <QTimer>

using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class FaceAttendance; }
QT_END_NAMESPACE

class FaceAttendance : public QMainWindow
{
    Q_OBJECT

public:
    FaceAttendance(QWidget *parent = nullptr);
    ~FaceAttendance();

    void timerEvent(QTimerEvent *e);

private slots:
    void timer_connect();
    void stop_connect();
    void start_connect();
    void receive_data();

private:
    Ui::FaceAttendance *ui;

    //摄像头
    VideoCapture cap;
    //haar级联分类器
    cv::CascadeClassifier cascade;
    //网络套接字
    QTcpSocket msocket;
    //定时器
    QTimer mtimer;

    //人脸发送去重
    int sameface_count;
};
#endif // FACEATTENDANCE_H
