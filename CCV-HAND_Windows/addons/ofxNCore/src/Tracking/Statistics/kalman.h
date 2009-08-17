#ifndef _KALMAN_H
#define _KALMAN_H


#include "useful.h"
#include "Coord.h"


CvKalman* initKalman(CvMat** , coord );

void copyMat (CvMat* , CvMat* );

float* updateKalman(CvKalman *, coord );

float* updateKalman(CvKalman *);


#endif
