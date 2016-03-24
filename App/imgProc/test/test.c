/*
 * test.c
 *
 *  Created on: 21-Feb-2015
 *      Author: Indumathi Duraipandian
 */

#include "test.h"

void doTest(void) {
  // Declare matrices for image,filter and the output
  PMAT image;
  PMAT filter;
  PMAT filteredImage;

  //Initialize matrices
  image = createMatrix(7,7);
  filter = createMatrix(5,5);

  float value = 1.0;

  // For testing purpose - Initialized a 7x7 matrix as image
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

  PMAT adjustedImage = imadjust(filteredImage,0,0.69,0,1,1);

  printMatrix(adjustedImage,"Adjusted image");

  destroyMatrix(image, "");
  destroyMatrix(filter, "");
  destroyMatrix(filteredImage, "");
  destroyMatrix(adjustedImage, "");
}