#ifndef REGISTERWIN_H
#define REGISTERWIN_H

#include <QWidget>
#include <opencv.hpp>

namespace Ui {
class RegisterWin;
}

class RegisterWin : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWin(QWidget *parent = nullptr);
    ~RegisterWin();
    void timerEvent(QTimerEvent *e);

private slots:
    void on_pushButton_clear_clicked();

    void on_pushButton_addphoto_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_opencamera_clicked();

    void on_pushButton_takephoto_clicked();

private:
    Ui::RegisterWin *ui;

    int timerid;
    cv::VideoCapture cap;
    cv::Mat image;
};

#endif // REGISTERWIN_H
