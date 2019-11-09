/*
 * color.cpp
 *
 *  Created on: Oct 22, 2019
 *      Author: szymon
 */

/*
 * thresholding.cpp
 *
 *  Created on: Sep 12, 2019
 *      Author: szymon
 */

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

const cv::String videoCapture = "Video capture"; // original video
const cv::String videoCapture2HSV = "HSV"; // original2hsv
const cv::String videoThresholding = "Thresholding"; //thresholding
const cv::String videoMorphology = "Morphology"; // morphology

const int maxValue = 255;
const int maxValueHue = 360 / 2;
int lowHue = 17, lowSaturation = 143, lowValue = 145;
int highHue = 27, highSaturation = 188, highValue = 214;

int typeValue = 1;
int const typeValueMax = 5;

int shapesValue = 2;
int const shapesValueMax = 2;

int kernelValue = 10;
int const kernelValueMax = 21;

cv::Mat frame, frameHSV, frameThreshold, frameMorphology;

void onLowHue(int, void*) {
	lowHue = std::min(highHue - 1, lowHue);
	cv::setTrackbarPos("Low Hue", videoThresholding, lowHue);
}

void onLowSaturation(int, void*) {
	lowSaturation = std::min(highSaturation - 1, lowSaturation);
	cv::setTrackbarPos("Low Saturation", videoThresholding, lowSaturation);
}

void onLowValue(int, void*) {
	lowValue = std::min(highValue - 1, lowValue);
	cv::setTrackbarPos("Low Value", videoThresholding, lowValue);
}

void onHighHue(int, void*) {
	highHue = std::max(lowHue + 1, highHue);
	cv::setTrackbarPos("High Hue", videoThresholding, highHue);
}

void onHighSaturation(int, void*) {
	highSaturation = std::max(lowSaturation + 1, highSaturation);
	cv::setTrackbarPos("High Saturation", videoThresholding, highSaturation);
}

void onHighValue(int, void*) {
	highValue = std::max(lowValue + 1, highValue);
	cv::setTrackbarPos("High Value", videoThresholding, highValue);
}

void morphology(int, void*) {
	cv::Mat structuredElement = cv::getStructuringElement(shapesValue,
			cv::Size(kernelValue + 1, kernelValue + 1));
	cv::morphologyEx(frameThreshold, frameMorphology, typeValue,
			structuredElement);
	cv::imshow(videoMorphology, frameMorphology);
}

int main(int argc, char *argv[]) {
	cv::VideoCapture cap;
	int waitTime;
	if (argc > 1) {
		cap.open(argv[1]);
		waitTime = 40;
	} else {
		cap.open(2);
		waitTime = 10;
	}

	cv::namedWindow(videoCapture);
//	cv::namedWindow(videoCapture2HSV);
	cv::namedWindow(videoMorphology);
	cv::namedWindow(videoThresholding);

	cv::createTrackbar("Low Hue", videoThresholding, &lowHue, maxValueHue,
			onLowHue);
	cv::createTrackbar("Low Saturation", videoThresholding, &lowSaturation,
			maxValue, onLowSaturation);
	cv::createTrackbar("Low Value", videoThresholding, &lowValue, maxValue,
			onLowValue);
	cv::createTrackbar("High Hue", videoThresholding, &highHue, maxValueHue,
			onHighHue);
	cv::createTrackbar("High Saturation", videoThresholding, &highSaturation,
			maxValue, onHighSaturation);
	cv::createTrackbar("High Value", videoThresholding, &highValue, maxValue,
			onHighValue);

	cv::createTrackbar(
			"MorphTypes\n 0: MORPH_OPEN\n 1: MORPH_CLOSE\n 2: MORPH_GRADIENT\n 3: MORPH_TOPHAT\n 4: MORPH_BLACKHAT\n 5: MORPH_HITMISS",
			videoMorphology, &typeValue, typeValueMax, morphology);
	cv::createTrackbar(
			"MorphShapes\n 0: MORPH_RECT\n 1: MORPH_CROSS\n 2: MORPH_ELLIPSE",
			videoMorphology, &shapesValue, shapesValueMax, morphology);
	cv::createTrackbar("Kernel size\n n+1", videoMorphology, &kernelValue,
			kernelValueMax, morphology);

	int iteration = 0;
	while (1) {
		++iteration;
		cap >> frame;
		if (frame.empty()) {
			break;
		}
		cv::cvtColor(frame, frameHSV, cv::COLOR_BGR2HSV);
		cv::imshow(videoCapture, frame);
//		cv::imshow(videoCapture2HSV, frameHSV);
		cv::inRange(frameHSV, cv::Scalar(lowHue, lowSaturation, lowValue),
				cv::Scalar(highHue, highSaturation, highValue), frameThreshold);
		cv::imshow(videoThresholding, frameThreshold);
		morphology(0, 0);
		if (iteration % 10 == 0) {
			cv::Moments moments;
			moments = cv::moments(frameMorphology);
			std::cout << "x = " << moments.m10 / (moments.m00 + 1e-5) << " y = "
					<< moments.m01 / (moments.m00 + 1e-5) << " mass = " << moments.m00 << std::endl;
		}
		cv::imshow(videoMorphology, frameMorphology);
		char key = (char) cv::waitKey(waitTime);
		if (key == 'q' || key == 27) // 27 == ESC
				{
			break;
		}
	}

	return 0;
}
