#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void _convolution(const Mat& in_image, const double *mask, int ksize, double coef, Mat& out_image)
{
	out_image = in_image.clone();
	int hsize = ksize / 2;
	double sum = 0;

	for (int i = hsize; i < in_image.rows - hsize; ++i)
		for (int j = hsize; j < in_image.cols - hsize; ++j)
		{
			sum = 0;
			for (int x = -hsize; x <= hsize; ++x)
				for (int y = -hsize; y <= hsize; ++y)
				{
					sum += mask[(x + hsize) * hsize + (y + hsize)] *
						static_cast<double>(in_image.data[(i + x) * in_image.cols + (j + y)]);
				}
			out_image.data[i * out_image.cols + j] = static_cast<uchar>(sum * coef);
		}
}

void _GaussianBlur(const Mat& in_image, Mat& out_image, int ksize, double sigma)
{
	double* mask = new double[ksize * ksize];

	double hsize = ksize / 2;
	for (int x = -hsize; x <= hsize; ++x)
		for (int y = -hsize; y <= hsize; ++y)
		{
			int el = (x + hsize) * ksize + (y + hsize);
			mask[el] = exp(-(x * x + y * y) / (2 * sigma * sigma)) /
				(2 * CV_PI * sigma * sigma);
		}
	_convolution(in_image, mask, ksize, 1, out_image);

	delete[] mask;
}

int lab03()
{
	string imageName("D:/Tif/Geneva.tif");
	Mat Image;

	//cv::IMREAD_COLOR
	Image = imread(imageName.c_str(), IMREAD_GRAYSCALE);

	// Check for invalid input
	if (Image.data == 0)
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	Mat Out;

	const int ksize = 3;
	double mask[ksize * ksize] = {
		0, 1, 0, 
		1, 2, 1, 
		0, 1, 0
	};
	double coef = 1./6.;

	_convolution(Image, mask, ksize, coef, Out);

	Mat Gauss;
	double sigma = 0.7386;
	const int ksize_gaussian = 3;

	_GaussianBlur(Image, Gauss, ksize_gaussian, sigma);

	imshow("Original image", Image); // Show our image inside it.
	imshow("Convolution image", Out);
	imshow("Gaussian image", Gauss);

	waitKey(0); // Wait for a keystroke in the window
	system("Pause");

	return 0;
}