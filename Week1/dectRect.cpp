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
#include "dectRect.h"

using namespace std;

using namespace cv;




//识别矩形及三角形

void dect_rect(Mat src, Mat imgRect)
{
	Mat srcTemp = imgRect.clone();
	//Mat dstImg(imgRect.rows, imgRect.cols, CV_8UC3, Scalar(255, 255, 255));
	//轮廓检测参数
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	vector<Point>tri;
	vector<Point>approx;
	vector<Point>squares;

	int num_r = 0;//矩形个数
	int num_t = 0;//三角形个数


	//轮廓检测
	findContours(srcTemp, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], approx, arcLength(Mat(contours[i]), true)*0.02, true);



		if (approx.size() == 4 && fabs(contourArea(Mat(approx))) > 3000 && isContourConvex(Mat(approx)))
		{
			double minDist = 1e10;
			for (int i = 0; i < 4; i++)
			{
				Point side = approx[i] - approx[(i + 1) % 4];
				double squaredSideLength = side.dot(side);
				minDist = min(minDist, squaredSideLength);
			}
			if (minDist < 30)
				break;
			for (int i = 0; i < 4; i++)
				squares.push_back(Point(approx[i].x, approx[i].y));
		}
		approxPolyDP(contours[i], approx, arcLength(Mat(contours[i]), true)*0.02, true);

		if (approx.size() == 3 && fabs(contourArea(Mat(approx))) > 3000 && isContourConvex(Mat(approx)))
		{
			double minDist = 1e10;
			for (int i = 0; i < 3; i++)
			{
				Point side = approx[i] - approx[(i + 1) % 3];
				double squaredSideLength = side.dot(side);
				minDist = min(minDist, squaredSideLength);
			}
			if (minDist < 30)
				break;
			for (int i = 0; i < 3; i++)
				tri.push_back(Point(approx[i].x, approx[i].y));
		}




		//drawContours(src, contours, i, Scalar(0, 0, 255), 3);


	}




	Point center_r[100];
	Point center_t[100];

	for (size_t i = 0; i < squares.size(); i += 4)
	{
		Point center;
		center.x = (squares[i].x + squares[i + 2].x) / 2;
		center.y = (squares[i].y + squares[i + 2].y) / 2;

		center_r[i] = center;

		int k = 1;
		for (int j = 0; j < i; j++)
		{
			double dif_x = center_r[i].x - center_r[j].x;
			double dif_y = center_r[i].y - center_r[j].y;



			double dis = (sqrt(dif_x*dif_x + dif_y * dif_y)) / center.x;
			//cout << "dif: " << dis << endl;

			if (dis < 0.1)
			{
				k = 0;
				break;
			}

		}


		if (k == 1)
		{
			line(src, squares[i], squares[i + 1], Scalar(0, 255, 0), 4);
			line(src, squares[i + 1], squares[i + 2], Scalar(0, 255, 0), 4);
			line(src, squares[i + 2], squares[i + 3], Scalar(0, 255, 0), 4);
			line(src, squares[i + 3], squares[i], Scalar(0, 255, 0), 4);
			cout << "矩形中心" << (i + 1) % 4 << center << endl;
			num_r++;
			circle(src, center, 3, Scalar(0, 255, 0), -1);



		}



	}


	for (size_t i = 0; i < tri.size(); i += 3)
	{
		Point center;
		center.x = (tri[i].x + tri[i + 1].x + tri[i + 2].x) / 3;
		center.y = (tri[i].y + tri[i + 1].y + tri[i + 2].y) / 3;

		center_t[i] = center;


		int m = 1;
		for (int j = 0; j < i; j++)
		{
			double dif_x = center_r[i].x - center_r[j].x;
			double dif_y = center_r[i].y - center_r[j].y;


			double dis = (sqrt(dif_x*dif_x + dif_y * dif_y)) / center.x;

			if (dis < 2)
			{
				m = 0;

				break;
			}

		}

		if (m == 1)
		{

			line(src, tri[i], tri[i + 1], Scalar(255, 0, 0), 4);
			line(src, tri[i + 1], tri[i + 2], Scalar(255, 0, 0), 4);
			line(src, tri[i], tri[i + 2], Scalar(255, 0, 0), 4);
			cout << "三角形中心" << (i + 1) % 3 << center << endl;
			num_t++;
			circle(src, center, 3, Scalar(255, 0, 0), -1);

		}







	}







	cout << "存在矩形印章个数: " << num_r << endl;
	cout << "存在三角形印章个数: " << num_t << endl;
	imshow("矩形", src);

}