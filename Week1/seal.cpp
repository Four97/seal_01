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
			if (S >=15)
			{
				if ((H >= 0 && H < 24) || H >= 240)
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


//锐化
void sharpen(const Mat& img, Mat& result)

{

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

//轮廓细化2.0
Mat thinImage(const Mat & src, const int maxIterations = -1)
{
	assert(src.type() == CV_8UC1);
	Mat dst;
	int width = src.cols;
	int height = src.rows;
	src.copyTo(dst);
	int count = 0;  //记录迭代次数  
	while (true)
	{
		count++;
		if (maxIterations != -1 && count > maxIterations) //限制次数并且迭代次数到达  
			break;
		std::vector<uchar *> mFlag; //用于标记需要删除的点  
		//对点标记  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记  
				//  p9 p2 p3  
				//  p8 p1 p4  
				//  p7 p6 p5  
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);
				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0)
					{
						//标记  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空  
		}

		//对点标记  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记  
				//  p9 p2 p3  
				//  p8 p1 p4  
				//  p7 p6 p5  
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
					{
						//标记  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空  
		}
	}
	return dst;
}

//提取轮廓
//int Con()
//{
//	IplImage* img = cvLoadImage("lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	IplImage* img_temp = cvCreateImage(cvGetSize(img), 8, 1);
//
//	cvThreshold(img, img, 128, 255, CV_THRESH_BINARY);
//
//	CvMemStorage* mem_storage = cvCreateMemStorage(0);
//	CvSeq *first_contour = NULL, *c = NULL;
//
//
//	//////////////////////////////////////////////////////////////////////////  
//	// 1、  
//	cvNamedWindow("contour1");
//	cvCopyImage(img, img_temp);
//	double t = (double)cvGetTickCount();
//	cvFindContours(img_temp, mem_storage, &first_contour);
//	cvZero(img_temp);
//	cvDrawContours(
//		img_temp,
//		first_contour,
//		cvScalar(100),
//		cvScalar(100),
//		1
//	);
//	t = (double)cvGetTickCount() - t;
//	cvShowImage("contour1", img_temp);
//
//	printf("run1 = %gms\n", t / (cvGetTickFrequency()*1000.));
//
//	cvClearMemStorage(mem_storage);
//
//
//	//////////////////////////////////////////////////////////////////////////  
//	// 2、  
//	cvNamedWindow("contour2");
//	cvCopyImage(img, img_temp);
//	t = (double)cvGetTickCount();
//	CvContourScanner scanner = cvStartFindContours(img_temp, mem_storage);
//	while (cvFindNextContour(scanner));
//	first_contour = cvEndFindContours(&scanner);
//
//	cvZero(img_temp);
//	cvDrawContours(
//		img_temp,
//		first_contour,
//		cvScalar(100),
//		cvScalar(100),
//		1
//	);
//	t = (double)cvGetTickCount() - t;
//	cvShowImage("contour2", img_temp);
//
//	printf("run2 = %gms\n", t / (cvGetTickFrequency()*1000.));
//
//	cvClearMemStorage(mem_storage);
//	cvReleaseImage(&img);
//	cvReleaseImage(&img_temp);
//
//	cvWaitKey();
//
//	/************************************************************************/
//	/* 经测试 run1 = 16.1431ms run2 = 15.8677ms (参考)
//	   不过可以肯定这两中算法时间复杂度是相同的                                     */
//	   /************************************************************************/
//
//	   //////////////////////////////////////////////////////////////////////////  
//	   // 上述两种方法完成了对轮廓的提取,如想绘制轮廓都得配合cvDrawContours来使用  
//	   // 而cvDrawContours 函数第5个参数为 max_level 经查ICVL含义如下:  
//	   //  
//	   // 绘制轮廓的最大等级。如果等级为0，绘制单独的轮廓。如果为1，绘制轮廓及在其后的相同的级别下轮廓。  
//	   // 如果值为2，所有的轮廓。如果等级为2，绘制所有同级轮廓及所有低一级轮廓，诸此种种。如果值为负数，  
//	   // 函数不绘制同级轮廓，但会升序绘制直到级别为abs(max_level)-1的子轮廓。  
//	   //  
//	   // 相信好多读者初次都无法理解等级的含义,而且测试时候输入>=1 的整数效果几乎一样  
//	   // 只有提取轮廓时候的提取模式设为 CV_RETR_CCOMP CV_RETR_TREE 时这个参数才有意义  
//	   //  
//	   // 经查FindContours 函数里面这样介绍提取模式(mode)的这两个参数:  
//	   // CV_RETR_CCOMP - 提取所有轮廓，并且将其组织为两层的 hierarchy: 顶层为连通域的外围边界，次层为洞的内层边界。   
//	   // CV_RETR_TREE - 提取所有轮廓，并且重构嵌套轮廓的全部 hierarchy   
//	   //   
//	   // 下面用第一种方法进行测试  
//
//	cvNamedWindow("contour_test");
//	cvNamedWindow("contour_raw");
//	img = cvLoadImage("contour.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	cvShowImage("contour_raw", img);
//	cvThreshold(img, img, 128, 255, CV_THRESH_BINARY);
//	img_temp = cvCloneImage(img);
//	cvFindContours(
//		img_temp,
//		mem_storage,
//		&first_contour,
//		sizeof(CvContour),
//		CV_RETR_CCOMP           //#1 需更改区域  
//	);
//
//	cvZero(img_temp);
//	cvDrawContours(
//		img_temp,
//		first_contour,
//		cvScalar(100),
//		cvScalar(100),
//		1                       //#2 需更改区域  
//	);
//	cvShowImage("contour_test", img_temp);
//	/************************************************************************/
//	/* (1, 2) = (CV_RETR_CCOMP, 1)  如图1
//	   (1, 2) = (CV_RETR_CCOMP, 2)  如图2
//	   (1, 2) = (CV_RETR_TREE, 1)   如图3
//	   (1, 2) = (CV_RETR_TREE, 2)   如图4
//	   (1, 2) = (CV_RETR_TREE, 6)   如图5
//	   经分析CV_RETR_CCOMP 只把图像分为两个层次,顶层和次层,一等级轮廓只匹配与其最接近
//	   的内侧轮廓即2等级
//	   CV_RETR_TREE 则从轮廓外到内按等级1 - n 全部分配
//	   CV_RETR_LIST 全部轮廓均为1级                        */
//	   /************************************************************************/
//
//	cvWaitKey();
//	cvReleaseImage(&img);
//	cvReleaseImage(&img_temp);
//	cvReleaseMemStorage(&mem_storage);
//	cvDestroyAllWindows();
//	return 0;
//}








