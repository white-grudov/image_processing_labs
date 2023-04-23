#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void erosion(const Mat& input, Mat& output, int ksize)
{
	output = input.clone();
	for (int i = ksize / 2; i < input.rows - ksize / 2; ++i)
		for (int j = ksize / 2; j < input.cols - ksize / 2; ++j)
		{
			if (input.data[i * input.cols + j] == 255)
			{
				for (int a = -ksize / 2; a <= ksize / 2; ++a)
					for (int b = -ksize / 2; b <= ksize / 2; ++b)
					{
						output.data[(i + a) * output.cols + (j + b)] = 255;
					}
			}
		}
}

void findBorders(const Mat& input, Mat& output, int ksize)
{
	Mat eroded;
	erosion(input, eroded, ksize);
	
	output = eroded.clone();
	for (int i = 0; i < input.cols * input.rows; ++i)
	{
		if (output.data[i] == input.data[i]) output.data[i] = 0;
	}
}

int lab06()
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

	Mat erodedImage;
	int ksize = 3;
	erosion(Image, erodedImage, ksize);

	imshow("Original image", Image); // Show our image inside it.
	imshow("Eroded image", erodedImage);

	Mat borders;
	findBorders(Image, borders, ksize);
	imshow("Borders", borders);

	waitKey(0); // Wait for a keystroke in the window
	system("Pause");

	return 0;
}
