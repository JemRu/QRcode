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
#include <opencv2/imgproc/types_c.h>//���汾����Ҫ��ӵ�ͷ�ļ�
#include "MyDetector.h"
#include "QRImageInfo.h"
#include "pixelVisibility.h"
using namespace std;
using namespace cv;
struct weight {      
	double distance; //Ȩ��
	int piontAcontourindex, piontAindex, x, y, pointBTableindex; // A ������ B ��x y
};
struct weightMerge {
	double distance; //Ȩ��
	int x1, y1, x2, y2; // A ������ B ��x y
};
struct visibility {
	int x, y, lableindex; // A B ������
};
struct index {
	int piontcontourindex, piontindex; // A B ������
};
class Connection
{
private:
	
	int width, height;
	vector<vector<int>> label;//ÿ�����ص�lable;
	vector <vector<Vec2i>> boundary_pixel;
	
	vector<vector<visibility>> LabelContours;//�������� �� ��֧ �������㼯 ������ contours һ��
	int radius; //α��˹�˴�С
	vector<vector<Point>> moduls;//�����㼯 moduls[i][0]��modul��������moduls[i][1]��modul�����ĵ㣬moduls[i][2]��moduls���Ը�˹�˺� �õ� 0�� 1��
	vector<vector<int>> modultags; //��ͼ��Ĵ�Сһ�£����ڱ�������ͼ���mudel���䣬���ڼ��mudel�Ƿ�ᱻ��ת.ͨ��moduls[tagnum]�ҵ���Ӧ��modul��
	vector<vector<int>> modulsInfo;//��ͼ��Ĵ�Сһ��,���ڼ�¼model�����ص���Ϣ 0�� 1��
	vector<vector<weight>> weights;//ά������֧�����ĸ���Ȩ�صļ��� 
	int Rmax; //���ڼ���correction_coefficient
	int modulPixelSize;
	info::QRImageInfo imageInfo;
	vector<weight> contourdis; //��¼��ÿ����������С��dis
	
	//���һ������һ����˼·��Ҫ�õ��ı���
	vector<weight> contourweight;
	//����merge�������㼯Խ��Խ��
	vector<Point> contour;
	Mat inputimage;
	Mat LabelImage;
	int labelNum;
public:
	Connection();
	~Connection();
	vector<vector<Point>> contours;//�����㼯

	Mat gaussian_kernal(int dim, int sigma);
	//���ӶԽ���
	void pre_dia(const char* inputImage, const char* outImage);
	//��ʴ���� ��ֹ���������ж��Խ�
	void pre_erode(const char* inputImage);
	//Ԥ�������Rmax�����modulPixelSize�����module���ϣ����modultags���ϣ����modulsInfo���ϣ�
	Mat pre_treatment(const char* inputImage);
	//
	int get_Width(const char* inputImage);

	int get_sep(const char* inputImage);
	//��÷�֧����ø�����֧������ �浽contours������ ���ض�����֧�ĸ���
	Mat get_lable(const char* inputImage);
	//��ø���֧���� 
	void get_all_boundary(const char* inputImage);
	//�õ�һ����Ŀɼ��㼯�� ������ֵ��ʾ 
	vector<visibility> get_visibility(int piontAcontourindex, int piontAindex, const char* inputImage);


	//����������ŷ�Ͼ���
	double Euclidean_distance(Point pixel1, Point pixel2);
	//�õ������㼰��·�����ı�ľ���ϵ��
	double correction_coefficient(Point pixel1, Point pixel2, const char* inputImage);
	//���������Ĵ�Ȩ���� Ȩ�� ����A����С��Ȩ�� ����Ӧ��B���������֧���� �Լ���֧�еĵ����� ����Ϊ�ṹ��weight
	weight degree_distance(int piontAcontourindex, int piontAindex, const char* inputImage);
	//����÷�֧����СȨ�� ���ظ÷�֧����СȨ�� A B ������
	weight contour_distace(int contourindex, const char* inputImage);
	//����ͼ�����з�֧�е���СȨ�ز���ͨ �����
	void img_distace(const char* inputImage, const char* outImage); 
	//��æ������ʼ 
	void img_connect(const char* inputImage, const char* outImage);
	void img_connect2(const char* inputImage, const char* outImage);


	//���һ������һ����˼·��Ҫ�õ��ķ���

	//����õ��������ؼ���Ȩ��ֵ
	void contour_merge_distace(vector<Point> pionts);
	//�õ�һ����Ŀɼ��㼯�� ������ֵ��ʾ 
	vector<visibility> get_merge_visibility(Point point, const char* inputImage);
	weightMerge degree_merge_distance(Point point, const char* inputImage);
	//����㼯 ������µĵ㼯(�´ε�����)
	Point contour_merge(vector<Point>  points, const char* inputImage, int num);
	void img_merge(const char* inputImage);
	//������ͼ�������ͨ����

	//��ͨ�Ŀ��
	int Width = 3;

};

