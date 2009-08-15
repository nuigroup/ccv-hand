#include "drawBob.h"
#include "BlobResult.h"


void drawBlob2 (IplImage * image, coord CcB, int R, int G, int B){

	cvLine( image,  cvPoint( (int)CcB.cX, (int) CcB.cY),  cvPoint( (int) CcB.cX,  (int)CcB.cY), CV_RGB(R, G , B), 4, 8, 0 );

	cvRectangle( image, cvPoint(CcB.MinX , CcB.MinY ), cvPoint ( CcB.MaxX, CcB.MaxY ), CV_RGB(R, G , B), 1, 8, 0);

}
