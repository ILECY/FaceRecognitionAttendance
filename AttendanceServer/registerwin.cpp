#include "registerwin.h"
#include "ui_registerwin.h"
#include <QFileDialog>
#include <qfaceobject.h>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QMessageBox>

RegisterWin::RegisterWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterWin)
{
    ui->setupUi(this);
}

RegisterWin::~RegisterWin()
{
    delete ui;
}

void RegisterWin::timerEvent(QTimerEvent *e)
{
    if(cap.isOpened()){
        cap >> image;
        if(image.data == nullptr) {return;}
    }
    cv::Mat rgbimage;
    cv::cvtColor(image, rgbimage, cv::COLOR_BGR2RGB);
    QImage qimg(rgbimage.data, rgbimage.cols, rgbimage.rows, rgbimage.step1(), QImage::Format_BGR888);
    QPixmap qmp = QPixmap::fromImage(qimg);
    qmp = qmp.scaledToWidth(ui->label_headpic->width());
    ui->label_headpic->setPixmap(qmp);
}

//清空所填数据
void RegisterWin::on_pushButton_clear_clicked()
{
    ui->lineEdit_name->clear();
    ui->lineEditl_tele->clear();
    ui->lineEdit_address->clear();
    ui->radioButton_male->clearMask();
    ui->radioButton_female->clearMask();
    ui->lineEdit_picfile->clear();
    ui->dateEdit_birth->setDate(QDate::currentDate());
}

//添加头像
void RegisterWin::on_pushButton_addphoto_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this);
    ui->lineEdit_picfile->setText(filepath);

    QPixmap qmp(filepath);
    qmp = qmp.scaledToWidth(ui->label_headpic->width());
    ui->label_headpic->setPixmap(qmp);
}

//添加员工信息
void RegisterWin::on_pushButton_add_clicked()
{
    QFaceObject faceobj;
    cv::Mat image = cv::imread(ui->lineEdit_picfile->text().toUtf8().data());
    auto faceID = faceobj.face_register(image);
    QString pic_path = QString("./data/%1.jpg").arg(QString(ui->lineEdit_name->text().toUtf8().toBase64()));
    //QString pic_path = QString("./data/%1.jpg").arg(ui->lineEdit_name->text().toUtf8().Base64Encoding);
    cv::imwrite(pic_path.toUtf8().data(), image);

    QSqlTableModel model;
    model.setTable("employee");
    QSqlRecord record = model.record();
    record.setValue("name", ui->lineEdit_name->text());
    record.setValue("sex", ui->radioButton_male->isChecked()?"male":"female");
    record.setValue("birthday", ui->dateEdit_birth->text());
    record.setValue("address", ui->lineEdit_address->text());
    record.setValue("phone", ui->lineEditl_tele->text());
    record.setValue("faceID", faceID);
    record.setValue("headfile", pic_path);
    bool is_success = model.insertRecord(0, record);
    if(is_success){
        QMessageBox::information(this, "提示", "添加成功");
        model.submitAll();
    }
    else{
        QMessageBox::information(this, "提示", "添加失败");
    }
}

void RegisterWin::on_pushButton_opencamera_clicked()
{
    if(ui->pushButton_opencamera->text() == "开启摄像头"){
        if(cap.open(0)){
            ui->pushButton_opencamera->setText("关闭摄像头");
            timerid = startTimer(100);
        }
    }else{
        killTimer(timerid);
        ui->pushButton_opencamera->setText("开启摄像头");
        cap.release();
    }
}

void RegisterWin::on_pushButton_takephoto_clicked()
{
    QString headfile = QString("./data/%1.jpg").arg(QString(ui->lineEdit_name->text().toUtf8().toBase64()));
    ui->lineEdit_picfile->setText(headfile);
    cv::imwrite(headfile.toUtf8().data(), image);
    killTimer(timerid);
    ui->pushButton_opencamera->setText("开启摄像头");
    cap.release();
}
