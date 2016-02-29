/*
 * templatematching.c
 *
 *  Created on: 29-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#include "templatematching.h"

uint8 imageData[TEMPLATEMATCHING_IMAGE_LENGTH];

uint8 silentRun = 1;
uint8 showImages = 0;

int main(int argc, char* argv[])
{
  struct bmpInfo imgHdr;
  int numObjs = 0;
  
  /* If enough inputs available */
	if(argc >= 2) {
      /* Open the given BMP file  */
      if(openBmpFile(&argv[1][0], &imgHdr, &imageData[0]) == 0) {
        /* Convert the read RGB file into gray scale and put it in a Matrix */
        PMAT gray = convertRGBToGrayScale(imageData, imgHdr.imageLength,
                                                     imgHdr.imageWidth,
                                                     imgHdr.imageHeight);
        PMAT opgray = convertRGBToGrayScale(imageData, imgHdr.imageLength,
                                                       imgHdr.imageWidth,
                                                       imgHdr.imageHeight);

        /* Begin image processing */
        PMAT contrastImg = imadjust(gray, 60, 180, 255, 0, 1); destroyMatrix(gray, "gray");
        PMAT noBgImg = removeBG(contrastImg);  destroyMatrix(contrastImg, "contrastImg");
        PMAT blurImg = imblur(noBgImg);  destroyMatrix(noBgImg, "noBgImg");
        //PMAT sharpImg = imsharpen(blurImg); destroyMatrix(blurImg, "blurImg1");
        //PMAT adjustedImg = imadjust(sharpImg, 0, 176, 0, 255, 1); destroyMatrix(sharpImg, "sharp1");
        //sharpImg = imsharpen(adjustedImg); destroyMatrix(adjustedImg, "adjust1");
        //adjustedImg = imadjust(sharpImg, 0, 210, 0, 255, 1); destroyMatrix(sharpImg, "sharp2");
        float threshold = quantile(blurImg, 0.45); 
        printf("Threshold = %f\n", threshold);
        
        PMAT binImage = binarizeImage(blurImg, threshold); destroyMatrix(blurImg, "blurImg");
        thickImage(binImage);
        
        //PMAT opImage = imageFromBinInfo(binImage);
        PMAT opImage = seperateObjsFromImage(binImage, &numObjs); destroyMatrix(binImage, "thickbin");
        
        PMAT opObjs = markObjsInImage(opgray, opImage, numObjs); destroyMatrix(opImage, "opImage");
        
        //dumpMatrixToFile(opImage, "image.txt", "%05.0f ", "      ");
        writeAsBMP("output.bmp", opObjs, &imageData[0]);
        /* Write sharpened image as output */
        destroyMatrix(opgray, "");
        destroyMatrix(opObjs, "");
      }
    }
    /* In case no inputs, go to test mode */
    else {
      /* Do test operations */
      doTest();
    }
  
  system("cygstart ./output.bmp");

  if(silentRun == 0) {
    printf("\n Enter any key to close\n ");
    getchar();
  }
  
	return 0;
}







