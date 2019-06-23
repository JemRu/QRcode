#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <queue>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>//换版本后需要多加的头文件

namespace detect {
	class AxisDetection {
	public:
		AxisDetection() {}
		~AxisDetection() {}

		//得到边界值
		void get_boundary(int n);
		void get_diff_boundary(int n, const char* inputImage);
		//计算距离长
		void  disboufield(const char* inputImage);
		void  visual_field(const char* inputImage);
		//计算中轴
		void  visual_field_Laplacian(const char* inputImage, const char* inputImage2);


	};
}
