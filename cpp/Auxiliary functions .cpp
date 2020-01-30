#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <math.h>

using namespace cv;
using namespace std;

void plot(string windowName, Mat image) {
	namedWindow(windowName, WINDOW_NORMAL);
	imshow(windowName, image);
	waitKey(0);
	destroyWindow(windowName);
	return;
}

void plot_circles(Mat& image, vector<Vec3f> circles) {
	//plot all circles
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3i c = circles[i];
		Point center = Point(c[0], c[1]);
		// circle outline
		int radius = c[2];
		circle(image, center, radius, Scalar(0, 0, 0), 3, LINE_AA);

	}
}

void find_circle_color(vector<Vec3f> circles, Mat I, vector<Vec3d>& colors)
{
	//	cout << I.at<Vec3d>(245, 1025);
	int i = 0, j, k, counter = 0;
	float xc, yc, R;
	Vec3d temp_colors;
	Vec3b intensity;
	Mat one_color_circle;
	for (size_t k = 0; k < circles.size(); k++)
	{
		xc = circles[k].val[0];
		yc = circles[k].val[1];
		R = circles[k].val[2];
		temp_colors = { 0,0,0 };
		for (i = 0; i < I.rows; i++)//y
		{
			for (j = 0; j < I.cols; j++)//x
			{
				if ((pow(j - xc, 2) + pow(i - yc, 2)) <= pow(R, 2))
				{
					//	cout << i << "," << j << endl;
					//	cout << I.at<Vec3f>(i, j)<<endl;
					intensity = I.at<Vec3b>(i, j);
					//		cout << intensity << endl;
					//	cout <<"Before:  "<<"temp:"<< temp_colors <<"intensity: "<< intensity << endl;
					temp_colors.val[0] = temp_colors.val[0] + double(intensity.val[0]);
					temp_colors.val[1] = temp_colors.val[1] + double(intensity.val[1]);
					temp_colors.val[2] = temp_colors.val[2] + double(intensity.val[2]);
					//	cout << "after" << temp_colors << endl;
					counter++;
				}
			}
		}
		//		cout << "sum: "<< temp_colors <<",  counter: "<<counter<< endl;
		//		cout << "sum: [" << temp_colors.val[0] <<"," << temp_colors.val[1] << "," << temp_colors.val[2] << "," << "],  counter: " << counter << endl;
		colors[k].val[0] = temp_colors.val[0] / counter;
		colors[k].val[1] = temp_colors.val[1] / counter;
		colors[k].val[2] = temp_colors.val[2] / counter;
		//	cout << "after dviding: " << colors[k] << endl;
		counter = 0;
	}
}

void classifey_color(vector<Vec3i> Ref_colors, vector<Vec3d> colors, vector<string> Ref_labels, vector<string>& Labels)
{
	//Returns the matching index in Ref_Colors for each color in colors_list
	const int Num_of_colors = colors.size();
	const int Num_of_ref_colors = Ref_colors.size();
	Mat D(Num_of_ref_colors, Num_of_colors, CV_64F);

	int i, j;
	//plot("D", D);
	double min_dist = 1000000;
	for (i = 0; i < Num_of_colors; i++)//colors
	{
		//cout <<"Avarage color: "<< colors[i]<<endl;

		for (j = 0; j < Num_of_ref_colors; j++)//
		{
			//calculate L2 norm distances between each color to each reference color
			//cout << i << "," << j << endl;
			//cout << "first ref: " << Ref_colors[j].val[0] << "second ref: " << double(Ref_colors[j].val[1]) << "third ref : " << double(Ref_colors[j].val[2]) << endl;
			//	cout << "first: " << pow((colors[i].val[0] - double(Ref_colors[j].val[0])), 2) << "second: " << pow((colors[i].val[0] - double(Ref_colors[j].val[0])), 2) << "third: " << pow((colors[i].val[0] - double(Ref_colors[j].val[0])), 2) << endl;
			D.at<double>(j, i) = sqrt(pow((colors[i].val[0] - double(Ref_colors[j].val[0])), 2)
				+ pow((colors[i].val[1] - double(Ref_colors[j].val[1])), 2)
				+ pow((colors[i].val[2] - double(Ref_colors[j].val[2])), 2));
			//cout << "distance=" << D.at<double>(j, i) << endl;
			if (D.at<double>(j, i) < min_dist)
			{
				min_dist = D.at<double>(j, i);
				//	cout << min_dist<<endl;
				//	cout << Ref_labels[j] << endl;
				//	strcpy(Labels[i], Ref_labels[j]);
				Labels[i] = Ref_labels[j];
				//	cout << "Lable " << i << "=" << Labels[i] << endl;
			}
		}
		//Classify according to the minimum norm distance
		min_dist = 1000000;
		//	cout <<"Lable "<<i<<"="<< Labels[i] << endl;
	}

}

