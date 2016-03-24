/*
 * imfilter.c
 *
 *  Created on: 31-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#include "imfilter.h"

float laplacianFilter[3][3] = {{0.1667, 0.6667, 0.1667},
                               {0.6667, -3.333, 0.6667},
                               {0.1667, 0.6667, 0.1667}};

float gaussianFilter[3][3] = {{0.0625, 0.1250, 0.0625},
                              {0.1250, 0.2500, 0.1250},
                              {0.0625, 0.1250, 0.0625}};

PMAT imfilter(PMAT image, PMAT filter, uint8 operation, uint8 mode) {

	float tempResult ;
	float tempPixelValue = 0.0;
	PMAT filteredImage = createMatrix(image->numberOfRows,image->numberOfColumns);

#ifdef DEBUG
	//For debugging initialised a temporary matrix with boundary padded
	// according to the mode given
	//PMAT tempMatrix = createMatrix(filter->numberOfRows,filter->numberOfColumns);
#endif
	//Iterate the rows of the image
	for (uint16 i = 0; i < image->numberOfRows; ++i) {
		//Iterate the columns of the image
		for (uint16 j = 0; j < image->numberOfColumns; ++j) {
				tempResult = 0.0;
				//Iterate the rows of the filter
				for (int l = 0; l < filter->numberOfRows; ++l) {
					//Iterate the columns of the filter
					for (int m = 0; m < filter->numberOfColumns; ++m) {
						// get the pixel value for the given filter mask position(l,m)
						tempPixelValue = getPixelValue(image,i,j,l-(filter->numberOfRows/2),m-(filter->numberOfColumns/2),mode);
#ifdef DEBUG
						//MAT(tempMatrix,l,m) = tempPixelValue;
#endif
						// for convolution operation the filter should be inverted
						if(operation == CONVOLUTION_OPERATION)
						{
							tempResult = tempResult + (tempPixelValue * MAT(filter,(filter->numberOfRows-1-l),(filter->numberOfColumns-1-m)));
						}
						// for correlation the filter should be used as it is
						else
						{
							tempResult = tempResult + (tempPixelValue * MAT(filter,l,m));
						}
					}
				}
#ifdef DEBUG
				//printMatrix(tempMatrix,"temporary Matrix");
#endif
				MAT(filteredImage,i,j) = tempResult;

		}
	}

#ifdef DEBUG
	printMatrix(filteredImage, "filterd Image");
#endif
  
  printf("Image filtering completed \n");

	return filteredImage;
}

float getPixelValue(PMAT image,uint32 imgPosX,uint32 imgPosY,
		                uint32 filterPosX,uint32 filterPosY,uint8 mode)
{
	float pixelValue = 0.0;
	// add the image and filter position
	sint32 imgCurrentPosX = imgPosX+filterPosX;
	sint32 imgCurrentPosY = imgPosY+filterPosY;

	// check whether the resulting added position lies outside the image boundary
	if((imgCurrentPosX < 0) ||
		(imgCurrentPosX > (image->numberOfRows-1)) ||
		(imgCurrentPosY < 0) ||
		(imgCurrentPosY > (image->numberOfColumns-1)))
	{
			// values outside the bounds of the image are the nearest array borders
			if(mode == MODE_REPLICATE)
			{
				imgCurrentPosX = ((imgCurrentPosX < 0) ? 0 : (imgCurrentPosX > image-> numberOfRows-1) ? (image-> numberOfRows-1) : imgCurrentPosX);
				imgCurrentPosY = ((imgCurrentPosY < 0) ? 0 : (imgCurrentPosY > image-> numberOfColumns-1) ? (image-> numberOfColumns-1) : imgCurrentPosY);

				pixelValue = MAT(image,imgCurrentPosX,imgCurrentPosY);
			}
			// values outside the bounds of the image are mirror reflecting the array across the border
			else if(mode == MODE_SYMMETRIC)
			{
				if(imgCurrentPosX < 0 || imgCurrentPosX > image->numberOfRows-1)
				{
					imgCurrentPosX = (image->numberOfRows -1) - (mod(imgCurrentPosX,image->numberOfRows));
				}
				if(imgCurrentPosY < 0 || imgCurrentPosY > image->numberOfColumns-1)
				{
					imgCurrentPosY = (image->numberOfColumns -1) - (mod(imgCurrentPosY,image->numberOfColumns));
				}

				pixelValue = MAT(image,imgCurrentPosX,imgCurrentPosY);
			}
			// values outside the bounds of the image are periodic
			else if(mode == MODE_CIRCULAR)
			{
				if(imgCurrentPosX < 0 || imgCurrentPosX > image->numberOfRows-1)
				{
					imgCurrentPosX = mod(imgCurrentPosX,image->numberOfRows);
				}
				if(imgCurrentPosY < 0 || imgCurrentPosY > image->numberOfColumns-1)
				{
					imgCurrentPosY = mod(imgCurrentPosY,image->numberOfColumns);
				}

				pixelValue = MAT(image,imgCurrentPosX,imgCurrentPosY);
			}
      /* MODE_PAD_ZERO */
      else
      {
        pixelValue = 0.0;
      }
	}
	//  If the postion is inside the image bound,use the current position's value
	else
	{
		pixelValue = MAT(image,imgCurrentPosX,imgCurrentPosY);
	}

	return pixelValue;

}

void normalizeImage(PMAT img)
{
  float mean = 0;
  float sd = 0;
  
  //Iterate the rows of the image
	for (uint16 i = 0; i < img->numberOfRows; ++i) {
		//Iterate the columns of the image
		for (uint16 j = 0; j < img->numberOfColumns; ++j) {
      mean = mean + MAT(img, i, j);
    }
  }
  
  mean = mean / (img->numberOfColumns * img->numberOfRows);
  
  //Iterate the rows of the image
	for (uint16 i = 0; i < img->numberOfRows; ++i) {
		//Iterate the columns of the image
		for (uint16 j = 0; j < img->numberOfColumns; ++j) {
      sd = sd + ((MAT(img, i, j) - mean) * (MAT(img, i, j) - mean));
    }
  }
  
  sd = sqrt(sd);
  
  //Iterate the rows of the image
	for (uint16 i = 0; i < img->numberOfRows; ++i) {
		//Iterate the columns of the image
		for (uint16 j = 0; j < img->numberOfColumns; ++j) {
      MAT(img, i, j) = (MAT(img, i, j) - mean) / sd;
    }
  }
}

// sharpening image - apply laplacian filter and
//subtract the resultant image from the original image
PMAT imsharpen(PMAT image)
{
	PMAT filter;
	filter = createMatrix(3,3);

	initMatrix(filter, (void*)laplacianFilter);

	printMatrix(filter,"filter");
	printMatrix(image,"image");

	PMAT filteredImage = imfilter(image, filter, CORRELATION_OPERATION, MODE_PAD_ZERO);

	PMAT sharpenedImage = subtractMatrices(image, filteredImage);

	destroyMatrix(filter, "");
	destroyMatrix(filteredImage, "");

	return sharpenedImage;
}

PMAT imblur(PMAT image)
{
  PMAT filter;
	filter = createMatrix(3,3);

	initMatrix(filter, (void*)gaussianFilter);

	printMatrix(filter,"filter");
	printMatrix(image,"image");

	PMAT blurredImage = imfilter(image, filter, CORRELATION_OPERATION, MODE_PAD_ZERO);

	destroyMatrix(filter, "");

	return blurredImage;
}


