#include "StructureEnhance.h"

using namespace std;
using namespace cv;

StructureEnhance::StructureEnhance()
{
}
StructureEnhance::~StructureEnhance()
{
}
Mat StructureEnhance::pre_treatment(const char* inputImage)
{
	Mat image = imread(inputImage);
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
				//simwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/modul.png", canvas);
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
						else if (canvas.at<Vec3b>(r, c)[0] == 255)
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

	cout << "Rmax: " << Rmax << endl;
	std::cout << "Block Size:" << imageInfo.getBlockSize() << std::endl;



	double Rrestall = 0;
	double fenxiang = 0;
	for (int i = 0; i < imageInfo.getBlockSize(); i++) 
	{
		std::cout << "Block " << i << " rest:" << imageInfo.getBlock(i) << std::endl;
		Rrestall = Rrestall + imageInfo.getBlock(i);

		double blobkused = Rmax - imageInfo.getBlock(i);
		std::cout << "Block " << i << " used:" << blobkused << std::endl;

		fenxiang  = fenxiang + pow(blobkused / Rmax ,2);
		std::cout << "Block " << i << " fenxiang:" << fenxiang << std::endl;
	}
		
	double evaluate = Rrestall / (Rmax * imageInfo.getBlockSize());

	double loss = fenxiang / imageInfo.getBlockSize();


	cout << "input code Rrest all/Rmax all : " << evaluate << endl;
	
	cout << "input code loss : " << loss << endl;


	//out for debug
	Mat canvas = imread(inputImage);
	cout << "moduls.size() " << moduls.size() << endl;
	for (int i = 0; i < moduls.size(); i++)
	{
		if (moduls[i][2].x == 0) 
		{
			canvas.at<Vec3b>(moduls[i][1].x, moduls[i][1].y) = black;
		}
		else
		{
			canvas.at<Vec3b>(moduls[i][1].x, moduls[i][1].y) = white;
		}
		Vec3b rand((rand() & 255), (rand() & 255), (rand() & 255));
		for (int j = 3; j < moduls[i].size(); j++)
		{
			canvas.at<Vec3b>(moduls[i][j].x, moduls[i][j].y) = rand;
		}
	}
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/moduls.png", canvas);


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
	//得到分支
	inputimage = imread(inputImage);
	cv::Mat srcImg2 = imread(inputImage);
	//cv::bitwise_xor(srcImg, cv::Scalar(255, 255, 255), srcImg);
	cv::Mat gray;
	cv::cvtColor(srcImg2, gray, CV_BGR2GRAY);
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

	cv::Mat dst2(srcImg2.size(), CV_8UC3);

	for (int r = 0; r < dst2.rows; r++)
	{
		std::vector<int> cur_vector;
		for (int c = 0; c < dst2.cols; c++)
		{
			int cur_lable = labelImage.at<int>(r, c);
			cv::Vec3b &pixel = dst2.at<cv::Vec3b>(r, c);
			pixel = colors[cur_lable];

			cur_vector.push_back(cur_lable);
		}
		label.push_back(cur_vector);
	}

	Mat src = cv::imread(inputImage);
	Mat gray2(src.size(), CV_8U);
	cvtColor(src, gray2, CV_BGR2GRAY);//转换成灰度图  
	threshold(gray2, gray2, 128, 255, cv::THRESH_BINARY);//转换成2值图像  

	vector<vector<Point>> temcontours;
	findContours(gray2,
		contours, // a vector of contours   
		RETR_LIST, // retrieve the external contours  
		CV_CHAIN_APPROX_NONE); // retrieve all pixels of each contours  

	//out for debug
	//ofstream file2("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/boundary_info.txt");
	//for (int i = 0; i < contours.size(); i++) 
	//{
	//	file2 << "contours: " << i << std::endl;
	//	for (int j = 0; j < contours[i].size(); j++) {
	//		file2 << contours[i][j].y << " " << contours[i][j].x  << std::endl;
	//	}
	//}

	////visual for debug
	//Mat canvas = cv::imread(inputImage);
	//std::vector<cv::Vec3b> colors2(contours.size());
	//for (int label = 0; label < contours.size(); label++)
	//{
	//	colors2[label] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
	//}
	//for (int r = 0; r < contours.size(); r++)
	//{
	//	for (int c = 0; c < contours[r].size(); c++)
	//	{
	//		Vec3b &pixel = canvas.at<cv::Vec3b>(contours[r][c].y, contours[r][c].x);
	//		pixel = colors2[r];
	//	}
	//}
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/color_contours.png", canvas);

	return out;
}
double StructureEnhance::Euclidean_distance(Point pixel1, Point pixel2)
{
	int y1 = pixel1.x;
	int x1 = pixel1.y;
	int y2 = pixel2.x;
	int x2 = pixel2.y;
	double distance = sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
	return distance;
}
double StructureEnhance::correction_coefficient(Point pixel1, Point pixel2)
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
	Point sp(pixel1.y, pixel1.x);
	Point ep(pixel2.y, pixel2.x);
	line(dst, sp, ep, cv::Scalar(255, 255, 255), Width);
	//out for debug
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/marge1.png",dst);

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
					if (it == changeModultags.end())
						changeModultags.push_back(modultags[i][j]);
				}
			}
		}
	}

	////debug out modultags
	//std::ofstream file("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/modultag.txt");
	//for (int i = 0; i < modultags.size(); i++)
	//{
	//	for (int j = 0; j < modultags[i].size(); j++)
	//	{
	//		file << " " << modultags[i][j];
	//	}
	//	file << "" << std::endl;
	//	//cout << label[i].size() << endl;
	//}

	

	for (int i = 0; i < changeModultags.size(); i++)
	{
		//caculate tempmoduls[changeModultags[i]] 0 or 1 
		int countW = 0;
		int countB = 0;
		int info = moduls[changeModultags[i]][2].x; //origin 0 or 1

		int modulx = moduls[changeModultags[i]][1].x; //center (x,y)
		int moduly = moduls[changeModultags[i]][1].y;

			if (modulsInfotemp[modulx][moduly] == 1)
			{
				countW = countW + radius*radius*3.14 / 2;
				//cout << "changeModultags in count origin modul is white : " << changeModultags[i] << endl;
			}
			else
			{
				countB = countB + radius*radius*3.14 / 2;
				//cout << "changeModultags in count origin modul is black :: " << changeModultags[i] << endl;
			}


		for (int j = 3; j < moduls[changeModultags[i]].size(); j++)
		{
			int modx = moduls[changeModultags[i]][j].x;
			int mody = moduls[changeModultags[i]][j].y;
			if (modulsInfotemp[modx][mody] == 1)
			{
				countW = countW + 1;
			}
			if (modulsInfotemp[modx][mody] == 0)
			{
				countB = countB + 1;
			}
		}

		//get change modul
		if (countW >= countB)
		{
			if (moduls[changeModultags[i]][2].x == 0)
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
			if (moduls[changeModultags[i]][2].x == 1)
			{
				vector<int> temp;
				temp.push_back(moduls[changeModultags[i]][0].x);
				temp.push_back(moduls[changeModultags[i]][0].y);
				pos.push_back(temp);
				modulchange = true;
				//cout << "changeModultags" << changeModultags[i];
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
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/marge1.png", dst);
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
vector<Point> StructureEnhance::get_visibility(Point pixel1)
{
	vector<Point> piont_visualContour;

	//find next point to get right point
	int temx = pixel1.y;
	int temy = pixel1.x;

	if (label[temx - 1][temy] == 0)
	{
		temx = temx - 1;
		//cout << "temx - 1" << endl;

	}
	else if (label[temx + 1][temy] == 0)
	{
		temx = temx + 1;
		//cout << "temx + 1" << endl;
	}
	else if (label[temx][temy + 1] == 0)
	{
		temy = temy + 1;
		//cout << "temy + 1" << endl;
	}
	else if (label[temx][temy - 1] == 0)
	{
		temy = temy - 1;
		//cout << "temy + 1" << endl;
	}
	Point p(temy, temx);
	piont_visualContour = conn::pixelVisibility(p, contours);


	//visual for debug
	Mat canvas = inputimage;
	Vec3b red(0,0,255);
	Vec3b blue(255, 0, 0);
	Vec3b green(0, 255, 0);
	Vec3b black(0, 0, 0);
	for (int i = 0; i < piont_visualContour.size() ; i++)
	{
		cv::Vec3b &pixel = canvas.at<cv::Vec3b>(piont_visualContour[i].y, piont_visualContour[i].x);
		pixel = red;
	}
	cv::Vec3b &pixel = canvas.at<cv::Vec3b>(pixel1.y, pixel1.x);
	pixel = blue;
	cv::Vec3b &pixel2 = canvas.at<cv::Vec3b>(temx, temy);
	pixel2 = green;
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/point_visualContour.png", canvas);

	return piont_visualContour;
}
Point StructureEnhance::degree_distance(int x, int y)
{
	
	Point out;
	/*clock_t start1, start2, finish1, finish2;
	double time1, time2;
	start1 = clock();
	cout << contours[piontAcontourindex].size() << "  size" << endl;*/
	Point p1(y, x);
	Point pv(x, y); 
	vector<Point> piontVisualContours = get_visibility(pv);

	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < contours[i].size(); j++)
		{
			if (contours[i][j].y == p1.x && contours[i][j].x == p1.y)
			{
				piontacontourindex = i;
				piontaindex = j;
				cout << "piontacontourindex" << piontacontourindex << "piontaindex" << piontaindex << endl;
			}
		}
	}
	Mat canvas = inputimage;
	Vec3b blue(255, 0, 0);
	Vec3b green(0, 255, 0);
	Vec3b red(0, 0, 255);
	///*finish1 = clock();
	//time1 = (double)(finish1 - start1) / CLOCKS_PER_SEC;
	//cout << "\n in degree_distance , get one piont visibility need: " << time1 << "s！" << endl;
	//start2 = clock();*/
	//

		Point p2(piontVisualContours[0].y, piontVisualContours[0].x);
		double coefficient = correction_coefficient(p1, p2);
		double Eudistance = Euclidean_distance(p1, p2);
		for (int i = 0; i < contours[piontacontourindex].size(); i++)
		{
			if (contours[piontacontourindex][i].y == p2.x && contours[piontacontourindex][i].x == p2.y)
			{
				piontbindex = i;
				cout << contours[piontacontourindex][i].x << contours[piontacontourindex][i].x << p2.x << p2.y << endl;
				break;
			}
		}
		double jem =1;
		int PathLength;
		int temp = contours[piontacontourindex].size() * 0.5;
		int sham = abs(piontaindex - piontbindex);
		//cout << "piontaindex" << piontaindex << "piontbindex" << piontbindex << endl;

		if (sham > temp)
		{
			PathLength = contours[piontacontourindex].size() - sham;
		}
		else
		{
			PathLength = sham;
		}
		double Degreedistance = Eudistance*coefficient / (PathLength*jem);

		cout << piontVisualContours.size() << endl;
		for (int i = 0; i < piontVisualContours.size(); i++)
		{
			Point p2(piontVisualContours[i].y, piontVisualContours[i].x);

			coefficient = correction_coefficient(p1, p2);
			Eudistance = Euclidean_distance(p1, p2);
			for (int i = 0; i < contours[piontacontourindex].size(); i++)
			{
				if (contours[piontacontourindex][i].y == p2.x && contours[piontacontourindex][i].x == p2.y)
				{
					piontbindex = i;
				}
			}
			temp = contours[piontacontourindex].size() * 0.5;
			sham = abs(piontaindex - piontbindex);
			if (sham > temp)
			{
				PathLength = contours[piontacontourindex].size() - sham;
			}
			else
			{
				PathLength = sham;
			}

			//cout << "size: " << contours[piontacontourindex].size() << " temp:" << temp << "piontaindex: " << piontaindex << "piontbindex: " << piontbindex << " sham:" << sham << endl;
			//cout << "coefficient: " << coefficient << " PathLength: " << PathLength << " Eudistance: " << Eudistance << " Min Degreedistance" << Degreedistance << endl;

			if (Eudistance*coefficient/(PathLength*jem) <= Degreedistance)
			{
				Degreedistance = Eudistance*coefficient / (PathLength*jem);
				out.x = p2.x;
				out.y = p2.y;
			}

			////debug
			//Mat canvas2 = imread("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/men_C_S0&modul.png");
			//cv::Vec3b &pixel = canvas2.at<cv::Vec3b>(p1.x, p1.y);
			//pixel = red;
			//cv::Vec3b &pixel2 = canvas2.at<cv::Vec3b>(p2.x, p2.y);
			//pixel2 = green;
			//stringstream input, iterimgstr, iterimgoutstr;
			//string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/point_coe_");
			//string iterformat(".png");
			//iterimgstr.str("");
			//iterimgstr << iterimg << piontbindex << "_" << coefficient<<"_"<< PathLength << iterformat;
			//string iterin = iterimgstr.str();

			//imwrite(iterin, canvas2);
		}

	//	/*finish2 = clock();
	//	time2 = (double)(finish2 - start2) / CLOCKS_PER_SEC;
	//	cout << "\n in degree_distance , get one piont min distance need: " << time2 << "s！" << endl;*/

	//	/*cout << "Min Degreedistance: " << Degreedistance << "piontA: " << contours[out.piontAcontourindex][out.piontAindex].y << " , "
	//	<< contours[out.piontAcontourindex][out.piontAindex].x << "  piontB: " << out.y << " , " << out.x << endl;*/

	//	////visual for debug
	//	////cout << "min dis: " << "piontB：( " << out.x << " , " << out.y << " )   in the lable : " << out.pointBTableindex << endl;
		
	Point start(p1.y, p1.x);
	Point end(out.y, out.x);
	line(canvas, start, end, cv::Scalar(255, 255, 255), Width);
	//cv::Vec3b &pixel = canvas.at<cv::Vec3b>(p1.x, p1.y);
	//pixel = red;
	//cv::Vec3b &pixel2 = canvas.at<cv::Vec3b>(out.x, out.y);
	//pixel2 = green;
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/getpoint.png", canvas);

	return out;
}
double StructureEnhance::evaluate(const char* inputImage)
{
	//图像相减
	Mat img =  imread(inputImage);
	Mat img1 = inputimage;

	//get detect points
	Vec3b Black = Vec3b(0, 0, 0);
	Vec3b White = Vec3b(255, 255, 255);
	Vec3b Red = Vec3b(0, 0, 255);

	//weight height
	cv::Mat dst(height, width, CV_8UC3);
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			if (img.at<Vec3b>(i, j)[0] == img1.at<Vec3b>(i, j)[0])
			{
				Vec3b &pixel = dst.at<cv::Vec3b>(i, j);
				pixel = Black;
			}
			else
			{
				Vec3b &pixel = dst.at<cv::Vec3b>(i, j);
				pixel = White;
			}
		}
	}
	
	//out for debug
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/evaluate/marge1.png",dst);

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
					if (it == changeModultags.end())
						changeModultags.push_back(modultags[i][j]);
				}
			}
		}
	}

	////debug out modultags
	//std::ofstream file("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/modultag.txt");
	//for (int i = 0; i < modultags.size(); i++)
	//{
	//	for (int j = 0; j < modultags[i].size(); j++)
	//	{
	//		file << " " << modultags[i][j];
	//	}
	//	file << "" << std::endl;
	//	//cout << label[i].size() << endl;
	//}



	for (int i = 0; i < changeModultags.size(); i++)
	{
		//caculate tempmoduls[changeModultags[i]] 0 or 1 
		int countW = 0;
		int countB = 0;
		int info = moduls[changeModultags[i]][2].x; //origin 0 or 1

		int modulx = moduls[changeModultags[i]][1].x; //center (x,y)
		int moduly = moduls[changeModultags[i]][1].y;

		if (modulsInfotemp[modulx][moduly] == 1)
		{
			countW = countW + radius*radius*3.14 / 2;
			//cout << "changeModultags in count origin modul is white : " << changeModultags[i] << endl;
		}
		else
		{
			countB = countB + radius*radius*3.14 / 2;
			//cout << "changeModultags in count origin modul is black :: " << changeModultags[i] << endl;
		}


		for (int j = 3; j < moduls[changeModultags[i]].size(); j++)
		{
			int modx = moduls[changeModultags[i]][j].x;
			int mody = moduls[changeModultags[i]][j].y;
			if (modulsInfotemp[modx][mody] == 1)
			{
				countW = countW + 1;
			}
			if (modulsInfotemp[modx][mody] == 0)
			{
				countB = countB + 1;
			}
		}

		//get change modul
		if (countW >= countB)
		{
			if (moduls[changeModultags[i]][2].x == 0)
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
			if (moduls[changeModultags[i]][2].x == 1)
			{
				vector<int> temp;
				temp.push_back(moduls[changeModultags[i]][0].x);
				temp.push_back(moduls[changeModultags[i]][0].y);
				pos.push_back(temp);
				modulchange = true;
				//cout << "changeModultags" << changeModultags[i];
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
	//imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/marge1.png", dst);
	
	vector<int> blockrest(imageInfo.getBlockSize());

	for (int i = 0; i < imageInfo.getBlockSize(); i++)
	{
		std::cout << "Block " << i << " rest:" << imageInfo.getBlock(i) << std::endl;

		double blobkused = Rmax - imageInfo.getBlock(i);
		blockrest[i] = imageInfo.getBlock(i);
		//fenxiang += sqrt(blobkused / Rmax);
	}

	if (modulchange)
	{
		vector<tuple<bool, int, int>> blockresult = imageInfo.tryModule(pos);
		for (int i = 0; i < blockresult.size(); i++)
		{

			if (get<0>(blockresult[i]))
			{
				cout << "If invert module" << "Block: " << get<1>(blockresult[i]) << " will be changed,rest " << get<2>(blockresult[i]) << endl;
				blockchange = true;
				
				Rused = Rmax - get<2>(blockresult[i]);
				deta = deta + (exp(Rused + 1) - exp(Rused));
				
				if (get<2>(blockresult[i]) <0 )
				{
					blockrest[get<1>(blockresult[i])] = 0;
				}
				else
				{	
					blockrest[get<1>(blockresult[i])] = get<2>(blockresult[i]);
				}

			}

		}
	}

	//blockrest[1] = 11;
	double fenxiang = 0;
	double Rrestall = 0;
	for (int i = 0; i < blockrest.size(); i++)
	{
		std::cout << "Block " << i << " rest:" << blockrest[i] << std::endl;
		Rrestall = Rrestall + blockrest[i];

		//std::cout << "Block " << i << " Rmax:" << Rmax << std::endl;

		double blobkused = Rmax - blockrest[i];
		std::cout << "Block " << i << " used:" << blobkused << std::endl;

		fenxiang = fenxiang + pow(blobkused / Rmax, 2);
		std::cout << "Block " << i << " fenxiang:" << fenxiang << std::endl;
	}


	vector<int> format;
	format.push_back(0);
	format.push_back(0);
	format.push_back(0);
	format.push_back(0);

	for (int i = 0; i < format.size(); i++)
	{
		std::cout << "format" << i << " used:" << format[i] << std::endl;
	}

	double evaluate = Rrestall / (Rmax * imageInfo.getBlockSize());
	double loss = fenxiang / imageInfo.getBlockSize();

	cout << "after optimization input code Rrest all/Rmax all : " << evaluate << endl;
	//cout << "after optimization input code format Rused all/Rmax all : " << 0 << endl;
	cout << "after optimization input code loss : " << loss << endl;







	//cout << "deta: " << deta << endl;
	/*finishc = clock();
	timec= (double)(finishc - startc) / CLOCKS_PER_SEC;
	cout << "time in correction_coefficient(), get one point coefficient need" << timec << "s！" << endl;*/
	//cout << deta << endl;
	return deta;
}