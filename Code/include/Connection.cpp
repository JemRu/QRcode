#include "Connection.h"

using namespace std;
using namespace cv;

Connection::Connection()
{
}
Connection::~Connection()
{
}
Mat Connection::gaussian_kernal(int dim, int sigma)
{
	int c = dim / 2;
	Mat K(dim, dim, CV_32FC1);
	//生成二维高斯核
	float s2 = 2.0 * sigma * sigma;
	for (int i = (-c); i <= c; i++)
	{
		int m = i + c;
		for (int j = (-c); j <= c; j++)
		{
			int n = j + c;
			float v = exp(-(1.0*i*i + 1.0*j*j) / s2);
			K.ptr<float>(m)[n] = v;
		}
	}
	Scalar all = sum(K);
	Mat gaussK;
	K.convertTo(gaussK, CV_32FC1, (1 / all[0]));
	all = sum(gaussK);
	return gaussK;
}
void Connection::pre_dia(const char* inputImage, const char* outImage)
{
	Mat image = imread(inputImage);
	//cout << inputImage << endl;
	//定义颜色
	Vec3b black(0, 0, 0);
	Vec3b white(255, 255, 255);
	Vec3b blue(255, 0, 0);
	Vec3b red(0, 0, 255);
	//获取自定义核
	Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
	//输出图像
	Mat out;
	//区分position alignment
	cv::Mat position(image.size(), CV_8UC3);
	for (int r = 0; r < position.rows; r++)
	{
		for (int c = 0; c < position.cols; c++)
		{
			Vec3b &pixel = position.at<cv::Vec3b>(r, c);
			pixel = black;
		}
	}
	//存放alignment
	cv::Mat alignment(image.size(), CV_8UC3);
	for (int r = 0; r < alignment.rows; r++)
	{
		for (int c = 0; c < alignment.cols; c++)
		{
			Vec3b &pixel = alignment.at<cv::Vec3b>(r, c);
			pixel = white;
		}
	}

	//连接 position block的对角线
	//提取二维码 position alignment信息
	cv::Mat matGray;
	cv::cvtColor(image, matGray, CV_BGR2GRAY);
	imageInfo = info::QRImageInfo(matGray);

	//确定对角线宽度
	int moduleSize = imageInfo.moduleSize;
	int connector = 0.7 * moduleSize;
	int scale = floor(2 * moduleSize / 3);

	//get position and ALIGNMENT
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			
				pair<int, int> pos = imageInfo.getPixel(i, j);//如果不是module center返回（-1，-1）,否则返回在modules中对应module的位置 
				if (pos.first != -1)
				{
					//position and aligment
					qrgen::Pixel::PixelRole role = imageInfo.getModuleRole(pos.first, pos.second);//modules(0,0)的module的role
					
					//cout << "Module:" << pos.first << " " << pos.second << " Center:" << j << " " << i << " role: " << role << endl;
					if (role == 1)
					{
						//Vec3b &pixel = position.at<cv::Vec3b>(i, j);
						//pixel = red;
						rectangle(position, Point(i + scale, j + scale), Point(i - scale, j - scale), white, -1, 8, 0);
						
					}
				}
		}
	}
	cv::bitwise_xor(position, cv::Scalar(255, 255, 255), position);
	cv::Mat gray;
	cv::cvtColor(position, gray, CV_BGR2GRAY);
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (gray < threshval) : (gray > threshval);
	Mat labelImage(position.size(), CV_32S);
	Mat stats(position.size(), CV_32S);
	Mat centroids(position.size(), CV_32S);
	int nLabels = connectedComponentsWithStats(bin, labelImage, stats, centroids, 4);
	int max = stats.at<int>(1, cv::CC_STAT_AREA);
	vector<int> positionlabel;//1 positon 0 alignment
	positionlabel.push_back(-1);
	for (int i = 1; i < nLabels; i++)
	{
		int temparea = stats.at<int>(i, cv::CC_STAT_AREA);
		if (temparea>=max)
		{
			max = temparea;
		}
	}

	int prmin = image.rows;
	int prmax = 0;
	int pcmin = image.cols;
	int pcmax = 0;
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (position.at<Vec3b>(i, j)[0] == 0)
			{
				int labelnum = labelImage.at<int>(i, j);
				int area = stats.at<int>(labelnum, cv::CC_STAT_AREA);
				if (area < max*0.9)
				{
					//aligment
					Vec3b &pixel = position.at<cv::Vec3b>(i, j);
					pixel = white;
					
					if (image.at<Vec3b>(i, j)[0] == 255)
					{
						Vec3b &pixel2 = alignment.at<cv::Vec3b>(i, j);
						pixel2 = black;
					}
				}
				else
				{
					//position
					if (i >= prmax)
						prmax = i;
					if (i <= prmin)
						prmin = i;
					if (j >= pcmax)
						pcmax = j;
					if (j <= pcmin)
						pcmin = j;

					if (image.at<Vec3b>(i, j)[0] == 255)
					{
						Vec3b &pixel2 = position.at<cv::Vec3b>(i, j);
						pixel2 = white;
;
					}
				}
			}
		}
	}

	//connect positon
	//use this some QRcode with unstandard module or size will be miss
	int miss = 1.2;//偏差系数
	moduleSize *= 1.2;
	vector<int> tempX, tempY;
	//left top
	tempX.push_back(prmin);
	tempY.push_back(pcmin);
	//right top
	tempX.push_back(prmax - 6 * moduleSize);
	tempY.push_back(pcmin);
	//left bottom
	tempX.push_back(prmin);
	tempY.push_back(pcmax - 6 * moduleSize);
	for (int i = 0; i < 3; i++)
	{
		Point sp, ep;
		sp.x = tempX[i];
		sp.y = tempY[i];
		ep.x = sp.x + moduleSize;
		ep.y = sp.y + moduleSize;
		line(image, sp, ep, cv::Scalar(255, 255, 255), connector);
		sp.x = tempX[i] + 6 * moduleSize;
		sp.y = tempY[i];
		ep.x = sp.x - moduleSize;
		ep.y = sp.y + moduleSize;
		line(image, sp, ep, cv::Scalar(255, 255, 255), connector);
		sp.x = tempX[i];
		sp.y = tempY[i] + 6 * moduleSize;
		ep.x = sp.x + moduleSize;
		ep.y = sp.y - moduleSize;
		line(image, sp, ep, cv::Scalar(255, 255, 255), connector);
		sp.x = tempX[i] + 6 * moduleSize;
		sp.y = tempY[i] + 6 * moduleSize;
		ep.x = sp.x - moduleSize;
		ep.y = sp.y - moduleSize;
		line(image, sp, ep, cv::Scalar(255, 255, 255), connector);
	}

	//connect alignment
	//别了吧

	
	//connect alignment
	imwrite(outImage, image);
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/alignment.png", alignment);

}
void Connection::pre_erode(const char* inputImage) {

	//进行腐蚀操作 防止自交
	Mat image = imread(inputImage);
	//获取自定义核
	Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
	//输出图像
	Mat out;

	erode(image, out, element);
	imwrite("data/input_0.png", out);

	//确定对角线宽度
	cv::Mat matGray;
	cv::cvtColor(image, matGray, CV_BGR2GRAY);
	imageInfo = info::QRImageInfo(matGray);
	int moduleSize = imageInfo.moduleSize;
	Width = 0.7 * moduleSize;
	int scale = floor(2*moduleSize / 3);

}
int Connection::get_Width(const char* inputImage) {

	//进行腐蚀操作 防止自交
	Mat image = imread(inputImage);

	//确定对角线宽度
	cv::Mat matGray;
	cv::cvtColor(image, matGray, CV_BGR2GRAY);
	imageInfo = info::QRImageInfo(matGray);
	int moduleSize = imageInfo.moduleSize;
	Width = 0.7 * moduleSize;
	int scale = floor(2 * moduleSize / 3);
	
	return Width;
}
Mat Connection::pre_treatment(const char* inputImage)
{
	Mat image = imread(inputImage);
	inputimage = imread(inputImage);
	Mat out = image;

	width = image.cols;
	height = image.rows;

	//init modules
	Vec3b black(0, 0, 0);
	Vec3b white(255, 255, 255);
	Vec3b blue(255, 0, 0);
	Vec3b red(0, 0, 255);
	cv::Mat dst(image.size(), CV_8UC3);
	cv::Mat dstcopy(image.size(), CV_8UC3);
	for (int r = 0; r < dst.rows; r++)
	{
		for (int c = 0; c < dst.cols; c++)
		{
			Vec3b &pixel = dst.at<cv::Vec3b>(r, c);
			pixel = black;
		}
	}
	for (int r = 0; r < dstcopy.rows; r++)
	{
		for (int c = 0; c < dstcopy.cols; c++)
		{
			Vec3b &pixel = dstcopy.at<cv::Vec3b>(r, c);
			pixel = black;
		}
	}

	//获得Rmax
	cv::Mat matGray;
	cv::cvtColor(out, matGray, CV_BGR2GRAY);
	/*
	首先初始化QRImageInfo输入灰度图，QRImageInfo已经包含Version，Error Correct Level,pixels,modules,codewords，blocks的信息
	*/
	imageInfo = info::QRImageInfo(matGray);
	/*
	Version和Error Correct Level是公有成员变量，可以直接获得
	*/
	//cout << "Version:" << imageInfo.version->getVersion() << endl;
	//cout << "Level:" << imageInfo.level << endl;
	int version = imageInfo.version->getVersion();
	int level = imageInfo.level;
	Rmax = qrgen::Version::VERSION_INFOS[version].lvlInfos[level].cbytes_pre_block / 2;
	int moduleSize = imageInfo.moduleSize;
	radius = moduleSize / 3;
	//get gaussian_kernal
	Mat gaussian;//
	//cout << moduleSize << endl;
	int tagnum = 0;
	//init modultags
	for (int r = 0; r < dst.rows; r++)
	{
		vector<int> tag;
		for (int c = 0; c < dst.cols; c++)
		{
			tag.push_back(-1);
		}
		modultags.push_back(tag);
	}
	//init modulsInfo
	for (int r = 0; r < dst.rows; r++)
	{
		vector<int> tag;
		for (int c = 0; c < dst.cols; c++)
		{
			tag.push_back(-1);
		}
		modulsInfo.push_back(tag);
	}
	//get modultags and moduls
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			std::pair<int, int> pos = imageInfo.getPixel(i, j);//如果不是module center返回（-1，-1）,否则返回在modules中对应module的位置 
			
			if (pos.first != -1)
			{
				//cout << "Module:" << pos.first << " " << pos.second << " Center:" << j << " " << i << endl;
				vector<Point> temp;
				temp.emplace_back(pos.first, pos.second);//modul index
				temp.emplace_back(i, j); //modul中心坐标
				temp.emplace_back(0, 0);//初始化为黑

				Point p(j, i);
				Mat canvas(height, width, CV_8UC3);
				for (int r = 0; r < dst.rows; r++)
				{
					for (int c = 0; c < dst.cols; c++)
					{
						Vec3b &pixel = canvas.at<cv::Vec3b>(r, c);
						pixel = black;
					}
				}
				circle(canvas, p, radius, Scalar(255, 255, 255), -1);
				//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/modul.png", canvas);

				int countW = 0;
				int countB = 0;
				//get other modul pixels and caculate 0 or 1
				for (int r = 0; r < canvas.rows; r++)
				{
					vector<int> tags;
					for (int c = 0; c < canvas.cols; c++)
					{
						if (r == temp[1].x && c == temp[1].y)
						{
							//嘻嘻 mudel center
							modultags[r][c] = tagnum;

							if (out.at<Vec3b>(r, c)[0] == 255)
							{
								countW = countW + radius*radius*3.14 / 2;
								modulsInfo[r][c] = 1;
							}
							else
							{
								countB = countB + radius*radius*3.14 / 2;
								modulsInfo[r][c] = 0;
							}
						}
						else if(canvas.at<Vec3b>(r, c)[0] == 255)
						{
							temp.emplace_back(r, c);
							modultags[r][c] = tagnum;

							if (out.at<Vec3b>(r, c)[0] == 255)
							{
								countW = countW + 1;
								modulsInfo[r][c] = 1;
							}
							else
							{
								countB = countB + 1;
								modulsInfo[r][c] = 0;
							}
						}
					}
				}
				if (countW >= countB) 
				{
					temp[2].x = 1;
					temp[2].y = 1;
				}

				moduls.push_back(temp);
				modulPixelSize = temp.size() - 3;
				tagnum++;
			}
		}
	}
	
	/*for (int i = 0; i < modultags.size(); i++)
	{
		for (int j = 0; j < modultags[i].size(); j++)
		{
			cout << modultags[i][j] << endl;
		}
	}*/

	////out for debug
	//Mat canvas = inputimage;
	//for (int i = 0; i < moduls.size(); i++)
	//{
	//	if (moduls[i][2].x == 0)  
	//	{
	//		canvas.at<Vec3b>(moduls[i][1].x, moduls[i][1].y) = black;
	//	}
	//	else
	//	{
	//		canvas.at<Vec3b>(moduls[i][1].x, moduls[i][1].y) = white;
	//	}
	//	Vec3b rand((rand() & 255), (rand() & 255), (rand() & 255));
	//	for (int j = 3; j < moduls[i].size(); j++)
	//	{
	//		canvas.at<Vec3b>(moduls[i][j].x, moduls[i][j].y) = rand;
	//	}
	//}
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/moduls.png", canvas);
	////out for debug
	//cout << "modultag: " << modultags[15][18] << "find modul index：( "<< moduls[modultags[15][18]][1].x << " , "<< moduls[modultags[15][18]][1].y << " )"<< endl;
	/*std::ofstream file("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/modulstags.txt");
	for (int i = 0; i < modultags.size(); i++)
	{
		for (int j = 0; j < modultags[i].size(); j++)
		{
			file << " " << modultags[i][j];
		}
		file << "" << std::endl;
	}*/

	return out;
}
int Connection::get_sep(const char* inputImage)
{
	//得到分支
	cv::Mat srcImg = cv::imread(inputImage);
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
	return nLabels;
}
Mat Connection::get_lable(const char* inputImage)
{
	//得到分支
	cv::Mat srcImg = imread(inputImage);
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
	labelNum = nLabels;
	LabelImage = labelImage;
	std::vector<cv::Vec3b> colors(nLabels);
	colors[0] = cv::Vec3b(0, 0, 0);

	//output_info << "Number of connected components = " << nLabels - 1 << std::endl << std::endl;
	//output_info << "Number of connected components = " << nLabels << std::endl << std::endl;

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

	//std::ofstream file("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/label_image.txt");
	//for (int i = 0; i < label.size(); i++)
	//{
	//	for (int j = 0; j < label[i].size(); j++)
	//	{
	//		file << " " << label[i][j];
	//	}
	//	file << "" << std::endl;
	//	//cout << label[i].size() << endl;
	//}
	//cout << label.size()<<endl;
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/colores_label.png", dst);


	return labelImage;
}
void Connection::get_all_boundary(const char* inputImage) {

	Mat src = cv::imread(inputImage);
	Mat gray2(src.size(), CV_8U);
	cvtColor(src, gray2, CV_BGR2GRAY);//转换成灰度图  
	threshold(gray2, gray2, 128, 255, cv::THRESH_BINARY);//转换成2值图像  

	vector<vector<Point>> temcontours;	
	findContours(gray2,
		temcontours, // a vector of contours   
		RETR_LIST, // retrieve the external contours  
		CV_CHAIN_APPROX_NONE); // retrieve all pixels of each contours  

	cout << "temcontours: " << temcontours.size() << std::endl;
	contours = temcontours;
	//remove lable hole contours
	// make LabelContours info
	/*cv::Mat labelImage = get_lable(inputImage);
	for (int i = 0; i < temcontours.size(); i++)
	{
		vector<Point> tempc;
		for (int j = 0; j < temcontours[i].size(); j++)
		{
			int tx = temcontours[i][j].y;
			int ty = temcontours[i][j].x;
			int cur_lable = labelImage.at<int>(tx, ty);
		}
	}

	cout << "contours: " << contours.size() << std::endl;*/
	
	//// make LabelContours info
	//cv::Mat labelImage = get_lable(inputImage);
	//for (int i = 0; i < contours.size(); i++)
	//{
	//	vector<visibility> tempcontour;
	//	for (int j = 0; j < contours[i].size(); j++)
	//	{
	//		int tx = contours[i][j].y;
	//		int ty = contours[i][j].x;
	//		int cur_lable = labelImage.at<int>(tx, ty);
	//		visibility temp;
	//		temp.x = tx;
	//		temp.y = ty;
	//		temp.lableindex = cur_lable;
	//		tempcontour.push_back(temp);
	//	}
	//	LabelContours.push_back(tempcontour);
	//	tempcontour.clear();
	//}


	//out for debug
	//ofstream file2("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/boundary_info.txt");
	//for (int i = 0; i < contours.size(); i++) 
	//{
	//	file2 << "contours: " << i << std::endl;
	//	for (int j = 0; j < contours[i].size(); j++) {
	//		file2 << contours[i][j].y << " " << contours[i][j].x  << std::endl;
	//	}
	//}

	//visual for debug
	/*Mat canvas = cv::imread(inputImage);
	std::vector<cv::Vec3b> colors(contours.size());
	Vec3b red(0, 0, 255);
	for (int label = 0; label < contours.size(); label++)
	{
		colors[label] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
	}
	for (int r = 0; r < contours.size(); r++)
	{
		for (int c = 0; c < contours[r].size(); c++)
		{
			cv::Vec3b &pixel = canvas.at<cv::Vec3b>(contours[r][c].y, contours[r][c].x);
			pixel = colors[r];
		}
	}
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/color_contours.png", canvas);*/
}
vector<visibility> Connection::get_visibility(int piontAcontourindex, int piontAindex, const char* inputImage) {
	vector<Point> piont_visualContour;

	//find next point to get right point
	int temx = contours[piontAcontourindex][piontAindex].y;
	int temy = contours[piontAcontourindex][piontAindex].x;

	if (label[temx-1][temy] == 0)
	{
		temx = temx - 1;
		//cout << "temx - 1" << endl;
		
	}
	else if (label[temx + 1][temy] == 0)
	{
		temx = temx + 1;
		//cout << "temx + 1" << endl;
	} else if(label[temx][temy + 1] == 0)
	{
		temy = temy + 1;
		//cout << "temy + 1" << endl;
	} else if (label[temx][temy - 1] == 0)
	{
		temy = temy - 1;
		//cout << "temy + 1" << endl;
	}
	Point p(temy, temx);
	piont_visualContour = conn::pixelVisibility(p, contours);

	////remove pionts on this lable 
	vector<visibility> piontVisualContours;
	int removeindex = LabelImage.at<int>(contours[piontAcontourindex][piontAindex].y, contours[piontAcontourindex][piontAindex].x);
	for (int i = 0; i < piont_visualContour.size(); i++)
	{
		int tx = piont_visualContour[i].y;
		int ty = piont_visualContour[i].x;
		//返回的 x,y 方向正确
		int cur_lable = LabelImage.at<int>(tx, ty);
		if (cur_lable!= removeindex)
		{
			if (inputimage.at<Vec3b>(tx, ty)[0] == 255)
			{
				visibility temp;
				temp.x = tx;
				temp.y = ty;
				temp.lableindex = cur_lable;
				piontVisualContours.push_back(temp);
			}
		}
	}


	////visual for debug
	//Mat canvas = cv::imread(inputImage);
	//Vec3b red(0,0,255);
	//Vec3b blue(255, 0, 0);
	//Vec3b green(0, 255, 0);
	//Vec3b black(0, 0, 0);
	//for (int i = 0; i < piont_visualContour.size() ; i++)
	//{
	//	cv::Vec3b &pixel = canvas.at<cv::Vec3b>(piont_visualContour[i].y, piont_visualContour[i].x);
	//	pixel = red;
	//}
	//cv::Vec3b &pixel = canvas.at<cv::Vec3b>(contours[piontAcontourindex][piontAindex].y, contours[piontAcontourindex][piontAindex].x);
	//pixel = blue;
	//cv::Vec3b &pixel2 = canvas.at<cv::Vec3b>(temx, temy);
	//pixel2 = green;
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/point_visualContour.png", canvas);

	////visual for debuggggggger
	//Mat canvas2 = imread(inputImage);
	//Vec3b red(0,0,255);
	//Vec3b blue(255, 0, 0);
	//Vec3b green(0, 255, 0);
	//Vec3b black(0, 0, 0);
	//for (int i = 0; i < piontVisualContours.size(); i++)
	//{
	//	cv::Vec3b &pixel = canvas2.at<cv::Vec3b>(piontVisualContours[i].x, piontVisualContours[i].y);
	//	pixel = red;
	//}
	//cv::Vec3b &pixel3 = canvas2.at<cv::Vec3b>(contours[piontAcontourindex][piontAindex].y, contours[piontAcontourindex][piontAindex].x);
	//pixel3 = blue;
	//cv::Vec3b &pixel4 = canvas2.at<cv::Vec3b>(temx, temy);
	//pixel4 = green;
	//stringstream input,iterimgstr, iterimgoutstr;
	//string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/pointVisualContours");
	//string iterformat(".png");
	//iterimgstr.str("");
	//iterimgstr << iterimg << piontAindex << iterformat;
	//string iterin = iterimgstr.str();
	//imwrite(iterin.c_str(), canvas2);

	return piontVisualContours;
}
double Connection::Euclidean_distance(Point pixel1, Point pixel2)
{
	int y1 = pixel1.x;
	int x1 = pixel1.y;
	int y2 = pixel2.x;
	int x2 = pixel2.y;
	double distance = sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
	return distance;
}
double Connection::correction_coefficient(Point pixel1, Point pixel2, const char* inputImage)
{
	/*clock_t startc, finishc;
	double timec;
	startc = clock();*/

	//get detect points
	Vec3b Black = Vec3b(0, 0, 0);
	Vec3b White = Vec3b(255, 255, 255);
	Vec3b Red = Vec3b(0, 0, 255);

	//weight height
	cv::Mat dst(height, width, CV_8UC3);
	for (int r = 0; r < dst.rows; r++)
	{
		for (int c = 0; c < dst.cols; c++)
		{
			Vec3b &pixel = dst.at<cv::Vec3b>(r, c);
			pixel = Black;
		}
	}
	Point sp, ep;
	sp = pixel1;
	ep = pixel2;
	line(dst, sp, ep, cv::Scalar(255, 255, 255), Width);
	//out for debug
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/marge1.png",dst);
	
	////calculate Ruse
	int Rused = 0;
	double deta = 1;

	bool blockchange = false;
	bool modulchange = false;

	
	
	vector<vector<int>> pos;//反转了的modul的索引
	vector<int> changeModultags;//相交的modultag
	vector<vector<int>> modulsInfotemp = modulsInfo;//这加入两个点的连接线后的 modulinfo 副本

	for (int i = 0; i < dst.rows; i++)
	{
		for (int j = 0; j < dst.cols; j++)
		{
			if (dst.at<cv::Vec3b>(i, j)[0] == 255)
			{
				////modulcenter 的索引 通过 module center 得到pos   dis!!
				//pair<int, int> temp = imageInfo.getPixel(i, j);
				////如果不是module center返回（-1，-1）,否则返回在modules中对应module的位置 
				//if (temp.first != -1)
				//{
				//	//cout << "Module:" << temp.first << " " << temp.second << " Center:" << i << " " << j << endl;
				//	pos.emplace_back(temp.first, temp.second);
				//	modulchange = true;

				//	Vec3b &pixel = dst.at<cv::Vec3b>(i, j);
				//	pixel = Red;
				//}

				//通过改变modul的50%以上

				if (modultags[i][j] != -1)
				{
					//改变对应modul的色值
					modulsInfotemp[i][j] = 1;
					vector<int>::iterator it = find(changeModultags.begin(), changeModultags.end(), (modultags[i][j]));
					//cout << modultags[i][j] << " " << changeModultags.size() << endl;
					if (it == changeModultags.end())
						changeModultags.push_back(modultags[i][j]);
				}
			}
		}
	}
	

	for (int i = 0; i < changeModultags.size(); i++)
	{
		//caculate tempmoduls[changeModultags[i]] 0 or 1 
		int countW = 0;
		int countB = 0;
		int info = moduls[changeModultags[i]][2].x; //origin 0 or 1
		int centerx = moduls[changeModultags[i]][1].x; //center (x,y)
		int centery = moduls[changeModultags[i]][1].y;
		for (int j = 3; j < moduls[changeModultags[i]].size(); j++)
		{
			int modulx = moduls[changeModultags[i]][j].x;
			int moduly = moduls[changeModultags[i]][j].y;
			if (modulx == centerx && moduly == centery)
			{
				if (modulsInfotemp[modulx][moduly] == 1)
				{
					countW = countW + radius*radius*3.14 / 2;
				}
				else
				{
					countB = countB + radius*radius*3.14 / 2;
				}
			}
			else 
			{
				if (modulsInfotemp[modulx][moduly] == 1)
				{
					countW = countW + 1;
				}
				else
				{
					countB = countB + 1;
				}
			}
		}
		//update info
		if (countW >= countB)
		{
			if (info != 1)
			{
				vector<int> temp;
				temp.push_back(moduls[changeModultags[i]][0].x);
				temp.push_back(moduls[changeModultags[i]][0].y);
				pos.push_back(temp);
				modulchange = true;
			}
		}
		else
		{
			if (info != 0)
			{
				vector<int> temp;
				temp.push_back(moduls[changeModultags[i]][0].x);
				temp.push_back(moduls[changeModultags[i]][0].y);
				pos.push_back(temp);
				modulchange = true;
			}
		}
	}

	/*Mat debug = inputimage;
	Vec3b green(0, 255, 0);
	Vec3b red(0, 0, 255);
	Vec3b black(0, 0, 0);
	for (int i = 0; i < modulsInfotemp.size(); i++)
	{
	for (int j = 0; j < modulsInfotemp[i].size(); j++)
	{
	if (modulsInfotemp[i][j] == 1)
	{
	cv::Vec3b &pixel2 = debug.at<cv::Vec3b>(i, j);
	pixel2 = green;
	}
	if (modulsInfotemp[i][j] == 0)
	{
	cv::Vec3b &pixel2 = debug.at<cv::Vec3b>(i, j);
	pixel2 = red;
	}
	if (modulsInfotemp[i][j] == -1)
	{
	cv::Vec3b &pixel2 = debug.at<cv::Vec3b>(i, j);
	pixel2 = black;
	}
	}
	}
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/tempmodulinfo.png", debug);*/

	//out for debug
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/marge1.png", dst);
	if (modulchange)
	{
		vector<tuple<bool, int, int>> blockresult = imageInfo.tryModule(pos);
		for (int i = 0; i < blockresult.size(); i++)
		{
			if (get<0>(blockresult[i]))
			{
				//cout << "If invert module" << "Block: " << get<1>(blockresult[i]) << " will be changed,rest " << get<2>(blockresult[i]) << endl;
				blockchange = true;
				Rused = Rmax - get<2>(blockresult[i]);
				deta = deta + (exp(Rused + 1) - exp(Rused));
			}
		}
	}

	//cout << "deta: " << deta << endl;
	/*finishc = clock();
	timec= (double)(finishc - startc) / CLOCKS_PER_SEC;
	cout << "time in correction_coefficient(), get one point coefficient need" << timec << "s！" << endl;*/
   
	return deta;
}
weight Connection::degree_distance(int  piontAcontourindex, int piontAindex, const char* inputImage)
{
	//cout << "piontAindex" << piontAindex << endl;
	weight out;//x,y正向
	out.piontAcontourindex = piontAcontourindex;
	out.piontAindex = piontAindex;
	out.pointBTableindex = 0;
	out.x = 0;
	out.y = 0;
	Point p1(contours[piontAcontourindex][piontAindex].x, contours[piontAcontourindex][piontAindex].y);
	
	/*clock_t start1, start2, finish1, finish2;
	double time1, time2;
	start1 = clock();
	cout << contours[piontAcontourindex].size() << "  size" << endl;*/

	vector<visibility> piontVisualContours = get_visibility(piontAcontourindex, piontAindex, inputImage);
	
	/*finish1 = clock();
	time1 = (double)(finish1 - start1) / CLOCKS_PER_SEC;
	cout << "\n in degree_distance , get one piont visibility need: " << time1 << "s！" << endl;
	start2 = clock();*/

	//若没有可视区域 跳过这个点！！！ 说明是实体中的 子轮廓 实体大轮廓中的一个洞
	if (piontVisualContours.size() < 1 )
	{
		out.distance = -1;
		//cout << "跳过这个点点" << endl;
	}
	else
	{
		Point p2(piontVisualContours[0].y, piontVisualContours[0].x);
		double coefficient = correction_coefficient(p1, p2, inputImage);
		double recordc = 0;
		double Eudistance = Euclidean_distance(p1, p2);
		double Degreedistance = Eudistance*coefficient;
		//cout << "piontVisualContours.size(): " << piontVisualContours.size() << endl;
		for (int i = 0; i < piontVisualContours.size(); i++)
		{
			Point p2(piontVisualContours[i].y, piontVisualContours[i].x);
			coefficient = correction_coefficient(p1, p2, inputImage);
			Eudistance = Euclidean_distance(p1, p2);
			//cout << "I: " << i << endl;
			//cout << "coefficient: " << coefficient << " Eudistance: " << Eudistance << " Min Degreedistance" << Degreedistance << endl;
			if (Eudistance*coefficient <= Degreedistance)
			{
				Degreedistance = Eudistance*coefficient;
				out.x = p2.x;
				out.y = p2.y;
				out.pointBTableindex = piontVisualContours[i].lableindex;
				out.distance = Degreedistance;
				recordc = coefficient;
			}
		}

		/*finish2 = clock();
		time2 = (double)(finish2 - start2) / CLOCKS_PER_SEC;
		cout << "\n in degree_distance , get one piont min distance need: " << time2 << "s！" << endl;*/

		/*cout << "Min Degreedistance: " << Degreedistance << "piontA: " << contours[out.piontAcontourindex][out.piontAindex].y << " , "
		<< contours[out.piontAcontourindex][out.piontAindex].x << "  piontB: " << out.y << " , " << out.x << endl;*/

		////visual for debug
		////cout << "min dis: " << "piontB：( " << out.x << " , " << out.y << " )   in the lable : " << out.pointBTableindex << endl;



		/*Mat canvas = cv::imread(inputImage);
		Vec3b blue(255, 0, 0);
		Vec3b green(0, 255, 0);
		cv::Vec3b &pixel = canvas.at<cv::Vec3b>(contours[piontAcontourindex][piontAindex].y, contours[piontAcontourindex][piontAindex].x);
		pixel = blue;
		cv::Vec3b &pixel2 = canvas.at<cv::Vec3b>(out.y, out.x);
		pixel2 = green;

		stringstream input, iterimgstr, iterimgoutstr;
		string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/point_mindis_");
		string iterformat(".png");
		iterimgstr.str("");
		iterimgstr << iterimg << piontAindex << "_" << recordc << iterformat;
		string iterin = iterimgstr.str();

		imwrite(iterin, canvas);*/
	}
	
	return out;
}
weight Connection::contour_distace(int contourindex, const char* inputImage) 
{


	/*Mat canvas = imread(inputImage);
	Vec3b red(0, 0, 255);
	Vec3b green(0, 255, 0);
	
	for (int i = 0; i < contours[contourindex].size(); i++)
	{
		cv::Vec3b &pixel = canvas.at<Vec3b>(contours[contourindex][i].y, contours[contourindex][i].x);
		pixel = red;
		cout << "contourindexxxx：" << contourindex << endl;
	}

	stringstream input, iterimgstr, iterimgoutstr;
	string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour_debug_");
	string iterformat(".png");
	iterimgstr.str("");
	iterimgstr << iterimg << contourindex << iterformat;
	string iterin = iterimgstr.str();
	imwrite(iterin, canvas);*/

	//cout << "parallel start"<< endl;

	//cout << "这个轮廓有这么多个点点" << contours[contourindex].size() << endl;

	//cout << "contourindexxxx：" << contourindex << endl;

	weight out,minout;//x,y正向
	out = degree_distance(contourindex, 0, inputImage);
	//init 
	minout.distance = -1;
	minout.piontAcontourindex = -1;
	minout.piontAindex = -1;
	minout.pointBTableindex = -1;
	minout.x = -1;
	minout.y = -1;

	//若第一个点就没有可见的区域直接跳过这个轮廓并把它删除了
	if (out.distance == -1)
	{
		contours.erase(contours.begin() + contourindex);
		//<<"  contours size is: "<< contours.size()
		//cout << "remove one contourindex: "<< contourindex  << endl;
		return minout;
	}

		//parallel
		cout << "connect component"<< contourindex  << endl;
		int size = contours[contourindex].size();
		//cout << "contours[contourindex].size()" << contours[contourindex].size() << endl;

		vector<int> distances(size),
			Degreedistances(size),
			xs(size),
			ys(size),
			piontAcontourindexs(size),
			piontAindexs(size),
			pointBTableindexs(size);

		#pragma omp parallel for
		for (int j = 0; j < contours[contourindex].size(); j++)
		{
			weight outtemp = degree_distance(contourindex, j, inputImage);
			//cout << "parallel: " << j << endl;

			////parallel
			Degreedistances[j] = outtemp.distance;
			xs[j] = outtemp.x;
			ys[j] = outtemp.y;
			piontAcontourindexs[j] = outtemp.piontAcontourindex;
			piontAindexs[j] = outtemp.piontAindex;
			pointBTableindexs[j] = outtemp.pointBTableindex;
		}

		
		//cout << "start find min" << endl;
		//parallel find min 
		distances = Degreedistances;
		////remove -1的点点
		//vector<int>::iterator del;
		for (int del = 0; del < Degreedistances.size();)
		{
			//cout << "Degreedistances.size(): " << Degreedistances.size() << "   del: " << del << endl;
			//cout << "distances.size(): " << distances.size() << "   del: " << del << endl;
			if (Degreedistances[del] < 0)
			{
				Degreedistances.erase(Degreedistances.begin() + del);
				distances.erase(distances.begin() + del);
				xs.erase(xs.begin() + del);
				ys.erase(ys.begin() + del);
				piontAcontourindexs.erase(piontAcontourindexs.begin() + del);
				piontAindexs.erase(piontAindexs.begin() + del);
				pointBTableindexs.erase(pointBTableindexs.begin() + del);
				//cout << "这些点点我们不要" << endl;
			}
			else
			{
				del++;
			}
		}

		if (Degreedistances.size() >= 1)
		{
			sort(Degreedistances.begin(), Degreedistances.end());
			vector<int>::iterator it = find(distances.begin(), distances.end(), Degreedistances[0]);
			int tempindex = it - distances.begin();
			minout.distance = Degreedistances[0];
			minout.piontAcontourindex = piontAcontourindexs[tempindex];
			minout.piontAindex = piontAindexs[tempindex];

			//cout << "piontAcontourindexs[tempindex]: " << piontAcontourindexs[tempindex] << "   minout.piontAcontourindex: " << minout.piontAcontourindex << endl;

			minout.pointBTableindex = pointBTableindexs[tempindex];
			minout.x = xs[tempindex];
			minout.y = ys[tempindex];

			//////debug
			//Mat canvas = imread(inputImage);
			//Vec3b blue(255, 0, 0);
			//Vec3b green(0, 255, 0);
			//Vec3b red(0, 0, 255);

			////cout << "piontAcontourindexs array size: " << piontAcontourindexs.size() << endl;
			//for (int i = 0; i < piontAcontourindexs.size(); i++)
			//{
			//	cv::Vec3b &pixel = canvas.at<Vec3b>(contours[piontAcontourindexs[i]][piontAindexs[i]].y, contours[piontAcontourindexs[i]][piontAindexs[i]].x);
			//	pixel = red;
			//}
			//cout << "piontAindexs[tempindex]: " << piontAindexs[tempindex] << "   minout.piontAindex: " << minout.piontAindex << endl;
			//cv::Vec3b &pixel = canvas.at<Vec3b>(contours[minout.piontAcontourindex][minout.piontAindex].y, contours[minout.piontAcontourindex][minout.piontAindex].x);
			//pixel = blue;
			//cv::Vec3b &pixel2 = canvas.at<Vec3b>(minout.y, minout.x);
			//pixel2 = green;
			//stringstream input, iterimgstr, iterimgoutstr;
			//string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour_mindis");
			//string iterformat(".png");
			//iterimgstr.str("");
			//iterimgstr << iterimg << contourindex << iterformat;
			//string iterin = iterimgstr.str();
			//imwrite(iterin, canvas);
		}
		else
		{
			//跳过这个contour
			cout << "跳过这个contour" << endl;
			//删除这个contour


			//contours.erase(contours.begin() + contourindex);
			//cout << "remove one contourindex: "<< contourindex <<"  contours size is: "<< contours.size() << endl;
			return minout;

		}
		//cout << "contour: " << contourindex << endl;
		///*cout << "soooooo mindis: " << mindis << "piontA: "<< contours[minout.piontAcontourindex][minout.piontAindex].y << " , "
		/*cout<< contours[minout.piontAcontourindex][minout.piontAindex].x << "  piontB: "<< minout.y << " , " << minout.x << endl;*/		
		//cout << minout.distance << minout.piontAcontourindex << minout.piontAindex << minout.pointBTableindex << minout.x << minout.y << endl;
		//cout << "运行到这里还是畅通的！" << endl;

	return minout;
}
void Connection::img_distace(const char* inputImage, const char* outImage)
{
	weight out, minout2;//x,y正向

	int size = contours.size();
	vector<int> distances(size),
		Degreedistances(size),
		xs(size),
		ys(size),
		piontAcontourindexs(size),
		piontAindexs(size),
		pointBTableindexs(size);
	//contours.size() - 2
	for (int i = 0; i < contours.size() - 2; i++)
	{
		out = contour_distace(i, inputImage);
		if (out.distance == -1)
		{
			//说明有一个contour被删掉了 下一个不需要i++ 你懂我意思吗
			i--;
		}
		else {
			Degreedistances.push_back(out.distance);
			xs.push_back(out.x);
			ys.push_back(out.y);
			piontAcontourindexs.push_back(out.piontAcontourindex);
			piontAindexs.push_back(out.piontAindex);
			pointBTableindexs.push_back(out.pointBTableindex);
		}
	}
	distances = Degreedistances;
	////remove -1的点点
	//vector<int>::iterator del;
	for (int del = 0; del < Degreedistances.size();)
	{
		//cout << "Degreedistances.size(): " << Degreedistances.size() << endl;
		//cout << "distances.size(): " << distances.size() << "   del: " << del << endl;
		if (Degreedistances[del] <= 0 || xs[del]==-1 )
		{
			Degreedistances.erase(Degreedistances.begin() + del);
			distances.erase(distances.begin() + del);
			xs.erase(xs.begin() + del);
			ys.erase(ys.begin() + del);
			piontAcontourindexs.erase(piontAcontourindexs.begin() + del);
			piontAindexs.erase(piontAindexs.begin() + del);
			pointBTableindexs.erase(pointBTableindexs.begin() + del);
			//cout << "这些点点我们不要" << endl;
		}
		else
		{
			del++;
		}
	}

	//cout << "new Degreedistances.size(): " << Degreedistances.size() << endl;
	//cout << "new distances.size(): " << distances.size() << endl;
	//cout << "new xs.size(): " << xs.size() << endl;
	//cout << "new ys.size(): " << ys.size() << endl;
	//cout << "new piontAindexs.size(): " << piontAindexs.size() << endl;
	//cout << "new piontAcontourindexs.size(): " << piontAcontourindexs.size() << endl;
	

	sort(Degreedistances.begin(), Degreedistances.end());
	vector<int>::iterator it = find(distances.begin(), distances.end(), Degreedistances[0]);
	int tempindex = it - distances.begin();



	minout2.distance = Degreedistances[0];
	minout2.piontAcontourindex = piontAcontourindexs[tempindex];
	minout2.piontAindex = piontAindexs[tempindex];
	minout2.pointBTableindex = pointBTableindexs[tempindex];
	minout2.x = xs[tempindex];
	minout2.y = ys[tempindex];

	/*cout << "tempindex: " << tempindex << endl;
	cout << "minout2.distance: " << minout2.distance << endl;
	cout << "Degreedistances[0]: " << Degreedistances[0] << endl;
	cout << "distances[tempindex]: " << distances[tempindex] << endl;

	cout << "minout2.x " << minout2.x << endl;
	cout << "minout2.y " << minout2.y << endl;*/
	//output
	//Mat canvas = inputimage;
	//Point sp(contours[minout2.piontAcontourindex][minout2.piontAindex].x, contours[minout2.piontAcontourindex][minout2.piontAindex].y);
	//Point ep(minout2.x, minout2.y);
	//line(canvas, sp, ep, cv::Scalar(255, 255, 255), Width);
	////out
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour1.png", canvas);

	////debug
	//Mat canvas2 = inputimage;
	//Vec3b blue(255, 0, 0);
	//Vec3b green(0, 255, 0);
	//Vec3b red(0, 0, 255);
	//for (int i = 0; i < xs.size(); i++)
	//{
	//	cv::Vec3b &pixel2 = canvas2.at<Vec3b>(ys[i], xs[i]);
	//	pixel2 = green;
	//}
	//cout << "xssize: " << xs.size() << endl;
	//for (int i = 0; i < piontAcontourindexs.size(); i++)
	//{
	//	cv::Vec3b &pixel = canvas2.at<Vec3b>(contours[piontAcontourindexs[i]][piontAindexs[i]].y, contours[piontAcontourindexs[i]][piontAindexs[i]].x);
	//	pixel = blue;
	//}
	//cv::Vec3b &pixel2 = canvas2.at<Vec3b>(minout2.y, minout2.x);
	//pixel2 = red;
	//cv::Vec3b &pixel = canvas2.at<Vec3b>(contours[minout2.piontAcontourindex][minout2.piontAindex].y, contours[minout2.piontAcontourindex][minout2.piontAindex].x);
	//pixel = red;
	//stringstream input, iterimgstr, iterimgoutstr;
	//string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour_1");
	//string iterformat(".png");
	//iterimgstr.str("");
	//iterimgstr << iterimg << iterformat;
	//string iterin = iterimgstr.str();
	//imwrite(iterin, canvas2);

}
void Connection::img_connect(const char* inputImage, const char* outImage)
{
	weight out, minout2;//x,y正向
	int size = contours.size();
	vector<int> distances,
		Degreedistances,
		xs,
		ys,
		piontAcontourindexs,
		piontAindexs,
		pointBTableindexs;
	//contours.size() - 2
	for (int i = 0; i < contours.size() - 2; i++)
	{
		out = contour_distace(i, inputImage);
		if (out.distance == -1)
		{
			//说明有一个contour被删掉了 下一个不需要i++ 你懂我意思吗
			i--;
		}
		else {
			Degreedistances.push_back(out.distance);
			xs.push_back(out.x);
			ys.push_back(out.y);
			piontAcontourindexs.push_back(out.piontAcontourindex);
			piontAindexs.push_back(out.piontAindex);
			pointBTableindexs.push_back(out.pointBTableindex);
		}
	}
	distances = Degreedistances;
	////remove -1的点点
	//vector<int>::iterator del;
	for (int del = 0; del < Degreedistances.size();)
	{
		//cout << "Degreedistances.size(): " << Degreedistances.size() << endl;
		//cout << "distances.size(): " << distances.size() << "   del: " << del << endl;
		if (Degreedistances[del] <= 0 || xs[del] == -1)
		{
			Degreedistances.erase(Degreedistances.begin() + del);
			distances.erase(distances.begin() + del);
			xs.erase(xs.begin() + del);
			ys.erase(ys.begin() + del);
			piontAcontourindexs.erase(piontAcontourindexs.begin() + del);
			piontAindexs.erase(piontAindexs.begin() + del);
			pointBTableindexs.erase(pointBTableindexs.begin() + del);
			//cout << "这些点点我们不要" << endl;
		}
		else
		{
			del++;
		}
	}

	sort(Degreedistances.begin(), Degreedistances.end());
	vector<int>::iterator it = find(distances.begin(), distances.end(), Degreedistances[0]);
	int tempindex = it - distances.begin();
	minout2.distance = Degreedistances[0];
	minout2.piontAcontourindex = piontAcontourindexs[tempindex];
	minout2.piontAindex = piontAindexs[tempindex];
	minout2.pointBTableindex = pointBTableindexs[tempindex];
	minout2.x = xs[tempindex];
	minout2.y = ys[tempindex];

	//output
	Mat canvas = inputimage;
	Point sp(contours[minout2.piontAcontourindex][minout2.piontAindex].x, contours[minout2.piontAcontourindex][minout2.piontAindex].y);
	Point ep(minout2.x, minout2.y);
	line(canvas, sp, ep, cv::Scalar(255, 255, 255), Width);
	//out
	imwrite(outImage, canvas);

}
void Connection::img_connect2(const char* inputImage, const char* outImage)
{
	weight out, minout2;//x,y正向
	int size = contours.size();
	vector<int> distances,
		Degreedistances,
		xs,
		ys,
		piontAcontourindexs,
		piontAindexs,
		pointBTableindexs;
	//contours.size() - 2
	for (int i = 0; i < contours.size() - 2; i++)
	{
		out = contour_distace(i, inputImage);
		if (out.distance == -1)
		{
			//说明有一个contour被删掉了 下一个不需要i++ 你懂我意思吗
			i--;
		}
		else {
			Degreedistances.push_back(out.distance);
			xs.push_back(out.x);
			ys.push_back(out.y);
			piontAcontourindexs.push_back(out.piontAcontourindex);
			piontAindexs.push_back(out.piontAindex);
			pointBTableindexs.push_back(out.pointBTableindex);
		}
	}
	distances = Degreedistances;
	////remove -1的点点
	//vector<int>::iterator del;
	for (int del = 0; del < Degreedistances.size();)
	{
		//cout << "Degreedistances.size(): " << Degreedistances.size() << endl;
		//cout << "distances.size(): " << distances.size() << "   del: " << del << endl;
		if (Degreedistances[del] <= 0 || xs[del] == -1)
		{
			Degreedistances.erase(Degreedistances.begin() + del);
			distances.erase(distances.begin() + del);
			xs.erase(xs.begin() + del);
			ys.erase(ys.begin() + del);
			piontAcontourindexs.erase(piontAcontourindexs.begin() + del);
			piontAindexs.erase(piontAindexs.begin() + del);
			pointBTableindexs.erase(pointBTableindexs.begin() + del);
			//cout << "这些点点我们不要" << endl;
		}
		else
		{
			del++;
		}
	}
	cout<< "Degreedistances.size：" << Degreedistances.size() << endl;
	vector<vector<int>> lableCon;
	for (int i = 0; i <= labelNum; i++)
	{
		vector<int> temp;
		temp.push_back(-1);
		lableCon.push_back(temp);
	}
	Mat canvas = inputimage;
	for (int i = 0; i < Degreedistances.size(); i++)
	{

		//int BTableindex = pointBTableindexs[tempindex];
		int Ax = contours[piontAcontourindexs[i]][piontAindexs[i]].x;
		int Ay = contours[piontAcontourindexs[i]][piontAindexs[i]].y;
		int Bx = xs[i];
		int By = ys[i];
		int Alableindex = LabelImage.at<int>(Ay, Ax);
		int Blableindex = LabelImage.at<int>(By, Bx);
		//cout << "Alableindex: "<< Alableindex << " ----> Blableindex: " << Blableindex << endl;
		vector<int>::iterator it = find(lableCon[Alableindex].begin(), lableCon[Alableindex].end(), Blableindex);
		if (it == lableCon[Alableindex].end())
		{
			Point sp(Ax, Ay);
			Point ep(Bx, By);
			line(canvas, sp, ep, cv::Scalar(255, 255, 255), Width);

			lableCon[Alableindex].push_back(Blableindex);
			lableCon[Blableindex].push_back(Alableindex);
		}
		else
		{
			cout << "connected" << endl;
		}
		//cout << contours[piontAcontourindexs[i]].size() << endl;

		//debug
		//Vec3b red(0, 0, 255);
		//for (int j = 0; j < contours[piontAcontourindexs[i]].size(); j++)
		//{
		//	//cout << contours[piontAcontourindexs[i]][j].x << endl;
		//	int x = contours[piontAcontourindexs[i]][j].x;
		//	int y = contours[piontAcontourindexs[i]][j].y;
		//	cv::Vec3b &pixel2 = canvas.at<Vec3b>(y, x);
		//	pixel2 = red;
		//}		
	}

	//debug
	/*for (int i = 0; i < lableCon.size(); i++)
	{
		for (int j = 0; j < lableCon[i].size(); j++)
		{
			cout << "lable[" << i << "][" << j << "]: " << lableCon[i][j] << endl;
		}
	}*/
	//out
	imwrite(outImage, canvas);

	////debug
	//Mat canvas2 = inputimage;
	//Vec3b blue(255, 0, 0);
	//Vec3b green(0, 255, 0);
	//Vec3b red(0, 0, 255);
	//for (int i = 0; i < xs.size(); i++)
	//{
	//	cv::Vec3b &pixel2 = canvas2.at<Vec3b>(ys[i], xs[i]);
	//	pixel2 = green;
	//}
	//cout << "xssize: " << xs.size() << endl;
	//for (int i = 0; i < piontAcontourindexs.size(); i++)
	//{
	//	cv::Vec3b &pixel = canvas2.at<Vec3b>(contours[piontAcontourindexs[i]][piontAindexs[i]].y, contours[piontAcontourindexs[i]][piontAindexs[i]].x);
	//	pixel = blue;
	//}
	//sort(Degreedistances.begin(), Degreedistances.end());
	//for (size_t i = 0; i < piontAcontourindexs.size(); i++)
	//{
	//	vector<int>::iterator it = find(distances.begin(), distances.end(), Degreedistances[i]);
	//	int tempindex = it - distances.begin();
	//	cout << "i: " << i << " tempindex: " << tempindex << endl;
	//	minout2.distance = Degreedistances[i];
	//	minout2.piontAcontourindex = piontAcontourindexs[tempindex];
	//	minout2.piontAindex = piontAindexs[tempindex];
	//	minout2.pointBTableindex = pointBTableindexs[tempindex];
	//	minout2.x = xs[tempindex];
	//	minout2.y = ys[tempindex];
	//	cv::Vec3b &pixel2 = canvas2.at<Vec3b>(minout2.y, minout2.x);
	//	pixel2 = red;
	//	cv::Vec3b &pixel = canvas2.at<Vec3b>(contours[minout2.piontAcontourindex][minout2.piontAindex].y, contours[minout2.piontAcontourindex][minout2.piontAindex].x);
	//	pixel = red;
	//}
	//imwrite(outImage, canvas2);

}







