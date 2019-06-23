#include <string>
#include <fstream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include "StabilityDectection.h"
#include "AxisDetection.h"
#include "Connection.h"
#include "parallel_for.h"
#include "StructureEnhance.h"
#include "ConnectSP.h"
int main()
{
	using namespace std;
	using namespace cv;
	//clock_t start, finish, finishpre;
	//double totaltime,timepre;
	//start = clock();

	/*------------------- Connection-main----------------------- - */
	//////这里是死忙连通迭代 已废弃
	//stringstream input;
	//stringstream output;
	//int round = 0;
	//bool dianfen = true;
	//while (dianfen)
	//{
	//	input.str("");
	//	output.str("");
	//	string filename("D:/workplace/visual studio 2015/QRcode/CarvingQR_connect/data/connect/code3_");
	//	string fileformat(".png");
	//	input << filename << round << fileformat;
	//	output << filename << (round + 1) << fileformat;
	//	string inputimage = input.str();
	//	string outputimage = output.str();
	//	
	//	Connection QRcode;
	//	int numlable = QRcode.get_sep(inputimage.c_str());
	//	if (numlable <= 2)
	//		dianfen = false;//跳出死忙迭代
	//	cout << "img lable num is " << numlable << endl;
	//	QRcode.pre_treatment(inputimage.c_str());
	//	QRcode.get_lable(inputimage.c_str());
	//	QRcode.get_all_boundary(inputimage.c_str());
	//	QRcode.img_connect(inputimage.c_str(), outputimage.c_str());

	//	round++;
	//}
	/*------------------- Connection-main-end---------------------- - */

	/////*------------------- Connection-main2 loss----------------------- - */
	//pre_treatment
	//string in;
	//cin >> in;
	//string input_imgstr = "data/input.png";
	//string pre_imgstr = "data/input_0.png";
	//Connection QR;
	//QR.pre_dia(input_imgstr.c_str(), pre_imgstr.c_str());

	//////iteration
	//QR.pre_erode(input_imgstr.c_str());
	//stringstream input;
	//stringstream output;
	//int round = 0;
	//bool dianfen = true;
	//while (dianfen)
	//{
	//	clock_t start, finish, finishpre;
	//	double totaltime, timepre;
	//	start = clock();

	//	input.str("");
	//	output.str("");
	//	string filename("data/input_");
	//	string fileformat(".png");
	//	input << filename << round << fileformat;
	//	output << filename << (round + 1) << fileformat;
	//	string inputimage = input.str();
	//	string outputimage = output.str();

	//	Connection QRcode;
	//	int numlable = QRcode.get_sep(inputimage.c_str());
	//	if (numlable <= 2)
	//		break;//跳出死忙迭代
	//	
	//	cout << "the num of independent components is " << numlable << endl;

	//	QRcode.pre_treatment(inputimage.c_str());//确定module宽度
	//	QRcode.get_lable(inputimage.c_str());
	//	QRcode.get_all_boundary(inputimage.c_str());
	//	QRcode.img_connect2(inputimage.c_str(), outputimage.c_str());

	//	finish = clock();
	//	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	//	std::cout << "\n time used" << totaltime << "s！" << std::endl;

	//	round++;
	//}
	///*------------------- Connection-main2-end---------------------- - */


	/*------------------- Connection-main3 SP start----------------------- - */
	Connection QR;
	int wid = QR.get_Width("data/input_0.png");
	cout << wid << endl;
	stringstream ss;
	stringstream ss2;
	int round = 0;
	while (true) {
		ss.str("");
		ss2.str("");
		string filename("data/input_");
		string fileformat(".png");
		ss << filename << round << fileformat;
		string inputimage = ss.str();
		ConnectSP pixel;
		int n = pixel.get_lable(inputimage.c_str());

		cout << n << endl;


		cout << "flag" << endl;

		if (n <= 2)
			break;

		pixel.get_all_boundary(n);

		pixel.distance_all();
		ss.str("");
		ss << filename << (round + 1) << fileformat;
		string outputimage = ss.str();

		pixel.merge(inputimage.c_str(), outputimage.c_str(), wid);

		round++;
	}
	/*------------------- Connection-main3 SP start----------------------- - */


	/////*------------------- Connection-for-test----------------------- - */
	//////test one contour
	//stringstream inputimgstr, erodeimgstr, outimgstr;
	//inputimgstr.str("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/QR_in.png");
	//erodeimgstr.str("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/Mario_0.png");
	//outimgstr.str("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/Mario_1.png");
	//string inputimg = inputimgstr.str();
	//string erodeimg = erodeimgstr.str();
	//string outimg = outimgstr.str();
	//Connection code;
	//code.pre_erode(inputimg.c_str(), erodeimg.c_str());//处理一次就行
	//code.pre_treatment(erodeimg.c_str());
	//code.get_lable(erodeimg.c_str());
	//code.get_all_boundary(erodeimg.c_str());
	////code.degree_distance(17, 17, erodeimg.c_str());
	//code.contour_distace(8,erodeimg.c_str());
	//code.img_distace(erodeimg.c_str(), outimg.c_str());
	//code.img_connect2(erodeimg.c_str(), outimg.c_str());
	////////code.img_merge("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour/contour");
	/*------------------- Connection-for-test end----------------------- - */


	///////*------------------- StructureEnhance ----------------------- - */
	//stringstream inputimgstr, outimgstr;
	//inputimgstr.str("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/stable/code2_0.png");
	//string inputimg = inputimgstr.str();
	//StructureEnhance code;
	//code.pre_treatment(inputimg.c_str());
	//code.degree_distance(181,180); //col,row

	///*------------------- StructureEnhance end ----------------------- - */
		
	///////////*------------------- evaluate ----------------------- - */
	//stringstream inputimgstr, outimgstr;
	//inputimgstr.str("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/evaluate/connoptimization_in.png");
	//string inputimg = inputimgstr.str();
	//StructureEnhance code;
	//code.pre_treatment(inputimg.c_str());
	//code.evaluate("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/evaluate/connoptimization_out13.png");
	////code.evaluate("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/evaluate/connoptimization_our.png");
	/////*------------------- evaluate end ----------------------- - */
	//

	//finish = clock();
	//totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	//std::cout << "\n totaltime is" << totaltime << "s！" << std::endl;

	

	/*------------------- Connection end------------------------ - */

	//cout << "Time: " << time << endl;
	/*------------------- StabilityDectection------------------------ - */
	//detect::StabilityDectection code;
	//vector<vector<Point>> contours = code.dectect_lable("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/code3_ccc4.png");
	//int random = 1;
	//vector<double> onepiontFC = code.piont_field_connection("file/contours.png", 1, 90);
	//vector<double> onecontourFC = code.contour_field_connection("file/contours.png", 0);
	//code.img_field_connection("file/contours.png");
	//code.visualimg_field_connectiona("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/contours.png");

	/*------------------ - 中轴的方法没啥用 dis------------------------ - */
	//detect::AxisDetection code;
	//code.disboufield("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/code3_c.png");
	//code.visual_field_Laplacian("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/code.png", "D:/workplace/visual studio 2015/QRcode/CarvingQR/data/diff_ledfield.png");
	/*------------------ - 中轴的方法没啥用 dis end------------------------ - */
	system("pause");
}

