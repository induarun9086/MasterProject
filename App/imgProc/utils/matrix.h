/*
 * matrix.h
 *
 *  Created on: 31-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#ifndef UTILS_MATRIX_H_
#define UTILS_MATRIX_H_

#include "misc.h"

#define MAT(a, i, j)    (((float*) ((a)->values))[j + (i * (a)->numberOfColumns)])
#define MATB(a, i, j)   (((uint8*) ((a)->values))[j + (i * (a)->numberOfColumns)])
#define MATU8(a, i, j)  (((uint8*) ((a)->values))[j + (i * (a)->numberOfColumns)])
#define MATS8(a, i, j)  (((sint8*) ((a)->values))[j + (i * (a)->numberOfColumns)])
#define MATU16(a, i, j) (((uint16*)((a)->values))[j + (i * (a)->numberOfColumns)])
#define MATS16(a, i, j) (((sint16*)((a)->values))[j + (i * (a)->numberOfColumns)])
#define MATU32(a, i, j) (((uint32*)((a)->values))[j + (i * (a)->numberOfColumns)])
#define MATS32(a, i, j) (((sint32*)((a)->values))[j + (i * (a)->numberOfColumns)])

#define PMAT struct matrix*

#define MAT_FLOAT  0
#define MAT_BINARY 1
#define MAT_UINT8  2
#define MAT_SINT8  3
#define MAT_UINT16 4
#define MAT_SINT16 5
#define MAT_UINT32 6
#define MAT_SINT32 7

struct matrix{
	uint32 numberOfRows;
	uint32 numberOfColumns;
        uint32 valuesSize;
        uint8  dataType;
	void* values;
};

PMAT createMatrix(uint32 numberOfRows, uint32 numberOfColumns);
PMAT createMatrixGnDataType(uint32 numberOfRows, uint32 numberOfColumns, uint8 dataType);
PMAT createMatrixFromGn(PMAT image);

void destroyMatrix(PMAT mat, char* name);

void printMatrix(PMAT mat, char* desc);
void printMatrixOverride(PMAT mat,char* desc,int override) ;
void dumpMatrixToFile(PMAT mat, char* fileName, char* pattern, char* empty);

PMAT subtractMatrices(PMAT mat1, PMAT mat2);

void ScalarMatrixDivision(PMAT mat, float divisor);

void ScalarMatrixMultiplication(PMAT mat, float multiplier);

void normalizeMatrix(PMAT mat, float oldMinVal, float oldMaxVal, float newMinVal, float newMaxVal);

void initMatrix(PMAT mat, void* values);

uint32 convertMatrixToBuffer(PMAT input, uint8* outputBuffer);

PMAT rotateLeft(PMAT input);

POS getMaxPos(PMAT img, float* opMaxVal);

#endif /* UTILS_MATRIX_H_ */
