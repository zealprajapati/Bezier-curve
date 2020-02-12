#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp> //include all the basic data types from opencv
#include <opencv2/highgui/highgui.hpp>//allow you to load image from computer or display image onto the screen
#include <opencv2/imgproc/imgproc.hpp> //provide functions to process an image

using namespace std;
using namespace cv;

vector<Point>point;
vector<Point>square;


bool press = false;
bool click = false;
bool rect = false;
bool straight_lines = true;
Mat img;
Mat result;
bool greenColor;
int tempcounter = 0;
int square_index = NULL;
int a, b, c, d;
int blue = 0;
int green = 0;
int red = 0;
int thickness = 1;
int special_thickness = thickness;
int sa;
int current_green = -1;

void drawPoints()
{
	if (straight_lines)
	{
		for (int i = 0; i < point.size(); i++)
		{
			circle(img, Point(point[i].x, point[i].y), 1, Scalar(0, 0, 255), 3);

		}
	}
}
void drawSpline()
{
	int splinecount = point.size() / 4;
	int count = 0;

	for (int i = 0; i < splinecount; i++)
	{
		double M_data[4][4] = { { -1, 3, -3, 1 }, { 3, -6, 3, 0 }, { -3, 3, 0, 0 }, { 1, 0, 0, 0 } };
		

		double G_data[4][2] = { { (double)point[count].x, (double)point[count].y }, { (double)point[count + 1].x, (double)point[count + 1].y }, {  (double)point[count + 2].x, (double)point[count + 2].y}, {  (double)point[count + 3].x, (double)point[count + 3].y} };

		Mat M = Mat(4, 4, CV_64FC1, M_data);
		Mat G = Mat(4, 2, CV_64FC1, G_data);

		result = M * G;

		//cout << result << endl;

		double ax = result.ptr<double>(0)[0];
		double bx = result.ptr<double>(1)[0];
		double cx = result.ptr<double>(2)[0];
		double dx = result.ptr<double>(3)[0];

		double ay = result.ptr<double>(0)[1];
		double by = result.ptr<double>(1)[1];
		double cy = result.ptr<double>(2)[1];
		double dy = result.ptr<double>(3)[1];

		for (double t = 0.0; t <= 1.0; t += 0.001)
		{
			int x = ax * pow(t, 3) + bx * pow(t, 2) + cx * t + dx;
			int y = ay * pow(t, 3) + by * pow(t, 2) + cy * t + dy;
			

			if (press && current_green == i)
				circle(img, Point(x, y), 1, Scalar(blue, green, red), special_thickness, 8, 0);
			else
				circle(img, Point(x, y), 1, Scalar(0, 0,0), thickness, 8, 0);
		}
		int x = ax * pow(0.5, 3) + bx * pow(0.5, 2) + cx * 0.5 + dx;
		int y = ay * pow(0.5, 3) + by * pow(0.5, 2) + cy * 0.5 + dy;

		//if (green)
		
		
		
		if (press && current_green == i)
			rectangle(img, Point(x - 15, y - 15), Point(x + 15, y + 15), Scalar(0, 255, 0), 1, 8, 0);
		else
			rectangle(img, Point(x - 15, y - 15), Point(x + 15, y + 15), Scalar(200, 200, 200), 1, 8, 0);
		//else
		//	rectangle(img, Point(x - 15, y - 15), Point(x + 15, y + 15), Scalar(200, 200, 200), 1, 8, 0);

		if (square.size() > 0)
		{
			for (int i = 0; i < square.size(); i++)
			{
				if (square[i].x == x && square[i].y == y)
				{
					rect = true;
				}
			}
			if (!rect)
			{
				square.push_back(Point(x, y));
			}
			rect = false;
		}
		else
		{
			square.push_back(Point(x, y));

		}

		if (straight_lines)
		{
			line(img, point[count], point[count + 1], Scalar(0, 0, 0), 1, 8, 0);
			line(img, point[count + 2], point[count + 3], Scalar(0, 0, 0), 1, 8, 0);
		}
		count += 4;
	}
}
void editingRegion(Point p)
{
	for (int i = 0; i < square.size(); i++)
	{
		if ((p.x >= square[i].x - 15) && (p.x <= square[i].x + 15) && (p.y >= square[i].y - 15) && (p.y <= square[i].y + 15))
		{
			current_green = i;
			press = true;
			//cout << "green";
			int x = result.ptr<double>(0)[0] * pow(0.5, 3) + result.ptr<double>(1)[0] * pow(0.5, 2) + result.ptr<double>(2)[0] * 0.5 + result.ptr<double>(3)[0];
			int y = result.ptr<double>(0)[1] * pow(0.5, 3) + result.ptr<double>(1)[1] * pow(0.5, 2) + result.ptr<double>(2)[1] * 0.5 + result.ptr<double>(3)[1];
			rectangle(img, Point(p.x - 15, p.y - 15), Point(p.x + 15, p.y + 15), Scalar(0, 255, 0), 1, 8, 0);
			square_index = i;
			break;
		}
		//special_thickness = thickness;
	}
}

