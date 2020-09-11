#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;

Mat findRandomPatch(Mat inputTexture, int limit, int blockSize, int overlap);

Mat findHorizontalPatch(Mat inputTexture, Mat outputTexture, int limit, int rowIndex, int blockSize, int overlap, double tolerance);

Mat findVerticalPatch(Mat inputTexture, Mat outputTexture, int limit, int colIndex, int blockSize, int overlap, double tolerance);

Mat findHorizontalVerticalPatch(Mat inputTexture, Mat outputTexture, int limit, int rowIndex,
	int colIndex, int blockSize, int overlap, double tolerance);