#pragma once



#ifndef DECTELLIPSE_H
#define DECTELLIPSE_H

// TODO: ���Ҫ�ڴ˴�Ԥ����ı�ͷ

bool checkEllipseShape(Mat src, vector<Point> contour, RotatedRect ellipse, double ratio);
void dectEllipse(Mat src, Mat img);

#endif //DECTELLIPSE_H