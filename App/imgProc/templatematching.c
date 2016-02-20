/*
 * templatematching.c
 *
 *  Created on: 29-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "templatematching.h"

uint8 imageData[TEMPLATEMATCHING_IMAGE_LENGTH];

int main(int argc, char* argv[])
{
	char inputFileName[100];
	FILE* inputFile = NULL;
	uint32 fileLength = 0;
	uint32 numBytesRead = 0;

	uint32 imageWidth = 0, imageHeight = 0;

	if(argc >= 1) {
		strcpy(inputFileName, argv[1]);
		imageWidth = atoi(argv[2]);
		imageHeight = atoi(argv[3]);
	}

	inputFile = fopen(inputFileName, "rb");

	if (inputFile != NULL) {
		fseek(inputFile, 0, SEEK_END);
		fileLength = ftell(inputFile);
		fseek(inputFile, 0, SEEK_SET);

		numBytesRead = fread(imageData, 1, fileLength, inputFile);

		fclose(inputFile);
	}

	struct matrix* gray = convertRGBToGrayScale(imageData,numBytesRead,imageWidth,imageHeight);

	fileLength = convertMatrixToBuffer(gray,imageData);

	FILE* outputFile = fopen("output.bmp", "wb");

		if (outputFile != NULL) {
			fwrite(imageData,fileLength,1,outputFile);
			fclose(outputFile);
		}


	//printMatrix(gray);

	// Declare matrices for image,filter and the output
	struct matrix* image;
	struct matrix* filter;
	struct matrix* filteredImage;

	//Intialise matrices
	image = createMatrix(7,7);
	filter = createMatrix(5,5);

	float value = 1.0;

	// For testing purpose - Initialised a 7x7 matrix as image
	for (uint8 i = 0; i < image->numberOfRows; ++i) {
		for (uint8 j = 0; j < image->numberOfColumns; ++j) {
			MAT(image, i, j) = value;
			value++;
		}
	}

#ifdef DEBUG
	printMatrix(image,"Loaded image");
#endif

	// For testing purpose - Initialised a 5x5 matrix as filter mask
	float filterValues[5][5] = {{1.0, 1.0, 1.0,  1.0, 1.0},
								{1.0, 1.0, 1.0,  1.0, 1.0},
								{1.0, 1.0,-24.0, 1.0, 1.0 },
								{1.0, 1.0, 1.0,  1.0, 1.0},
								{1.0, 1.0, 1.0,  1.0, 1.0}};

	initMatrix(filter, (void*)filterValues);

	// Apply filter
	// operation - Convolution/Correlation
	// Mode - by which mode the values outside the array bound is computed
	// Mode - Zero padding/Symmetric/Circular/Replicate
	filteredImage = imfilter(image,filter,CORRELATION_OPERATION,MODE_CIRCULAR);

	filteredImage = imsharpen(filteredImage);

	printMatrix(filteredImage,"Sharpened image");

	struct matrix* adjustedImage = imadjust(filteredImage,0,0.69,0,1,1);

	printMatrix(adjustedImage,"Adjusted image");

	destroyMatrix(image);
	destroyMatrix(filter);
	destroyMatrix(filteredImage);
	destroyMatrix(adjustedImage);

	return 0;
}







