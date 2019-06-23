#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
//需要引用的头文件只有QRImageInfo.h这一个
#include "QRImageInfo.h"

namespace info {
	void  example() {
		std::string image_name = "G:/project/QRdecoder/data/code4_c.png";
		cv::Mat matSrc = cv::imread(image_name, 1);
		cv::Mat matGray;
		cv::cvtColor(matSrc, matGray, CV_BGR2GRAY);

		/*
		首先初始化QRImageInfo输入灰度图，QRImageInfo已经包含Version，Error Correct Level,pixels,modules,codewords，blocks的信息
		*/
		info::QRImageInfo imageInfo(matGray);
		/*
		Version和Error Correct Level是公有成员变量，可以直接获得
		*/
		std::cout << "Version:" << imageInfo.version->getVersion() << std::endl;
		std::cout << "Level:" << imageInfo.level << std::endl;

		/*
		为了保证安全，pixels,modules,codewords，blocks的信息都为私有成员函数，其中codewords完全不可见
		pixels的大小与image完全相同，每个module的中心的像素点存对应在modules中的（y,x）坐标，其余点为-1
		*/
		std::cout << "Module Center" << std::endl;
		int height = matGray.rows;
		int width = matGray.cols;

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				std::pair<int, int> pos = imageInfo.getPixel(y, x);//如果不是module center返回（-1，-1）,否则返回在modules中对应module的位置 
				if (pos.first != -1)
					//pos.first 索引    ...       y 坐标
					std::cout << "Module:" << pos.first << " " << pos.second << " Center:" << y << " " << x << std::endl;
			}
		}
		/*
		对于modules层面,可获取参数有两个，一个是modules size 二维码矩阵大小，另一个是module role为每个module的角色，Role有10种,其中DATA,CHECK是能够损失的，其余不详
		UNKNOWN, //not used
		POSITION, //position pattern
		ALIGNMENT,
		TIMING,
		FORMAT,
		VERSION_PATTERN,
		UNUSED,
		DATA,
		CHECK,
		EXTRA
		*/
		std::cout << "Modules Size:" << imageInfo.getModuleSize() << std::endl;
		qrgen::Pixel::PixelRole role = imageInfo.getModuleRole(0, 0);//modules(0,0)的module的role

																	 /*对于block层面可获取参数有两个 block size和每个block的余量*/
		std::cout << "Block Size:" << imageInfo.getBlockSize() << std::endl;
		for (int i = 0; i < imageInfo.getBlockSize(); i++) std::cout << "Block " << i << " rest:" << imageInfo.getBlock(i) << std::endl;

		/*
		除此之外还有两个方法tryModule(y,x)和invertModule(y,x)
		tryModule是测试修改modules(y,x)后block的变化情况返回三元数<bool,int,int>分别为modules(y,x)所在blocks是否发生改变，所在block的index，所在block的余量r为多少
		inverModule为直接修改modules,codewords,blocks，没有返回值，所以tryModule 和invertModule配合使用，先测试观察变化，然后修改。
		*/
		//是否有 block减1 blockindex blockrest
		/*std::tuple<bool, int, int> status = imageInfo.tryModule(15, 15);
		if (std::get<0>(status)) {
			std::cout << "If invert module(15,15)," << "Block" << std::get<1>(status) << " will be changed,rest " << std::get<2>(status) << std::endl;
		}
		else {
			std::cout << "If invert module(15,15)," << "Block" << std::get<1>(status) << " will not changed,rest " << std::get<2>(status) << std::endl;
		}
		imageInfo.invertModule(15, 15);*/
	}
}