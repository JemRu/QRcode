#include "StabilityDectection.h"
using namespace std;
using namespace cv;

vector<vector<Point>> detect::StabilityDectection::dectect_lable(const char * inputImage)
{
	Mat src = cv::imread(inputImage);
	imshow("src", src);

	Mat gray2(src.size(), CV_8U);
	cvtColor(src, gray2, CV_BGR2GRAY);//转换成灰度图  
	threshold(gray2, gray2, 128, 255, cv::THRESH_BINARY);//转换成2值图像  

	threshold(gray2, gray2, 128, 255, cv::THRESH_BINARY);
	bitwise_xor(gray2, cv::Scalar(255, 255, 255), gray2);

	vector<vector<Point>> contoursout;

	findContours(gray2,
		contours, // a vector of contours   
		CV_RETR_EXTERNAL, // retrieve the external contours  
		CV_CHAIN_APPROX_NONE); // retrieve all pixels of each contours  

							   // Print contours' length  
	cout << "Contours: " << contours.size() << std::endl;
	//vector<vector<Point>>::const_iterator itContours = contours.begin();
	//for (; itContours != contours.end(); ++itContours)
	//{

	//	cout << "Size: " << itContours->size() << std::endl;

	//}
	//减少过小的轮廓 不需要处理
	for (int i = contours.size()-1; i >= 0; i--)
	{
		if (contours[i].size() < 30)
		{
			contours.pop_back();
		}
	}

	cout << "Contours removed: " << contours.size() << std::endl;

	// draw black contours on white image  检测out
	Mat result(gray2.size(), CV_8U, Scalar(255));
	drawContours(result, contours,
		-1, // draw all contours  
		Scalar(0), // in black  
		1); // with a thickness of 1
			//std::ofstream contour_all("file/countours_all.txt");
			//for (int j = 0; j < contours.size(); j++)
			//{
			//	//if (contours[j].size() >10)
			//	//{
			//	for (int i = 0; i < contours[j].size(); i++)
			//	{
			//		contour_all << contours[j][i].x << " " << contours[j][i].y << std::endl;
			//	}
			//	//}
			//	//不同轮廓分隔符
			//	contour_all << "hello" << std::endl << "hello" << std::endl;
			//	//seed_list << contours[j][1].x << endl;
			//	//seed_list << contours[j][1].y << endl;
			//	//seed_list << contours[j][1].y << endl;
			//}

			//namedWindow("Contours");
			//imshow("Contours", result);
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/contours.png", result);
	//waitKey(0);
	contoursout = contours;
	return contoursout;
}

double detect::StabilityDectection::degree_connection(int piontacontourindex, int piontaindex, int piontbcontourindex, int piontbindex)
{
	int para = 50;
	//两点间的欧拉距离很好算 piontacontourindex = piontbcontourindex
	double EulerDistance;
	int piontax = contours[piontacontourindex][piontaindex].y;
	int piontay = contours[piontacontourindex][piontaindex].x;
	int piontbx = contours[piontbcontourindex][piontbindex].y;
	int piontby = contours[piontbcontourindex][piontbindex].x;
	EulerDistance = sqrt(pow(piontax - piontbx, 2) + pow(piontay - piontby, 2));
	//最短的路径距离 每扩散一个像素加1
	int PathLength;
	int temp = contours[piontacontourindex].size() * 0.5;
	int sham = abs(piontaindex - piontbindex);
	if (sham > temp)
	{
		PathLength = contours[piontacontourindex].size() - sham;
	}
	else
	{
		PathLength = sham;
	}
	//double connection = EulerDistance*para - PathLength;
	double connection = PathLength - EulerDistance*para;
	//double connection = log(PathLength/(EulerDistance*para));
	//double connection = log(PathLength/(EulerDistance*para));
	//cout << "pionta: " << piontax << "::" << piontay << " |||piontb:" << piontbx << "::" << piontby << endl;
	//cout << "index: " << piontaindex << " and " << piontbindex << " EulerDistance is : " << EulerDistance << " PathLength is ：" << PathLength << endl;
	return connection;
}

