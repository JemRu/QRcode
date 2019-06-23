#pragma once
#include <iostream>
#include <algorithm>
#include <climits>
#include <fstream>
#include <iomanip>
#include <vector>
#include <omp.h>
#include <Version.h>
#include <Level.h>
#include "math.h"
#include "stdlib.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>//换版本后需要多加的头文件
#include "MyDetector.h"
#include "QRImageInfo.h"
#include "pixelVisibility.h"
using namespace std;
using namespace cv;
class StructureEnhance
{
private:
	int width, height;
	vector<vector<int>> label;//每个像素的lable;
	vector <vector<Vec2i>> boundary_pixel;
	int radius; //伪高斯核大小
	vector<vector<Point>> moduls;//轮廓点集 moduls[i][0]：modul的索引，moduls[i][1]：modul的中心点，moduls[i][2]：moduls乘以高斯核后 得到 0黑 1白
	vector<vector<int>> modultags; //和图像的大小一致，用于标明整个图像的mudel分配，用于检测mudel是否会被翻转.通过moduls[tagnum]找到对应的modul；
	vector<vector<int>> modulsInfo;//和图像的大小一致,用于记录model中像素的信息 0黑 1白
	int Rmax; //用于计算correction_coefficient
	int modulPixelSize;
	info::QRImageInfo imageInfo;
	//用于merge的轮廓点集越来越大
	vector<Point> contour;
	Mat inputimage;
	Mat LabelImage;
	int labelNum;
	vector<vector<Point>> contours;//轮廓点集

	int piontacontourindex;
	int piontaindex, piontbindex;

public:
	StructureEnhance();
	~StructureEnhance();
	//预处理 获得轮廓信息，获得Rmax；获得modulPixelSize；获得module集合；获得modultags集合；获得modulsInfo集合；
	Mat pre_treatment(const char* inputImage);
	//得到二维可见点集 即连接备选点集
	vector<Point> get_visibility(Point pixel1);
	//计算两点间的欧氏距离
	double Euclidean_distance(Point pixel1, Point pixel2);
	//得到两个点及其路径所改变的纠错系数
	double correction_coefficient(Point pixel1, Point pixel2);
	//计算两点间的带权距离 权重 返回A点最小的权重 ，对应的B点的所属分支索引 以及分支中的点索引 定义为结构体weight
	Point degree_distance(int x,int y);
	double evaluate(const char* inputImage);
	int Width = 3;
};