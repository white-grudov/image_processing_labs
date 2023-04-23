#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void __convolution(const Mat& in_image, const double* mask, int ksize, double coef, Mat& out_image, bool mod = false)
{
	out_image = in_image.clone();
	int hsize = ksize / 2;
	double sum = 0;

	for (int i = hsize; i < in_image.rows - hsize; ++i)
		for (int j = hsize; j < in_image.cols - hsize; ++j)
		{
			sum = 0;
			for (int x = 0; x < ksize; ++x)
				for (int y = 0; y < ksize; ++y)
				{
					sum += static_cast<double>(in_image.data[(i + x - hsize) * in_image.cols + (j + y - hsize)])
						* mask[x * ksize + y];
				}
			int value = static_cast<size_t>(sum * coef);
			if (value > 255) value = 255;
			if (mod) value = abs(value);
			out_image.data[i * out_image.cols + j] = value;
		}
}

void _SobelOperator(const Mat& in_image, Mat& out_image)
{
	const int ksize = 3;
	double maskGx[ksize * ksize] = {
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1
	};
	double maskGy[ksize * ksize] = {
		-1, -2, -1,
		 0,  0,  0,
		 1,  2,  1
	};
	double kf = 1. / 4.;

	Mat ImageGx, ImageGy;
	__convolution(in_image, maskGx, ksize, kf, ImageGx, true);
	__convolution(in_image, maskGy, ksize, kf, ImageGy, true);
	
	//imshow("gx", ImageGx);
	//imshow("gy", ImageGy);

	for (int i = 0; i < out_image.rows * out_image.cols; ++i)
		out_image.data[i] = sqrt(ImageGx.data[i] * ImageGx.data[i] + ImageGy.data[i] * ImageGy.data[i]);
}

int lab04()
{
	string imageName("D:/Tif/Geneva.tif");
	Mat Image;

	//cv::IMREAD_COLOR
	Image = imread(imageName.c_str(), IMREAD_GRAYSCALE);

	// Check for invalid input
	if (Image.data == 0)
	{
		cout << "Could not open or find the image" << endl;
		return -1;
	}

	Mat Out = Image.clone();

	_SobelOperator(Image, Out);

	imshow("Original image", Image); // Show our image inside it.
	imshow("Sobel image", Out);

	waitKey(0); // Wait for a keystroke in the window
	system("Pause");

	return 0;
}