	#ifndef COMPARE_IMAGE
	#define COMPARE_IMAGE	
	#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <numeric>
	double compare_image(const char * file1, const char * file2);
#endif