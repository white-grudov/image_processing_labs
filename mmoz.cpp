#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>



int _()
{
	std::string str = "Image";
	std::string imageName("D:/Tif/Tif/IM1.tif");
	int rows = 512;
	int cols = 512;
	cv::Mat Image;

	//cv::IMREAD_COLOR
	Image = cv::imread(imageName.c_str(), cv::IMREAD_GRAYSCALE);


	cv::Mat gray(rows, cols, CV_8UC1, cv::Scalar(0));

	// Check for invalid input
	if (Image.data == 0)
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	for (int i = 0; i < 50; i++)
		std::cout << "test" << std::endl;

	cv::imshow("Display window", Image);                // Show our image inside it.

	cv::waitKey(0); // Wait for a keystroke in the window

	system("Pause");

	

	return 0;
}
