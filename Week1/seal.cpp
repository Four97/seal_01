// seal.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#include <math.h>
#include "D:\VS\openCV34\opencv\build\include\opencv2\highgui\highgui.hpp"
#include <D:\VS\openCV34\opencv\sources\include\opencv2\opencv.hpp>
#include "D:\VS\openCV34\opencv\build\include\opencv\cv.hpp"
#include "D:\VS\openCV34\opencv\build\include\opencv\cxcore.hpp"

#include <stdlib.h>
#include <stdio.h>
#include "D:\VS\openCV34\opencv\build\include\opencv2\core\core.hpp"
using namespace std;

using namespace cv;





float PI = 3.14159;
Mat imgHSI;

Mat img;





int rgbtohsi(Mat image) {
	Mat image_hsi;
	vector<Mat> chans;
	

	int nl = image.rows;
	int nc = image.cols;
	if (image.isContinuous()) {
		nc = nc * nl;
		nl = 1;
	}
	for (int i = 0; i < nl; i++) {
		uchar *src = image.ptr<uchar>(i);
		uchar *dst = image.ptr<uchar>(i);
		for (int j = 0; j < nc; j++) {
			float b = src[j * 3] / 255 ;
			float g = src[j * 3 + 1] / 255.0;
			float r = src[j * 3 + 2] / 255.0;
			float num = (float)(0.5*((r - g) + (r - b)));
			float den = (float)sqrt((r - g)*(r - g) + (r - b)*(g - b));
			float H, S, I;
			if (den == 0) {	//分母不能为0
				H = 0;
			}
			else {
				double theta = acos(num / den);
				if (b <= g)
					H = theta / (PI * 2);
				else
					H = (2 * PI - theta) / (2 * PI);
			}
			double minRGB = min(min(r, g), b);
			den = r + g + b;
			if (den == 0)	//分母不能为0
				S = 0;
			else
				S = 1 - 3 * minRGB / den;
			I = den / 3.0;
			//将S分量和H分量都扩充到[0,255]区间以便于显示;
			//一般H分量在[0,2pi]之间，S在[0,1]之间
			dst[3 * j] = H * 255;
			dst[3 * j + 1] = S *255;

			dst[3 * j + 2] = I *255;
		}
		
	}
	imshow("image_HSI", image);
	
	return 0;

}





void filteredRed(const Mat &inputImage, Mat &resultGray, Mat &resultColor) {
	
	
	resultGray = Mat(inputImage.rows, inputImage.cols, CV_8U, cv::Scalar(255));
	resultColor = Mat(inputImage.rows, inputImage.cols, CV_8UC3, cv::Scalar(255, 255, 255));
	double H = 0.0, S = 0.0, I = 0.0;
	for (int i = 0; i < inputImage.rows; i++)
	{
		for (int j = 0; j < inputImage.cols; j++)
		{
			H = inputImage.at<Vec3b>(i, j)[0];
			S = inputImage.at<Vec3b>(i, j)[1];
			I = inputImage.at<Vec3b>(i, j)[2];
			if (S >=35)
			{
				if ((H >= 0 && H < 24) || H >= 340)
				{ 
					
					resultGray.at<uchar>(i, j) = 0;
					resultColor.at<Vec3b>(i, j)[0] = inputImage.at<Vec3b>(i, j)[0];
					resultColor.at<Vec3b>(i, j)[1] = inputImage.at<Vec3b>(i, j)[1];
					resultColor.at<Vec3b>(i, j)[2] = inputImage.at<Vec3b>(i, j)[2];

						
					
					
				}
			}
		}
	}
	imshow("filter1", resultColor);
	imshow("filter2", resultGray);

}



int main()
{






	const char* imagename = "2.jpg";



	//从文件中读入图像

	Mat src = imread(imagename, 1);
	
	


	//如果读入图像失败

	if (src.empty())

	{

		fprintf(stderr, "Can not load image %s\n", imagename);

		return -1;

	}

	//显示图像

	imshow("image_src", src);

	rgbtohsi(src);
	Mat  output_gray, output_red;
	filteredRed(src, output_gray, output_red);


	

	




	//此函数等待按键，按键盘任意键就返回

	waitKey();

	return 0;


}





//void colorFilter(CvMat *inputImage, CvMat *&outputImage)
//{
//	int i, j;
//	IplImage* image = cvCreateImage(cvGetSize(inputImage), 8, 3);
//	cvGetImage(inputImage, image);
//	IplImage* hsv = cvCreateImage(cvGetSize(image), 8, 3);
//
//	cvCvtColor(image, hsv, CV_BGR2HSV);
//	int width = hsv->width;
//	int height = hsv->height;
//	for (i = 0; i < height; i++)
//		for (j = 0; j < width; j++)
//		{
//			CvScalar s_hsv = cvGet2D(hsv, i, j);//获取像素点为（j, i）点的HSV的值 
//			/*
//				opencv 的H范围是0~180，红色的H范围大概是(0~8)∪(160,180)
//				S是饱和度，一般是大于一个值,S过低就是灰色（参考值S>80)，
//				V是亮度，过低就是黑色，过高就是白色(参考值220>V>50)。
//			*/
//			CvScalar s;
//			if (!(((s_hsv.val[0] > 0) && (s_hsv.val[0] < 8)) || (s_hsv.val[0] > 120) && (s_hsv.val[0] < 180)))
//			{
//				s.val[0] = 0;
//				s.val[1] = 0;
//				s.val[2] = 0;
//				cvSet2D(hsv, i, j, s);
//			}
//
//
//		}
//	outputImage = cvCreateMat(hsv->height, hsv->width, CV_8UC3);
//	cvConvert(hsv, outputImage);
//	cvNamedWindow("filter");
//	cvShowImage("filter", hsv);
//	waitKey(0);
//	cvReleaseImage(&hsv);
//}



// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
