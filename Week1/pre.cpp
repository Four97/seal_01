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






//转HSI
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
			float b = src[j * 3] / 255;
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
					H = theta / (CV_PI * 2);
				else
					H = (2 * CV_PI - theta) / (2 * CV_PI);
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
			dst[3 * j + 1] = S * 255;

			dst[3 * j + 2] = I * 255;
		}

	}
	//imshow("image_HSI", image);

	return 0;

}





//提取红色
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
			if (S >= 30)
			{
				if ((H >= 0 && H < 24) || H >= 200)//HSV是150 hsi是240
				{

					resultGray.at<uchar>(i, j) = 0;
					resultColor.at<Vec3b>(i, j)[0] = inputImage.at<Vec3b>(i, j)[0];
					resultColor.at<Vec3b>(i, j)[1] = inputImage.at<Vec3b>(i, j)[1];
					resultColor.at<Vec3b>(i, j)[2] = inputImage.at<Vec3b>(i, j)[2];




				}
			}
		}
	}
	//imshow("filter1", resultColor);
	//imshow("filter2", resultGray);

}


//锐化
void sharpen(const Mat& img, Mat& result) {

	result.create(img.size(), img.type());

	//处理边界内部的像素点, 图像最外围的像素点应该额外处理

	for (int row = 1; row < img.rows - 1; row++)

	{

		//前一行像素点

		const uchar* previous = img.ptr<const uchar>(row - 1);

		//待处理的当前行

		const uchar* current = img.ptr<const uchar>(row);

		//下一行

		const uchar* next = img.ptr<const uchar>(row + 1);

		uchar *output = result.ptr<uchar>(row);

		int ch = img.channels();

		int starts = ch;

		int ends = (img.cols - 1) * ch;

		for (int col = starts; col < ends; col++)

		{

			//输出图像的遍历指针与当前行的指针同步递增, 以每行的每一个像素点的每一个通道值为一个递增量, 因为要考虑到图像的通道数

			*output++ = saturate_cast<uchar>(5 * current[col] - current[col - ch] - current[col + ch] - previous[col] - next[col]);

		}

	} //end loop

	//处理边界, 外围像素点设为 0

	result.row(0).setTo(Scalar::all(0));

	result.row(result.rows - 1).setTo(Scalar::all(0));

	result.col(0).setTo(Scalar::all(0));

	result.col(result.cols - 1).setTo(Scalar::all(0));

}



//轮廓细化1.0
void thin(const Mat &src, Mat &dst, const int iterations)
{
	const int height = src.rows - 1;
	const int width = src.cols - 1;

	//拷贝一个数组给另一个数组
	if (src.data != dst.data)
	{
		src.copyTo(dst);
	}


	int n = 0, i = 0, j = 0;
	Mat tmpImg;
	uchar *pU, *pC, *pD;
	bool isFinished = false;

	for (n = 0; n < iterations; n++)
	{
		dst.copyTo(tmpImg);
		isFinished = false;   //一次 先行后列扫描 开始
		//扫描过程一 开始
		for (i = 1; i < height; i++)
		{
			pU = tmpImg.ptr<uchar>(i - 1);
			pC = tmpImg.ptr<uchar>(i);
			pD = tmpImg.ptr<uchar>(i + 1);
			for (int j = 1; j < width; j++)
			{
				if (pC[j] > 0)
				{
					int ap = 0;
					int p2 = (pU[j] > 0);
					int p3 = (pU[j + 1] > 0);
					if (p2 == 0 && p3 == 1)
					{
						ap++;
					}
					int p4 = (pC[j + 1] > 0);
					if (p3 == 0 && p4 == 1)
					{
						ap++;
					}
					int p5 = (pD[j + 1] > 0);
					if (p4 == 0 && p5 == 1)
					{
						ap++;
					}
					int p6 = (pD[j] > 0);
					if (p5 == 0 && p6 == 1)
					{
						ap++;
					}
					int p7 = (pD[j - 1] > 0);
					if (p6 == 0 && p7 == 1)
					{
						ap++;
					}
					int p8 = (pC[j - 1] > 0);
					if (p7 == 0 && p8 == 1)
					{
						ap++;
					}
					int p9 = (pU[j - 1] > 0);
					if (p8 == 0 && p9 == 1)
					{
						ap++;
					}
					if (p9 == 0 && p2 == 1)
					{
						ap++;
					}
					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
					{
						if (ap == 1)
						{
							if ((p2*p4*p6 == 0) && (p4*p6*p8 == 0))
							{
								dst.ptr<uchar>(i)[j] = 0;
								isFinished = true;
							}

							//   if((p2*p4*p8==0)&&(p2*p6*p8==0))
						   //    {                           
						   //         dst.ptr<uchar>(i)[j]=0;
						   //         isFinished =TRUE;                            
						   //    }

						}
					}
				}

			} //扫描过程一 结束


			dst.copyTo(tmpImg);
			//扫描过程二 开始
			for (i = 1; i < height; i++)  //一次 先行后列扫描 开始
			{
				pU = tmpImg.ptr<uchar>(i - 1);
				pC = tmpImg.ptr<uchar>(i);
				pD = tmpImg.ptr<uchar>(i + 1);
				for (int j = 1; j < width; j++)
				{
					if (pC[j] > 0)
					{
						int ap = 0;
						int p2 = (pU[j] > 0);
						int p3 = (pU[j + 1] > 0);
						if (p2 == 0 && p3 == 1)
						{
							ap++;
						}
						int p4 = (pC[j + 1] > 0);
						if (p3 == 0 && p4 == 1)
						{
							ap++;
						}
						int p5 = (pD[j + 1] > 0);
						if (p4 == 0 && p5 == 1)
						{
							ap++;
						}
						int p6 = (pD[j] > 0);
						if (p5 == 0 && p6 == 1)
						{
							ap++;
						}
						int p7 = (pD[j - 1] > 0);
						if (p6 == 0 && p7 == 1)
						{
							ap++;
						}
						int p8 = (pC[j - 1] > 0);
						if (p7 == 0 && p8 == 1)
						{
							ap++;
						}
						int p9 = (pU[j - 1] > 0);
						if (p8 == 0 && p9 == 1)
						{
							ap++;
						}
						if (p9 == 0 && p2 == 1)
						{
							ap++;
						}
						if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
						{
							if (ap == 1)
							{
								//   if((p2*p4*p6==0)&&(p4*p6*p8==0))
								//   {                           
							   //         dst.ptr<uchar>(i)[j]=0;
							   //         isFinished =TRUE;                            
							   //    }

								if ((p2*p4*p8 == 0) && (p2*p6*p8 == 0))
								{
									dst.ptr<uchar>(i)[j] = 0;
									isFinished = true;
								}

							}
						}
					}

				}

			} //一次 先行后列扫描完成          
		  //如果在扫描过程中没有删除点，则提前退出
			if (isFinished == false)
			{
				break;
			}
		}

	}
}