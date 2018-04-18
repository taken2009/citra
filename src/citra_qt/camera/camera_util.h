// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <vector>
#include "common/common_types.h"

namespace cv {
class Mat;
}
class QImage;

namespace CameraUtil {

std::vector<u16> Rgb2Yuv(QImage source, int width, int height);
std::vector<u16> ProcessImage(QImage source, int width, int height, bool output_rgb,
                              bool flip_horizontal, bool flip_vertical);
#ifdef ENABLE_OPENCV_CAMERA
QImage cvMat2QImage(const cv::Mat& mat);
#endif

} // namespace CameraUtil
