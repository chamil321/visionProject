#include "stdafx.h"
#include "LicencePlateDetector.h"
#include "Plate Detector\DetectPlatesInit.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <cstdio>

void readPlates(int plateno);

LicencePlateDetector::LicencePlateDetector()
{

}

LicencePlateDetector::~LicencePlateDetector()
{
}

int carCount = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////

int detectLicencePlate(cv::Mat &currentFrame, cv::Mat &pureFrame,int &car_count) {
	bool show_steps = false;	// make this true to show steps

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

	//cv::imshow("imgOriginalScene", imgOriginalScene);           // show scene image

	if (vectorOfPossiblePlates.empty()) {                                               // if no plates were found
		std::cout << std::endl << "no license plates were detected" << std::endl;       // inform user no plates were found
	}
	else {                                                                            // else
																					  // if we get in here vector of possible plates has at leat one plate

																					  // sort the vector of possible plates in DESCENDING order (most number of chars to least number of chars)
		std::sort(vectorOfPossiblePlates.begin(), vectorOfPossiblePlates.end(), PossiblePlate::sortDescendingByNumberOfChars);

		// suppose the plate with the most recognized chars (the first plate in sorted by string length descending order) is the actual plate
		PossiblePlate licPlate = vectorOfPossiblePlates.front();

		if(show_steps) cv::imshow("imgPlate", licPlate.imgPlate);            // show crop of plate and threshold of plate
		if (show_steps) cv::imshow("imgThresh", licPlate.imgThresh);

		if (licPlate.strChars.length() == 0) {                                                      // if no chars were found in the plate
			std::cout << std::endl << "no characters were detected" << std::endl << std::endl;      // show message
			return(0);                                                                              // and exit program
		}


		// added for recognition of number plate characters with japan
		cv::imshow("plate", licPlate.imgPlate);
		// save the number plate
		cv::imwrite("plate" + std::to_string(carCount) + ".jpg", licPlate.imgPlate);
		std::string filename = "-l eng plate" + std::to_string(carCount) + ".jpg out" + std::to_string(carCount);
		LPCSTR parameters = filename.c_str();
		//execute the tesseract application to detect characters - it should be in windows system path
		ShellExecute(NULL, "open","tesseract" , parameters, NULL, SW_SHOWDEFAULT);

		std::thread plate_read_thread(readPlates,carCount);
		plate_read_thread.detach();

		//increment car count
		carCount++;

		drawRedRectangleAroundPlate(imgOriginalScene, licPlate);                // draw red rectangle around plate

		//std::cout << std::endl << "license plate read from image = " << licPlate.strChars << std::endl;     // write license plate text to std out
		//std::cout << std::endl << "-----------------------------------------" << std::endl;

		writeLicensePlateCharsOnImage(imgOriginalScene, licPlate);              // write license plate text on the image

		cv::imshow("imgOriginalScene", imgOriginalScene);                       // re-show scene image

		cv::imwrite("licence_plates/plate_detect"+std::to_string((car_count))+".png", imgOriginalScene);                  // write image out to file
	}

	//cv::waitKey(0);                 // hold windows open until user presses a key

	return(0);
}

void readPlates(int plateno) {
	// read the file written by tesseract

	while (true) {
		std::string line;
		std::string filename = "out" + std::to_string(plateno) + ".txt";
		//std::cout << "reading " << filename << std::endl;
		std::ifstream myfile(filename);
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				std::cout << line << '\n';
			}
			myfile.close();
			std::remove(filename.c_str());
			break;
		}

		else std::cout << "plate" + std::to_string(plateno)<<" cannot read" <<std::endl;
		Sleep(2000);
	}
}