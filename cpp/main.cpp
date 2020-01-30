// opencv_first_test.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <math.h>

using namespace cv;
using namespace std;

vector<Vec3f> Add_new_circles(vector<Vec3f> circlesA, vector<Vec3f> circlesB, Mat I);

void plot(string windowName, Mat image);

void plot_circles(Mat& image, vector<Vec3f> circles);

void find_circle_color(vector<Vec3f> circles, Mat I, vector<Vec3d>& colors);

void classifey_color(vector<Vec3i> Ref_colors, vector<Vec3d> colors, vector<string> Ref_labels, vector<string>& Labels);

int main(int argc, char** argv)
{
	// Read the image file
	//Mat image(800, 1200, CV_8UC3);
	Mat image = imread("C:/Users/Aaronov/Documents/Visual Studio 2015/Projects/opencv_first_test/assorted_tomatoes.jpg");

	if (image.empty()) // Check for failure
	{
		cout << "Could not open or find the image" << endl;
		system("pause"); //wait for any key press
		return -1;
	}
	Mat Gray_image;
	Mat Edge_Gray,Edge_B,Edge_G,Edge_R;
	vector<Mat> I_channels;
	split(image,I_channels);
	cvtColor(image, Gray_image, COLOR_BGR2GRAY);
	int cannymin = 20, cannymax = 100;
	Canny(Gray_image, Edge_Gray, cannymin, cannymax);
	Canny(I_channels[0], Edge_B, cannymin, cannymax);
	Canny(I_channels[1], Edge_G, cannymin, cannymax);
	Canny(I_channels[2], Edge_R, cannymin, cannymax);
	//plot("Edge", Edge_image);

	//find circles
	vector<Vec3f> circlesGray,circlesB,circlesG,circlesR;
	HoughCircles(Edge_Gray, circlesGray, HOUGH_GRADIENT,1,10, cannymax, 45/*sensativity*/, 25, 70);
	HoughCircles(Edge_B, circlesB, HOUGH_GRADIENT, 1, 10, cannymax, 45/*sensativity*/, 25, 70);
	HoughCircles(Edge_G, circlesG, HOUGH_GRADIENT, 1,10, cannymax, 45/*sensativity*/, 25, 70);
	HoughCircles(Edge_R, circlesR, HOUGH_GRADIENT, 1, 10, cannymax, 45/*sensativity*/, 25, 70);
	

	cout << "circles Gray= " << circlesGray.size() << endl;
	cout << "circles Blue= " << circlesB.size() << endl;
	cout << "circles Green= " << circlesG.size() << endl;
	cout << "circles Red= " << circlesR.size() << endl;

	vector<Vec3f> empty;
	vector<Vec3f>circles= Add_new_circles(empty, circlesG, image);
	circles = Add_new_circles(circles, circlesR, image);
	circles = Add_new_circles(circles, circlesGray, image);
	circles = Add_new_circles(circles, circlesB, image);


	plot_circles(image, circles);
	cout << "final circles= " << circles.size() << endl;

	int N = circles.size();
	vector<Vec3d> colors(N);
	//get average color for each circle
	find_circle_color(circles, image, colors);
/*	for (int i = 0; i < colors.size(); i++)
		cout << colors[i] << endl;*/
	vector<string> Labels(N);
	vector<string> Ref_labels = { "Red","Yellow","Green","Brown" };
	vector<Vec3i> Ref_colors = { {40,40,200},{20,155,200},{60,160,130},{42,42,140} };
	/*	for (int i = 0; i < Ref_colors.size(); i++)
			cout << Ref_colors[i]<<endl;*/
	classifey_color(Ref_colors,colors, Ref_labels, Labels);
	
	String result_window = "Result"; //Name of the window
	
	for (int i = 0; i < Labels.size(); i++)
		putText(image, Labels[i], Point(circles[i].val[0], circles[i].val[1]), FONT_HERSHEY_COMPLEX_SMALL,1,Scalar(0,0,0),1.8,8,false);
		//cout << Labels[i]<<endl;
	plot(result_window, image);



	//bitwise_not(image, image);//invert image
	//Mat I_blur;
	//GaussianBlur(image, I_blur, Size(5, 5), 0);
	Mat HSV_image;
	cvtColor(image, HSV_image, COLOR_BGR2HSV);

	String windowName = "My HelloWorld Window"; //Name of the window
	string windowNameHSV = "HSV image";

	namedWindow(windowName, WINDOW_NORMAL); // Create a window
	namedWindow(windowNameHSV, WINDOW_NORMAL); // Create a window

	int hight = image.rows;
	int width = image.size().width;

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	//Create trackbars in "Control" window
	createTrackbar("LowH", windowNameHSV, &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", windowNameHSV, &iHighH, 179);

	createTrackbar("LowS", windowNameHSV, &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", windowNameHSV, &iHighS, 255);

	createTrackbar("LowV", windowNameHSV, &iLowV, 255); //Value (0 - 255)
	createTrackbar("HighV", windowNameHSV, &iHighV, 255);
/*	//TRANSFORMATION
	Mat matRotation = getRotationMatrix2D(Point(image.cols/2, image.rows/2),0, 0.5);
	Mat imgRotated;
	warpAffine(image, imgRotated, matRotation, image.size());*/

	while (true) {
		imshow(windowName, image); // Show our image inside the created window.
	//	imshow("Rotated Image", imgRotated); // Show our image inside the created window.
	//	imshow("Blured Image", image);

		//waitKey(0); // Wait for any keystroke in the window
		
		Mat imgThresholded;

		inRange(HSV_image, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		imshow(windowNameHSV, imgThresholded);
		if (waitKey(10) == 27)
		{
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}
	}

	destroyAllWindows(); //destroy the created window

	
	
	//////////////video////////////////////////////
	
	
	VideoCapture cap1("C:/Users/Aaronov/Documents/Visual Studio 2015/Projects/opencv_first_test/Wildlife.wmv");

	// if not success, exit program
	if (cap1.isOpened() == false)
	{
		cout << "Cannot open the video file" << endl;
		cin.get(); //wait for any key press
		return -1;
	}
	
	double fps = cap1.get(CAP_PROP_FPS);
	cout << "Frames per seconds : " << fps << endl;

	String window_name2 = "My First Video";

	namedWindow(window_name2, WINDOW_NORMAL); //create a window
	int angle = 0;
	createTrackbar("Angle", window_name2, &angle, 360);
	int scale = 50;
	createTrackbar("Scale factor", window_name2, &scale, 100);
	while (true)
	{
		Mat frame;
		bool bSuccess = cap1.read(frame); // read a new frame from video 

										 //Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			cout << "Found the end of the video" << endl;
			break;
		}


		//Rotate video
		
		Mat matRotation = getRotationMatrix2D(Point(frame.cols / 2, frame.rows / 2), angle, scale/50.0);
		Mat vidRotated;
		warpAffine(frame, vidRotated, matRotation, frame.size());


		//show the frame in the created window
		//imshow(window_name2, frame);
		imshow(window_name2, vidRotated);


		//wait for for 10 ms until any key is pressed.  
		//If the 'Esc' key is pressed, break the while loop.
		//If the any other key is pressed, continue the loop 
		//If any key is not pressed withing 10 ms, continue the loop
		if (waitKey(10) == 27)
		{
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}
	}

	return 0;
}