//int contourindex = 1;
//
//weight out, minout;//x,y正向
//out = code.degree_distance(contourindex, 0, erodeimg.c_str());
////init 
//minout.distance = -1;
//minout.piontAcontourindex = -1;
//minout.piontAindex = -1;
//minout.pointBTableindex = -1;
//minout.x = -1;
//minout.y = -1;
//const char* inputImage = erodeimg.c_str();
//////parallel
//int size = code.contours[contourindex].size();
//vector<int> distances(size),
//Degreedistances(size),
//xs(size),
//ys(size),
//piontAcontourindexs(size),
//piontAindexs(size),
//pointBTableindexs(size);
////cout<< code.contours[contourindex].size();
//
//const auto Connect = [&code, &contourindex, &inputImage, &Degreedistances, &xs, &ys, &piontAcontourindexs, &piontAindexs, &pointBTableindexs](int i)
//{
//	weight outtemp = code.degree_distance(contourindex, i, inputImage);
//	//cout << "parallel: " << i << endl;
//	//printf("OpenMP Test, 线程编号为: %d\n", omp_get_thread_num());
//	//cout << "contours point " << j << " )" << endl;
//	//if (out.distance <= mindis)
//	//{
//	//	mindis = out.distance;
//	//	minout = out;
//
//	//	/*cout << "out  mindis: " << mindis << "piontA: " << contours[out.piontAcontourindex][out.piontAindex].y << " , "
//	//		<< contours[out.piontAcontourindex][out.piontAindex].x << "  piontB: " << out.y << " , " << out.x << endl;*/
//	//}
//
//	//parallel
//	Degreedistances[i] = outtemp.distance;
//	xs[i] = outtemp.x;
//	ys[i] = outtemp.y;
//	piontAcontourindexs[i] = outtemp.piontAcontourindex;
//	piontAindexs[i] = outtemp.piontAindex;
//	pointBTableindexs[i] = outtemp.pointBTableindex;
//};

