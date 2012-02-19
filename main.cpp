#include <iostream>
#include <opencv2\opencv.hpp>

using namespace cv;
using std::cout; using std::endl;

int main()
{
	VideoCapture capture(0);
	VideoCapture captureFromAvi("njemalipy.avi");
	Mat frame(Size(640,480),CV_8UC3); //frame captured from camera
	Mat videoToShow(Size(320,240),CV_8UC3); //avi capture frame
	captureFromAvi>>videoToShow;

	Mat negImage = Mat(frame.size(),8,3);
	Mat cpyImage = Mat(frame.size(),8,3);
	Mat blankImage = Mat::zeros(videoToShow.size(),CV_8UC3);
	bitwise_not(blankImage,blankImage);
	int boardWidth = 5;
	int boardHeight = 4;
	int boardSquares = 20;
	Size boardSize = Size(boardWidth,boardHeight);
	vector<Point2f> corners;
	Point2f q[4],p[4]; // points to calculate perspective q - imageToShow
					   // p - chessboard
	q[0].x = 0;                q[0].y = 0;
	q[1].x = videoToShow.cols; q[1].y = 0;
	q[2].x = videoToShow.cols; q[2].y = videoToShow.rows;
	q[3].x = 0;                q[3].y = videoToShow.rows;

	double computationTime = 0;
	
	while(captureFromAvi.grab())
	{
		capture>>frame;	
		findChessboardCorners(frame,boardSize,corners,
		CALIB_CB_FAST_CHECK + CV_CALIB_CB_NORMALIZE_IMAGE+ CV_CALIB_CB_ADAPTIVE_THRESH);
		for(vector<Point2f>::iterator it = corners.begin(); it != corners.end(); it++)
		{
			circle(frame,*it,3,Scalar(255,0,0),2);
		}
		if(corners.size() == boardSquares) //valid detected corners number
		{
			captureFromAvi>>videoToShow;
			p[0].x = corners[ 0].x ; p[0].y = corners[ 0].y;
			p[1].x = corners[ 4].x ; p[1].y = corners[ 4].y;
			p[2].x = corners[19].x ; p[2].y = corners[19].y;
			p[3].x = corners[15].x ; p[3].y = corners[15].y;
			Mat warpMatrix = getPerspectiveTransform(q,p);
			//operations to obtain 2D-Augmentation
			negImage = Mat::zeros(negImage.size(),CV_8UC3);
			cpyImage = Mat::zeros(cpyImage.size(),CV_8UC3);
			warpPerspective(videoToShow,negImage,warpMatrix,negImage.size());
			warpPerspective(blankImage,cpyImage,warpMatrix,cpyImage.size());
			bitwise_not(cpyImage,cpyImage);
			bitwise_and(cpyImage,frame,cpyImage);
			bitwise_or(cpyImage,negImage,frame);
		}
		imshow("acq",frame);
		waitKey(1);
	}

	waitKey();
	return 0;
}