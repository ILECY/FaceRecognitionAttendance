#ifndef QFACEOBJECT_H
#define QFACEOBJECT_H

#include <QObject>
#include <seeta/FaceEngine.h>
#include <opencv.hpp>

class QFaceObject : public QObject
{
    Q_OBJECT
public:
    explicit QFaceObject(QObject *parent = nullptr);
    ~QFaceObject();

public slots:
    int64_t face_register(cv::Mat& faceimage);
    int64_t face_query(cv::Mat& faceimage);
signals:
    void return_id(int64_t face_id);

private:
    seeta::FaceEngine *fengineptr;

};

#endif // QFACEOBJECT_H
