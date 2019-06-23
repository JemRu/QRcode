#include "AxisDetection.h"
using namespace cv;
using namespace std;

void detect::AxisDetection::get_boundary(int n)
{
}

void detect::AxisDetection::get_diff_boundary(int n, const char * inputImage)
{
}

void detect::AxisDetection::disboufield(const char * inputImage)
{

	Mat src = imread(inputImage, CV_8UC1);
	Mat out = imread(inputImage);
	//cv::bitwise_xor(src, cv::Scalar(255, 255, 255), src);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	Mat Laplacian;
	cv::cvtColor(drawing, Laplacian, CV_BGR2GRAY);
	Mat Laplacianed;
	cv::cvtColor(drawing, Laplacianed, CV_BGR2GRAY);
	vector<uchar> colors(2);
	colors[0] = uchar(0);


	findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	Mat raw_dist_l_all(src.size(), CV_32FC1);
	Mat raw_dist_all(src.size(), CV_32FC1);
	for (int j = 0; j < src.rows; j++)
	{
		for (int i = 0; i < src.cols; i++)
		{
			raw_dist_l_all.at<float>(j, i) = -1;
			raw_dist_all.at<float>(j, i) = -1;
		}
	}

	for (int index = 0; index < contours.size(); index++)
	{
		//计算顶点到轮廓的距离
		Mat raw_dist(src.size(), CV_32FC1);
		Mat raw_dist_l(src.size(), CV_32FC1);

		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				raw_dist.at<float>(j, i) = pointPolygonTest(contours[index], Point2f(i, j), true);
				raw_dist_l.at<float>(j, i) = pointPolygonTest(contours[index], Point2f(i, j), true);
			}
		}

		double minVal; double maxVal;
		minMaxLoc(raw_dist, &minVal, &maxVal, 0, 0, Mat());
		minVal = abs(minVal); maxVal = abs(maxVal);

		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				//在外部
				if (raw_dist.at<float>(j, i) < 0)
				{
					//file2 << "-1" << " " ;
				}
				//在内部
				else if (raw_dist.at<float>(j, i) > 0)
				{

					float k = raw_dist.at<float>(j, i);
					raw_dist_all.at<float>(j, i) = k / maxVal;

					float temp = (4 * k - (raw_dist.at<float>(j + 1, i) + raw_dist.at<float>(j - 1, i) + raw_dist.at<float>(j, i + 1) + raw_dist.at<float>(j, i - 1))) / maxVal;

					raw_dist_l.at<float>(j, i) = temp;
					raw_dist_l_all.at<float>(j, i) = temp;

				}
				else
				{
					raw_dist_l_all.at<float>(j, i) = 0;
					raw_dist_all.at<float>(j, i) = 0;
				}
			}
		}

		//用户型化的方式显示距离

		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				//在外部
				if (raw_dist.at<float>(j, i) < 0)
				{
					//drawing.at<Vec3b>(j, i)[0] = 255 - (int)abs(raw_dist.at<float>(j, i)) * 255 / minVal;
				}
				//在内部
				else if (raw_dist.at<float>(j, i) > 0)
				{
					drawing.at<Vec3b>(j, i)[2] = 255 - (int)raw_dist.at<float>(j, i) * 255 / maxVal;

					out.at<Vec3b>(j, i)[0] = 0;
					out.at<Vec3b>(j, i)[1] = 0;
					out.at<Vec3b>(j, i)[2] = 255 - (int)raw_dist.at<float>(j, i) * 255 / maxVal;

					float gary = (int)raw_dist.at<float>(j, i) * 255 / maxVal;

					//cout << "gray" << (int)raw_dist.at<float>(j, i) * 255 / maxVal << endl;
					//cout << "float" << (int)raw_dist.at<float>(j, i) / maxVal << endl;

					colors[1] = uchar(gary);
					//Laplacian.at<uchar>(j, i) = colors[1];
					uchar &pixel = Laplacian.at<uchar>(j, i);
					pixel = colors[1];
				}
				else
					// 在边上
				{
					drawing.at<Vec3b>(j, i)[0] = 255; drawing.at<Vec3b>(j, i)[1] = 255; drawing.at<Vec3b>(j, i)[2] = 255;
					out.at<Vec3b>(j, i)[0] = 0;
					out.at<Vec3b>(j, i)[1] = 0;
					out.at<Vec3b>(j, i)[2] = 255;
					uchar &pixel = Laplacian.at<uchar>(j, i);
					pixel = colors[0];
				}
			}
		}

		double minVal_; double maxVal_;
		minMaxLoc(raw_dist_l, &minVal_, &maxVal_, 0, 0, Mat());
		minVal_ = abs(minVal_); maxVal_ = abs(maxVal_);

		//cout << "maxVal_" << maxVal_ << endl;

		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				//在外部
				if (raw_dist_l.at<float>(j, i) < 0)
				{
					//drawing.at<Vec3b>(j, i)[0] = 255 - (int)abs(raw_dist.at<float>(j, i)) * 255 / minVal;
				}
				//在内部
				else if (raw_dist_l.at<float>(j, i) > 0)
				{
					//灰度
					int gray = raw_dist_l.at<float>(j, i) * 255 / maxVal_;
					colors[1] = uchar(gray);
					//Laplacian.at<uchar>(j, i) = colors[1];
					uchar &pixel = Laplacianed.at<uchar>(j, i);
					pixel = colors[1];
				}
				else
					// 在边上
				{
					uchar &pixel = Laplacianed.at<uchar>(j, i);
					pixel = colors[0];
				}
			}
		}
	}

	//data output
	std::ofstream file("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/label_float.txt");

	for (int j = 0; j < src.rows; j++)
	{
		for (int i = 0; i < src.cols; i++)
		{
			file << raw_dist_all.at<float>(j, i) << " ";
		}
		file << "" << endl;
	}

	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/diff_Lfield.png", Laplacian);
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/diff_Ledfield.png", Laplacianed);
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/visual_field.png", out);
}