vector<Vec3f> Add_new_circles(vector<Vec3f> circlesA, vector<Vec3f> circlesB, Mat I) {
	// adds non - overlapping circles parameters from list B to list A
	if (circlesA.size() < 1) // initialize if we receive an empty list
	{
		if (circlesB.size() > 1)
		{
			circlesA.resize(1);// %copy first circle parameters to A
			circlesA[0] = circlesB[0];
		}
		else //both lists are empty
		{
			string msg = "empty input arguments'";
			throw(msg);
		}
	}
	
	int max_color_dist = 60;
	int Max_circle_distance = 10;
	int Max_radius_distance = 20;
	int A_length = circlesA.size();
	int B_length = circlesB.size();
	int min_dist_idx;
	double min_dist;
	vector<Vec3f> circlesC= circlesA;// , new_circles(B_length);// (A_length + B_length);
	vector<float> xdist(A_length), ydist(A_length), center_distances(A_length), rad_distances(A_length);
	vector<int> overlap_center_index(A_length,A_length);///TO CHECK!!!!!
	vector<Vec3d> colors(2);
	//Initialize output lists
	int added_circles = 0,move_on, overlapping;
	for (int i = 0; i < B_length; i++)//for each circle in list B
	{
		//cout << "comparision: " << i << endl;
		//cout << "circle from B: " << circlesB[i] << endl;
		//update length of variables according to circlesC length
		xdist.resize(circlesC.size());
		ydist.resize(circlesC.size());
		center_distances.resize(circlesC.size());
		rad_distances.resize(circlesC.size());
		overlap_center_index.resize(circlesC.size());
		//cout << overlap_center_index[0];
		move_on = 0;
		overlapping = 0;
		for (int k = 0; k< overlap_center_index.size(); k++)
			overlap_center_index[k] = circlesC.size();
		//find distance between the center of the candidate circle to all
			//current circle centers on list A
		for (int k = 0; k < circlesC.size(); k++)
		{
			
			xdist[k] = circlesC[k].val[0] - circlesB[i].val[0];
			ydist[k] = circlesC[k].val[1] - circlesB[i].val[1];
			center_distances[k] = sqrt(pow(xdist[k], 2) + pow(ydist[k], 2));
			rad_distances[k]= abs(circlesC[k].val[2] - circlesB[i].val[2]);// same for radius distances
			if (center_distances[k] < max(circlesC[k].val[2], circlesB[i].val[2]))
			{//look for overlapings(we consider overlap when distance between centers<max(R_a, R_b))
				overlap_center_index[overlapping] = k;
			//	cout << "overlaping circle number: " << overlapping << " is: " << circlesC[k] << endl;
				overlapping++;
			}
		}
		
		if (overlapping>0)
		{// we found center overlap
			// check if one of the overlapping circles in the list is very similar to
			//the the new circle - if so, dont add the circle and move to next circle in B
			for (int j = 0; j<overlapping; j++)
			{
				if (center_distances[overlap_center_index[j]] < Max_circle_distance && rad_distances[overlap_center_index[j]] < Max_radius_distance)
					move_on = 1;
			}
			//check total overlap - if one of the circles in overlaping_list is containing the circle from B or
			//containd by him(it happens when d< | R_a - R_b | -leave only the largest
			if (!move_on) 
			{
				for (int j = 0; overlap_center_index[j] != circlesC.size(); j++)
					if (center_distances[overlap_center_index[j]]< rad_distances[overlap_center_index[j]])
					{ 
						move_on = 1;
						if (circlesC[overlap_center_index[j]].val[2] < circlesB[i].val[2]) // if the circle in B is larger
							circlesC[overlap_center_index[j]] = circlesB[i];//Replace circle A with B in the new list							
					}
			}
			//overlapping is not total but large enough it could be an elipce
			if (!move_on)
			{
				vector<int> D(overlapping);
				min_dist = 256;
				min_dist_idx = circlesC.size();
				for (int j = 0; overlap_center_index[j] != circlesC.size(); j++) // for each overlaping circle
				{
					//check average color inside the two circles.			
					find_circle_color({ {circlesC[overlap_center_index[j]]},{ circlesB[i]} }, I, colors);
					//calculate L2 distance between the two average colors					
					D[j]=(sqrt(pow((colors[0].val[0] - colors[1].val[0]), 2)
						+ pow((colors[0].val[1] - colors[1].val[1]), 2)
						+ pow((colors[0].val[2] - colors[1].val[2]), 2)));
					if (D[j] < min_dist)
					{
						min_dist = D[j];
						min_dist_idx = j;
					}						
				}
				if (min_dist < max_color_dist) // the two overlaping circles has similar color - we decide its elipce
					//average the radius and centers
				{					
					circlesC[overlap_center_index[min_dist_idx]].val[0] = (circlesC[overlap_center_index[min_dist_idx]].val[0]+  circlesB[i].val[0])/2;
					circlesC[overlap_center_index[min_dist_idx]].val[1] = (circlesC[overlap_center_index[min_dist_idx]].val[1] + circlesB[i].val[1]) / 2;
					circlesC[overlap_center_index[min_dist_idx]].val[2] = (circlesC[overlap_center_index[min_dist_idx]].val[2] + circlesB[i].val[2]) / 2;
				}
				else {
					//overlaping circles are with different colors - add the new circle to the list
					added_circles = added_circles + 1;
					//cout << circlesC[circlesC.size()-1];
					circlesC.resize(circlesC.size() + 1);
					//cout << circlesC[circlesC.size() - 2];
					circlesC[circlesC.size()-1] = circlesB[i];
				}					
			}
		}
		else {//There is no overlap at all so we can add the new circle to the list
			added_circles = added_circles + 1;
			circlesC.resize(circlesC.size() + 1);
			circlesC[circlesC.size()-1] = circlesB[i];
			//cout << "Updated list:" << endl;
			//for (int k = 0; k < circlesC.size();k++)
			//	cout << k << "." << circlesC[k] << endl;
		}
	}
	/*vector<Vec3f> circlesC(A_length + added_circles);
	for (int j = 0; j < A_length; j++)
		circlesC[j] = circlesA[j];
	for (int j = A_length; j < A_length + added_circles;j++)
		circlesC[j] = new_circles[j- A_length];*/
	return circlesC;
}

