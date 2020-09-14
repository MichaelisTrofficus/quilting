#include "patchHelpers.h"
#include "generalHelpers.h"
#include <ctime>


vector <pair<int, int>> backtracking(int minIndex, Mat indexMatrix) {
	vector <pair<int, int>> minPath;
	int nRows = indexMatrix.size[0];
	int nCols = indexMatrix.size[1];
	int currentRow = nRows - 1;

	while (currentRow >= 0) {
		minPath.push_back(make_pair(minIndex, currentRow));
		minIndex += indexMatrix.at<schar>(Point(minIndex, currentRow));
		currentRow -= 1;
	}
	
	return minPath;
}


vector <pair <int, int>> findMinCut(Mat inputOverlap, Mat outputOverlap , int blockSize, int overlap) {
	Mat matSub, matSquared, errorSurface;
	vector <pair <int, int>> minPath;
	vector <double> parents;
	vector <double> lastRow;
	Mat cumulativeMatrix;  
	Mat indexMatrix; 
	int x, y;
	int minIndex;
	double minValue;
	double cumulativeValue;

	absdiff(outputOverlap, inputOverlap, matSub);
	matSub.convertTo(matSub, CV_64FC3);
	matSquared = matSub.mul(matSub);

	errorSurface = sumChannels(matSquared);
	cumulativeMatrix = Mat::zeros(Size(errorSurface.size[1], errorSurface.size[0]), CV_64F);
	indexMatrix = Mat::zeros(Size(errorSurface.size[1], errorSurface.size[0]), CV_8S);

	for (int i = 0; i < errorSurface.size[0]; i++) {
		for (int j = 0; j < errorSurface.size[1]; j++) {
			if (i == 0) {
				// First row.
				cumulativeMatrix.at<double>(Point(j, i)) = errorSurface.at<double>(Point(j, i));
				indexMatrix.at<schar>(Point(j, i)) = 0;
			}
			else {
				if (j == 0) {
					parents = { DBL_MAX, cumulativeMatrix.at<double>(Point(j, i - 1)), cumulativeMatrix.at<double>(Point(j + 1, i - 1)) };
				}
				else if (j == (errorSurface.size[1] - 1)) {
					parents = { cumulativeMatrix.at<double>(Point(j - 1 , i - 1)), cumulativeMatrix.at<double>(Point(j, i - 1)), DBL_MAX };
				}
				else {
					parents = { cumulativeMatrix.at<double>(Point(j - 1 , i - 1)), cumulativeMatrix.at<double>(Point(j, i - 1)),
					cumulativeMatrix.at<double>(Point(j + 1, i - 1)) };
				}

				minIndex = getMinIndex(parents);
				minValue = parents[minIndex];

				cumulativeValue = errorSurface.at<double>(Point(j, i)) + minValue;
				cumulativeMatrix.at<double>(Point(j, i)) = cumulativeValue;

				if (i == (errorSurface.size[0] - 1)) {
					lastRow.push_back(cumulativeValue);
				}

				if (minIndex == 0) {
					indexMatrix.at<schar>(Point(j, i)) = -1;
				}
				else if (minIndex == 1) {
					indexMatrix.at<schar>(Point(j, i)) = 0;
				}
				else {
					indexMatrix.at<schar>(Point(j, i)) = 1;
				}
			}
		}
	}

	// We check the index of the min value of the last row
	minIndex = getMinIndex(lastRow);
	// Now we backtrack
	minPath = backtracking(minIndex, indexMatrix);

	return minPath;
}


Mat findRandomPatch(Mat inputTexture, int limit, int blockSize, int overlap) {
	vector <pair <int, int>> availableVertices;
	Mat randomBlock;
	int randomIndex, x, y;

	for (int i = 0; i < limit; i++) {
		for (int j = 0; j < limit; j++) {
			availableVertices.push_back(make_pair(i, j));
		}
	}
	// We randomly select one of the patches
	srand(time(0));
	randomIndex = rand() % availableVertices.size();
	x = availableVertices[randomIndex].first;
	y = availableVertices[randomIndex].second;

	// We select region of interest (ROI), that is the selected block
	randomBlock = inputTexture(Rect(x, y, blockSize, blockSize)).clone();
	return randomBlock;
}


