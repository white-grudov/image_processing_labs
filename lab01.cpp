#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void decrease(const Mat in_image, Mat& out_image, int n) {
	out_image = Mat(in_image.rows / n, in_image.cols / n, CV_8UC1, cv::Scalar(0));

	for (int i = 0; i < out_image.rows; i++) {
		for (int j = 0; j < out_image.cols; j++) {
			out_image.data[i * out_image.cols + j] = in_image.data[(i * in_image.cols + j) * n];
		}
	}
}

void increase(const Mat in_image, Mat& out_image, int n) {
	out_image = Mat(in_image.rows * n, in_image.cols * n, CV_8UC1, cv::Scalar(0));

	for (int i = 0; i < out_image.rows; i++) {
		for (int j = 0; j < out_image.cols; j++) {
			out_image.data[i * out_image.cols + j] = in_image.data[i / n * in_image.cols + j / n];
		}
	}
}

int lab01()
{
	string imageName("D:/Tif/Tif/IM1.tif");
	Mat Image;

	//cv::IMREAD_COLOR
	Image = imread(imageName.c_str(), IMREAD_GRAYSCALE);

	// Check for invalid input
	if (Image.data == 0)
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	Mat decreasedImage, increasedImage;
	decrease(Image, decreasedImage, 2);
	increase(Image, increasedImage, 2);


	imshow("Original image", Image); // Show our image inside it.
	imshow("Decreased image", decreasedImage);
	imshow("Increased image", increasedImage);

	waitKey(0); // Wait for a keystroke in the window
	system("Pause");

	return 0;
}
