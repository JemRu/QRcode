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
#include <iomanip>
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
struct weight {      
	double distance; //权重
	int piontAcontourindex, piontAindex, x, y, pointBTableindex; // A 的索引 B 的x y
};
struct weightMerge {
	double distance; //权重
	int x1, y1, x2, y2; // A 的索引 B 的x y
};
struct visibility {
	int x, y, lableindex; // A B 的索引
};
struct index {
	int piontcontourindex, piontindex; // A B 的索引
};
class Connection
{
private:
	
	int width, height;
	vector<vector<int>> label;//每个像素的lable;
	vector <vector<Vec2i>> boundary_pixel;
	
	vector<vector<visibility>> LabelContours;//独立区域 即 分支 的轮廓点集 索引与 contours 一致
	int radius; //伪高斯核大小
	vector<vector<Point>> moduls;//轮廓点集 moduls[i][0]：modul的索引，moduls[i][1]：modul的中心点，moduls[i][2]：moduls乘以高斯核后 得到 0黑 1白
	vector<vector<int>> modultags; //和图像的大小一致，用于标明整个图像的mudel分配，用于检测mudel是否会被翻转.通过moduls[tagnum]找到对应的modul；
	vector<vector<int>> modulsInfo;//和图像的大小一致,用于记录model中像素的信息 0黑 1白
	vector<vector<weight>> weights;//维护各分支轮廓的各点权重的集合 
	int Rmax; //用于计算correction_coefficient
	int modulPixelSize;
	info::QRImageInfo imageInfo;
	vector<weight> contourdis; //记录着每个轮廓的最小的dis
	
	//检测一个连接一个的思路需要用到的变量
	vector<weight> contourweight;
	//用于merge的轮廓点集越来越大
	vector<Point> contour;
	Mat inputimage;
	Mat LabelImage;
	int labelNum;
public:
	Connection();
	~Connection();
	vector<vector<Point>> contours;//轮廓点集

	Mat gaussian_kernal(int dim, int sigma);
	//连接对角线
	void pre_dia(const char* inputImage, const char* outImage);
	//腐蚀操作 防止可视区域判断自交
	void pre_erode(const char* inputImage);
	//预处理，获得Rmax；获得modulPixelSize；获得module集合；获得modultags集合；获得modulsInfo集合；
	Mat pre_treatment(const char* inputImage);
	//
	int get_Width(const char* inputImage);

	int get_sep(const char* inputImage);
	//获得分支，获得各个分支的轮廓 存到contours数组中 返回独立分支的个数
	Mat get_lable(const char* inputImage);
	//获得各分支轮廓 
	void get_all_boundary(const char* inputImage);
	//得到一个点的可见点集合 用索引值表示 
	vector<visibility> get_visibility(int piontAcontourindex, int piontAindex, const char* inputImage);


	//计算两点间的欧氏距离
	double Euclidean_distance(Point pixel1, Point pixel2);
	//得到两个点及其路径所改变的纠错系数
	double correction_coefficient(Point pixel1, Point pixel2, const char* inputImage);
	//计算两点间的带权距离 权重 返回A点最小的权重 ，对应的B点的所属分支索引 以及分支中的点索引 定义为结构体weight
	weight degree_distance(int piontAcontourindex, int piontAindex, const char* inputImage);
	//计算该分支的最小权重 返回该分支的最小权重 A B 的索引
	weight contour_distace(int contourindex, const char* inputImage);
	//计算图像所有分支中的最小权重并连通 并输出
	void img_distace(const char* inputImage, const char* outImage); 
	//死忙迭代开始 
	void img_connect(const char* inputImage, const char* outImage);
	void img_connect2(const char* inputImage, const char* outImage);


	//检测一个连接一个的思路需要用到的方法

	//计算得到传入像素集合权重值
	void contour_merge_distace(vector<Point> pionts);
	//得到一个点的可见点集合 用索引值表示 
	vector<visibility> get_merge_visibility(Point point, const char* inputImage);
	weightMerge degree_merge_distance(Point point, const char* inputImage);
	//输入点集 输出更新的点集(下次的输入)
	Point contour_merge(vector<Point>  points, const char* inputImage, int num);
	void img_merge(const char* inputImage);
	//对输入图像进行连通处理

	//连通的宽度
	int Width = 3;

};

