#pragma once
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>//���汾����Ҫ��ӵ�ͷ�ļ�
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

	//�����߿�ȣ�
	int width;
	//��ȡ 
	void init_width(const char* inputImage);
	//��ȡͼ���lable;
	int get_lable(const char* inputImage);
	//���ָ��ĳ��lable�µı߽���������;
	void boundary_class_define(int label_id);
	//��ȡ����components��boundary��������
	void get_all_boundary(int n);

	//��������֮��ľ��룻
	double distance_between(cv::Vec2i boundary_pixel1, cv::Vec2i boundary_pixel);

	//���㲻ͬcomponents��ľ��룻
	double distance_components(int c_candidate);

	//���в�ͬcomponents��ľ���;
	double distance_all();

	void merge(const char* inputImage, const char* outputImage, int num);
	//void break_out(const char* inputImage, const char* outputImage);

	//ÿ�����ص�lable;
	std::vector<std::vector<int>> label;
	//conponents�µ�boundary����
	std::vector < std::vector<cv::Vec2i>> boundary_pixel;

	//min��ʾcomponents�ı�ţ���label_id��min_id��¼�Ǹ�components��boudary����һ����
	int min1, min1_id;
	int min2, min2_id;

	//g_min��¼components�ı�� g_min1��¼components���,g_min_id��¼����boundary�ϵ�ĳһ��
	int g_min1, g_min1_id;
	int g_min2, g_min2_id;

};