//igl::parallel_for(size, Connect, 8);
//
////cout << "find min" << endl;
////parallel find min 
//distances = Degreedistances;
//////remove -1的点点
////vector<int>::iterator del;
//for (int del = 0; del < Degreedistances.size();)
//{
//	//cout << "Degreedistances.size(): " << Degreedistances.size() << "   del: " << del << endl;
//	//cout << "distances.size(): " << distances.size() << "   del: " << del << endl;
//	if (Degreedistances[del] < 0)
//	{
//		Degreedistances.erase(Degreedistances.begin() + del);
//		distances.erase(distances.begin() + del);
//		xs.erase(xs.begin() + del);
//		ys.erase(ys.begin() + del);
//		piontAcontourindexs.erase(piontAcontourindexs.begin() + del);
//		piontAindexs.erase(piontAindexs.begin() + del);
//		pointBTableindexs.erase(pointBTableindexs.begin() + del);
//		//cout << "这些点点我们不要" << endl;
//	}
//	else
//	{
//		del++;
//	}
//}
//
//if (Degreedistances.size() >= 1)
//{
//	sort(Degreedistances.begin(), Degreedistances.end());
//	vector<int>::iterator it = find(distances.begin(), distances.end(), Degreedistances[0]);
//	int tempindex = it - distances.begin();
//	minout.distance = Degreedistances[0];
//	minout.piontAcontourindex = piontAcontourindexs[tempindex];
//	minout.piontAindex = piontAindexs[tempindex];
//
//	//cout << "piontAcontourindexs[tempindex]: " << piontAcontourindexs[tempindex] << "   minout.piontAcontourindex: " << minout.piontAcontourindex << endl;
//
//	minout.pointBTableindex = pointBTableindexs[tempindex];
//	minout.x = xs[tempindex];
//	minout.y = ys[tempindex];
//
//
//	Mat canvas = imread(inputImage);
//	Vec3b blue(255, 0, 0);
//	Vec3b green(0, 255, 0);
//	Vec3b red(0, 0, 255);
//
//	cout << "piontAcontourindexs array size: " << piontAcontourindexs.size() << endl;
//	for (int i = 0; i < piontAcontourindexs.size(); i++)
//	{
//		cv::Vec3b &pixel = canvas.at<Vec3b>(code.contours[piontAcontourindexs[i]][piontAindexs[i]].y, code.contours[piontAcontourindexs[i]][piontAindexs[i]].x);
//		pixel = red;
//	}
//
//	cout << "piontAindexs[tempindex]: " << piontAindexs[tempindex] << "   minout.piontAindex: " << minout.piontAindex << endl;
//
//	cv::Vec3b &pixel = canvas.at<Vec3b>(code.contours[minout.piontAcontourindex][minout.piontAindex].y, code.contours[minout.piontAcontourindex][minout.piontAindex].x);
//	pixel = blue;
//	cv::Vec3b &pixel2 = canvas.at<Vec3b>(minout.y, minout.x);
//	pixel2 = green;
//	stringstream input, iterimgstr, iterimgoutstr;
//	string iterimg("D:/workplace/visual studio 2015/QRcode/CarvingQR/data/connect/contour_mindis");
//	string iterformat(".png");
//	iterimgstr.str("");
//	iterimgstr << iterimg << contourindex << iterformat;
//	string iterin = iterimgstr.str();
//	imwrite(iterin, canvas);
//}
//else
//{
//	//跳过这个contour
//	cout << "跳过这个contour" << endl;
//}
