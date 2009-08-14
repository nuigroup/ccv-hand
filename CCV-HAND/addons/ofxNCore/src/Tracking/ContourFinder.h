/*
* ContourFinder.h
*
* Finds white blobs in binary images and identifies
* centroid, bounding box, area, length and polygonal contour
* The result is placed in a vector of Blob objects.
*
* Created on 2/2/09.
* Adapted from openframeworks ofxCvContourFinder
*
*/

#ifndef CONTOUR_FINDER_H
#define CONTOUR_FINDER_H

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "Blob.h"

#define TOUCH_MAX_CONTOURS 128
#define TOUCH_MAX_CONTOUR_LENGTH 1024

class ContourFinder {

  public:

    ContourFinder();
    ~ContourFinder();
	
	int findContours( ofxCvGrayscaleImage& input,
                       int minArea, int maxArea,
                       int nConsidered, bool bFindHoles,
                       bool bUseApproximation = true);
                       // approximation = don't do points for all points of the contour, if the contour runs
                       // along a straight line, for example...
		
    int                        nBlobs;     // how many did we find
    vector <Blob>	   blobs;      // the blobs, in a std::vector...
    Blob 	           getBlob(int num);

  protected:

    // this is stuff, not for general public to touch -- we need
    // this to do the blob detection, etc.
    ofxCvGrayscaleImage inputCopy;
    CvMemStorage*       contour_storage;
    CvMemStorage*       storage;
    CvMoments*          myMoments;

    // internally, we find cvSeqs, they will become blobs.
    int                 nCvSeqsFound;
    CvSeq*              cvSeqBlobs[TOUCH_MAX_CONTOURS];

    // imporant!!
    void                reset();

};



#endif