int main()
{






	const char* imagename = "5.jpg";



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

	////锐化 引入了很多噪声 效果不好
	//Mat img_s;
	//sharpen(src, img_s);
	//imshow("sharpen", img_s);

	//转HSI
	rgbtohsi(src);
	Mat  output_gray, output_red;
	filteredRed(src, output_gray, output_red);//过滤红色




	//反色、细化、再反色
	Mat out_thin;
	bitwise_not(output_gray, output_gray);
	//out_thin = thinImage(output_gray);
	thin(output_gray, out_thin, 50);
	bitwise_not(out_thin, out_thin);
	imshow("4", out_thin);

	Mat out_f;
	bilateralFilter(out_thin, out_f, 50, 50 * 2, 50 / 2);//双边滤波
	imshow("5", out_f);



	//开运算,如果需要印章里面的字体，就不能用开闭运算
	//Mat img_open;
	//Mat element(5, 5, CV_8U, Scalar(1));
	//morphologyEx(out_f, img_open, MORPH_OPEN, element);
	//imshow("open", img_open);
	


	//提取轮廓
	//vector<vector<Point>> contours;
	//vector<Vec4i> hierarchy;
	//Mat drawImage = Mat::zeros(out_f.size(), CV_8UC3);
	//findContours(output_gray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//drawContours(drawImage, contours, 1, Scalar(255, 255, 255), 2);
	//for (int i = 0; i < (int)contours.size(); i++)
	//{
	//	drawContours(drawImage, contours, i, Scalar(255, 255, 255), 2);
	//}
	//imshow("contour_test", drawImage);




	vector<Vec3f> circles;
	HoughCircles(out_f, circles, CV_HOUGH_GRADIENT, 1.5, out_f.rows / 20, 200, 60, 0, 0);
	//依次在图中绘制出圆
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		//绘制圆心
		circle(out_f, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//绘制圆轮廓
		circle(out_f, center, radius, Scalar(155, 50, 255), 3, 8, 0);
		cout << "center: " << center << endl;
		cout << "Radius : " << radius << endl;

	}
	
	imshow("【效果图】", out_f);








	

	




	//此函数等待按键，按键盘任意键就返回

	waitKey();

	return 0;


}








// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
