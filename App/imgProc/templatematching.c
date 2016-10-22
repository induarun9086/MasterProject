/*
 * templatematching.c
 *
 *  Created on: 29-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#include "templatematching.h"

#ifdef PC_SIMU
#define OP_PATH_PREFIX "./temp/"
#else
#define OP_PATH_PREFIX "/home/pi/App/"
#endif

uint8 imageData[TEMPLATEMATCHING_IMAGE_LENGTH];
uint8 filterData[TEMPLATEMATCHING_IMAGE_LENGTH];

uint8 silentRun  = 1;
uint8 verboseLvl = 0;

#ifdef PC_SIMU
uint8 showImages = 1;
#else
uint8 showImages = 0;
#endif

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
    
    if((runMode == RUN_MODE_SEPERATE_OBJS) ||
       (runMode == RUN_MODE_MARK_OBJS)) {
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
  if((runMode == RUN_MODE_SEPERATE_OBJS) ||
     (runMode == RUN_MODE_MARK_OBJS)) {
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
      
      /* Extract the edge */
      PMAT edgeImg = sobelEdgeFilter(gray); destroyMatrix(gray, "gray");
      /* Blur the image */
      PMAT blurImg = imblur(edgeImg);  destroyMatrix(edgeImg, "edgeImg");
      /* Find the threshold pixel value */
      float threshold = quantile(blurImg, 0.21);
      printf("Threshold = %f\n", threshold);
      /* Binarize the image based on the threshold */
      PMAT binImage = binarizeImage(blurImg, threshold); destroyMatrix(blurImg, "blurImg");
      /* thicken the binary image */
      thickImage(binImage);
      /* Separate the objects in the image */
      PMAT opImage = seperateObjsFromImage(binImage, &numObjs); destroyMatrix(binImage, "thickbin");
      /* Mark boundaries around the separated images */
      PMAT * opObjs = markObjsInImage(oriImg, opImage, numObjs); destroyMatrix(opImage, "opImage"); 

      if(runMode == RUN_MODE_MARK_OBJS) {
        sprintf(&opFileName[0], OP_PATH_PREFIX"output_%02d.bmp", runSeg);
        writeAsBMP(&opFileName[0], opObjs[0], &imageData[0]);
        for(int i = 0; ((i <= 256) && (opObjs[i] != 0)); i++) {
          destroyMatrix(opObjs[i], "");
        }
      } else {
        /* Extract the individual objects and write as separate files */
        for(int i = 0; ((i <= 256) && (opObjs[i] != 0)); i++) {
          sprintf(&opFileName[0], OP_PATH_PREFIX"output_%02d_%03d.bmp", runSeg, i);
          writeAsBMP(&opFileName[0], opObjs[i], &imageData[0]);
          destroyMatrix(opObjs[i], "");
        }
      }

      destroyMatrix(oriImg, "");
      
#if ((defined _WIN32) || (defined _WIN64) || (defined CYGWIN))
      system("cygstart "OP_PATH_PREFIX"output_000.bmp");
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
      
      /* Scale down the input image and filter image to speed up processing */
      PMAT scaledScene  = scaleDown(scene, scaleFactor, scaleFactor);destroyMatrix(scene, "scene");
      PMAT scaledfilter = scaleDown(filter, scaleFactor, scaleFactor);destroyMatrix(filter, "filter");
      
      /* Normalize the image and filter */
      normalizeImage(scaledScene);
      normalizeImage(scaledfilter);
      
      /* Correlate the image with filter and normalize the image */
      PMAT result = imfilter(scaledScene, scaledfilter, CORRELATION_OPERATION, MODE_PAD_ZERO);
      normalizeMatrix(result, -0.5, 0.5, 0, 255);
      
      /* Get the maximum of the correlation */
      float maxVal = 0;
      POS matchPos = getMaxPos(result, &maxVal);
      printf("Given image section found at x:%d, y:%d\n", matchPos.x, matchPos.y);
      
      /* Scale up the found image position */
      matchPos.x *= scaleFactor;
      matchPos.y *= scaleFactor;
      
      /* Find the original size of the filter */
      int width  = scaledfilter->numberOfColumns * scaleFactor;
      int height = scaledfilter->numberOfRows * scaleFactor;
      
      /* Mark the found boundary in the original image */
      PMAT markedImg = markBoundaryInImage(oriImg, maxVal, matchPos, width, height);
      sprintf(&opFileName[0], OP_PATH_PREFIX"markedImg_%02d.bmp", runSeg);
      writeAsBMP(&opFileName[0], markedImg, &imageData[0]);
      
      destroyMatrix(scaledScene, "scene");
      destroyMatrix(scaledfilter, "filter");
      destroyMatrix(result, "result");
      destroyMatrix(markedImg, "");
            
#if ((defined _WIN32) || (defined _WIN64) || (defined CYGWIN))
      system("cygstart "OP_PATH_PREFIX"markedImg.bmp");
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







