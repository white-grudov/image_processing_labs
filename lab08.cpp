#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const int HIGH = 256;

int OptimalThresholdIter(const Mat& image)
{
	int hist[HIGH] = { 0 }; // Histogram
	int t0 = 0; // Initial threshold value

	for (int i = 0; i < image.rows * image.cols; ++i)
	{
		hist[image.data[i]] += 1;
		t0 += image.data[i];
	}
	t0 /= image.rows * image.cols;

	int tk = 0, mu0, mu1;
	int numer, denom;

	do
	{
		t0 = tk;
		mu0 = 0, mu1 = 0;

		numer = 0, denom = 0;
		for (int i = 0; i < t0 + 1; ++i)
		{
			numer += i * hist[i];
			denom += hist[i];
		}
		mu0 = numer / denom;

		numer = 0, denom = 0;
		for (int i = t0 + 1; i < HIGH; ++i)
		{
			numer += i * hist[i];
			denom += hist[i];
		}
		mu1 = numer / denom;

		tk = (mu0 + mu1) / 2;

	} while (t0 != tk);

	return tk;
}

void Thresholding(const Mat& input, Mat& output, int threshold)
{
	output = input.clone();

	for (int i = 0; i < input.rows * input.cols; ++i)
	{
		if (input.data[i] >= threshold) output.data[i] = 255;
		else output.data[i] = 0;
	}
}

int main()
{
	string imageName("D:/Tif/IM15.tif");
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
	int threshold = OptimalThresholdIter(Image);
	cout << threshold << endl;

	Thresholding(Image, newImage, threshold);

	imshow("Original image", Image); // Show our image inside it.
	imshow("New image", newImage);

	waitKey(0); // Wait for a keystroke in the window
	system("Pause");

	return 0;
}