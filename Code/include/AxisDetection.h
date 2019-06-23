#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <queue>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>//���汾����Ҫ��ӵ�ͷ�ļ�

namespace detect {
	class AxisDetection {
	public:
		AxisDetection() {}
		~AxisDetection() {}

		//�õ��߽�ֵ
		void get_boundary(int n);
		void get_diff_boundary(int n, const char* inputImage);
		//������볤
		void  disboufield(const char* inputImage);
		void  visual_field(const char* inputImage);
		//��������
		void  visual_field_Laplacian(const char* inputImage, const char* inputImage2);


	};
}
