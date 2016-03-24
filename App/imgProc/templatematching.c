/*
 * templatematching.c
 *
 *  Created on: 29-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#include "templatematching.h"

uint8 imageData[TEMPLATEMATCHING_IMAGE_LENGTH];
uint8 filterData[TEMPLATEMATCHING_IMAGE_LENGTH];

uint8 silentRun = 1;
uint8 showImages = 0;

int main(int argc, char* argv[])
{
  struct bmpInfo imgHdr;
  struct bmpInfo filterHdr;
  int numObjs = 0;
  char opFileName[1024];
  int runMode = RUN_MODE_UNDEF;
  int err = 0;
  int runSeg = 0;

  /* If enough inputs available */
	if(argc >= 3) {
    runMode = atoi(&argv[1][0]);
    runSeg = atoi(&argv[2][0]);
    
    if(runMode == RUN_MODE_SEPERATE_OBJS) {
      if(argc < 4) {
        runMode = RUN_MODE_UNDEF;
      }
    } else if(runMode == RUN_MODE_FIND_GN_OBJ) {
      if(argc < 4) {
        runMode = RUN_MODE_UNDEF;
      }
    }
  }
  
  /* Find the objects in the given scene */
  if(runMode == RUN_MODE_SEPERATE_OBJS) {
    /* Open the given scene BMP file  */
    err = openBmpFile(&argv[3][0], &imgHdr, &imageData[0]);
    if(err  == 0) {
      /* Convert the read RGB file into gray scale and put it in a Matrix */
      PMAT gray = convertRGBToGrayScale(imageData, imgHdr.imageLength,
                                                   imgHdr.imageWidth,
                                                   imgHdr.imageHeight);
      PMAT oriImg = convertRGBToMatrix(imageData, imgHdr.imageLength,
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
      PMAT * opObjs = markObjsInImage(oriImg, opImage, numObjs); destroyMatrix(opImage, "opImage"); 

      //dumpMatrixToFile(opImage, "image.txt", "%05.0f ", "      ");

      for(int i = 0; ((i <= 256) && (opObjs[i] != 0)); i++) {
        sprintf(&opFileName[0], "./output_%02d_%03d.bmp", runSeg, i);
        writeAsBMP(&opFileName[0], opObjs[i], &imageData[0]);
        destroyMatrix(opObjs[i], "");
      }

      destroyMatrix(oriImg, "");
      
#if ((defined _WIN32) || (defined _WIN64) || (defined CYGWIN))
      system("cygstart ./output_000.bmp");
#endif
    } else {
      printf(" File couldn't be opened %s, err:%d\n", &argv[2][0], err);
    } 
  }else if(runMode == RUN_MODE_FIND_GN_OBJ) {
    int scaleFactor = 4;
    /* Search for the given object in the given scene */
    /* Open the given scene and filter BMP files  */
    err = openBmpFile(&argv[3][0], &imgHdr, &imageData[0]);
    if(err == 0) {
       err = openBmpFile(&argv[4][0], &filterHdr, &filterData[0]);
    }
    if(err == 0) {
      /* Convert the read RGB scene file into gray scale and put it in a Matrix */
      PMAT scene = convertRGBToGrayScale(imageData, imgHdr.imageLength,
                                                    imgHdr.imageWidth,
                                                    imgHdr.imageHeight);
      PMAT oriImg = convertRGBToMatrix(imageData, imgHdr.imageLength,
                                                  imgHdr.imageWidth,
                                                  imgHdr.imageHeight);
      /* Convert the read RGB filter file into gray scale and put it in a Matrix */
      PMAT filter = convertRGBToGrayScale(filterData, filterHdr.imageLength,
                                                      filterHdr.imageWidth,
                                                      filterHdr.imageHeight);
      
      PMAT scaledScene = scaleDown(scene, scaleFactor, scaleFactor);destroyMatrix(scene, "scene");
      PMAT scaledfilter = scaleDown(filter, scaleFactor, scaleFactor);destroyMatrix(filter, "filter");
      
      normalizeImage(scaledScene);
      normalizeImage(scaledfilter);
      
      PMAT result = imfilter(scaledScene, scaledfilter, CORRELATION_OPERATION, MODE_PAD_ZERO);
      normalizeMatrix(result, -0.5, 0.5, 0, 255);
      
      float maxVal = 0;
      POS matchPos = getMaxPos(result, &maxVal);
      printf("Given image section found at x:%d, y:%d\n", matchPos.x, matchPos.y);
      
      matchPos.x *= scaleFactor;
      matchPos.y *= scaleFactor;
      
      int width = scaledfilter->numberOfColumns * scaleFactor;
      int height = scaledfilter->numberOfRows * scaleFactor;
      
      PMAT markedImg = markBoundaryInImage(oriImg, maxVal, matchPos, width, height);
      sprintf(&opFileName[0], "./markedImg_%02d.bmp", runSeg);
      writeAsBMP(&opFileName[0], markedImg, &imageData[0]);
      
      destroyMatrix(scaledScene, "scene");
      destroyMatrix(scaledfilter, "filter");
      destroyMatrix(result, "result");
      destroyMatrix(markedImg, "");
            
#if ((defined _WIN32) || (defined _WIN64) || (defined CYGWIN))
      system("cygstart ./markedImg.bmp");
#endif
    } else {
      printf("File open failed: %d for files %s and %s\n", err, &argv[3][0], &argv[4][0]);
    }
  } else if(runMode == RUN_MODE_DO_TESTS) {
    /* Do test operations */
    doTest();
  } else {
    printf("Not enough parameters \n");
  }

  if(silentRun == 0) {
    printf("\n Enter any key to close\n ");
    getchar();
  }
  
	return 0;
}







