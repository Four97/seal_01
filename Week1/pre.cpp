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






//תHSI
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
			if (den == 0) {	//��ĸ����Ϊ0
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
			if (den == 0)	//��ĸ����Ϊ0
				S = 0;
			else
				S = 1 - 3 * minRGB / den;
			I = den / 3.0;
			//��S������H���������䵽[0,255]�����Ա�����ʾ;
			//һ��H������[0,2pi]֮�䣬S��[0,1]֮��
			dst[3 * j] = H * 255;
			dst[3 * j + 1] = S * 255;

			dst[3 * j + 2] = I * 255;
		}

	}
	//imshow("image_HSI", image);

	return 0;

}





//��ȡ��ɫ
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
				if ((H >= 0 && H < 24) || H >= 200)//HSV��150 hsi��240
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


//��
void sharpen(const Mat& img, Mat& result) {

	result.create(img.size(), img.type());

	//����߽��ڲ������ص�, ͼ������Χ�����ص�Ӧ�ö��⴦��

	for (int row = 1; row < img.rows - 1; row++)

	{

		//ǰһ�����ص�

		const uchar* previous = img.ptr<const uchar>(row - 1);

		//������ĵ�ǰ��

		const uchar* current = img.ptr<const uchar>(row);

		//��һ��

		const uchar* next = img.ptr<const uchar>(row + 1);

		uchar *output = result.ptr<uchar>(row);

		int ch = img.channels();

		int starts = ch;

		int ends = (img.cols - 1) * ch;

		for (int col = starts; col < ends; col++)

		{

			//���ͼ��ı���ָ���뵱ǰ�е�ָ��ͬ������, ��ÿ�е�ÿһ�����ص��ÿһ��ͨ��ֵΪһ��������, ��ΪҪ���ǵ�ͼ���ͨ����

			*output++ = saturate_cast<uchar>(5 * current[col] - current[col - ch] - current[col + ch] - previous[col] - next[col]);

		}

	} //end loop

	//����߽�, ��Χ���ص���Ϊ 0

	result.row(0).setTo(Scalar::all(0));

	result.row(result.rows - 1).setTo(Scalar::all(0));

	result.col(0).setTo(Scalar::all(0));

	result.col(result.cols - 1).setTo(Scalar::all(0));

}



//����ϸ��1.0
void thin(const Mat &src, Mat &dst, const int iterations)
{
	const int height = src.rows - 1;
	const int width = src.cols - 1;

	//����һ���������һ������
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
		isFinished = false;   //һ�� ���к���ɨ�� ��ʼ
		//ɨ�����һ ��ʼ
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

			} //ɨ�����һ ����


			dst.copyTo(tmpImg);
			//ɨ����̶� ��ʼ
			for (i = 1; i < height; i++)  //һ�� ���к���ɨ�� ��ʼ
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

			} //һ�� ���к���ɨ�����          
		  //�����ɨ�������û��ɾ���㣬����ǰ�˳�
			if (isFinished == false)
			{
				break;
			}
		}

	}
}