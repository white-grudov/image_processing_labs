#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

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
	friend const bool operator==(const pos& a, const pos& b)
	{
		return a._x == b._x && a._y == b._y;
	}
};

void Tracing(const Mat& input, Mat& output, const pos& start)
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
		output.data[current._x * input.cols + current._y] = value;
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

void BoundaryTracing(const Mat& input, Mat& output)
{
	output = Mat(input.rows, input.cols, CV_8UC1, cv::Scalar(0));

	for (int i = 0; i < input.rows; ++i)
	{
		for (int j = 0; j < input.cols; ++j)
		{
			if (input.data[i * input.cols + j] == value)
			{
				Tracing(input, output, pos(i, j));
				return;
			}
		}
	}
}

int lab09()
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

	BoundaryTracing(Image, newImage);

	imshow("Original image", Image); // Show our image inside it.
	imshow("New image", newImage);

	waitKey(0); // Wait for a keystroke in the window
	system("Pause");

	return 0;
}