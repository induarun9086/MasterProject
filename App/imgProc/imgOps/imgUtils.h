/*
 * imgUtils.h
 *
 *  Created on: 21-Feb-2015
 *      Author: Indumathi Duraipandian
 */

#ifndef IMGOPS_IMGUTILS_H_
#define IMGOPS_IMGUTILS_H_

#include "../utils/misc.h"
#include "../utils/matrix.h"

PMAT removeBG(PMAT image);
PMAT imadjust(PMAT image, float low_in, float high_in, float low_out, float high_out, float gamma);
float quantile(PMAT image, float quantile);
PMAT binarizeImage(PMAT image, float threshold);
float doMorphPass(PMAT image, PMAT st, int numMatchReq, uint8 fgVal, uint8 bgVal);
void thickImage(PMAT image);
PMAT imageFromBinInfo(PMAT binImage);
PMAT seperateObjsFromImage(PMAT binImage, int* numObjsFound);
PMAT * markObjsInImage(PMAT oriImg, PMAT objImg, int numObjsFound);
PMAT scaleDown(PMAT img, int widthScale, int heightScale);
PMAT markBoundaryInImage(PMAT img, float maxVal, POS center, int width, int height);

#endif /* IMGOPS_IMGUTILS_H_ */
