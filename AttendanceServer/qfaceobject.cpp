#include "qfaceobject.h"

QFaceObject::QFaceObject(QObject *parent) : QObject(parent)
{
    seeta::ModelSetting FDmode("D:/FaceRecognition/SeetaFace/bin/model/fd_2_00.dat", seeta::ModelSetting::GPU, 0);
    seeta::ModelSetting PDmode("D:/FaceRecognition/SeetaFace/bin/model/pd_2_00_pts5.dat", seeta::ModelSetting::GPU, 0);
    seeta::ModelSetting FRmode("D:/FaceRecognition/SeetaFace/bin/model/fr_2_10.dat", seeta::ModelSetting::GPU, 0);
    fengineptr = new seeta::FaceEngine(FDmode, PDmode, FRmode);

    //导入已有的人脸数据库
    fengineptr->Load("./face.db");
}

QFaceObject::~QFaceObject()
{
    delete fengineptr;
}

int64_t QFaceObject::face_register(cv::Mat &faceimage)
{
    SeetaImageData simage;
    simage.data = faceimage.data;
    simage.width = faceimage.cols;
    simage.height = faceimage.rows;
    simage.channels = faceimage.channels();
    int64_t faceid = fengineptr->Register(simage);
    if(faceid >= 0){
        fengineptr->Save("./face.db");
    }
    return faceid;
}

int64_t QFaceObject::face_query(cv::Mat &faceimage)
{
    SeetaImageData simage;
    simage.data = faceimage.data;
    simage.width = faceimage.cols;
    simage.height = faceimage.rows;
    simage.channels = faceimage.channels();
    float similarity = 0;
    int64_t faceid = fengineptr->Query(simage, &similarity);
    if(similarity > 0.7){
        emit return_id(faceid);
    } else {
        emit return_id(-1);
    }
    return faceid;
}
