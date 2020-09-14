#include "generalHelpers.h"

double l2Loss(Mat mat1, Mat mat2) {
	Mat matSub, matSquared;
	double loss;
	Scalar lossChannels;
	int nElements;

	nElements = mat1.total() * mat1.channels();
	absdiff(mat1, mat2, matSub);
	matSub.convertTo(matSub, CV_64FC3);
	matSquared = matSub.mul(matSub);

	lossChannels = sum(matSquared);
	loss = (1.0 / nElements) * (lossChannels.val[0] + lossChannels.val[1] + lossChannels.val[2]);
	return loss;
}


int getMinIndex(vector <double> lossVector) {
	return min_element(lossVector.begin(), lossVector.end()) - lossVector.begin();;
}


Mat sumChannels(Mat matSquared) {
	Mat bgr[3];
	Mat errorSurface;

	split(matSquared, bgr);
	errorSurface = 1/3.0 * (bgr[0] + bgr[1] + bgr[2]);
	return errorSurface;
}