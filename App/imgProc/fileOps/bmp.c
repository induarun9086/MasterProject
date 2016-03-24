/*
 * bmp.c
 *
 *  Created on: 21-Feb-2015
 *      Author: Indumathi Duraipandian
 */

#include "bmp.h"

uint8 imageHeader[BMP_FILE_HEADER_BUF_LENGTH];

uint32 openBmpFile(char* fileName, struct bmpInfo* imgInfo, uint8* imgData) {
  uint32 err = 0;
  uint32 fileLength   = 0;
	uint32 numBytesRead = 0;
  struct stat fileStat;

  /* Open the input file */
  FILE* filePtr = fopen(fileName, "rb");

  /* When the file is opened */
  if (filePtr != NULL) {
    /* Find the length of the opened file */
    stat(fileName, &fileStat);
    fileLength = fileStat.st_size;

    /* Read BMP header */
    numBytesRead = fread(&imageHeader[0], BMP_FILE_BMP_HEADER_LENGTH, 1, filePtr);

    /* find image data offset and image length */
    imgInfo->imageOffset = ((uint32*)(&imageHeader[2]))[2];
    imgInfo->imageLength  = fileLength - imgInfo->imageOffset;

    /* Read DIB header */
    numBytesRead = fread(&imageHeader[BMP_FILE_BMP_HEADER_LENGTH], 1,
                         (imgInfo->imageOffset - BMP_FILE_BMP_HEADER_LENGTH), filePtr);

    /* find image width and height */
    imgInfo->imageWidth  = ((uint32*)(&imageHeader[BMP_FILE_BMP_HEADER_LENGTH]))[1];
    imgInfo->imageHeight = ((uint32*)(&imageHeader[BMP_FILE_BMP_HEADER_LENGTH]))[2];

    /* Read image data */
    numBytesRead = fread(imgData, 1, imgInfo->imageLength, filePtr);

    /* Close the file */
    fclose(filePtr);
  }
  else {
    err = __LINE__;
  }

  return err;
}

void writeAsBMP(char* fileName, PMAT img, uint8* imgBuf) {
  uint8  bmpHeader[BMP_FILE_BMP_HEADER_LENGTH + BMP_FILE_BMP_INFO_HEADER_LENGTH] = {0};
  uint32 imgOffset = BMP_FILE_BMP_HEADER_LENGTH + BMP_FILE_BMP_INFO_HEADER_LENGTH;
  
  /* Convert the matrix back to buffer */
  uint32 fileLength = convertMatrixToBuffer(img, imgBuf);
  
  /* Build the header */
  bmpHeader[0] = 'B';
  bmpHeader[1] = 'M';
  
  /* Set file length */
  (*((uint32*)(&bmpHeader[2]))) = fileLength + imgOffset;
  
  /* Set image offset */
  (*((uint32*)(&bmpHeader[10]))) = imgOffset;
  
  /* Set core header size */
  (*((uint32*)(&bmpHeader[14]))) = BMP_FILE_BMP_INFO_HEADER_LENGTH;
  
  /* Set image width */
  (*((uint32*)(&bmpHeader[18]))) = img->numberOfColumns;
  
  /* Set image height */
  (*((sint32*)(&bmpHeader[22]))) = (-img->numberOfRows);
  
  /* Set color panes */
  (*((uint16*)(&bmpHeader[26]))) = 1;
  
  /* Set image offset */
  (*((uint16*)(&bmpHeader[28]))) = 24;
  
  /* Open the output file */
  FILE* filePtr = fopen(fileName, "wb");

  /* When the output file is opened */
  if (filePtr != NULL) {
    /* Write output data as BMP with the read header */
    fwrite(bmpHeader, 1, imgOffset,   filePtr);
    fwrite(imgBuf,    1, fileLength,  filePtr);

    /* Close output file */
    fclose(filePtr);
  }
}
