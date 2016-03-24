/*
 * bmp.h
 *
 *  Created on: 21-Feb-2015
 *      Author: Indumathi Duraipandian
 */

#ifndef FILEOPS_BMP_H_
#define FILEOPS_BMP_H_

#include "../utils/misc.h"
#include "../utils/matrix.h"

#define BMP_FILE_HEADER_BUF_LENGTH (256)

#define BMP_FILE_BMP_HEADER_LENGTH (14)

#define BMP_FILE_BMP_CORE_HEADER_LENGTH    (12)
#define BMP_FILE_OS2_1X_HEADER_LENGTH      (12)
#define BMP_FILE_BMP_INFO_HEADER_LENGTH    (40)
#define BMP_FILE_OS2_2X_HEADER_LENGTH      (64)
#define BMP_FILE_BMP_INFO_V2_HEADER_LENGTH (52)
#define BMP_FILE_BMP_INFO_V3_HEADER_LENGTH (56)
#define BMP_FILE_BMP_INFO_V4_HEADER_LENGTH (108)
#define BMP_FILE_BMP_INFO_V5_HEADER_LENGTH (124)

struct bmpInfo {
  uint32 imageLength;
  uint32 imageOffset;
  uint32 imageWidth;
  sint32 imageHeight;
};

uint32 openBmpFile(char* fileName, struct bmpInfo* header, uint8* imgData);
void writeAsBMP(char* fileName, PMAT img, uint8* imgBuf);

#endif /* FILEOPS_BMP_H_ */