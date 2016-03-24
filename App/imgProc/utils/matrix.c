/*
 * matrix.c
 *
 *  Created on: 31-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#include "matrix.h"
#include "../fileOps/bmp.h"

extern uint8 silentRun;
extern uint8 showImages;
extern uint8 imageData[];

// Initializes the given matrix with given number of rows,columns
// And allocates memory for the size of the matrix
PMAT createMatrix(uint32 numberOfRows, uint32 numberOfColumns)
{
  return createMatrixGnDataType(numberOfRows, numberOfColumns, MAT_FLOAT);
}

PMAT createMatrixGnDataType(uint32 numberOfRows, uint32 numberOfColumns, uint8 dataType)
{
	PMAT mat;
  int size = 0;

	mat = malloc(sizeof(struct matrix));

	mat->numberOfRows = numberOfRows;
	mat->numberOfColumns = numberOfColumns;
  mat->dataType = dataType;
  
  if(dataType == MAT_FLOAT) {
    size = sizeof(float);
  } else if(dataType == MAT_BINARY) {
    size = sizeof(uint8);
  } else if(dataType == MAT_UINT32) {
    size = sizeof(uint32);
  } else {
    assert(0);
  }

  mat->valuesSize = numberOfRows * numberOfColumns * size;
  mat->values = malloc(mat->valuesSize);
  memset(mat->values, 0, mat->valuesSize);

	return mat;
}

PMAT createMatrixFromGn(PMAT image)
{
  PMAT mat;

	mat = malloc(sizeof(struct matrix));

	mat->numberOfRows = image->numberOfRows;
	mat->numberOfColumns = image->numberOfColumns;
  mat->dataType = image->dataType;
  mat->valuesSize = image->valuesSize;

	mat->values = malloc(mat->valuesSize);
  memcpy(mat->values, image->values, mat->valuesSize);

	return mat;
}

// Destroys the matrix and free the allocated spaces
void destroyMatrix(PMAT mat, char* name) {
	
  if((showImages > 0) && (name[0] != 0)) {
    char fname[128] = {0};
    strcat(&fname[0], "./temp/");
    strcat(&fname[0], name);
    strcat(&fname[0], ".bmp");
    writeAsBMP(fname, mat, &imageData[0]);
    
#if ((defined _WIN32) || (defined _WIN64) || (defined CYGWIN))
    char cmd[256] = {0};
    strcat(&cmd[0], "cygstart ./temp/");
    strcat(&cmd[0], name);
    strcat(&cmd[0], ".bmp");
    system(cmd);
#endif
  }
  
  mat->numberOfRows = 0;
	mat->numberOfColumns = 0;

	free(mat->values);
	free(mat);
}

void printMatrix(PMAT mat,char* desc) 
{
  printMatrixOverride(mat, desc, 0);
}

// Prints the matrix
void printMatrixOverride(PMAT mat,char* desc,int override) 
{
	uint32 i;
	uint32 j;
  
  if((silentRun == 0) || (override > 0))
  {
    printf("\n --------------------------------------- \n");
    printf("\t%s",desc);
    printf("\n --------------------------------------- \n");
    for (i = 0; i < mat->numberOfRows; ++i) {
      for (j = 0; j < mat->numberOfColumns; ++j) {
        if(mat->dataType == MAT_FLOAT) {
          printf("%10.3f ", MAT(mat, i, j));
        } else if(mat->dataType == MAT_BINARY) {
          printf("%03d ", MATB(mat, i, j));
        } else {
          assert(0);
        }
      }
      printf("\n");
    }
  }
}

// Dump matrix to file
void dumpMatrixToFile(PMAT mat, char* fileName, char* pattern, char* empty) 
{
  FILE* fp = fopen(fileName, "w");
  for (int i = 0; i < mat->numberOfRows; ++i) {
		for (int j = 0; j < mat->numberOfColumns; ++j) {
      if(mat->dataType == MAT_FLOAT) {
        if(MAT(mat, i, j) == 0) {
          fprintf(fp, empty);
        } else {
          fprintf(fp, pattern, MAT(mat, i, j));
        }
      } else if(mat->dataType == MAT_BINARY) {
        if(MATB(mat, i, j) == 0) {
          fprintf(fp, empty);
        } else {
          fprintf(fp, pattern, MATB(mat, i, j));
        }
      } else {
        assert(0);
      }
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}

// utility for subtracting 2 matrices
PMAT subtractMatrices(PMAT mat1, PMAT mat2) {
	assert(mat1->numberOfRows == mat2->numberOfRows);
	assert(mat1->numberOfColumns == mat2->numberOfColumns);

	PMAT output = createMatrix(mat1->numberOfRows,
			mat1->numberOfColumns);

	for (int i = 0; i < mat1->numberOfRows; ++i) {
		for (int j = 0; j < mat1->numberOfColumns; ++j) {
			MAT(output,i,j) = MAT(mat1,i,j) - MAT(mat2, i, j);
		}

	}

	return output;
}

void ScalarMatrixDivision(PMAT mat, float divisor) {
	for (int i = 0; i < mat->numberOfRows; ++i) {
			for (int j = 0; j < mat->numberOfColumns; ++j) {
				MAT(mat,i,j) = MAT(mat,i,j)/divisor;
			}
	}
}

void ScalarMatrixMultiplication(PMAT mat, float multiplier){
	for (int i = 0; i < mat->numberOfRows; ++i) {
				for (int j = 0; j < mat->numberOfColumns; ++j) {
					MAT(mat,i,j) = MAT(mat,i,j)*multiplier;
				}
		}
}

void normalizeMatrix(PMAT mat, float oldMinVal, float oldMaxVal, float newMinVal, float newMaxVal)
{
  for (int i = 0; i < mat->numberOfRows; ++i) {
    for (int j = 0; j < mat->numberOfColumns; ++j) {
      MAT(mat, i, j) = (((newMaxVal - newMinVal) * (MAT(mat, i, j) - oldMinVal)) / (oldMaxVal - oldMinVal)) + newMinVal;
    }
  }
}

void initMatrix(PMAT mat, void* values) {
	for (int i = 0; i < mat->numberOfRows; ++i) {
		for (int j = 0; j < mat->numberOfColumns; ++j) {
      if(mat->dataType == MAT_FLOAT) {
        MAT(mat,i,j) = ((float*) values)[j + (i * mat->numberOfColumns)];
      } else if(mat->dataType == MAT_BINARY) {
        MATB(mat,i,j) = ((uint8*) values)[j + (i * mat->numberOfColumns)];
      } else {
        assert(0);
      }
		}
	}
}

// Convert the grayscale to buffer for writing the image in the output file
uint32 convertMatrixToBuffer(PMAT input, uint8* outputBuffer) {
	uint32 bufferSize = 0;
	for (int i = 0; i < input->numberOfRows; ++i) {
    int numColBytes = 0;
		for (int j = 0; j < input->numberOfColumns; ++j) {
      if(input->dataType == MAT_FLOAT) {
        outputBuffer[bufferSize] = (uint8) MAT(input, i, j);
        outputBuffer[bufferSize+1] = (uint8) MAT(input, i, j);
        outputBuffer[bufferSize+2] = (uint8) MAT(input, i, j);
      } else if(input->dataType == MAT_BINARY) {
        outputBuffer[bufferSize] = (uint8) MATB(input, i, j)*255;
        outputBuffer[bufferSize+1] = (uint8) MATB(input, i, j)*255;
        outputBuffer[bufferSize+2] = (uint8) MATB(input, i, j)*255;
      } else if(input->dataType == MAT_UINT32) {
        uint32 rgbVal = MATU32(input, i, j);
        outputBuffer[bufferSize+2] = (uint8) (rgbVal & 0x000000ff);   rgbVal = rgbVal >> 8;
        outputBuffer[bufferSize+1] = (uint8) (rgbVal & 0x000000ff); rgbVal = rgbVal >> 8;
        outputBuffer[bufferSize] = (uint8) (rgbVal & 0x000000ff);
      } else {
        assert(0);
      }
			
			bufferSize+=3;
      numColBytes+=3;
		}
    
    while((numColBytes%4) != 0) {
      outputBuffer[bufferSize] = 0;
      bufferSize++; numColBytes++;
    }
	}
	return bufferSize;
}

PMAT rotateLeft(PMAT input)
{
  PMAT rotatedMat = createMatrixGnDataType(input->numberOfColumns, input->numberOfRows, input->dataType);
  
  for (int i = 0; i < input->numberOfRows; ++i) {
		for (int j = 0; j < input->numberOfColumns; ++j) {
      if(input->dataType == MAT_FLOAT) {
        MAT(rotatedMat, ((rotatedMat->numberOfRows - 1) - j), i) = MAT(input, i, j);
      } else if(input->dataType == MAT_BINARY) {
        MATB(rotatedMat, ((rotatedMat->numberOfRows - 1) - j), i) = MATB(input, i, j);
      } else {
        assert(0);
      }
    }
  }
  
  return rotatedMat;
}

POS getMaxPos(PMAT img, float* opMaxVal)
{
  POS maxPos;
  float maxVal = 0;
  
  maxPos.x = 0;  
  maxPos.y = 0;

  for (int i = 0; i < img->numberOfRows; ++i) {
		for (int j = 0; j < img->numberOfColumns; ++j) {
      if(MAT(img, i, j) > maxVal) {
        maxVal = MAT(img, i, j);
        maxPos.x = i;
        maxPos.y = j;
      }
    }
  }
  
  printf("getMaxPos max (%f) at pos: %d,%d \n", maxVal, maxPos.y, maxPos.x);
  *opMaxVal = maxVal;
  
  return maxPos;
}
