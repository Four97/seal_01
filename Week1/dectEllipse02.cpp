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
#include "dectEllipse.h"

using namespace std;

using namespace cv;

void thin(const Mat &src, Mat &dst, const int iterations);


//检测椭圆是否真实存在
bool checkEllipseShape(Mat src, vector<Point> contour, RotatedRect ellipse, double ratio = 0.01)
{
	//get all the point on the ellipse point
	vector<Point> ellipse_point;

	//get the parameter of the ellipse
	Point2f center = ellipse.center;
	double a_2 = pow(ellipse.size.width*0.5, 2);
	double b_2 = pow(ellipse.size.height*0.5, 2);
	double ellipse_angle = (ellipse.angle*3.1415926) / 180;


	//the uppart
	for (int i = 0; i < ellipse.size.width; i++)
	{
		double x = -ellipse.size.width*0.5 + i;
		double y_left = sqrt((1 - (x*x / a_2))*b_2);

		//rotate
		//[ cos(seta) sin(seta)]
		//[-sin(seta) cos(seta)]
		cv::Point2f rotate_point_left;
		rotate_point_left.x = cos(ellipse_angle)*x - sin(ellipse_angle)*y_left;
		rotate_point_left.y = +sin(ellipse_angle)*x + cos(ellipse_angle)*y_left;

		//trans
		rotate_point_left += center;

		//store
		ellipse_point.push_back(Point(rotate_point_left));
	}
	//the downpart
	for (int i = 0; i < ellipse.size.width; i++)
	{
		double x = ellipse.size.width*0.5 - i;
		double y_right = -sqrt((1 - (x*x / a_2))*b_2);

		//rotate
		//[ cos(seta) sin(seta)]
		//[-sin(seta) cos(seta)]
		cv::Point2f rotate_point_right;
		rotate_point_right.x = cos(ellipse_angle)*x - sin(ellipse_angle)*y_right;
		rotate_point_right.y = +sin(ellipse_angle)*x + cos(ellipse_angle)*y_right;

		//trans
		rotate_point_right += center;

		//store
		ellipse_point.push_back(Point(rotate_point_right));

	}


	//vector<vector<Point> > contours1;
	//contours1.push_back(ellipse_point);
	//drawContours(src,contours1,-1,Scalar(255,0,0),2);

	//match shape
	double a0 = matchShapes(ellipse_point, contour, CV_CONTOURS_MATCH_I1, 0);
	//cout << "²âÊÔ" << a0 << endl;
	if (a0 > 0.01)//0.01
	{
		//cout << "a0" << a0 << endl;
		return true;

	}

	return false;
}

//检测可能存在的椭圆
void dectEllipse(Mat src, Mat img)
{
	Mat srcTemp = src.clone();
	Mat imgTemp = img.clone();

	//imwrite("imgTemp.jpg", imgTemp);


	//双边滤波
	Mat outgray_2;
	outgray_2 = img.clone();
	bilateralFilter(outgray_2, imgTemp, 15, 90, 3);//15,90,3
	//namedWindow("imgTemp", WINDOW_NORMAL);
	//imshow("imgTemp", imgTemp);

	//细化
	Mat out_thin;
	thin(imgTemp, out_thin, 7);//7
	//namedWindow("out_thin", WINDOW_NORMAL);
	//imshow("out_thin", out_thin);

	//imwrite("out_thin.jpg", out_thin);
	// convert into gray
	//cvtColor(imgTemp, imgTemp, CV_BGR2GRAY);

	Mat threshold_output;
	vector<vector<Point> > contours;
	vector<Point > contours2;

	int num_c = 0;//圆的个数
	int num_e = 0;//椭圆个数
	vector<Point2f > center1;

	// find contours
	//int threshold_value = threshold(imgTemp, threshold_output, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	findContours(out_thin, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);


	//imshow("threshold_output", threshold_output);
	//imshow("imgTemp", imgTemp);
	//fit ellipse
	vector<RotatedRect> minEllipse(contours.size());
	vector<RotatedRect> minEllipse2;
	for (int i = 0; i < contours.size(); i++)
	{

		//point size check
		if (contours[i].size() < 10)
		{
			continue;
		}

		//point area
		if (contourArea(contours[i]) < 3000)//500
		{
			continue;
			//cout << "contourArea: " << contourArea(contours[i]) << endl;
		}






		minEllipse[i] = fitEllipse(Mat(contours[i]));

		//check shape
		if (checkEllipseShape(out_thin, contours[i], minEllipse[i]))
		{
			continue;
		}


		int k = 1;
		for (int j = 0; j < minEllipse2.size(); j++)
		{
			double dif_x = minEllipse[i].center.x - minEllipse2[j].center.x;
			double dif_y = minEllipse[i].center.y - minEllipse2[j].center.y;
			double a_2 = pow(minEllipse2[j].size.width*0.5, 2);
			//double a_2 = minEllipse[i].size.width*0.5;
			double dis = (dif_x*dif_x + dif_y * dif_y) / a_2;

			if (dis < 0.1)//a^2Ê± 9e-5
			{
				k = 0;
				break;

			}



		}


		if (k == 1)
		{
			minEllipse2.push_back(fitEllipse(Mat(contours[i])));
		}






	}

	//cout << "num: " << num << endl;


	for (int j = 0; j < minEllipse2.size(); j++)
	{



		//cout << 1 << endl;
		Scalar color_e = Scalar(0, 255, 255);
		Scalar color_c = Scalar(255, 255, 0);
		// ellipse

		//ellipse(srcTemp, minEllipse[i], color_e, 2);


		Point2f center = minEllipse2[j].center;

		double a_2 = pow(minEllipse2[j].size.width*0.5, 2);
		double b_2 = pow(minEllipse2[j].size.height*0.5, 2);

		double dif = abs(a_2 - b_2) / a_2;


		if (dif < 0.12)//长短轴
		{
			num_c++;

			cout << "圆中心: " << center << endl;
			ellipse(srcTemp, minEllipse2[j], color_c, 2);
		}
		else
		{
			num_e++;
			//cout << "dif: " << dif << endl;
			cout << "椭圆中心: " << center << endl;
			ellipse(srcTemp, minEllipse2[j], color_e, 2);
		}

		//cout << "area: " << contourArea(contours[i]) << endl;






	}







	/// 结果在窗体中显示
	namedWindow("检测结果", WINDOW_NORMAL);
	//imshow("检测结果", srcTemp);
	imwrite("检测结果.jpg", srcTemp);
	cout << "存在圆印章个数: " << num_c << endl;
	cout << "存在椭圆印章个数: " << num_e << endl;


}