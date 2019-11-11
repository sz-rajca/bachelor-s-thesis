#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <opencv2/imgproc.hpp>

std::vector<float> velocity(cv::Point2f point, float distance, float windowWidth) {
	static float previousErrorAngular;
	static float previousErrorLinear;
	float kp = 0.01;
	float kd = 0.001;
	std::vector<float> result;

	float errorLinear = 100 - distance;
	float derivativeLinear = (errorLinear - previousErrorLinear) / 0.1;
	result.push_back(kp * errorLinear + kd * derivativeLinear);
	previousErrorLinear = errorLinear;

	float errorAngular = windowWidth / 2.0 - point.x; // center of a scene
	float derivativeAngular = (errorAngular - previousErrorAngular) / 0.1;
	result.push_back(kp * errorAngular + kd * derivativeAngular);
	previousErrorAngular = errorAngular;

	return result;
}

cv::Point2f coordinatesOfMarker(
    const std::vector<std::vector<cv::Point2f>> &corners) {
	cv::Point2f coordinates(
	    (corners[0][0].x + corners[0][1].x + corners[0][2].x + corners[0][3].x) / 4,
	    (corners[0][0].y + corners[0][1].y + corners[0][2].y + corners[0][3].y) / 4);
	return coordinates;
}

const cv::String keys =
    "{id | 0 | Camera id }"
    "{v  |   | Input from video file }";

int main(int argc, char *argv[]) {
	cv::CommandLineParser parser(argc, argv, keys);
	parser.about("Object tracking using an ArUco marker");

	int camId = parser.get<int>("id");

	cv::String video;
	if (parser.has("v")) {
		video = parser.get<cv::String>("v");
	}

	if (!parser.check()) {
		parser.printErrors();
		return 0;
	}

	cv::VideoCapture inputVideo;
	int waitTime = 10;

	if (!video.empty()) {
		inputVideo.open(video);
		waitTime = 40;
	} else {
		inputVideo.open(camId);
		waitTime = 40;
	}

	cv::Ptr<cv::aruco::DetectorParameters> detectorParams =
	    cv::aruco::DetectorParameters::create();
	cv::Ptr<cv::aruco::Dictionary> dictionary =
	    cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

	cv::Mat camMatrix, distCoeffs;
	int totalIterations = 0;

	while (inputVideo.grab()) {
		totalIterations++;
		cv::Mat image, imageCopy;
		inputVideo.retrieve(image);
		std::vector<int> ids;
		std::vector<std::vector<cv::Point2f> > corners, rejected;

		cv::aruco::detectMarkers(image, dictionary, corners, ids,
		                         detectorParams, rejected);
		image.copyTo(imageCopy);

		if (ids.size() > 0) {
			cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
			float height = sqrt(pow(abs(corners[0][0].x - corners[0][1].x), 2) +
			                    pow(abs(corners[0][0].y - corners[0][1].y), 2));
			cv::Point2f coordinates = coordinatesOfMarker(corners);
			std::vector<float> vels = velocity(coordinates, height,
			                                   inputVideo.get(cv::CAP_PROP_FRAME_WIDTH));
			std::cout << vels[0] << " " << vels[1] << std::endl;
		}

		cv::imshow("out", imageCopy);
		char key = (char) cv::waitKey(waitTime);
		if (key == 27) {
			break;
		}
	}

	return 0;
}
