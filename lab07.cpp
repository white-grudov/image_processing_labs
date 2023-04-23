#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

struct coord
{
	int x;
	int y;
};

void regionGrowing(Mat& image, int* arr, coord coords, int label, int value)
{
	vector<coord> stack;
	stack.push_back(coords);

	while (!stack.empty())
	{
		coord current = stack.back();
		stack.pop_back();

		image.data[current.x * image.cols + current.y] = label;

		for (int a = -1; a <= 1; ++a)
		{
			for (int b = -1; b <= 1; ++b)
			{
				const int pos = (current.x + a) * image.cols + (current.y + b);
				if (image.data[pos] == value && arr[pos] == 0)
				{
					stack.push_back(coord{ current.x + a, current.y + b });
					arr[pos] = 1;
				}
			}
		}
	}
}

void labeling(const Mat& input, Mat& output, int threshold, int value)
{
	output = input.clone();
	int objectsCount = 0;
	
	int* arr = new int[output.rows * output.cols];
	for (int i = 0; i < output.rows * output.cols; ++i)
		arr[i] = 0;

	// Thresholding
	for (int i = 0; i < output.rows * output.cols; ++i)
	{
		output.data[i] = (output.data[i] > threshold) ? value : 0;
	}

	// Labeling
	for (int i = 1; i < output.rows - 1; ++i)
	{
		for (int j = 1; j < output.cols - 1; ++j)
		{
			if (arr[i * output.cols + j] == 0)
			{
				if (output.data[i * output.cols + j] != value)
					continue;
				objectsCount++;

				arr[i * output.cols + j] = 1;
				regionGrowing(output, arr, coord{ i, j }, objectsCount, value);
			}
		}
	}
	for (int i = 0; i < output.rows * output.cols; ++i)
	{
		if (output.data[i] == 3) output.data[i] = 255;
		else output.data[i] = 0;
	}
	cout << objectsCount << endl;
	delete[] arr;
}

int lab07()
{
	string imageName("D:/Tif/im_kola.tif");
	Mat Image;

	//cv::IMREAD_COLOR
	Image = imread(imageName.c_str(), IMREAD_GRAYSCALE);

	// Check for invalid input
	if (Image.data == 0)
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	Mat labeled;
	int threshold = 180;
	int value = 255;
	labeling(Image, labeled, threshold, value);

	imshow("Original image", Image); // Show our image inside it.
	imshow("Labeling", labeled);

	waitKey(0); // Wait for a keystroke in the window
	system("Pause");

	return 0;
}
