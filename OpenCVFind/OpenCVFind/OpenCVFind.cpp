#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <vector>
#include <stdio.h>
#include <string.h>

using namespace cv;
using namespace std;

#define g_boardwidth 8
#define g_boardheight 6

bool g_found = false;					//success find chess bord corners
bool g_mustInitUndistort = false;			//是否进行相机标定标志位
int g_boardtotal;
Size g_boardSize;


vector<vector<Point2f>> g_imagePoints;			//角点坐标
vector<vector<Point3f>> g_objectPoints;			//世界坐标

Mat g_camerMatirx;								//相机内参矩阵
Mat g_disCoeffs;									//畸变矩阵

Mat g_grayImage, g_srcImage, g_cvmaImage;

void initFindCorner(char * fileName)
{
	g_boardSize = Size(g_boardwidth, g_boardheight);
	g_boardtotal = g_boardheight * g_boardwidth;
	
	g_srcImage = imread(fileName);
	imshow("【源图像】", g_srcImage);

	cvtColor(g_srcImage, g_grayImage, COLOR_BGR2GRAY);
	g_cvmaImage = g_srcImage.clone();
}

//保存角点坐标数据
void addPoints(vector<Point2f> & CornerConers, vector<Point3f> & objectConers)
{
	g_imagePoints.push_back(CornerConers);
	g_objectPoints.push_back(objectConers);
}

void findCornerWork(char * filename)
{
	vector<Point2f> CornerConers;
	vector<Point3f> objectConers;

	g_found = findChessboardCorners(g_grayImage, g_boardSize, CornerConers, 3);
	
	cornerSubPix(g_grayImage, CornerConers, Size(12, 12), Size(-1, -1), TermCriteria(
		CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,	40,	0.001));
	
	//输入角点的世界坐标值
	for (int i = 0; i < g_boardheight; i++)
	{
		for (int j = 0; j < g_boardwidth; j++)
		{
			objectConers.push_back(Point3f(i, j, 0.0f));
		}
	}

	if (CornerConers.size() == g_boardSize.area())
	{
		addPoints(CornerConers, objectConers);
		printf("%s:Success find Corner.\n", filename);
	}

	drawChessboardCorners(g_cvmaImage, g_boardSize, CornerConers, g_found);
	imshow("【角点标识图】", g_cvmaImage);
}

void calcuCornerCoor(char * prefix, char * postfix, int ImageN)
{
	char filename[200] = {0};
	int i;
	for ( i = 1; i < ImageN; i++)
	{
		sprintf(filename, "%s%d%s", prefix, i, postfix);

		initFindCorner(filename);
		findCornerWork(filename);
		waitKey(10);
	}
}

//相机标定
double calibrate(Size & imagesize)
{
	g_mustInitUndistort = true;
	vector<Mat> tvecs, rvecs;

	return calibrateCamera(g_objectPoints, g_imagePoints, imagesize, g_camerMatirx, g_disCoeffs, rvecs, tvecs);
}

//去畸变
Mat Remap(Mat & image)
{
	Mat undistorted;
	Mat map1, map2;
	if (g_mustInitUndistort)
	{
		//计算畸变参数
		initUndistortRectifyMap(g_camerMatirx, g_disCoeffs, Mat(), Mat(), image.size(), CV_32FC1, map1, map2);
		g_mustInitUndistort = false;
	}

	//映射函数
	remap(image, undistorted, map1, map2, INTER_LINEAR);

	return undistorted;
}

//打印内参，和畸变系数
void PrintfCamerMatrix()
{
	int romNumber = g_camerMatirx.rows;
	int colNumber = g_camerMatirx.cols;

	cout << " Camera intrinsic: " << romNumber << " * " << colNumber << endl;
	for (int i = 0; i < romNumber; i++)
	{
		for (int j = 0; j < colNumber; j++)
		{
			printf("%-5.3f\t\t", g_camerMatirx.at<double>(i, j));
		}
		cout << endl;
	}

	int romdisNumber = g_disCoeffs.rows;
	int coldisNumber = g_disCoeffs.cols;

	cout << " disCoeffs: " << romdisNumber << " * " << coldisNumber << endl;
	for (int i = 0; i < romdisNumber; i++)
	{
		for (int j = 0; j < coldisNumber; j++)
		{
			printf("%-3.5f", g_disCoeffs.at<double>(i, j));
		}
		cout << endl;
	}
	cout << endl;
}

int main(int argc, char * argv[])
{
	//定义基本变量
	char prefixName[100] = { "E:\\openCV document\\OpenCV demarcate\\OpenCVFind\\Image\\" };

	char postfixName[10] = { ".bmp" };
	int ImageN = 25;

	//相机标定
	calcuCornerCoor(prefixName, postfixName, ImageN);
	calibrate(g_srcImage.size());

	//去畸变
	char str[200] = { 0 };
	int d_Image = 1;
	sprintf(str, "%s%d%s", prefixName, d_Image, postfixName);

	Mat Image = imread(str);
	Mat dstImage = Remap(Image);

	imshow("原图像", Image);
	imshow("去畸变", dstImage);

	PrintfCamerMatrix();
	waitKey(0);

	return 0;
}

