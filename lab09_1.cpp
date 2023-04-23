#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const int value = 255;

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
	friend const pos operator+=(pos& a, const pos& b)
	{
		a = a + b;
		return a;
	}
	void toLeft()
	{
		pos temp = *this;
		this->_x = temp._y;
		this->_y = -temp._x;
	}
	void toRight()
	{
		pos temp = *this;
		this->_x = -temp._y;
		this->_y = temp._x;
	}
};

void SquareTracing(const Mat& input, Mat& output, const pos& start)
{
	output.data[start._x * input.cols + start._y] = value;

	pos step(1, 0);
	step.toLeft();
	pos next = start + step;

	while (!(next == start))
	{
		if (input.data[next._x * input.cols + next._y] == value)
		{
			output.data[next._x * input.cols + next._y] = value;
			step.toLeft();
		}
		else step.toRight();
		next += step;
	}
}

void _BoundaryTracing(const Mat& input, Mat& output)
{
	output = Mat(input.rows, input.cols, CV_8UC1, cv::Scalar(0));

	for (int i = 0; i < input.rows; ++i)
	{
		for (int j = 0; j < input.cols; ++j)
		{
			if (input.data[i * input.cols + j] == value)
			{
				SquareTracing(input, output, pos(i, j));
				return;
			}
		}
	}
}

int lab09_1()
{
	string imageName("D:/Tif/contour.tif");
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

	_BoundaryTracing(Image, newImage);

	imshow("Original image", Image); // Show our image inside it.
	imshow("New image", newImage);

	waitKey(0); // Wait for a keystroke in the window
	system("Pause");

	return 0;
}