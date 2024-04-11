#include "mainwindow.h"
#include <QApplication>

#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "./include/arcface/arcface.h"
#include "./include/arcface/mtcnn.h"

#if NCNN_VULKAN
#include "gpu.h"
#endif // NCNN_VULKAN

using namespace cv;
using namespace std;

cv::Mat ncnn2cv(ncnn::Mat img)
{
    unsigned char pix[img.h * img.w * 3];
    img.to_pixels(pix, ncnn::Mat::PIXEL_BGR);
    cv::Mat cv_img(img.h, img.w, CV_8UC3);
    for (int i = 0; i < cv_img.rows; i++)
    {
        for (int j = 0; j < cv_img.cols; j++)
        {
            cv_img.at<cv::Vec3b>(i,j)[0] = pix[3 * (i * cv_img.cols + j)];
            cv_img.at<cv::Vec3b>(i,j)[1] = pix[3 * (i * cv_img.cols + j) + 1];
            cv_img.at<cv::Vec3b>(i,j)[2] = pix[3 * (i * cv_img.cols + j) + 2];
        }
    }
    return cv_img;
}

int arcface()
{
    Mat img1;
    Mat img2;
    // if (argc == 3)
    // {
    //     img1 = imread(argv[1]);
    //     img2 = imread(argv[2]);
    // }
    // else{
        img1 = imread("./resource/gyy1.jpeg");
        img2 = imread("./resource/gyy2.jpeg");
    // }
    ncnn::Mat ncnn_img1 = ncnn::Mat::from_pixels(img1.data, ncnn::Mat::PIXEL_BGR, img1.cols, img1.rows);
    ncnn::Mat ncnn_img2 = ncnn::Mat::from_pixels(img2.data, ncnn::Mat::PIXEL_BGR, img2.cols, img2.rows);

    MtcnnDetector detector("./models");

    double start = (double)getTickCount();
    vector<FaceInfo> results1 = detector.Detect(ncnn_img1);
    cout << "Detection Time: " << (getTickCount() - start) / getTickFrequency() << "s" << std::endl;

    start = (double)getTickCount();
    vector<FaceInfo> results2 = detector.Detect(ncnn_img2);
    cout << "Detection Time: " << (getTickCount() - start) / getTickFrequency() << "s" << std::endl;

    ncnn::Mat det1 = preprocess(ncnn_img1, results1[0]);
    ncnn::Mat det2 = preprocess(ncnn_img2, results2[0]);
    

    Arcface arc("./models");

    start = (double)getTickCount();
    vector<float> feature1 = arc.getFeature(det1);
    cout << "Extraction Time: " << (getTickCount() - start) / getTickFrequency() << "s" << std::endl;

    start = (double)getTickCount();
    vector<float> feature2 = arc.getFeature(det2);
    cout << "Extraction Time: " << (getTickCount() - start) / getTickFrequency() << "s" << std::endl;

    std::cout << "Similarity: " << calcSimilar(feature1, feature2) << std::endl;;

    return 0;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
