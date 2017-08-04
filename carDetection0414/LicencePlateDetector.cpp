#include "stdafx.h"
#include "LicencePlateDetector.h"
#include "Plate Detector\DetectPlatesInit.h"

LicencePlateDetector::LicencePlateDetector()
{
}

LicencePlateDetector::~LicencePlateDetector()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int detectLicencePlate(cv::Mat &currentFrame, cv::Mat &pureFrame) {

	bool blnKNNTrainingSuccessful = loadKNNDataAndTrainKNN();           // attempt KNN training

	if (blnKNNTrainingSuccessful == false) {                            // if KNN training was not successful
																		// show error message
		std::cout << std::endl << std::endl << "error: error: KNN traning was not successful" << std::endl << std::endl;
		return(0);                                                      // and exit program
	}

	cv::Mat imgOriginalScene = pureFrame.clone();           // input image

															//imgOriginalScene = cv::imread("test1_digit.png");         // open image

	if (imgOriginalScene.empty()) {                             // if unable to open image
		std::cout << "error: image not read from file\n\n";     // show error message on command line
		return(0);                                              // and exit program
	}

	std::vector<PossiblePlate> vectorOfPossiblePlates = detectPlatesInScene(imgOriginalScene);          // detect plates

	vectorOfPossiblePlates = detectCharsInPlates(vectorOfPossiblePlates);                               // detect chars in plates

	cv::imshow("imgOriginalScene", imgOriginalScene);           // show scene image

	if (vectorOfPossiblePlates.empty()) {                                               // if no plates were found
		std::cout << std::endl << "no license plates were detected" << std::endl;       // inform user no plates were found
	}
	else {                                                                            // else
																					  // if we get in here vector of possible plates has at leat one plate

																					  // sort the vector of possible plates in DESCENDING order (most number of chars to least number of chars)
		std::sort(vectorOfPossiblePlates.begin(), vectorOfPossiblePlates.end(), PossiblePlate::sortDescendingByNumberOfChars);

		// suppose the plate with the most recognized chars (the first plate in sorted by string length descending order) is the actual plate
		PossiblePlate licPlate = vectorOfPossiblePlates.front();

		cv::imshow("imgPlate", licPlate.imgPlate);            // show crop of plate and threshold of plate
		cv::imshow("imgThresh", licPlate.imgThresh);

		if (licPlate.strChars.length() == 0) {                                                      // if no chars were found in the plate
			std::cout << std::endl << "no characters were detected" << std::endl << std::endl;      // show message
			return(0);                                                                              // and exit program
		}

		drawRedRectangleAroundPlate(imgOriginalScene, licPlate);                // draw red rectangle around plate

		std::cout << std::endl << "license plate read from image = " << licPlate.strChars << std::endl;     // write license plate text to std out
		std::cout << std::endl << "-----------------------------------------" << std::endl;

		writeLicensePlateCharsOnImage(imgOriginalScene, licPlate);              // write license plate text on the image

		cv::imshow("imgOriginalScene", imgOriginalScene);                       // re-show scene image

		cv::imwrite("imgOriginalScene.png", imgOriginalScene);                  // write image out to file
	}

	//cv::waitKey(0);                 // hold windows open until user presses a key

	return(0);
}
