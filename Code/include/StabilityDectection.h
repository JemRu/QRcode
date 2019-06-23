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
#include <opencv2/imgproc/types_c.h>//换版本后需要多加的头文件
namespace detect {
	class StabilityDectection
	{
	private:
		std::vector<std::vector<cv::Point>> contours;//独立区域的轮廓点集
	public:
		StabilityDectection() {}
		~StabilityDectection() {}

		//独立区域检测 得到独立区域的轮廓点集
		std::vector<std::vector<cv::Point>> dectect_lable(const char* inputImage);
		
		//计算两点间的需连接程度
		double degree_connection(int piontacontourindex, int piontaindex, int piontbcontourindex, int piontbindex);

		//对于某一个点 返回其需连接的程度的集合 max min 值加入到 最后两位   inputImage 不可视化不用
		std::vector<double> piont_field_connection(const char* inputImage, int contourindex, int piontindex);

		//对于某一个轮廓返回其上各点的最大 connection 值 max min 值加入到 最后两位  inputImage 不可视化不用
		std::vector<double> contour_field_connection(const char* inputImage, int contourindex);

		//对于一幅图所有轮廓进行检测 
		std::vector<std::vector<double>> img_field_connection(const char* inputImage);

		//对于一幅图所有轮廓 每个轮廓阈值不同的 可视化 
		void visualimg_field_connectiona(const char* inputImage);

		//对于一幅图所有轮廓 每个轮廓阈值相同的 可视化 
		void visualimg_field_connectionb(const char* inputImage);

	};
}