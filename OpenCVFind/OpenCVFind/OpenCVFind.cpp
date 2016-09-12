#include <opencv2\opencv.hpp>
#include <vector>
#include <stdio.h>
#include <string.h>

using namespace cv;
using namespace std;

bool g_found = false;					//success find chess bord corners
int g_boardwidth = 6;
int g_boardheight = 7;
int g_boardtotal;
Size g_boardSize;
vector<vector<Point2f>> g_imagePoints;			//角点坐标
vector<vector<Point3f>> g_objectPoints;			//世界坐标

Mat camerMatirx;								//相机内参矩阵
Mat disCoeffs;									//畸变矩阵

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

void findCornerWork()
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

	for (int i = 0; i < g_boardtotal; i++)
	{
		cout << CornerConers[i].x << "   " << CornerConers[i].y << endl;
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
		findCornerWork();
		waitKey(10);
	}
}
int main(int argc, char * argv[])
{
	char prefixName[100] = { "E:\\openCV document\\OpenCV demarcate\\Image\\" };
	char postfixName[10] = { ".jpg" };
	int ImageN = 15;
	calcuCornerCoor(prefixName, postfixName, ImageN);

	return 0;
}

