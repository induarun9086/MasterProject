/*
 * imfilter.h
 *
 *  Created on: 31-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#ifndef IMGOPS_IMFILTER_H_
#define IMGOPS_IMFILTER_H_

#include "../utils/misc.h"
#include "../utils/matrix.h"

#define CONVOLUTION_OPERATION (0)
#define CORRELATION_OPERATION (1)

#define MODE_PAD_ZERO (0)
#define MODE_SYMMETRIC (1)
#define MODE_REPLICATE (2)
#define MODE_CIRCULAR (3)

PMAT imfilter(PMAT image,PMAT filter,uint8 operation,uint8 mode);
void normalizeImage(PMAT img);
float getPixelValue(PMAT image,uint32 imgPosX,uint32 imgPosY,uint32 filterPosX,uint32 filterPosY,uint8 mode);

PMAT imsharpen(PMAT image);

PMAT imblur(PMAT image);

#endif /* IMGOPS_IMFILTER_H_ */
