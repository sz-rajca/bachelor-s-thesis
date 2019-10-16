#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>

cv::Point2f coordinatesOfMarker(
		const std::vector<std::vector<cv::Point2f>> &corners) {
	cv::Point2f coordinates(
			(corners[0][0].x + corners[0][1].x + corners[0][2].x
					+ corners[0][3].x) / 4,
			(corners[0][0].y + corners[0][1].y + corners[0][2].y
					+ corners[0][3].y) / 4);
	return coordinates;
}

int main(int argc, char *argv[]) {
	cv::VideoCapture inputVideo;
	int waitTime;

	std::cout << argv[1] << std::endl;
	if (argc > 1) {
		inputVideo.open(argv[1]);
		waitTime = 40;
	} else {
		inputVideo.open(0);
		waitTime = 10;
	}

	cv::Ptr<cv::aruco::DetectorParameters> detectorParams =
			cv::aruco::DetectorParameters::create();
	cv::Ptr<cv::aruco::Dictionary> dictionary =
			cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);

	cv::Mat camMatrix, distCoeffs;
	int totalIterations = 0;
	int width = inputVideo.get(cv::CAP_PROP_FRAME_WIDTH);

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
			if (totalIterations % 30 == 0) {
				cv::Point2f coordinates = coordinatesOfMarker(corners);
				std::cout << "x = " << coordinates.x << "y = " << coordinates.y
						<< std::endl;
				if (coordinates.x < width * 0.3) {
					std::cout << "left" << std::endl;
				}
				if (coordinates.x > width * 0.7) {
					std::cout << "right" << std::endl;
				}
			}
		}

		cv::imshow("out", imageCopy);
		char key = (char) cv::waitKey(waitTime);
		if (key == 27)
			break;
	}

	return 0;
}