void detect::AxisDetection::visual_field(const char * inputImage)
{
}

void detect::AxisDetection::visual_field_Laplacian(const char * inputImage, const char * inputImage2)
{
	//Laplacianed可视化
	Mat out_Lap = imread(inputImage);
	Mat out_axis = imread(inputImage);
	Mat img_diff_f = imread(inputImage);

	Mat img_Ledfield = imread(inputImage2);
	for (int i = 0; i < img_diff_f.rows; i++)
	{
		for (int j = 0; j < img_diff_f.cols; j++)
		{
			if (img_diff_f.at<Vec3b>(i, j)[0] != 0)
			{
				out_Lap.at<Vec3b>(i, j)[0] = img_Ledfield.at<Vec3b>(i, j)[0];
				out_Lap.at<Vec3b>(i, j)[1] = img_Ledfield.at<Vec3b>(i, j)[1];
				out_Lap.at<Vec3b>(i, j)[2] = img_Ledfield.at<Vec3b>(i, j)[2];
			}
		}
	}
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/out_Laplacian.png", out_Lap);

	//中轴可视化

	cvtColor(img_Ledfield, img_Ledfield, CV_BGR2GRAY);
	int threshval = 60;
	cv::Mat bin_ = threshval < 65 ? (img_Ledfield < threshval) : (img_Ledfield > threshval);
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/axis_threshold.png", bin_);

	Mat bin = imread("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/axis_threshold.png");
	for (int i = 0; i < bin.rows; i++)
	{
		for (int j = 0; j < bin.cols; j++)
		{
			if (bin.at<Vec3b>(i, j)[0] == 255)
			{
				out_axis.at<Vec3b>(i, j)[0] = 255;
				out_axis.at<Vec3b>(i, j)[1] = 255;
				out_axis.at<Vec3b>(i, j)[2] = 255;
			}
		}
	}

	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/out_axis.png", out_axis);
}
