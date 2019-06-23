#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <queue>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/types_c.h>//���汾����Ҫ��ӵ�ͷ�ļ�
namespace detect {
	class StabilityDectection
	{
	private:
		std::vector<std::vector<cv::Point>> contours;//��������������㼯
	public:
		StabilityDectection() {}
		~StabilityDectection() {}

		//���������� �õ���������������㼯
		std::vector<std::vector<cv::Point>> dectect_lable(const char* inputImage);
		
		//���������������ӳ̶�
		double degree_connection(int piontacontourindex, int piontaindex, int piontbcontourindex, int piontbindex);

		//����ĳһ���� �����������ӵĳ̶ȵļ��� max min ֵ���뵽 �����λ   inputImage �����ӻ�����
		std::vector<double> piont_field_connection(const char* inputImage, int contourindex, int piontindex);

		//����ĳһ�������������ϸ������� connection ֵ max min ֵ���뵽 �����λ  inputImage �����ӻ�����
		std::vector<double> contour_field_connection(const char* inputImage, int contourindex);

		//����һ��ͼ�����������м�� 
		std::vector<std::vector<double>> img_field_connection(const char* inputImage);

		//����һ��ͼ�������� ÿ��������ֵ��ͬ�� ���ӻ� 
		void visualimg_field_connectiona(const char* inputImage);

		//����һ��ͼ�������� ÿ��������ֵ��ͬ�� ���ӻ� 
		void visualimg_field_connectionb(const char* inputImage);

	};
}