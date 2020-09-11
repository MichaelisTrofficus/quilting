#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;

double l2Loss(Mat mat1, Mat mat2);

int getMinIndex(vector <double> lossVector);

Mat sumChannels(Mat matSquared);