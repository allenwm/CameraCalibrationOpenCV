#include <opencv2\opencv.hpp>
#include <vector>
#include <stdio.h>

using namespace cv;
using namespace std;

bool g_found = false;					//success find chess bord corners
int g_boardwidth = 6;
int g_boardheight = 7;
int g_boardtotal;
Size g_boardSize;
Mat g_image_points;
Mat g_grayImage, g_srcImage, g_cvmaImage;
char * g_pinName;

void initFindCorner()
{
	g_boardSize = Size(g_boardwidth, g_boardheight);
	g_boardtotal = g_boardheight * g_boardwidth;

	g_image_points = Mat::zeros(Size(2, g_boardtotal), CV_8U);
	
	g_srcImage = imread(g_pinName);
	imshow("【源图像】", g_srcImage);

	cvtColor(g_srcImage, g_grayImage, COLOR_BGR2GRAY);
	g_cvmaImage = g_srcImage.clone();
}

void findCornerWork()
{
	g_found = findChessboardCorners(g_grayImage, g_boardSize, g_image_points, 3);
	
	cornerSubPix(g_grayImage, g_image_points, Size(12, 12), Size(-1, -1), TermCriteria(
		CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,	40,	0.001));

	for (int i = 0; i < g_boardtotal; i++)
	{
		uchar * data = g_image_points.ptr<uchar>(i);
		for (int j = 0; j < 2; j++)
		{
			cout << (int)data[j] << "   ";
		}
		cout << endl;
	}

	drawChessboardCorners(g_cvmaImage, g_boardSize, g_image_points, g_found);
	imshow("【角点标识图】", g_cvmaImage);
}

int main(int argc, char * argv[])
{
	g_pinName = new char[128];

	int i;
	for ( i = 1; i < 15; i++)
	{
		sprintf(g_pinName, "E:\\openCV document\\OpenCV demarcate\\Image\\%d.jpg", i);
		initFindCorner();
		findCornerWork();
		waitKey(10);
	}
	return 0;
}

