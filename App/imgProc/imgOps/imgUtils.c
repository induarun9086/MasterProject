/*
 * imgUtils.c
 *
 *  Created on: 31-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#include "imgUtils.h"

PMAT objs[257] = {0};

int getBinId(int* binIds, int numBins, int binId);

int getBinId(int* binIds, int numBins, int binId)
{
  int bId = 0;
  int endLoop = 0;
  
  for (int i = 0; ((i < numBins) && (endLoop == 0)); i++) {
    if(binIds[i] == binId) {
      endLoop = 1;
      bId = i;
    }
  }
  
  return bId;
}

PMAT removeBG(PMAT image)
{
  PMAT noBgImg = createMatrixFromGn(image);
	int bins[10] = {0};
  int binId[10] = {0};
  int maxBins = 10;
  
	for (int i = 0; i < image->numberOfRows; ++i) {
		for (int j = 0; j < image->numberOfColumns; ++j) {
      for (int k = 0; k < maxBins; k++) {
        if((MAT(image, i, j) <= ((k+1) * (256 / maxBins))) &&
           (MAT(image, i, j) >= ((k) * (256 / maxBins)))) {
          bins[k]++;
          binId[k] = k;
        }
      }
    }
  }
  
  for (int i = 0; i < maxBins; ++i) {
		for (int j = i+1; j < maxBins; ++j) {
      if(bins[i] < bins[j]) {
        int t = bins[i]; bins[i] = bins[j]; bins[j] = t;
        t = binId[i]; binId[i] = binId[j]; binId[j] = t;
      }
    }
  }
  
  int endLoop = 0;
  int numBgPx = bins[binId[0]];
  int minBin  = binId[0], maxBin = binId[0]+1;
  int maxBgPx = (image->numberOfRows * image->numberOfColumns * 0.75);
  for (int k = 1; ((k < maxBins) && (endLoop == 0)); k++) {
    if((binId[0] - k) > 0) {
      int bId = getBinId(&binId[0], maxBins, (binId[0] - k));
      int t = bins[bId];
      if((numBgPx + t) >= maxBgPx) {
        endLoop = 1;
      } else {
        numBgPx += t;
        minBin = (binId[0] - k);
      }
    }
    if(((binId[0] + k) < maxBins) && (endLoop == 0)) {
      int bId = getBinId(&binId[0], maxBins, (binId[0] + k));
      int t = bins[bId];
      if((numBgPx + t) >= maxBgPx) {
        endLoop = 1;
      } else {
        numBgPx += t;
        maxBin = (binId[0] + k);
      }
    }
  }
  
  printf("removeBG maxBgBin:%d, minBin:%d, maxBin:%d numBgPx:%d\n", binId[0], minBin, maxBin, numBgPx);
  
  for (int i = 0; i < image->numberOfRows; ++i) {
		for (int j = 0; j < image->numberOfColumns; ++j) {
      if((MAT(image, i, j) >= (minBin * (256 / maxBins))) &&
         (MAT(image, i, j) <= (maxBin * (256 / maxBins)))) {
        MAT(noBgImg, i, j) = 0;
      }
    }
  }
  
  return noBgImg;
}

PMAT imadjust(PMAT image, float low_in, float high_in, float low_out, float high_out, float gamma)
{
	PMAT adjustedImage = createMatrix(image->numberOfRows,
                                    image->numberOfColumns);
	float maxIntensity = 0;
	float minIntensity = 0;
	for (int i = 0; i < image->numberOfRows; ++i) {
		for (int j = 0; j < image->numberOfColumns; ++j) {
				if(MAT(image,i,j) > maxIntensity)
				{
					maxIntensity = MAT(image,i,j);
				}
				if(MAT(image,i,j) < minIntensity)
				{
					minIntensity = MAT(image,i,j);
				}
			}
		}
  
	float oldRange = 255 / (maxIntensity - minIntensity);

	for (int i = 0; i < image->numberOfRows; ++i) {
				for (int j = 0; j < image->numberOfColumns; ++j) {

					float temp = MAT(image,i,j);
          
					if(temp < low_in)
					{
						MAT(adjustedImage,i,j) = low_out;
					}
					else if(temp > high_in)
					{
						MAT(adjustedImage,i,j) = high_out;
					}
					else
					{
            //temp = (temp - minIntensity) / oldRange;
						MAT(adjustedImage,i,j) = temp * gamma;
					}
				}
	}

	return adjustedImage;
}

float quantile(PMAT image, float quantileReq)
{
  float quantileValue = 0.0;
  
#if QUANTILE_SLOW_VERSION
  
  float temp;
  int numElements = image->numberOfRows * image->numberOfColumns;
  PMAT sortedValues = createMatrix(1, numElements);
  PMAT quantile = createMatrix(1, numElements);
  
  /* Copy the given to a row vector */
  for (int i = 0; i < image->numberOfRows; ++i) {
				for (int j = 0; j < image->numberOfColumns; ++j) {
          int k = (i * image->numberOfColumns);
          MAT(sortedValues, 0, (k + j)) = MAT(image, i, j);
          MAT(quantile, 0, (k + j)) = ((0.5 + k) / numElements);
        }
  }
  
  /* Sort the vector */
  for (int i = 0; i < (image->numberOfRows  * image->numberOfColumns); ++i) {
    for (int j = i; j < (image->numberOfRows  * image->numberOfColumns); ++j) {
      if(MAT(sortedValues, 0, i) > MAT(sortedValues, 0, j)) {
        temp = MAT(sortedValues, 0, i);
        MAT(sortedValues, 0, i) = MAT(sortedValues, 0, j);
        MAT(sortedValues, 0, j) = temp;
      }
    }
    if((i % 1000) == 0) printf("i=%d \r", i);
  }
  
  /* If requested quantile is less then the smallest element
     the required quantile value is the minimum element value */
  if(quantileReq <= MAT(quantile, 0, 0)) {
    quantileValue = MAT(sortedValues, 0, 0);
  } else {
    int qualtileFound = 0;
    float prevQuantile = MAT(quantile, 0, 0);
    float prevQuantileValue = MAT(sortedValues, 0, 0);
    /* Go through all the elements */
    for(int i = 1; ((i < numElements) && (qualtileFound == 0)); i++) {
      /* Find the range for the given quantile */
      if((quantileReq >= prevQuantile) && (quantileReq <= MAT(quantile, 0, i))) {
        /* interpolate for the required value and set as found */
        qualtileFound = 1;
        /* (x - x1) */
        quantileValue = (quantileReq - prevQuantile);
        /* ((x - x1) / (x2 - x1)) */
        quantileValue = (quantileValue / (MAT(quantile, 0, i) - prevQuantile));
        /* ((x - x1) / (x2 - x1)) * (y2 - y1) */
        quantileValue = (quantileValue * (MAT(sortedValues, 0, i) - prevQuantileValue));
        /* y1 + ((x - x1) / (x2 - x1)) * (y2 - y1) */
        quantileValue = (prevQuantileValue + quantileValue);
      }
      /* Update previous values */
      prevQuantile = MAT(quantile, 0, i);
      prevQuantileValue = MAT(sortedValues, 0, i);
    }
    
    /* If still not found use the maximum value */
    if(qualtileFound == 0) {
      quantileValue = MAT(sortedValues, 0, (numElements-1));
    }
  }
  
  destroyMatrix(sortedValues);
  destroyMatrix(quantile);
