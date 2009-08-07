#ifndef  _EXTRACTBLOB_H
#define  _EXTRACTBLOB_H


#include "Blob.h"
#include "BlobResult.h"
#include "useful.h"
#include "drawBob.h"


coord extractBlob(CBlobResult blobs, coord selectedCoord);

void drawInitialBlobs(IplImage * tmp_frame, CBlobResult blobs);

CBlob getNearestBlob(CBlobResult blobs, coord coordinate);

CBlobResult getBlobs2(IplImage* tmp_frame, IplImage* binBack);

float getDistance (double X1, double X2, double Y1, double Y2);

#endif
