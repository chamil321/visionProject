#pragma once
#ifndef LICENCE_PLATE_DETECTOR
#define LICENCE_PLATE_DETECTOR

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

class LicencePlateDetector
{
public:
	LicencePlateDetector();
	~LicencePlateDetector();
	int detectLicencePlate(cv::Mat &currentFrame, cv::Mat &pureFrame);
};

#endif // !1