#else
  
  /* The fast version is based on the assumption that for 
     image processing on gray scale images the values will
     always be in the range 0 - 255 */
  PMAT valuesInfo = createMatrix(1, 256);
  PMAT values     = createMatrix(1, 256);
  int  numValues  = 0;
  
  /* Go through the given matrix and find the values found */
  for (int i = 0; i < image->numberOfRows; ++i) {
    for (int j = 0; j < image->numberOfColumns; ++j) {
      int currVal = (int)MAT(image, i, j);
      MAT(valuesInfo, 0, currVal) = 1;
    }
  }
  
  for(int i = 0; i < 256; i++) {
    if(MAT(valuesInfo, 0, i) == 1) {
      MAT(values, 0, numValues) = i;
      numValues++;
    }
  }
  
  PMAT quantile = createMatrix(1, numValues);
  for(int i = 0; i < numValues; i++) {
    MAT(quantile, 0, i) = ((0.5 + i) / numValues);
  }
  
  /* If requested quantile is less then the smallest element
     the required quantile value is the minimum element value */
  if(quantileReq <= MAT(quantile, 0, 0)) {
    quantileValue = MAT(values, 0, 0);
  } else {
    int qualtileFound = 0;
    float prevQuantile = MAT(quantile, 0, 0);
    float prevQuantileValue = MAT(values, 0, 0);
    /* Go through all the elements */
    for(int i = 1; ((i < numValues) && (qualtileFound == 0)); i++) {
      /* Find the range for the given quantile */
      if((quantileReq >= prevQuantile) && (quantileReq <= MAT(quantile, 0, i))) {
        /* interpolate for the required value and set as found */
        qualtileFound = 1;
        /* (x - x1) */
        quantileValue = (quantileReq - prevQuantile);
        /* ((x - x1) / (x2 - x1)) */
        quantileValue = (quantileValue / (MAT(quantile, 0, i) - prevQuantile));
        /* ((x - x1) / (x2 - x1)) * (y2 - y1) */
        quantileValue = (quantileValue * (MAT(values, 0, i) - prevQuantileValue));
        /* y1 + ((x - x1) / (x2 - x1)) * (y2 - y1) */
        quantileValue = (prevQuantileValue + quantileValue);
      }
      /* Update previous values */
      prevQuantile = MAT(quantile, 0, i);
      prevQuantileValue = MAT(values, 0, i);
    }
    
    /* If still not found use the maximum value */
    if(qualtileFound == 0) {
      quantileValue = MAT(values, 0, (numValues-1));
    }
  }

  destroyMatrix(valuesInfo, "");
  destroyMatrix(values, "");
  destroyMatrix(quantile, "");
