#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

const int hist_size = 256;

Mat histogram(const Mat& image)
{
	const int hist_height = 200;

	int hist[hist_size] = { 0 };
	for (int i = 0; i < image.rows * image.cols; i++)
		hist[image.data[i]]++;

	for (int& value : hist)
	{
		double max = (double)* max_element(hist, hist + hist_size) / hist_height;
		value /= max;
	}

	Mat hist_image(hist_height, hist_size, CV_8UC1, cv::Scalar(0));

	for (int i = 0; i < hist_size; i++)
		for (int j = 0; j < hist[i]; j++)
			hist_image.data[(hist_height - j - 1) * hist_size + i] = 255;

	resize(hist_image, hist_image, Size(), 2.5, 2.5, 0);
	return hist_image;
}

void HistogramEqualization(const Mat& in_image, Mat& out_image, Mat& in_histogram, Mat& out_histogram)
{
	int rows = in_image.rows, cols = in_image.cols;

	int hist[hist_size] = { 0 };
	int cum_hist[hist_size] = { 0 };

	for (int i = 0; i < rows * cols; i++)
		hist[in_image.data[i]]++;

	cum_hist[0] = hist[0];
	for (int i = 1; i < hist_size; i++)
		cum_hist[i] = cum_hist[i - 1] + hist[i];

	double alpha = double(hist_size) / rows / cols;

	for (int i = 0; i < rows * cols; i++)
	{
		int new_data = cum_hist[in_image.data[i]] * alpha;
		out_image.data[i] = new_data > 255 ? 255 : new_data;
	}

	in_histogram = histogram(in_image);
	out_histogram = histogram(out_image);
}

int lab02()
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

	Mat Out = Image.clone();
	Mat hist_in, hist_out;

	HistogramEqualization(Image, Out, hist_in, hist_out);

	imshow("Original image", Image); // Show our image inside it.
	imshow("Result image", Out);
	imshow("Histogram in", hist_in);
	imshow("Histogram out", hist_out);

	waitKey(0); // Wait for a keystroke in the window
	system("Pause");

	return 0;
}