//vector<visibility> Connection::get_merge_visibility(Point point, const char* inputImage) {
//	vector<Point> piont_visualContour;
//
//	//find next point to get right point
//	int temx = point.y;
//	int temy = point.x;
//
//	if (label[temx - 1][temy] == 0)
//	{
//		temx = temx - 1;
//		//cout << "temx - 1" << endl;
//
//	}
//	else if (label[temx + 1][temy] == 0)
//	{
//		temx = temx + 1;
//		//cout << "temx + 1" << endl;
//	}
//	else if (label[temx][temy + 1] == 0)
//	{
//		temy = temy + 1;
//		//cout << "temy + 1" << endl;
//	}
//	else if (label[temx][temy - 1] == 0)
//	{
//		temy = temy - 1;
//		//cout << "temy + 1" << endl;
//	}
//	Point p(temy, temx);
//	piont_visualContour = conn::pixelVisibility(p, contours);
//
//	////remove pionts on this lable 
//	vector<visibility> piontVisualContours;
//	Mat security = imread(inputImage);
//	cv::Mat labelImage = get_lable(inputImage);
//	int removeindex = labelImage.at<int>(point.y, point.x);
//	for (int i = 0; i < piont_visualContour.size(); i++)
//	{
//		int tx = piont_visualContour[i].y;
//		int ty = piont_visualContour[i].x;
//		//返回的 x,y 方向正确
//		int cur_lable = labelImage.at<int>(tx, ty);
//		if (cur_lable != removeindex)
//		{
//			if (security.at<Vec3b>(tx, ty)[0] == 255)
//			{
//				visibility temp;
//				temp.x = tx;
//				temp.y = ty;
//				temp.lableindex = cur_lable;
//				piontVisualContours.push_back(temp);
//			}
//		}
//	}
//
//
//	////visual for debug
//	//Mat canvas = cv::imread(inputImage);
//	//Vec3b red(0,0,255);
//	//Vec3b blue(255, 0, 0);
//	//Vec3b green(0, 255, 0);
//	//Vec3b black(0, 0, 0);
//	//for (int i = 0; i < piont_visualContour.size() ; i++)
//	//{
//	//	cv::Vec3b &pixel = canvas.at<cv::Vec3b>(piont_visualContour[i].y, piont_visualContour[i].x);
//	//	pixel = red;
//	//}
//	//cv::Vec3b &pixel = canvas.at<cv::Vec3b>(contours[piontAcontourindex][piontAindex].y, contours[piontAcontourindex][piontAindex].x);
//	//pixel = blue;
//	//cv::Vec3b &pixel2 = canvas.at<cv::Vec3b>(temx, temy);
//	//pixel2 = green;
//	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/point_visualContour.png", canvas);
//
//	////visual for debuggggggger
//	//Mat canvas2 = imread(inputImage);
//	//Vec3b red(0,0,255);
//	//Vec3b blue(255, 0, 0);
//	//Vec3b green(0, 255, 0);
//	//Vec3b black(0, 0, 0);
//	//for (int i = 0; i < piontVisualContours.size(); i++)
//	//{
//	//cv::Vec3b &pixel = canvas2.at<cv::Vec3b>(piontVisualContours[i].x, piontVisualContours[i].y);
//	//pixel = red;
//	//}
//	//cv::Vec3b &pixel3 = canvas2.at<cv::Vec3b>(point.y, point.x);
//	//pixel3 = blue;
//	//cv::Vec3b &pixel4 = canvas2.at<cv::Vec3b>(temx, temy);
//	//pixel4 = green;
//
//	//stringstream input,iterimgstr, iterimgoutstr;
//	//string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/pointVisualContours");
//	//string iterformat(".png");
//	//iterimgstr.str("");
//	//iterimgstr << iterimg << iterformat;
//	//string iterin = iterimgstr.str();
//	//imwrite(iterin, canvas2);
//
//	return piontVisualContours;
//}
//weightMerge Connection::degree_merge_distance(Point point, const char* inputImage)
//{
//	weightMerge out;//x,y正向
//	out.x1 = point.x;
//	out.y1 = point.y;
//	out.x2 = 0;
//	out.y2 = 0;
//	Point p1 = point;
//
//	//clock_t start1, start2, finish1, finish2;
//	//double time1, time2;
//	//start1 = clock();
//	//cout << contours[piontAcontourindex].size() << "  size" << endl;
//
//	vector<visibility> piontVisualContours = get_merge_visibility(point, inputImage);
//	//finish1 = clock();
//	//time1 = (double)(finish1 - start1) / CLOCKS_PER_SEC;
//	//cout << "\n in degree_distance , get one piont visibility need: " << time1 << "s！" << endl;
//	//start2 = clock();
//
//	//若没有可视区域 跳过这个contour 说明是实体中的 子轮廓 实体大轮廓中的一个洞
//	if (piontVisualContours.size() < 1)
//	{
//		out.distance = -1;
//	}
//	else
//	{
//		Point p2(piontVisualContours[0].y, piontVisualContours[0].x);
//		double coefficient = correction_coefficient(p1, p2, inputImage);
//		double Eudistance = Euclidean_distance(p1, p2);
//		double Degreedistance = Eudistance*coefficient;
//
//		for (int i = 0; i < piontVisualContours.size(); i++)
//		{
//			Point p2(piontVisualContours[i].y, piontVisualContours[i].x);
//			coefficient = correction_coefficient(p1, p2, inputImage);
//			Eudistance = Euclidean_distance(p1, p2);
//
//			////cout << "coefficient: " << coefficient << " Eudistance: " << Eudistance << " Min Degreedistance" << Degreedistance << endl;
//			if (Eudistance*coefficient <= Degreedistance)
//			{
//				Degreedistance = Eudistance*coefficient;
//				out.x2 = p2.x;
//				out.y2 = p2.y;
//				out.distance = Degreedistance;
//			}
//		}
//
//		/*finish2 = clock();
//		time2 = (double)(finish2 - start2) / CLOCKS_PER_SEC;
//		cout << "\n in degree_distance , get one piont min distance need: " << time2 << "s！" << endl;*/
//
//		/*cout << "Min Degreedistance: " << Degreedistance << "piontA: " << contours[out.piontAcontourindex][out.piontAindex].y << " , "
//		<< contours[out.piontAcontourindex][out.piontAindex].x << "  piontB: " << out.y << " , " << out.x << endl;*/
//
//		////visual for debug
//		////cout << "min dis: " << "piontB：( " << out.x << " , " << out.y << " )   in the lable : " << out.pointBTableindex << endl;
//
//
//
//		/*Mat canvas = cv::imread(inputImage);
//		Vec3b blue(255, 0, 0);
//		Vec3b green(0, 255, 0);
//		cv::Vec3b &pixel = canvas.at<cv::Vec3b>(out.y1, out.x1);
//		pixel = blue;
//		cv::Vec3b &pixel2 = canvas.at<cv::Vec3b>(out.y2, out.x2);
//		pixel2 = green;
//
//		stringstream input, iterimgstr, iterimgoutstr;
//		string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/point_mindis");
//		string iterformat(".png");
//		iterimgstr.str("");
//		iterimgstr << iterimg << iterformat;
//		string iterin = iterimgstr.str();
//
//		imwrite(iterin, canvas);*/
//	}
//
//	return out;
//}
//Point Connection::contour_merge(vector<Point>  points, const char* inputImage, int num)
//{
//	weightMerge out, minout;//x,y正向
//	out = degree_merge_distance(points[0], inputImage);
//	minout = out;
//	double mindis = out.distance;
//
//	if (out.distance == -1) //跳过这个轮廓
//	{
//		cout << "起始轮廓不ok" << endl;
//		//debug
//		Mat canvas = imread(inputImage);
//		Vec3b red(0, 0, 255);
//		for (int i = 0; i < points.size(); i++)
//		{
//			cv::Vec3b &pixel2 = canvas.at<Vec3b>(points[i].y, points[i].x);
//			pixel2 = red;
//		}
//		stringstream input, iterimgstr, iterimgoutstr;
//		string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour/contourbad");
//		string iterformat(".png");
//		iterimgstr.str("");
//		iterimgstr << iterimg << num << iterformat;
//		string iterin = iterimgstr.str();
//		imwrite(iterin, canvas);
//		Point badpoint(-1, -1);
//		return badpoint;
//	}
//	else
//	{
//		////parallel
//		int size = points.size();
//		vector<int> distances(size),
//			Degreedistances(size),
//			x1s(size),
//			x2s(size),
//			y1s(size),
//			y2s(size);
//
//		#pragma omp parallel for
//		for (int j = 0; j < points.size(); j++)
//		{
//			out = degree_merge_distance(points[j], inputImage);
//
//			//printf("OpenMP Test, 线程编号为: %d\n", omp_get_thread_num());
//			//cout << "contours point " << j << " )" << endl;
//			Degreedistances[j] = out.distance;
//			x1s[j] = out.x1;
//			y1s[j] = out.y1;
//			x2s[j] = out.x2;
//			y2s[j] = out.y2;
//
//		}
//
//		//parallel find min 
//		distances = Degreedistances;
//		sort(Degreedistances.begin(), Degreedistances.end());
//		vector<int>::iterator it = find(distances.begin(), distances.end(), Degreedistances[0]);
//		int tempindex = it - distances.begin();
//		minout.distance = Degreedistances[0];
//		minout.x1 = x1s[tempindex];
//		minout.y1 = y1s[tempindex];
//		minout.x2 = x2s[tempindex];
//		minout.y2 = y2s[tempindex];
//
//		//out
//		Mat canvas = imread(inputImage);
//		Mat canvasv = imread(inputImage);
//		Vec3b blue(255, 0, 0);
//		Vec3b green(0, 255, 0);
//		Vec3b red(0, 0, 255);
//
//		Point sp(minout.x1, minout.y1);
//		Point ep(minout.x2, minout.y2);
//		line(canvas, sp, ep, cv::Scalar(255, 255, 255), Width);
//
//		//for (int i = 0; i < points.size(); i++)
//		//{
//		//	cv::Vec3b &pixel = canvasv.at<Vec3b>(points[i].y, points[i].x);
//		//	pixel = red;
//		//}
//
//		cv::Vec3b &pixel = canvasv.at<Vec3b>(sp.y, sp.x);
//		pixel = blue;
//		cv::Vec3b &pixel2 = canvasv.at<Vec3b>(ep.y, ep.x);
//		pixel2 = green;
//
//		stringstream input, iterimgstr, iterimgstrv, iterimgoutstr;
//		string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour/contour");
//		//string iterimgv("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour/contourv");
//		string iterformat(".png");
//		iterimgstr.str("");
//		iterimgstr << iterimg << (num + 1) << iterformat;
//		string iterin = iterimgstr.str();
//		//iterimgstrv.str("");
//		//iterimgstrv << iterimgv << (num + 1) << iterformat;
//		//string iterinv = iterimgstrv.str();
//
//		imwrite(iterin, canvas);
//		//imwrite(iterinv, canvasv);
//
//		return sp;
//	}
//	
//}
//void Connection::img_merge(const char* inputImage)
//{
//	bool flag = true;
//	int iteranum = 0;
//	stringstream input;
//	string iterimg(inputImage);
//	string iterformat(".png");
//
//	stringstream iterimgstr3;
//	iterimgstr3.str("");
//	iterimgstr3 << iterimg << iteranum << iterformat;
//	string iterin3 = iterimgstr3.str();
//
//	Mat canvas = imread(iterin3.c_str());
//	pre_treatment(iterin3.c_str());
//	get_lable(iterin3.c_str());
//	get_all_boundary(iterin3.c_str());
//
//	vector<int> newsizes;
//	vector<int> arrysizes;
//	for (int i = 0; i < contours.size(); i++)
//	{
//		newsizes.push_back(contours[i].size());
//		arrysizes.push_back(contours[i].size());
//	}
//	sort(newsizes.begin(), newsizes.end());
//	vector<int>::iterator it = find(arrysizes.begin(), arrysizes.end(), newsizes[0]);
//	int tempindexs = it - arrysizes.begin();
//	vector<Point> points = contours[tempindexs];
//
//	int newindex = 0;
//	int count = 0;
//	while (flag)
//	{
//		stringstream iterimgstr;
//		iterimgstr.str("");
//		iterimgstr << iterimg << iteranum << iterformat;
//		string iterin = iterimgstr.str();
//
//		cout << "开始： " << iterin << endl;
//		//-1 stop 
//		int num = get_sep(iterin.c_str());
//
//		if (num <= 2)	
//			flag = false;
//
//		pre_treatment(iterin.c_str());
//		get_lable(iterin.c_str());
//		get_all_boundary(iterin.c_str());
//
//		bool goon = false;
//		while (!goon)
//		{
//			Point temppoint = contour_merge(points, iterin.c_str(), iteranum);
//			//如果返回-1需要重新传入点集
//			if (temppoint.x == -1)
//			{
//				
//				count = count+1;
//				Mat gray2 = imread(iterin.c_str());
//				cvtColor(gray2, gray2, CV_BGR2GRAY);//转换成灰度图  
//				threshold(gray2, gray2, 128, 255, cv::THRESH_BINARY);//转换成2值图像  
//				vector<vector<Point>> temcontours;
//				findContours(gray2, temcontours, RETR_LIST, CV_CHAIN_APPROX_NONE);
//				vector<int> newsize;
//				vector<int> arrysize;
//				for (int i = 0; i < temcontours.size(); i++)
//				{
//					newsize.push_back(temcontours[i].size());
//					arrysize.push_back(temcontours[i].size());
//				}
//				sort(newsize.begin(), newsize.end());
//				vector<int>::iterator it = find(arrysize.begin(), arrysize.end(), newsize[count]);
//				int tempindex = it - arrysize.begin();
//				points = temcontours[tempindex];
//
//				cout << count << endl;
//
//			}
//			else
//			{
//				goon = true;
//			}
//		}
//		
//		iteranum = iteranum + 1;
//
//
//		stringstream iterimgstr2;
//		iterimgstr2.str("");
//		iterimgstr2 << iterimg << iteranum << iterformat;
//		string iterin2 = iterimgstr2.str();
//
//
//		//update contour points
//		//cout << "new了: " << iterin2.c_str() << endl;
//		//Mat labelImage = get_lable(iterin.c_str());
//		/*int indexl = labelImage.at<int>(temppoint.y, temppoint.x);
//		Vec3b white(255, 255, 255);
//		Vec3b black(0, 0, 0);
//		for (int r = 0; r < canvas.rows; r++)
//		{
//			for (int c = 0; c < canvas.cols; c++)
//			{
//				int labell = labelImage.at<int>(r, c);
//				if (labell == indexl)
//				{
//					Vec3b &pixel = canvas.at<Vec3b>(r, c);
//					pixel = white;
//				}
//				else
//				{
//					Vec3b &pixel = canvas.at<Vec3b>(r, c);
//					pixel = black;
//				}
//			}
//		}*/
//		Mat gray2 = imread(iterin2.c_str());
//		Mat huabu = imread(iterin2.c_str());
//		cvtColor(gray2, gray2, CV_BGR2GRAY);//转换成灰度图  
//		threshold(gray2, gray2, 128, 255, cv::THRESH_BINARY);//转换成2值图像  
//		vector<vector<Point>> temcontours;
// 		findContours(gray2,temcontours,RETR_LIST,CV_CHAIN_APPROX_NONE);
//		vector<int> newsize;
//		vector<int> arrysize;
//		for (int i = 0; i < temcontours.size(); i++)
//		{
//			newsize.push_back(temcontours[i].size());
//			arrysize.push_back(temcontours[i].size());
//		}
//		sort(newsize.begin(), newsize.end());
//		vector<int>::iterator it = find(arrysize.begin(), arrysize.end(), newsize[count]);
//		int tempindex = it - arrysize.begin();
//		points = temcontours[tempindex];
//
//		/*Vec3b red(0, 0, 255);
//		for (int i = 0; i < points.size(); i++)
//		{
//			Vec3b &pixel = huabu.at<Vec3b>(points[i].y, points[i].x);
//			pixel = red;
//		}
//		imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour/contournew.png", huabu);*/
//		//visual for debug
//		
//		vector<Vec3b> colors(temcontours.size());
//		for (int label = 0; label < temcontours.size(); label++)
//		{
//			colors[label] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
//		}
//		for (int r = 0; r < temcontours.size(); r++)
//		{
//			for (int c = 0; c < temcontours[r].size(); c++)
//			{
//				cv::Vec3b &pixel = huabu.at<cv::Vec3b>(temcontours[r][c].y, temcontours[r][c].x);
//				pixel = colors[r];
//			}
//		}
//		imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour/contournew.png", huabu);
//		
//		//没有很大卵用的释放资源
//		temcontours.clear();
//		temcontours.shrink_to_fit();
//		contours.clear();
//		contours.shrink_to_fit(); 
//	}
//}