#endif
  
  return quantileValue;
}

PMAT binarizeImage(PMAT image, float threshold)
{
  PMAT binImage = createMatrixGnDataType(image->numberOfRows, image->numberOfColumns, MAT_BINARY);
  
  for (int i = 0; i < image->numberOfRows; ++i) {
    for (int j = 0; j < image->numberOfColumns; ++j) {
      if(MAT(image, i, j) >= threshold) {
        MATB(binImage, i, j) = 1;
      }
    }
  }
    
  return binImage;
}

float doMorphPass(PMAT image, PMAT st, int numMatchReq, uint8 fgVal, uint8 bgVal)
{
  float numChange = 0;
  int offset = st->numberOfRows / 2;
  uint8 currVal = 0;
  
  PMAT input = createMatrixFromGn(image);
  
  for (int i = 0; i < image->numberOfRows; ++i) {
		for (int j = 0; j < image->numberOfColumns; ++j) {
      int match = 0;
      
      for (int k = -offset; ((k <= offset)); k++) {
        for (int l = -offset; ((l <= offset)); l++) {
          if(((i+k) < 0) || ((i+k) >= image->numberOfRows) || 
             ((j+l) < 0) || ((j+l) >= image->numberOfColumns)) {
             currVal = 0;
           } else {
             currVal = MATB(input, (i+k), (j+l));
           }
          if((currVal == MATB(st, (k+offset), (l+offset))) ||
             (MATB(st, (k+offset), (l+offset)) == 255)) {
            match++;
          }
        }
      }
      
      if((match >= numMatchReq) && (fgVal != 255)) {
        if(MATB(input, i, j) != fgVal) { numChange++; }
        MATB(image, i, j) = fgVal;
      } else if((match < numMatchReq) && (bgVal != 255)) {
        if(MATB(image, i, j) != bgVal) { numChange++; }
        MATB(image, i, j) = bgVal;
      } else {
        MATB(image, i, j) = MATB(input, i, j);
      }
    }
  }
  
  destroyMatrix(input, "");
  
  return numChange;
}

