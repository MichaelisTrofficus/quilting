#include "generalHelpers.h"

double l2Loss(Mat mat1, Mat mat2) {
	Mat matSub;
	Mat matSquared;
	double loss;
	Scalar lossChannels;
	int nElements;

	nElements = mat1.size[0] * mat1.size[1] * 3;
	absdiff(mat1, mat2, matSub);

	matSub.convertTo(matSub, CV_64FC3);
	matSquared = matSub.mul(matSub);

	lossChannels = sum(matSquared);
	loss = (1.0 / nElements) * (lossChannels[0] + lossChannels[1] + lossChannels[2]);
	return loss;
}


int getMinIndex(vector <double> lossVector) {
	return min_element(lossVector.begin(), lossVector.end()) - lossVector.begin();;
}


Mat sumChannels(Mat matSquared) {
	Mat bgr[3];
	Mat errorSurface;

	split(matSquared, bgr);
	errorSurface = bgr[0] + bgr[1] + bgr[2];
	return errorSurface;
}