std::vector<double> detect::StabilityDectection::piont_field_connection(const char * inputImage, int contourindex, int piontindex)
{
	Mat canvas = cv::imread(inputImage);
	int indexc, indexp;
	indexc = contourindex;
	indexp = piontindex;
	vector<double> FielConnection;
	int maxconnection, minconnection;
	double connection;
	if (contours[indexc][0].x != contours[indexc][indexp].x || contours[indexc][0].y != contours[indexc][indexp].y)
	{
		connection = degree_connection(indexc, 0, indexc, indexp);
		maxconnection = connection;
		minconnection = connection;
	}
	else
	{
		connection = degree_connection(indexc, 1, indexc, indexp);
		maxconnection = connection;
		minconnection = connection;
	}

	for (int k = 0; k < contours[indexc].size(); k++)
	{
		if (k != piontindex)
		{
			connection = degree_connection(indexc, k, indexc, indexp);
			if (connection > maxconnection)
			{
				maxconnection = connection;
			}
			if (connection < minconnection)
			{
				minconnection = connection;
			}
			FielConnection.push_back(connection);
			//cout << "FielConnection: " << connection << endl;
		}
		else
		{
			FielConnection.push_back(0);
		}

	}
	FielConnection.push_back(maxconnection);
	FielConnection.push_back(minconnection);

	/*------------------ 上色上色啦~------------------------ - */
	//上色了上色了 今天青岛终于下大雪啦
	// draw black contours on white image  
	Mat result(canvas.size(), CV_8U, Scalar(255));

	////draw field_connection of one point 
	//vector<int> colorgray;
	//for (int k = 0; k < contours[indexc].size(); k++)
	//{
	//	double coefficient;
	//	if (FielConnection[k] - minconnection == 0)
	//	{
	//		coefficient = 0;
	//	}
	//	else
	//	{
	//		coefficient = (double)(FielConnection[k] - minconnection) / (maxconnection - minconnection);
	//	}
	//	int color = 255 * coefficient;
	//	if (color >= 255)
	//	{
	//		color = 254;
	//	}
	//	//cout << "coefficient: " << coefficient << endl;
	//	//cout << "color: " << color << endl;
	//	colorgray.push_back(color);
	//	result.at<uchar>(contours[indexc][k].y, contours[indexc][k].x) = color;
	//}
	//applyColorMap(result, result, COLORMAP_JET);
	//result.at<Vec3b>(contours[indexc][indexp].y, contours[indexc][indexp].x)[0] = 0;
	//result.at<Vec3b>(contours[indexc][indexp].y, contours[indexc][indexp].x)[1] = 0;
	//result.at<Vec3b>(contours[indexc][indexp].y, contours[indexc][indexp].x)[2] = 0;
	//imwrite("file/onepoint.png", result);
	/*------------------------------可视化完成------------------------ - */

	return FielConnection;
}

std::vector<double> detect::StabilityDectection::contour_field_connection(const char * inputImage, int contourindex)
{
	Mat canvas = cv::imread(inputImage);
	vector<double> contourFC;

	for (int i = 0; i < contours[contourindex].size(); i++)
	{
		vector<double> onepiontFC = piont_field_connection(inputImage, contourindex, i);
		int maxindex = onepiontFC.size() - 2;
		contourFC.push_back(onepiontFC[maxindex]);
		//cout << "contourFC: " << onepiontFC[maxindex] << endl;
	}

	///*------------------------------可视化------------------------ - */
	////上色了又上色了 
	//// draw black contours on white image  
	//vector<double>::iterator Max = max_element(contourFC.begin(), contourFC.end());
	//vector<double>::iterator Min = min_element(contourFC.begin(), contourFC.end());
	//double maxc, minc;
	//maxc = *Max;
	//minc = *Min;
	//contourFC.push_back(maxc);
	//contourFC.push_back(minc);
	//Mat result(canvas.size(), CV_8U, Scalar(255));
	//
	////draw field_connection of one point 
	//vector<int> colorgray;
	//for (int k = 0; k < contours[contourindex].size(); k++)
	//{
	//	double coefficient;
	//	if (contourFC[k] - minc == 0)
	//	{
	//		coefficient = 0;
	//	}
	//	else
	//	{
	//		coefficient = (double)(contourFC[k] - minc) / (maxc - minc);
	//	}
	//	int color = 255 * coefficient;
	//	if (color >= 255)
	//	{
	//		color = 254;
	//	}
	//	colorgray.push_back(color);
	//	//cout << "color: " << color << endl;
	//	result.at<uchar>(contours[contourindex][k].y, contours[contourindex][k].x) = color;
	//}
	//applyColorMap(result, result, COLORMAP_JET);
	//imwrite("file/onecontour.png", result);
	/////*------------------------------可视化------------------------ - */

	return contourFC;
}

