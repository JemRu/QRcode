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
#include <opencv2/imgproc/types_c.h>//���汾����Ҫ��ӵ�ͷ�ļ�
#include "MyDetector.h"
#include "QRImageInfo.h"
#include "pixelVisibility.h"
using namespace std;
using namespace cv;
class StructureEnhance
{
private:
	int width, height;
	vector<vector<int>> label;//ÿ�����ص�lable;
	vector <vector<Vec2i>> boundary_pixel;
	int radius; //α��˹�˴�С
	vector<vector<Point>> moduls;//�����㼯 moduls[i][0]��modul��������moduls[i][1]��modul�����ĵ㣬moduls[i][2]��moduls���Ը�˹�˺� �õ� 0�� 1��
	vector<vector<int>> modultags; //��ͼ��Ĵ�Сһ�£����ڱ�������ͼ���mudel���䣬���ڼ��mudel�Ƿ�ᱻ��ת.ͨ��moduls[tagnum]�ҵ���Ӧ��modul��
	vector<vector<int>> modulsInfo;//��ͼ��Ĵ�Сһ��,���ڼ�¼model�����ص���Ϣ 0�� 1��
	int Rmax; //���ڼ���correction_coefficient
	int modulPixelSize;
	info::QRImageInfo imageInfo;
	//����merge�������㼯Խ��Խ��
	vector<Point> contour;
	Mat inputimage;
	Mat LabelImage;
	int labelNum;
	vector<vector<Point>> contours;//�����㼯

	int piontacontourindex;
	int piontaindex, piontbindex;

public:
	StructureEnhance();
	~StructureEnhance();
	//Ԥ���� ���������Ϣ�����Rmax�����modulPixelSize�����module���ϣ����modultags���ϣ����modulsInfo���ϣ�
	Mat pre_treatment(const char* inputImage);
	//�õ���ά�ɼ��㼯 �����ӱ�ѡ�㼯
	vector<Point> get_visibility(Point pixel1);
	//����������ŷ�Ͼ���
	double Euclidean_distance(Point pixel1, Point pixel2);
	//�õ������㼰��·�����ı�ľ���ϵ��
	double correction_coefficient(Point pixel1, Point pixel2);
	//���������Ĵ�Ȩ���� Ȩ�� ����A����С��Ȩ�� ����Ӧ��B���������֧���� �Լ���֧�еĵ����� ����Ϊ�ṹ��weight
	Point degree_distance(int x,int y);
	double evaluate(const char* inputImage);
	int Width = 3;
};