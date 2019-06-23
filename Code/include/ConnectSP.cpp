#include "ConnectSP.h"
ConnectSP::ConnectSP()
{
}
ConnectSP::~ConnectSP()
{
}
std::ofstream output_info("data/output_info.txt");
//获取lable，不同lable表示不同的components

//获取尺寸信息，放在cgal程序下
std::ofstream size_info("data/size_info.txt");

void ConnectSP::init_width(const char* inputImage) {

	Mat image = imread(inputImage);
	cv::Mat matGray;
	cv::cvtColor(image, matGray, CV_BGR2GRAY);

	info::QRImageInfo imageInfo;
	imageInfo = info::QRImageInfo(matGray);

	//确定对角线宽度
	int moduleSize = imageInfo.moduleSize;
	int connector = 0.7 * moduleSize;
	width = connector;
}

int ConnectSP::get_lable(const char* inputImage) {

	cv::Mat srcImg = imread(inputImage, cv::IMREAD_COLOR);
	//cv::bitwise_xor(srcImg, cv::Scalar(255, 255, 255), srcImg);
	cv::Mat gray;

	cv::cvtColor(srcImg, gray, CV_BGR2GRAY);

	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (gray < threshval) : (gray > threshval);

	//反相处理：
	cv::bitwise_xor(bin, cv::Scalar(255, 255, 255), bin);
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);

	//int nLabels = cv::connectedComponents(bin, labelImage, 8);
	int nLabels = connectedComponents(bin, labelImage, 4);


	std::vector<cv::Vec3b> colors(nLabels);
	colors[0] = cv::Vec3b(0, 0, 0);

	for (int label = 1; label < nLabels; label++)
	{
		colors[label] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
		//output_info << "Component " << label << std::endl;

	}

	cv::Mat dst(srcImg.size(), CV_8UC3);

	for (int r = 0; r < dst.rows; r++)
	{
		std::vector<int> cur_vector;
		for (int c = 0; c < dst.cols; c++)
		{
			int cur_lable = labelImage.at<int>(r, c);
			cv::Vec3b &pixel = dst.at<cv::Vec3b>(r, c);
			pixel = colors[cur_lable];

			cur_vector.push_back(cur_lable);
		}
		label.push_back(cur_vector);
	}

	std::ofstream file("data/labled_image.txt");
	for (int i = 0; i < label.size(); i++)
	{
		for (int j = 0; j < label[i].size(); j++)
		{
			file << " " << label[i][j];
		}
		file << "" << std::endl;
		//cout << label[i].size() << endl;
	}
	//cout << label.size()<<endl;

	imshow("Source", srcImg);
	imshow("2", dst);
	imwrite("colores_lable.png", dst);


	return nLabels;
}

//获取指定lable下的像素
void ConnectSP::boundary_class_define(int label_id) {
	int current_lable = label_id;
	//传入实参（给定lable编号）时，获取boundary的像素值
	std::ofstream out_boundary("data/boundary_pixel.txt");
	//  label.size()是列数，相当于y

	std::vector < cv::Vec2i> point;
	for (int i = 1; i < label.size() - 1; i++)
	{
		//label[[i].size()是行数，相当于y
		for (int j = 1; j < label[i].size() - 1; j++)
		{
			if (label[i][j] == current_lable)
			{
				if ((label[i - 1][j] == 0 || label[i][j - 1] == 0) || (label[i + 1][j] == 0 || label[i][j + 1] == 0))
				{

					//boundary.push_back(j);
					//boundary_pixel.push_back(boundary);
					out_boundary << "(" << i << "," << j << ")" << std::endl;
					cv::Vec2i v(i, j);
					point.push_back(v);

				}
			}
		}
	};
	boundary_pixel.push_back(point);
}

//获取所有components的boundary像素 
void ConnectSP::get_all_boundary(int n) {
	for (int i = 1; i < n; i++) {
		boundary_class_define(i);
	}
	//输出用于化简得轮廓
	std::ofstream contours("input201708/countours_all.poly");
	//std::ofstream file2("data_info/boundary_info.txt");
	int temp = boundary_pixel.size() + 1;
	for (int i = 0; i < boundary_pixel.size(); i++)
	{
		/*******************jem**********************/
		//perimeter.push_back(boundary_pixel[i].size());
		/*******************jem**********************/
		int num = boundary_pixel[i].size();
		//file2 << num << " " << "in" << std::endl;
		contours << "POLYGON" << std::endl;
		for (int j = 0; j < boundary_pixel[i].size(); j++) {
			//file2 << i + 1 << " ";
			//file2 << boundary_pixel[i][j][0] << " " << boundary_pixel[i][j][1] << std::endl;
			contours << boundary_pixel[i][j][0] << " " << boundary_pixel[i][j][1] << std::endl;
		}
		for (int q = 1; q < num; q++)
		{
			//file2 << q << " ";
		}

		//file2 << num << std::endl;
	}
}

double ConnectSP::distance_between(cv::Vec2i boundary_pixel1, cv::Vec2i boundary_pixel2)
{
	int y1 = boundary_pixel1[1];
	int x1 = boundary_pixel1[0];

	int y2 = boundary_pixel2[1];
	int x2 = boundary_pixel2[0];

	double distance = sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
	return distance;
}

double ConnectSP::distance_components(int c_candidate) {

	double min_distance = INFINITY;
	int c_min = boundary_pixel.size() - 1;
	for (int i = 0; i < boundary_pixel[c_min].size(); i++) {
		for (int j = 0; j < boundary_pixel[c_candidate].size(); j++) {
			double temp_d = distance_between(
				boundary_pixel[c_min][i],
				boundary_pixel[c_candidate][j]);
			if (temp_d < min_distance) {
				min_distance = temp_d;
				min1 = c_min;
				min1_id = i;
				min2 = c_candidate;
				min2_id = j;
			}
		}
	}
	return min_distance;
}


double ConnectSP::distance_all()
{
	double global_min = INFINITY;
	for (int i = 0; i < boundary_pixel.size() - 1; i++)
	{
		double min_all = distance_components(i);
		if (min_all < global_min)
		{
			global_min = min_all;
			g_min1 = min1;
			g_min1_id = min1_id;
			g_min2 = min2;
			g_min2_id = min2_id;

		}

	}
	output_info << g_min1 << "," << g_min1_id << std::endl;
	output_info << g_min2 << "," << g_min2_id << std::endl;

	return global_min;
}


void ConnectSP::merge(const char* inputImage, const char* outputImage, int num) {
	cv::Mat srcImg = imread(inputImage, cv::IMREAD_COLOR);
	cv::Vec2i start_point = boundary_pixel[g_min1][g_min1_id];
	cv::Vec2i end_point = boundary_pixel[g_min2][g_min2_id];
	cv::Point sp(start_point[1], start_point[0]), ep(end_point[1], end_point[0]);
	output_info << sp << std::endl;
	output_info << ep << std::endl;


	//如果采用白色像素全部相连。
	cv::line(srcImg, sp, ep, cv::Scalar(255, 255, 255), num);

	//imshow("merge", srcImg);
	cv::imwrite(outputImage, srcImg);
	//imwrite("E:\paper_cut_complex\output.png", srcImg);
}
