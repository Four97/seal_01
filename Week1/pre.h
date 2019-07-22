#pragma once
#include <iostream>

#include <math.h>
#include "D:\VS\openCV34\opencv\build\include\opencv2\highgui\highgui.hpp"
#include <D:\VS\openCV34\opencv\sources\include\opencv2\opencv.hpp>
#include "D:\VS\openCV34\opencv\build\include\opencv\cv.hpp"
#include "D:\VS\openCV34\opencv\build\include\opencv\cxcore.hpp"
#include "D:\VS\openCV34\opencv\sources\modules\imgproc\include\opencv2\imgproc.hpp"

#include <stdlib.h>
#include <stdio.h>
#include "D:\VS\openCV34\opencv\build\include\opencv2\core\core.hpp"

using namespace std;

using namespace cv;



#ifndef PRE_H
#define PRE_H

// TODO: 添加要在此处预编译的标头

int rgbtohsi(Mat image);
void filteredRed(const Mat &inputImage, Mat &resultGray, Mat &resultColor);
void sharpen(const Mat& img, Mat& result);
void thin(const Mat &src, Mat &dst, const int iterations);

#endif //PRE_H