#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void convolution(const Mat& in_image, const double* mask, int ksize, double coef, Mat& out_image, bool mod = false)
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

void GaussianBlur(const Mat& in_image, Mat& out_image, int ksize, double sigma)
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
    convolution(in_image, mask, ksize, 1, out_image);

    delete[] mask;
}

void SobelOperator(const Mat& in_image, Mat& out_image, double* theta)
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
    double kf = 1. / 2.;

    Mat ImageGx, ImageGy;

    convolution(in_image, maskGx, ksize, kf, ImageGx, true);
    convolution(in_image, maskGy, ksize, kf, ImageGy, true);

    for (int i = 0; i < out_image.rows * out_image.cols; ++i)
    {
        out_image.data[i] = hypot(ImageGx.data[i], ImageGy.data[i]);
        theta[i] = atan2(ImageGy.data[i], ImageGx.data[i]) * (180 / CV_PI);
    }

}
void CannyAlgorithm(Mat input, Mat& output, int ksize, double sigma, int lowThreshold, int highThreshold)
{
    // Blur
    Mat Gauss = Mat(input.rows, input.cols, CV_8UC1, cv::Scalar(0));
    GaussianBlur(input, Gauss, ksize, sigma);

    // Gradient operator
    Mat gradient = Mat(input.rows, input.cols, CV_8UC1, cv::Scalar(0));
    double* theta = new double[Gauss.rows * Gauss.cols];
    SobelOperator(Gauss, gradient, theta);
    //imshow("Gradient", gradient);

    // Non-maximum supression
    for (int i = 0; i < input.rows * input.cols; ++i)
    {
        if (theta[i] < 22.5 || theta[i] >= 157.5)
            theta[i] = 0;
        else if (theta[i] >= 22.5 && theta[i] < 67.5)
            theta[i] = 45;
        else if (theta[i] >= 67.5 && theta[i] < 112.5)
            theta[i] = 90;
        else if (theta[i] >= 112.5 && theta[i] < 157.5)
            theta[i] = 135;
    }
    Mat supressed = Mat(input.rows, input.cols, CV_8UC1, cv::Scalar(0));
    int a = 0;
    int b = 0;
    for (int i = ksize / 2; i < gradient.rows - ksize / 2; ++i)
        for (int j = ksize / 2; j < gradient.cols - ksize / 2; ++j)
        {
            a = 0, b = 0;
            switch (static_cast<int>(theta[i * gradient.cols + j]))
            {
            case 0:
            {
                a = gradient.data[i * gradient.cols + j + 1];
                b = gradient.data[i * gradient.cols + j - 1];
                break;
            }
            case 45:
            {
                a = gradient.data[(i + 1) * gradient.cols + j - 1];
                b = gradient.data[(i - 1) * gradient.cols + j + 1];
                break;
            }
            case 90:
            {
                a = gradient.data[(i + 1) * gradient.cols + j];
                b = gradient.data[(i - 1) * gradient.cols + j];
                break;
            }
            case 135:
            {
                a = gradient.data[(i - 1) * gradient.cols + j - 1];
                b = gradient.data[(i + 1) * gradient.cols + j + 1];
                break;
            }
            }
            if (gradient.data[i * gradient.cols + j] >= a && gradient.data[i * gradient.cols + j] >= b)
                supressed.data[i * supressed.cols + j] = gradient.data[i * gradient.cols + j];
            else
                supressed.data[i * supressed.cols + j] = 0;
        }
    //imshow("Supressed", supressed);

    // Double threshold
    Mat filtrated = Mat(input.rows, input.cols, CV_8UC1, cv::Scalar(0));
    for (int i = 0; i < input.cols * input.rows; ++i)
    {
        if (supressed.data[i] < lowThreshold) filtrated.data[i] = 0;
        else if (supressed.data[i] > highThreshold) filtrated.data[i] = 255;
    }

    // Edge tracking
    for (int i = ksize / 2; i < input.rows - ksize / 2; ++i)
        for (int j = ksize / 2; j < input.cols - ksize / 2; ++j)
        {
            if (supressed.data[i * supressed.cols + j] > lowThreshold && supressed.data[i * supressed.cols + j] < highThreshold)
            {
                for (int a = -1; a <= 1; ++a)
                    for (int b = -1; b <= 1; ++b)
                    {
                        if (filtrated.data[(i + a) * filtrated.cols + (j + b)] == 255)
                            filtrated.data[i * filtrated.cols + j] = 255;
                    }
            }
            if (filtrated.data[i * filtrated.cols + j] != 255) filtrated.data[i * filtrated.cols + j] = 0;
        }
    output = filtrated.clone();

    delete[] theta;
}

int lab05()
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
    int ksize = 3;
    double sigma = 1;

    int low = 10;
    int ratio = 3;

    CannyAlgorithm(Image, Out, ksize, sigma, low, low * ratio);

    imshow("Original image", Image); // Show our image inside it.
    imshow("Canny image", Out);

    Mat blurred = Image.clone();
    Mat builtIn = Image.clone();
    low = 40;
    blur(Image, blurred, Size(ksize, ksize));
    Canny(blurred, builtIn, low, low * ratio, ksize);

    imshow("Built-in Canny", builtIn);

    waitKey(0); // Wait for a keystroke in the window
    system("Pause");

    return 0;
}