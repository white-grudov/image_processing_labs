#include <iostream>
#include <fstream>
#include <thread>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

const int value = 255;
const int connectivity = 8;

struct pos
{
	int _x, _y;
	pos(int x, int y)
	{
		_x = x; _y = y;
	}
	friend const pos operator+(const pos& a, const pos& b)
	{
		return pos(a._x + b._x, a._y + b._y);
	}
	friend const pos operator-(const pos& a, const pos& b)
	{
		return pos(a._x - b._x, a._y - b._y);
	}
	friend const bool operator==(const pos& a, const pos& b)
	{
		return a._x == b._x && a._y == b._y;
	}
	const double abs()
	{
		return sqrt(this->_x * this->_x + this->_y * this->_y);
	}
};


void writeToFile(vector<double> values)
{
	ofstream out;
	out.open("D:/учеба/2.1/ммоз/values.txt");

	if (out.is_open())
		for (double value : values)
			out << value << endl;

	out.close();
}


int mod(int a, int b)
{
	while (a < 0) a += b;
	return a % b;
}

void tracing(const Mat& input, Mat& output, const pos& start, vector<pos>& contour)
{
	int dir = 7;
	int temp_dir = 0;
	pos current = start;
	pos temp(0, 0);

	pos directions[connectivity] =
	{
		pos(1, 0), pos(1, -1), pos(0, -1), pos(-1, -1),
		pos(-1, 0), pos(-1, 1), pos(0, 1), pos(1, 1)
	};
	do
	{
		contour.push_back(current);
		temp_dir = dir % 2 ? (dir + 6) % 8 : (dir + 7) % 8;
		temp = current + directions[temp_dir];

		while (input.data[temp._x * input.cols + temp._y] != value)
		{
			temp_dir++;
			if (temp_dir == connectivity) temp_dir = 0;
			temp = current + directions[temp_dir];
		}
		current = temp;
		dir = temp_dir;
	} while (!(current == start));
}

void boundaryTracing(const Mat& input, Mat& output, vector<pos>& contour)
{
	output = Mat(input.rows, input.cols, CV_8UC1, cv::Scalar(0));

	for (int i = 0; i < input.rows; ++i)
	{
		for (int j = 0; j < input.cols; ++j)
		{
			if (input.data[i * input.cols + j] == value)
			{
				tracing(input, output, pos(i, j), contour);
				return;
			}
		}
	}
}

void Curvature(const vector<pos>& p, vector<double>& values, const int k)
{
	const int n = p.size();
	pos pk(0, 0), mk(0, 0);
	pos f(0, 0), b(0, 0);
	double df = 0, db = 0;
	double theta_f = 0, theta_b = 0, theta_i = 0;
	double delta_f = 0, delta_b = 0, ki = 0;

	for (int i = 0; i < n; ++i)
	{
		pk = p[mod(i + k, n)];
		mk = p[mod(i - k, n)];

		f = p[i] - pk; b = p[i] - mk;

		df = f.abs();
		db = b.abs();

		theta_f = atan2(fabs(pk._x - p[i]._x), fabs(pk._y - p[i]._y));
		theta_b = atan2(fabs(mk._x - p[i]._x), fabs(mk._y - p[i]._y));
		theta_i = (theta_f + theta_b) / 2.;
		
		delta_f = theta_f - theta_i;
		delta_b = theta_b - theta_i;

		ki = (delta_f * (db + df)) / (2 * db * df);
		values.push_back(ki);
	}
}

int lab10()
{
	//string imageName("D:/Tif/contour.tif");
	//string imageName("D:/Tif/line.tif");
	string imageName("D:/Tif/circle.tif");
	//string imageName("D:/Tif/square.tif");
	Mat Image;

	//cv::IMREAD_COLOR
	Image = imread(imageName.c_str(), IMREAD_GRAYSCALE);

	// Check for invalid input
	if (Image.data == 0)
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	Mat newImage;

	vector<pos> contour;
	boundaryTracing(Image, newImage, contour);

	int k = 5;
	vector<double> values;
	Curvature(contour, values, k);

	//for (int i = 0; i < values.size(); ++i)
	//	cout << i << ": " << values[i] << endl;

	writeToFile(values);
	thread t1(system, "python D:/учеба/2.1/ммоз/contour_graph.py");

	imshow("Original image", Image); // Show our image inside it.
	//imshow("New image", newImage);

	waitKey(0); // Wait for a keystroke in the window
	system("Pause");

	t1.join();

	return 0;
}