void onMouse(int evt, int x, int y, int flags, void* param)
{
	if (evt == CV_EVENT_LBUTTONDOWN)
	{
		if (press)
			click = true;
		for (int i = 0; i < point.size(); i++)
		{
			a = pow((point[i].x - x), 2);
			b = pow((point[i].y - y), 2);
			c = sqrt(a + b);
			if (c <= 5)
			{

				tempcounter = i;
				break;

			}
			straight_lines = true;

		}
		if (tempcounter > 0)
		{
			point[tempcounter].x = x;
			point[tempcounter].y = y;

		}
		else if (!press)
		{
			point.push_back(Point(x, y));
		}


	}
	else if (evt == CV_EVENT_LBUTTONUP)
	{
		tempcounter = 0;
		press = false;
		click = false;
	}

	else if (evt == CV_EVENT_MOUSEMOVE)
	{
		press = false;
		editingRegion(Point(x, y));
		if (click)
		{
			Point newPoint;
			newPoint.x = x - square[square_index].x;
			newPoint.y = y - square[square_index].y;

			int splinecount = point.size() / 4;

			int counter = 0;
			for (int i = 0; i < splinecount; i++)
			{
				if (i == square_index)
				{
					point[counter] = point[counter] + newPoint;
					point[counter + 1] = point[counter + 1] + newPoint;
					point[counter + 2] = point[counter + 2] + newPoint;
					point[counter + 3] = point[counter + 3] + newPoint;

				}
				counter += 4;

			}
			square[square_index] = Point(x, y);
		}
		if (tempcounter > 0)
		{
			point[tempcounter].x = x;
			point[tempcounter].y = y;
		}

	}

	else if (evt == CV_EVENT_RBUTTONDOWN)
	{
		if (point.size() > 0)
		{
			point.pop_back();
		}
	}

}
int main(int argc, char** argv)
{

	img = Mat(600, 800, CV_8UC3);

	namedWindow("canvas");
	setMouseCallback("canvas", onMouse);
	while(1)
	{
		img = Scalar(255, 255, 255);
		square.clear();
		drawSpline();
		drawPoints();
		imshow("canvas", img);




		char c = waitKey(1);

		if (c == 'i' || c == 'I')
		{
			special_thickness++;
		}


		else if (c == 'd' || c == 'D')
		{
			if (special_thickness > thickness)
			{
				special_thickness--;
			}
			else
			{
				special_thickness == 1;
			}
		}
		else if (c == 'r' || c == 'R')
		{
			red = 255;
			green = 0;
			blue = 0;
		}
		else if (c == 'g' || c == 'G')
		{
			red = 0;
			green = 255;
			blue = 0;
		}
		else if (c == 'b' || c == 'B')
		{
			red = 0;
			green = 0;
			blue = 255;
		}
		else if (c == 'p' || c == 'P')
		{
			red = 255;
			green = 105;
			blue = 180;
		}
		else if (c == 'y' || c == 'Y')
		{
			red = 255;
			green = 255;
			blue = 0;
		}
		else if (c == '.')
		{
			if (straight_lines)
			{
				straight_lines = false;
			}
			else
			{
				straight_lines = true;
			}
		}
		else if (c == 's' || c == 'S')
		{
			ostringstream name;
			name << "Spline" << sa << ".jpeg";
			imwrite(name.str(), img);
			sa++;
			cout << name.str() << "has been saved InClass" << "\n";
		}
		else if (c == 27)
		{

			break;
		}
	}

	return 0;
}