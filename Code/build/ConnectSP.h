#pragma once
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>//换版本后需要多加的头文件
#include "MyDetector.h"
#include "QRImageInfo.h"
#include "pixelVisibility.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <vector>
#include "math.h"
using namespace std;
using namespace cv;
class ConnectSP
{
public:
	ConnectSP();
	~ConnectSP();

	//连接线宽度；
	int width;
	//获取 
	void init_width(const char* inputImage);
	//获取图像的lable;
	int get_lable(const char* inputImage);
	//获得指定某个lable下的边界像素坐标;
	void boundary_class_define(int label_id);
	//获取所有components的boundary像素坐标
	void get_all_boundary(int n);

	//计算两点之间的距离；
	double distance_between(cv::Vec2i boundary_pixel1, cv::Vec2i boundary_pixel);

	//计算不同components间的距离；
	double distance_components(int c_candidate);

	//所有不同components间的距离;
	double distance_all();

	void merge(const char* inputImage, const char* outputImage, int num);
	//void break_out(const char* inputImage, const char* outputImage);

	//每个像素的lable;
	std::vector<std::vector<int>> label;
	//conponents下的boundary坐标
	std::vector < std::vector<cv::Vec2i>> boundary_pixel;

	//min表示components的编号，即label_id。min_id记录是该components的boudary的哪一个点
	int min1, min1_id;
	int min2, min2_id;

	//g_min记录components的编号 g_min1记录components编号,g_min_id记录所有boundary上的某一点
	int g_min1, g_min1_id;
	int g_min2, g_min2_id;

};