std::vector<std::vector<double>> detect::StabilityDectection::img_field_connection(const char * inputImage)
{
	vector<vector<double>> contoursFC;
	for (int i = 0; i < contours.size(); i++)
	{
		vector<double> contourFC = contour_field_connection(inputImage, i);
		vector<double>::iterator Max = max_element(contourFC.begin(), contourFC.end());
		vector<double>::iterator Min = min_element(contourFC.begin(), contourFC.end());
		double maxc, minc;
		maxc = *Max;
		minc = *Min;
		contourFC.push_back(maxc);
		contourFC.push_back(minc);
		contoursFC.push_back(contourFC);
		//out
		/*for (int index = 0; index < contourFC.size(); index++)
		{
		cout << "contourFC in img_field_connection: " << contourFC[index] << endl;
		}*/

	}
	return contoursFC;
}

void detect::StabilityDectection::visualimg_field_connectiona(const char * inputImage)
{
	Mat canvas = cv::imread(inputImage);
	vector<vector<double>> contoursFC;
	contoursFC = img_field_connection(inputImage);

	/*for (int index = 0; index < contoursFC[0].size(); index++)
	{
	cout << "contourFC visualimg_field_connectiona : " << contoursFC[0][index] << endl;
	}*/

	Mat result(canvas.size(), CV_8U, Scalar(255));
	//draw field_connection of one point 
	for (int i = 0; i < contours.size(); i++)
	{
		double minconnection, maxconnection;
		minconnection = contoursFC[i][contoursFC[i].size() - 1];
		maxconnection = contoursFC[i][contoursFC[i].size() - 2];

		for (int k = 0; k < contoursFC[i].size() - 2; k++)
		{
			double coefficient;
			if (contoursFC[i][k] - minconnection == 0)
			{
				coefficient = 0;
			}
			else
			{
				coefficient = (double)(contoursFC[i][k] - minconnection) / (maxconnection - minconnection);
			}
			int color = 255 * coefficient;
			if (color >= 255)
			{
				color = 254;
			}
			//cout << "coefficient: " << coefficient << endl;
			//cout << "color: " << color << endl;
			result.at<uchar>(contours[i][k].y, contours[i][k].x) = color;
		}
	}
	applyColorMap(result, result, COLORMAP_JET);
	for (int i = 0; i < contours.size(); i++)
	{
		for (int k = 0; k < contours[i].size(); k++)
		{
			Scalar color = result.at<Vec3b>(contours[i][k].y, contours[i][k].x);
			canvas.at<Vec3b>(contours[i][k].y, contours[i][k].x) = Vec3b(color(0), color(1), color(2));
		}
	}
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/out.png", canvas);
}

void detect::StabilityDectection::visualimg_field_connectionb(const char * inputImage)
{
	Mat canvas = cv::imread(inputImage);
	vector<vector<double>> contoursFC;
	contoursFC = img_field_connection(inputImage);

	/*for (int index = 0; index < contoursFC[0].size(); index++)
	{
	cout << "contourFC visualimg_field_connectiona : " << contoursFC[0][index] << endl;
	}*/

	double minconnection, maxconnection;
	minconnection = contoursFC[0][contoursFC[0].size() - 1];
	maxconnection = contoursFC[0][contoursFC[0].size() - 2];
	for (int i = 0; i < contours.size(); i++)
	{
		for (int k = 0; k < contours[i].size(); k++)
		{
			if (contoursFC[i][k] < minconnection)
			{
				minconnection = contoursFC[i][k];
			}
			if (contoursFC[i][k] > maxconnection)
			{
				maxconnection = contoursFC[i][k];
			}
		}
	}

	Mat result(canvas.size(), CV_8U, Scalar(255));
	//draw field_connection of one point 
	for (int i = 0; i < contours.size(); i++)
	{
		for (int k = 0; k < contoursFC[i].size() - 2; k++)
		{
			double coefficient;
			if (contoursFC[i][k] - minconnection == 0)
			{
				coefficient = 0;
			}
			else
			{
				coefficient = (double)(contoursFC[i][k] - minconnection) / (maxconnection - minconnection);
			}
			int color = 255 * coefficient;
			if (color >= 255)
			{
				color = 254;
			}
			//cout << "coefficient: " << coefficient << endl;
			//cout << "color: " << color << endl;
			result.at<uchar>(contours[i][k].y, contours[i][k].x) = color;
		}
	}
	applyColorMap(result, result, COLORMAP_JET);
	for (int i = 0; i < contours.size(); i++)
	{
		for (int k = 0; k < contours[i].size(); k++)
		{
			Scalar color = result.at<Vec3b>(contours[i][k].y, contours[i][k].x);
			canvas.at<Vec3b>(contours[i][k].y, contours[i][k].x) = Vec3b(color(0), color(1), color(2));
		}
	}
	imwrite("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/outb.png", canvas);
}