void thickImage(PMAT image)
{
  int numChange = 0;
  int numPasses = 0;
  uint8 single[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
  uint8 st1[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  uint8 st2[9] = {255, 1, 1, 255, 0, 2, 0, 255, 1};
  
  PMAT st[8];

  PMAT singleSt = createMatrixGnDataType(3, 3, MAT_BINARY); initMatrix(singleSt, single);
  st[0] = createMatrixGnDataType(3, 3, MAT_BINARY); initMatrix(st[0], st1);
  st[1] = createMatrixGnDataType(3, 3, MAT_BINARY); initMatrix(st[1], st2);
  st[2] = rotateLeft(st[0]); st[3] = rotateLeft(st[1]);
  st[4] = rotateLeft(st[2]); st[5] = rotateLeft(st[3]);
  st[6] = rotateLeft(st[4]); st[7] = rotateLeft(st[5]);
  
  /* Remove any single pixels */
  doMorphPass(image, singleSt, 9, 0, 255);
  
  numPasses = 0;
  do {
    /* Set as there is some change */
    numChange = 0;

    /* For all the eight structural elements */
    for(int i = 0; i < 1; i++) {
      if(numPasses == 0) {
        printMatrix(st[i], "St");
      }
      numChange += doMorphPass(image, st[i], 1, 1, 255);
    }
    numPasses++;
  }while((numChange > 0) && (numPasses < 15));
  
  printf("thickImage numPasses: %d\n", numPasses);
}

PMAT imageFromBinInfo(PMAT binImage)
{
  PMAT image = createMatrix(binImage->numberOfRows, binImage->numberOfColumns);
  
  for (int i = 0; i < image->numberOfRows; ++i) {
		for (int j = 0; j < image->numberOfColumns; ++j) {
      MAT(image, i, j) = (float)(MATB(binImage, i, j) * 255);
    }
  }
  
  return image;
}

PMAT seperateObjsFromImage(PMAT binImage, int* numObjsFound)
{
  PMAT image = createMatrix(binImage->numberOfRows, binImage->numberOfColumns);
  uint8 currVal = 0;
  uint8 obj[1000] = {0};
  int currImgVal = 0;
  int numObjs = 0;
  int prevObjs = 0;
  int label = 0;
  int numPassReq = 0;
  int numPasses = 30;
  
  for (int p = 0; p < numPasses; p++) {
    for (int i = 0; i < binImage->numberOfRows; ++i) {
      for (int j = 0; j < binImage->numberOfColumns; ++j) {
        int match = 0;
        label = 0;
        int x = i, y = j;
        if((p%2) == 0) {
          x = binImage->numberOfRows - 1 - i;
          y = binImage->numberOfColumns - 1 - j;
        }
        if(p < (numPasses - 1)) {
          for (int k = -1; ((k <= 1)); k++) {
            for (int l = -1; ((l <= 1)); l++) {
              if(((x+k) < 0) || ((x+k) >= binImage->numberOfRows) || 
                 ((y+l) < 0) || ((y+l) >= binImage->numberOfColumns)) {
                 currVal = 0; currImgVal = 0;
               } else {
                 currVal = MATB(binImage, (x+k), (y+l));
                 currImgVal = (int)MAT(image, (x+k), (y+l));
               }

              if(currImgVal > 0) {
                if((label == 0) || (currImgVal < label)) {
                  label = currImgVal;
                }
              }
              if(currVal >= 1) {
                match++;
              }
            }
          }
        } else {
          if(MAT(image, x, y) > 0) {
            match = 9;
            label = MAT(image, x, y);
          }
        }

        if(match == 9) {
          if(p == 0) {
            if(label == 0) {
              numObjs++;
              if(numObjs < 1000) {
                label = numObjs;
                obj[label] = p+1;
              } else {
                assert(0);
              }
            }
          }
          if(p < (numPasses - 1)) {
            MAT(image, x, y) = label;
            obj[label] = p+1;
          } else {
            MAT(image, x, y) = label * 30;
            obj[label] = p+1;
          }
        } else {
          MAT(image, x, y) = 0;
        }
      }
    }

    if((p > 0) && (p < (numPasses - 1))) {
      prevObjs = numObjs;
      numObjs  = 0;
      for (int i=0; i < 1000; i++) {
        if(obj[i] == (p+1)) {
          numObjs++;
        }
      }
      if(prevObjs == numObjs) {
        p = numPasses - 2;
      }
    }
    numPassReq++;
  }
  
  *numObjsFound = numObjs;
  printf("seperateObjs: numPases: %d numObjs: %d\n", numPassReq, numObjs);

  return image;
}

PMAT * markObjsInImage(PMAT oriImg, PMAT objImg, int numObjsFound)
{
  int numObjs = 0;
  int numMarkedObjs = 1;
  int label = 1;
  int left, top, right, bottom;
  int numPx = 0;
  int labels[256] = {0};
  
  for(int i = 0; i < 257; i++) {
    objs[i] = 0;
  }
  
  objs[0] = createMatrixFromGn(oriImg);
  
  if(numObjsFound > 256) { assert(0); }
  
  /* Find the labels present in the separated image
   * It could happen that the separated image doesn't
   * contain the labels in order (because when separating
   * some labels will be removed after identifying that
   * they were part of the same object). So now find out
   * the labels found in the separated image.
   * ex: {1, 5, 6, 8, 9, 10, 23...} */
  numPx = 0;
  /* For all the pixels */
  for (int i = 0; (i < objImg->numberOfRows); ++i) {
    for (int j = 0; (j < objImg->numberOfColumns); ++j) {
      /* If the pixel has a label */
      if((MAT(objImg, i, j)) != 0) {
        numPx = 0;
        /* For all the labels */
        for (int k = 0; ((k < 256) && (numPx == 0)) ;k++) {
          /* Check if we already have this label in our list */
          if(labels[k] == (MAT(objImg, i, j))) {
            numPx++;
          }
        }
        /* If we don't have this in our list */
        if(numPx == 0){
          /* Store the label */
          labels[numObjs] = MAT(objImg, i, j);
          numObjs++;
        }
      }
    }
  }

  /* Now find the boundaries for the objects specified 
   * by the labels in our list, until we have found all
   * the objects reported as found */
  label = 0;
  numObjs = 0;
  do
  {
    numPx = 0;
    
    /* Initialize the boundaries as the boundary of the image
     * inverted. */
    right = 0, bottom = 0;
    left = objImg->numberOfColumns, top = objImg->numberOfRows;
    
    /* Update the boundary of the object with the current label */
    
    /* For all the pixels in the image */
    for (int i = 0; i < objImg->numberOfRows; ++i) {
      for (int j = 0; j < objImg->numberOfColumns; ++j) {
        /* If the pixel matches the current label */
        if((MAT(objImg, i, j)) == labels[label]) {
          /* If the pixel is left to the left of the current left boundary
           * If the pixel is right to the right of the current right boundary
           * If the pixel is above the top of the current top boundary
           * If the pixel is below to the bottom of the current bottom boundary
           * update the corresponding boundary */
          if(j < left) { left = j; }
          if(j > right) { right = j; }
          if(i < top) { top = i; }
          if(i > bottom) { bottom = i; }
          /* And also count the number of pixels of this label */
          numPx++;
        }
      }
    }

    /* If we have a at-least one pixel with the current label */
    if(numPx > 0) {
      /* Count up the number of objects found */
      numObjs++;
      /* Check if the object is big enough to be a real
       * object, the number of pixels equal to the area
       * of the object. i.e. w*h. Ignore the small
       * objects.
       */
      if(numPx > ((oriImg->numberOfRows * oriImg->numberOfColumns) / 25)) {
        /* initialize the object */
        objs[numMarkedObjs] = createMatrixGnDataType((bottom - top) + 1, (right - left) + 1, MAT_UINT32);
    
        /* Again loop through all the pixels */
        for (int i = 0; i < oriImg->numberOfRows; ++i) {
          for (int j = 0; j < oriImg->numberOfColumns; ++j) {
            /* If the current pixel is on the top/bottom or
             * left/right border and inside the left/right or
             * top/bottom boundaries, mark the pixel as a 
             * boundary.
             */
            if(((i == top) && (j >= left) && (j <= right)) || 
               ((i == bottom) && (j >= left) && (j <= right)) || 
               ((j == left) && (i >= top) && (i <= bottom)) || 
               ((j == right) && (i >= top) && (i <= bottom))) {
              MAT(objs[0], i, j) = 0;
            }
            
            /* If the current pixel is inside the boundary */
            if((i >= top) && (i <= bottom) && (j >= left) && (j <= right)) {
              /* Save the pixel in the object */
              MATU32(objs[numMarkedObjs], (i - top), (j - left)) = MATU32(oriImg, i, j);
            }
          }
        }
        
        /* Increment the number of marked objects */
        numMarkedObjs++;
      }
    }
    
    label++;
    
  }while(numObjs < numObjsFound);
  
  printf("markObjsInImage numObjs:%d numMarkedObjs:%d\n", numObjs, numMarkedObjs);
  
  return objs;
}

PMAT scaleDown(PMAT img, int widthScale, int heightScale)
{
  if((widthScale < 1) || (heightScale < 1)) {
    assert(0);
  }
  
  PMAT scaleImg = createMatrix(img->numberOfRows/heightScale, img->numberOfColumns/widthScale);
  
  int numRows = img->numberOfRows - (img->numberOfRows % heightScale);
  int numCols = img->numberOfColumns - (img->numberOfColumns % widthScale);
  
  for(int i = 0; i < numRows; i+=heightScale) {
    for(int j = 0; j < numCols; j+=widthScale) {
      float avgVal = 0;
      
      for(int k = 0; k < heightScale; k++) {
        for(int l = 0; l < widthScale; l++) {
          avgVal += MAT(img, (i+k), (j+l));
        }
      }
      
      avgVal /= (widthScale * heightScale);
      
      MAT(scaleImg, 
        (i/widthScale), (j/heightScale)) = avgVal;
    }
  }
  
  printf("scaleDown image scaled down to %d x %d \n", scaleImg->numberOfColumns, scaleImg->numberOfRows);
  
  return scaleImg;
}

PMAT markBoundaryInImage(PMAT img, float maxVal, POS center, int width, int height)
{
  PMAT markedImg = createMatrixFromGn(img);
  
  if(maxVal > 235) {
    if((center.x < (height / 2)) || (center.y < (width / 2))) {
      assert(0);
    }

    int topEdge = (center.x - (height / 2));
    int bottomEdge = (center.x + (height / 2));
    int leftEdge = (center.y - (width / 2));
    int rightEdge = (center.y + (width / 2));

    for(int i = 0; i < img->numberOfRows; i++) {
      for(int j = 0; j < img->numberOfColumns; j++) {
        if((((i == topEdge) || (i == bottomEdge)) && ((j >= leftEdge) && (j <= rightEdge))) ||
           (((j == leftEdge) || (j == rightEdge)) && ((i >= topEdge)  && (i <= bottomEdge)))) {
          MAT(markedImg, i, j) = 0;
        }
      }
    }
  }
  
  return markedImg;
}
