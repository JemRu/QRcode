#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
//��Ҫ���õ�ͷ�ļ�ֻ��QRImageInfo.h��һ��
#include "QRImageInfo.h"

namespace info {
	void  example() {
		std::string image_name = "G:/project/QRdecoder/data/code4_c.png";
		cv::Mat matSrc = cv::imread(image_name, 1);
		cv::Mat matGray;
		cv::cvtColor(matSrc, matGray, CV_BGR2GRAY);

		/*
		���ȳ�ʼ��QRImageInfo����Ҷ�ͼ��QRImageInfo�Ѿ�����Version��Error Correct Level,pixels,modules,codewords��blocks����Ϣ
		*/
		info::QRImageInfo imageInfo(matGray);
		/*
		Version��Error Correct Level�ǹ��г�Ա����������ֱ�ӻ��
		*/
		std::cout << "Version:" << imageInfo.version->getVersion() << std::endl;
		std::cout << "Level:" << imageInfo.level << std::endl;

		/*
		Ϊ�˱�֤��ȫ��pixels,modules,codewords��blocks����Ϣ��Ϊ˽�г�Ա����������codewords��ȫ���ɼ�
		pixels�Ĵ�С��image��ȫ��ͬ��ÿ��module�����ĵ����ص���Ӧ��modules�еģ�y,x�����꣬�����Ϊ-1
		*/
		std::cout << "Module Center" << std::endl;
		int height = matGray.rows;
		int width = matGray.cols;

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				std::pair<int, int> pos = imageInfo.getPixel(y, x);//�������module center���أ�-1��-1��,���򷵻���modules�ж�Ӧmodule��λ�� 
				if (pos.first != -1)
					//pos.first ����    ...       y ����
					std::cout << "Module:" << pos.first << " " << pos.second << " Center:" << y << " " << x << std::endl;
			}
		}
		/*
		����modules����,�ɻ�ȡ������������һ����modules size ��ά������С����һ����module roleΪÿ��module�Ľ�ɫ��Role��10��,����DATA,CHECK���ܹ���ʧ�ģ����಻��
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
		qrgen::Pixel::PixelRole role = imageInfo.getModuleRole(0, 0);//modules(0,0)��module��role

																	 /*����block����ɻ�ȡ���������� block size��ÿ��block������*/
		std::cout << "Block Size:" << imageInfo.getBlockSize() << std::endl;
		for (int i = 0; i < imageInfo.getBlockSize(); i++) std::cout << "Block " << i << " rest:" << imageInfo.getBlock(i) << std::endl;

		/*
		����֮�⻹����������tryModule(y,x)��invertModule(y,x)
		tryModule�ǲ����޸�modules(y,x)��block�ı仯���������Ԫ��<bool,int,int>�ֱ�Ϊmodules(y,x)����blocks�Ƿ����ı䣬����block��index������block������rΪ����
		inverModuleΪֱ���޸�modules,codewords,blocks��û�з���ֵ������tryModule ��invertModule���ʹ�ã��Ȳ��Թ۲�仯��Ȼ���޸ġ�
		*/
		//�Ƿ��� block��1 blockindex blockrest
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