#pragma once



#ifndef DECTELLIPSE_H
#define DECTELLIPSE_H

// TODO: 添加要在此处预编译的标头

bool checkEllipseShape(Mat src, vector<Point> contour, RotatedRect ellipse, double ratio);
void dectEllipse(Mat src, Mat img);

#endif //DECTELLIPSE_H