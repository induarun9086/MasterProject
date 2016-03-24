/*
 * RGBToGray.h
 *
 *  Created on: 31-Oct-2015
 *      Author: indudinesh
 */

#ifndef IMGOPS_RGBTOGRAY_H_
#define IMGOPS_RGBTOGRAY_H_

#include "../utils/misc.h"
#include "../utils/matrix.h"

PMAT convertRGBToGrayScale(uint8* rgb,uint32 rgbSize,uint32 width,sint32 height);
PMAT convertRGBToMatrix(uint8* rgb,uint32 rgbSize,uint32 width,sint32 height);

#endif /* IMGOPS_RGBTOGRAY_H_ */
