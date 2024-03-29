// seal.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"


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

#include "pre.h"

using namespace std;

using namespace cv;





//float PI = 3.14159;
Mat imgHSI;

Mat img;
RNG rng(12345);


int rgbtohsi(Mat image);//转成hsi图像
void filteredRed(const Mat &inputImage, Mat &resultGray, Mat &resultColor);//提取红色部分
void sharpen(const Mat& img, Mat& result);//锐化
void thin(const Mat &src, Mat &dst, const int iterations);//细化
void edge_detection(Mat& src);//霍夫直线检测 (没有用上)
void dect_rect(Mat src, Mat imgRect);//检测三角形以及矩形
bool checkEllipseShape(Mat src, vector<Point> contour, RotatedRect ellipse, double ratio);//判断拟合椭圆是否符合实际
void dectEllipse(Mat src, Mat img);//检测椭圆













int main()
{


	clock_t startTime, endTime;
	startTime = clock();
	
		//从文件中读入图像
	/*	const char* imagename = "1.png";*/
		cout << "请输入图片名（文件夹/带图片后缀）：" <<  endl;
		char imagename[100];
		cin >> imagename;
		startTime = clock();
		Mat src = imread(imagename, -1);

	/*for (int h = 0; h < 20; h++)
	{*/

		Mat srcTemp = src.clone();
		Mat output_gray, output_red;
		Mat gray;


		/*Mat srcTemp02 = src.clone();*/



		//如果读入图像失败
		if (src.empty())

		{

			fprintf(stderr, "Can not load image %s\n", imagename);
			return -1;

		}

		//显示图像

		//namedWindow("image_src", WINDOW_NORMAL);
		imshow("image_src", src);


		//转HSI
		rgbtohsi(srcTemp);
		/*cvtColor(src, srcTemp, CV_BGR2HSV);*/

		//过滤红色
		filteredRed(srcTemp, output_gray, output_red);

		//灰度二值化
		cvtColor(output_red, gray, CV_BGR2GRAY);
		/*blur(gray, gray, Size(2, 2));*/
		threshold(gray, output_gray, 0, 255, THRESH_BINARY | THRESH_TRIANGLE);


		//检测矩形以及三角形
		Mat out_thin;
		bitwise_not(output_gray, output_gray);
		/*thin(output_gray, out_thin, 10);*/
		namedWindow("output_gray", WINDOW_NORMAL);
		imshow("output_gray", output_gray);
		dect_rect(src, output_gray);

		//检测圆以及椭圆
		/*bitwise_not(output_gray, output_gray);*/
		dectEllipse(src, output_gray);

		
	
	/*}*/


	





	endTime = clock();
	/*cout << "The run time is:" << (double)clock() / CLOCKS_PER_SEC << "s" << endl;*/
	cout << "The run time of 20 times is:" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
	
	//此函数等待按键，按键盘任意键就返回
	waitKey();

	return 0;


}