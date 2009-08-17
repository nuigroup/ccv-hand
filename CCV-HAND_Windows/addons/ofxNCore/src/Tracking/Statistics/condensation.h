#ifndef _CONDENSATION_H
#define _CONDENSATION_H


#include "useful.h"
#include "Coord.h"
#include "SampleStat.h"

CvConDensation* initCondensation ( CvMat** indexMat, int nSample, int maxWidth, int maxHeight );
coord updateCondensation ( CvConDensation* ConDens, coord Measurement, float * stdDX_ptr, float * stdDY_ptr);
void updateProcessProbDens ( CvConDensation* ConDens, coord Measurement, float * stdDX_ptr, float * stdDY_ptr);
#endif
