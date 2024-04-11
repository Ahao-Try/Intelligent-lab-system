#ifndef MTCNN_H
#define MTCNN_H

#include <cmath>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include "../ncnn/net.h"
#include "base.h"

using namespace std;

class MtcnnDetector {
public:
    MtcnnDetector(string model_folder = ".");
    ~MtcnnDetector();
    vector<FaceInfo> Detect(ncnn::Mat img);
private:
    float minsize = 20;
    float threshold[3] = {0.6f, 0.7f, 0.8f};//这是三个阈值，分别对应于 P 网络、R 网络和 O 网络的置信度阈值。当检测到的人脸框的置信度超过这些阈值时，认为检测是有效的
    float factor = 0.709f;
    const float mean_vals[3] = {127.5f, 127.5f, 127.5f};//通常是训练数据集图像像素值的平均值，以将输入图像的均值调整为零，有助于减小图像中的光照和色彩差异对模型的影响。
    const float norm_vals[3] = {0.0078125f, 0.0078125f, 0.0078125f};//归一化值：通常是一个小数，它用于将图像像素值缩放到一个较小的范围内，以便于神经网络的训练。
    ncnn::Net Pnet;
    ncnn::Net Rnet;
    ncnn::Net Onet;
    ncnn::Net Lnet;
    vector<FaceInfo> Pnet_Detect(ncnn::Mat img);
    vector<FaceInfo> Rnet_Detect(ncnn::Mat img, vector<FaceInfo> bboxs);
    vector<FaceInfo> Onet_Detect(ncnn::Mat img, vector<FaceInfo> bboxs);
    void Lnet_Detect(ncnn::Mat img, vector<FaceInfo> &bboxs);
    vector<FaceInfo> generateBbox(ncnn::Mat score, ncnn::Mat loc, float scale, float thresh);
    void doNms(vector<FaceInfo> &bboxs, float nms_thresh, string mode);
    void refine(vector<FaceInfo> &bboxs, int height, int width, bool flag = false);
};

#endif