Mat findHorizontalPatch(Mat inputTexture, Mat outputTexture, int limit, int rowIndex, int blockSize, int overlap, double tolerance) {
	vector <pair <int, int>> indexVector;
	vector <pair <int, int>> blocksVector;
	vector <pair <int, int>> minPath;
	vector <double> lossVector;
	Mat inputOverlapRoi;
	Mat inputOverlapRoiT;
	Mat block;
	Mat mask;
	Mat outputOverlapRoi = outputTexture(Rect(0, rowIndex, blockSize, overlap)).clone();
	Mat outputOverlapRoiT;
	double loss;

	for (int i = 0; i < limit; i++) {
		for (int j = 0; j < limit; j++) {
			Mat inputOverlapRoi = inputTexture(Rect(i, j, blockSize, overlap));
			loss = l2Loss(inputOverlapRoi, outputOverlapRoi);
			indexVector.push_back(make_pair(i, j));
			lossVector.push_back(loss);
		}
	}

	int minIndex = getMinIndex(lossVector);
	double minValue = lossVector[minIndex];

	for (int i = 0; i < lossVector.size(); i++) {
		if (lossVector[i] <= minValue * (1.0 + tolerance)) {
			blocksVector.push_back(indexVector[i]);
		}
	}

	srand(time(0));
	int randomIndex = rand() % blocksVector.size();
	int x = blocksVector[randomIndex].first;
	int y = blocksVector[randomIndex].second;

	Mat patchBlock = inputTexture(Rect(x, y, blockSize, blockSize)).clone();

	inputOverlapRoi = inputTexture(Rect(x, y, blockSize, overlap)).clone();
	transpose(inputOverlapRoi, inputOverlapRoiT);
	transpose(outputOverlapRoi, outputOverlapRoiT);
	mask = Mat::zeros(Size(inputOverlapRoi.size[1], inputOverlapRoi.size[0]), CV_8U);

	// Now that we have the optimuum block, we need to find the minCut
	minPath = findMinCut(inputOverlapRoiT, outputOverlapRoiT, blockSize, overlap);

	for (int i = 0; i < minPath.size(); i++) {
		x = minPath[i].first;
		y = minPath[i].second;
		for (int j = 0; j < x; j++) {
			mask.at<uchar>(Point(i, j)) = 1;
		}
	}

	// Ahora aqui copiamos output en input pero con la mascara, de modo que solo se pegue lo que esta a 1
	outputOverlapRoi.copyTo(inputOverlapRoi, mask);
	inputOverlapRoi.copyTo(patchBlock(Rect(0, 0, blockSize, overlap)));

	return patchBlock;
}


Mat findVerticalPatch(Mat inputTexture, Mat outputTexture, int limit, int colIndex, int blockSize, int overlap, double tolerance) {
	vector <pair <int, int>> indexVector;
	vector <pair <int, int>> blocksVector;
	vector <pair <int, int>> minPath;
	vector <double> lossVector;
	Mat inputOverlapRoi;
	Mat block, patchBlock, refBlock, resBlock;
	Mat mask;
	double loss, minValue;
	int x, y, minIndex, randomIndex;


	Mat outputOverlapRoi = outputTexture(Rect(colIndex, 0, overlap, blockSize)).clone();
	refBlock = outputTexture(Rect(colIndex, 0, blockSize, blockSize)).clone();

	for (int i = 0; i < limit; i++) {
		for (int j = 0; j < limit; j++) {
			inputOverlapRoi = inputTexture(Rect(i, j, overlap, blockSize));
			loss = l2Loss(inputOverlapRoi, outputOverlapRoi);
			indexVector.push_back(make_pair(i, j));
			lossVector.push_back(loss);
		}
	}

	minIndex = getMinIndex(lossVector);
	minValue = lossVector[minIndex];

	for (int i = 0; i < lossVector.size(); i++) {
		if (lossVector[i] <= minValue * (1.0 + tolerance)) {
			blocksVector.push_back(indexVector[i]);
		}
	}

	randomIndex = rand() % blocksVector.size();

	x = blocksVector[randomIndex].first;
	y = blocksVector[randomIndex].second;

	patchBlock = inputTexture(Rect(x, y, blockSize, blockSize)).clone();

	resBlock = Mat::zeros(Size(patchBlock.size[1], patchBlock.size[0]), CV_8UC3);
	mask = Mat::zeros(Size(patchBlock.size[1], patchBlock.size[0]), CV_8U);
	inputOverlapRoi = inputTexture(Rect(x, y, overlap, blockSize)).clone();

	// Now that we have the optimuum block, we need to find the minCut
	minPath = findMinCut(inputOverlapRoi, outputOverlapRoi, blockSize, overlap);

	for (int i = 0; i < minPath.size(); i++) {
		x = minPath[i].first;
		y = minPath[i].second;
		for (int j = 0; j < x; j++) {
			mask.at<uchar>(Point(j , i)) = 1;
		}
	}

	patchBlock.copyTo(resBlock);
	refBlock.copyTo(resBlock, mask);
	return resBlock;
}


