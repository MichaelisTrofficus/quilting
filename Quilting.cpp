#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include "generalHelpers.h"
#include "patchHelpers.h"
using namespace std;
using namespace cv;



int main(int argc, char *argv[])
{
	string pathTextures = argv[1];
	string pathOutput = argv[2];
	int blockSize = atoi(argv[3]);
	int generationScale = atoi(argv[4]);
	
	double tolerance = 0.1;
	int overlap = (int)(blockSize / 6.);

	Mat inputTexture, outputTexture;
	inputTexture = imread(pathTextures);

	if (!inputTexture.data)
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	int inputRows = inputTexture.rows;
	int inputCols = inputTexture.cols;
	int inputChannels = inputTexture.channels();

	cout << "Input texture shape: " << inputTexture.size << endl;

	int outputRows = (int)(inputRows * generationScale);
	int outputCols = (int)(inputCols * generationScale);
	int outputChannels = inputChannels;

	int nH = (int) ceil((outputRows - blockSize)*1.0 / (blockSize - overlap));
	int nW = (int) ceil((outputCols - blockSize)*1.0 / (blockSize - overlap));

	outputTexture = Mat::zeros(Size(blockSize + nH*(blockSize - overlap), blockSize + nW*(blockSize - overlap)), CV_8UC3);
	outputRows = outputTexture.size[0];
	outputCols = outputTexture.size[1];

	cout << "Output texture shape: " << outputTexture.size << endl;

	int limit = inputCols - blockSize;

	for (int i = 0; i < outputRows - overlap; i++) {
		cout << "Row " << i << " out of " << outputRows << endl;
		for (int j = 0; j < outputCols - overlap; j++) {
			if (i % (blockSize - overlap) == 0 && j % (blockSize - overlap) == 0) {
				if (i == 0 && j == 0) {
					// Trivial case: We select a random patch
					Mat block = findRandomPatch(inputTexture, limit, blockSize, overlap);
					block.copyTo(outputTexture(Rect(j, i, blockSize, blockSize)));
				}
				else if (i == 0) {
					// Case when i==0. That is, we are in first row.
					Mat block = findVerticalPatch(inputTexture, outputTexture, limit, j, blockSize, overlap, tolerance);
					int sizeX = blockSize;

					if (j + blockSize >= outputCols) {
						sizeX = outputCols - j;
						block = block(Rect(0, 0, sizeX, blockSize));
					} 
					block.copyTo(outputTexture(Rect(j, i, sizeX, blockSize)));
				}
				else if (j == 0) {
					// Case when j==0. That is, we are in first col.
					
					Mat block = findHorizontalPatch(inputTexture, outputTexture, limit, i, blockSize, overlap, tolerance);
					int sizeY = blockSize;

					if (i + blockSize >= outputRows) {
						sizeY = outputRows - i;
						block = block(Rect(0, 0, blockSize, sizeY));
					}

					block.copyTo(outputTexture(Rect(j, i, blockSize, sizeY)));
				}
				else {

						Mat block = findHorizontalVerticalPatch(inputTexture, outputTexture, limit, i, j, blockSize, overlap, tolerance);

						int sizeX = blockSize;
						int sizeY = blockSize;

						if ((i + blockSize) >= outputRows) {
							sizeY = outputRows - i;
							block = block(Rect(0, 0, sizeX, sizeY));
						}

						if ((j + blockSize) >= outputCols) {
							sizeX = outputCols - j;
							block = block(Rect(0, 0, sizeX, sizeY));
						}

						block.copyTo(outputTexture(Rect(j, i, sizeX, sizeY)));
				}
			}
			else {
				continue;
			}
		}
	}

	namedWindow("Input", WINDOW_AUTOSIZE);
	imshow("Input", inputTexture);
	namedWindow("Output", WINDOW_AUTOSIZE);
	imshow("Output", outputTexture);

	imwrite(pathOutput, outputTexture);

	waitKey(0);
	return 0;
}
