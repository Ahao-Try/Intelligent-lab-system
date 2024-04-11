#ifndef ARCFACE_H
#define ARCFACE_H

#include <cmath>
#include <vector>
#include <string>
#include "../ncnn/net.h"
#include "base.h"

ncnn::Mat preprocess(ncnn::Mat img, FaceInfo info);

float calcSimilar(std::vector<float> feature1, std::vector<float> feature2);

class Arcface {
public:
    Arcface(std::string model_folder = ".");
    ~Arcface();
    std::vector<float> getFeature(ncnn::Mat img);

private:
    ncnn::Net net;

    const int feature_dim = 128;

    void normalize(std::vector<float> &feature);
};

#endif