Mat findHorizontalVerticalPatch(Mat inputTexture, Mat outputTexture, int limit, int rowIndex,
	int colIndex, int blockSize, int overlap, double tolerance) {
	vector <pair <int, int>> indexVector;
	vector <pair <int, int>> blocksVector;
	vector <double> lossVector;
	vector <pair <int, int>> minPathHorizontal;
	vector <pair <int, int>> minPathVertical;
	Mat block, patchBlock, resBlock;
	Mat inputOverlapRoiHorizontal;
	Mat inputOverlapRoiVertical;
	Mat maskHorizontal;
	Mat maskVertical;
	Mat mask, mask1, mask2;
	Mat outputOverlapRoiHorizontal = outputTexture(Rect(colIndex, rowIndex, blockSize, overlap)).clone();
	Mat outputOverlapRoiVertical = outputTexture(Rect(colIndex, rowIndex, overlap, blockSize)).clone();
	Mat inputOverlapRoiHorizontalT, inputOverlapRoiVerticalT, outputOverlapRoiHorizontalT, outputOverlapRoiVerticalT;
	Mat upDownOverlap;
	double l2Sum;
	double loss;

	for (int i = 0; i < limit; i++) {
		for (int j = 0; j < limit; j++) {
			l2Sum = 0.0;
			inputOverlapRoiHorizontal = inputTexture(Rect(i, j, blockSize, overlap)).clone();
			loss = l2Loss(inputOverlapRoiHorizontal, outputOverlapRoiHorizontal);
			l2Sum += loss;
			inputOverlapRoiVertical = inputTexture(Rect(i, j, overlap, blockSize)).clone();
			loss = l2Loss(inputOverlapRoiVertical, outputOverlapRoiVertical);
			l2Sum += loss;
			indexVector.push_back(make_pair(i, j));
			lossVector.push_back(l2Sum);
		}
	}

	int minIndex = getMinIndex(lossVector);
	double minValue = lossVector[minIndex];

	for (int i = 0; i < lossVector.size(); i++) {
		if (lossVector[i] <= (minValue * (1 + tolerance))) {
			blocksVector.push_back(indexVector[i]);
		}
	}

	srand(time(0));
	int randomIndex = rand() % blocksVector.size();
	int x = blocksVector[randomIndex].first;
	int y = blocksVector[randomIndex].second;

	patchBlock = inputTexture(Rect(x, y, blockSize, blockSize)).clone();
	resBlock = Mat::zeros(Size(patchBlock.size[1], patchBlock.size[0]), CV_8UC3);
	mask = Mat::zeros(Size(patchBlock.size[1], patchBlock.size[0]), CV_8U);

	upDownOverlap = Mat::zeros(Size(patchBlock.size[1], patchBlock.size[0]), CV_8UC3);
	outputOverlapRoiHorizontal.copyTo(upDownOverlap(Rect(0, 0, blockSize, overlap)));
	outputOverlapRoiVertical.copyTo(upDownOverlap(Rect(0, 0, overlap, blockSize)));

	inputOverlapRoiHorizontal = inputTexture(Rect(x, y, blockSize, overlap)).clone();
	inputOverlapRoiVertical = inputTexture(Rect(x, y, overlap, blockSize)).clone();
	transpose(inputOverlapRoiHorizontal, inputOverlapRoiHorizontalT);
	transpose(outputOverlapRoiHorizontal, outputOverlapRoiHorizontalT);

	mask1 = Mat::zeros(Size(patchBlock.size[1], patchBlock.size[0]), CV_8U);
	mask2 = Mat::zeros(Size(patchBlock.size[1], patchBlock.size[0]), CV_8U);
	//maskHorizontal = Mat::zeros(Size(inputOverlapRoiHorizontal.size[1], inputOverlapRoiHorizontal.size[0]), CV_8U);
	//maskVertical = Mat::zeros(Size(inputOverlapRoiVertical.size[1], inputOverlapRoiVertical.size[0]), CV_8U);

	// Horizontal Overlapping
	minPathHorizontal = findMinCut(inputOverlapRoiHorizontalT, outputOverlapRoiHorizontalT, blockSize, overlap);

	for (int i = 0; i < minPathHorizontal.size(); i++) {
		x = minPathHorizontal[i].first;
		y = minPathHorizontal[i].second;
		for (int j = 0; j < x; j++) {
			mask1.at<uchar>(Point(i, j)) = 1;
		}
	}

	//maskHorizontal.copyTo(mask1(Rect(0, 0, blockSize, overlap)));

	// Vertical Overlapping
	minPathVertical = findMinCut(inputOverlapRoiVertical, outputOverlapRoiVertical, blockSize, overlap);

	for (int i = 0; i < minPathVertical.size(); i++) {
		x = minPathVertical[i].first;
		y = minPathVertical[i].second;
		for (int j = 0; j < x; j++) {
			mask2.at<uchar>(Point(j, i)) = 1;
		}
	}

	//maskVertical.copyTo(mask2(Rect(0, 0, overlap, blockSize)));
	bitwise_or(mask1, mask2, mask);

	// Ahora aqui copiamos output en input pero con la mascara, de modo que solo se pegue lo que esta a 1
	patchBlock.copyTo(resBlock);
	upDownOverlap.copyTo(resBlock, mask);
	return resBlock;

}
