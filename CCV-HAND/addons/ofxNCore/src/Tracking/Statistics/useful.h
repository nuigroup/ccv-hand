#ifndef  _USEFUL_H
#define  _USEFUL_H


#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>
#include "cv.h"
#include "cxcore.h"


#include <vector>
#include <math.h>
#include <iostream>


#include "Blob.h"
#include "BlobResult.h"
using namespace std;

#define NUMBER_OF_MATRIX 6
//#define PI 3.14159265

struct coordinate {
	int Minx,Maxx,Maxy,Miny;
	int cX;
	int cY;
	bool flag;
};

struct matrixDesc{
		int nCols;
		int nRows;
};

#endif
