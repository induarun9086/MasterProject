/*
 * RGBToGray.c
 *
 *  Created on: 31-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#include "RGBToGray.h"

// function to convert RGB to gray values
PMAT convertRGBToGrayScale(uint8* rgb,uint32 rgbSize,uint32 width,sint32 height)
{
	PMAT grayScale = createMatrix(abs(height),width);

	float grayValue;
	float red,green,blue;

	uint32 j,k;
  uint32 colBytes = (width*3);
  int idx = 0;
  int colByteCount = 0;
  
	for (int i = 0; i < rgbSize; i+=3) {
		red = (float)rgb[i];
		green = (float)rgb[i+1];
		blue = (float)rgb[i+2];

		grayValue = 0.2989*red + 0.5870*green + 0.1140*blue;

		j = (idx/3)/width;
		k = (idx/3)%width;

    if((j < height) && (k < width)) {
      if(height < 0) {
        MAT(grayScale,j,k) = grayValue;
      } else if(height > 0) {
        MAT(grayScale,(height - 1 - j), k) = grayValue;
      } else {
        assert(0);
      }
    }
    
    colByteCount += 3;
    idx += 3;
    
    if((colByteCount == colBytes) && ((colBytes%4) != 0)) {
      i += (4 - (colBytes%4));
      colByteCount = 0;
    }
	}

	return grayScale;
}

PMAT convertRGBToMatrix(uint8* rgb,uint32 rgbSize,uint32 width,sint32 height)
{
  PMAT rgbImg = createMatrixGnDataType(abs(height),width,MAT_UINT32);

  uint32 rgbVal;
	uint32 j,k;

	for (int i = 0; i < rgbSize; i+=3) {
    rgbVal = 0;
		rgbVal = rgbVal | (uint32)rgb[i];   rgbVal = rgbVal << 8;
		rgbVal = rgbVal | (uint32)rgb[i+1]; rgbVal = rgbVal << 8;
		rgbVal = rgbVal | (uint32)rgb[i+2];

		j = (i/3)/width;
		k = (i/3)%width;

    if((j < height) && (k < width)) {
      if(height < 0) {
        MATU32(rgbImg,j,k) = rgbVal;
      } else if(height > 0) {
        MATU32(rgbImg,(height - 1 - j), k) = rgbVal;
      } else {
        assert(0);
      }
    }
	}

	return rgbImg;
}
