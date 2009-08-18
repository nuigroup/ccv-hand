/****************************************************************************
*						AAMLibrary
*			http://code.google.com/p/aam-library
* Copyright (c) 2008-2009 by GreatYao, all rights reserved.
*****************************************************************************
** Modified by: Thiago de Freitas Oliveira Araújo
*  NUI Group Community Core Vision Hand Tracking
*  Google Summer of Code 2009
*  Student: Thiago de Freitas Oliveira Araújo
*  Mentor: Laurence Muller
*
*  Copyright 2009 NUI Group/Inc. All rights reserved.
*
*   License GPL v2.0
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer as
* the first lines of this file unmodified.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY NUI GROUP ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL BEN WOODHOUSE BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

* Web: http://nuigroup.com
*      http://ccv.nuigroup.com/
*      http://thiagodefreitas.wordpress.com
*      http://www.multigesture.net
*
* Still under development...
***************************************************************************/

#include "VJfacedetect.h"

#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "highgui.h"


using namespace std;


    static CvScalar colors[] =
     {
         {{0,0,255}},
         {{0,128,255}},
         {{0,255,255}},
         {{0,255,0}},
        {{255,128,0}},
         {{255,255,0}},
         {{255,0,0}},
         {{255,0,255}}
     };

//============================================================================
VJfacedetect::VJfacedetect()
{
	__cascade = 0;
	__storage = 0;
}

//============================================================================
VJfacedetect::~VJfacedetect()
{
	cvReleaseMemStorage(&__storage);
	cvReleaseHaarClassifierCascade(&__cascade);
}

//============================================================================
void VJfacedetect::LoadCascade(const char* cascade_name /* = "haarcascade_frontalface_alt2.xml" */)
{
	__cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0);
	if(__cascade == 0)
	{
		printf("ERROR(%s, %d): Can't load cascade file!\n", __FILE__, __LINE__);
		exit(0);
	}
	__storage = cvCreateMemStorage(0);
}

//============================================================================
bool VJfacedetect::DetectFace(std::vector<AAM_Shape> &Shape, const IplImage* image)
{
	IplImage* small_image = cvCreateImage
		(cvSize(image->width/2, image->height/2), image->depth, image->nChannels);
	cvPyrDown(image, small_image, CV_GAUSSIAN_5x5);

	CvSeq* pFaces = cvHaarDetectObjects(small_image, __cascade, __storage,
		1.1, 3, CV_HAAR_DO_CANNY_PRUNING);

	cvReleaseImage(&small_image);

    printf("Found quantity %d\n", pFaces->total);
	if(0 == pFaces->total)//can't find a face
		return false;

	Shape.resize(pFaces->total);
	for (int i = 0; i < pFaces->total; i++)
    {
		Shape[i].resize(2);
		CvRect* r = (CvRect*)cvGetSeqElem(pFaces, i);

		CvPoint pt1, pt2;
		pt1.x = r->x * 2;
		pt2.x = (r->x + r->width) * 2;
		pt1.y = r->y * 2;
		pt2.y = (r->y + r->height) * 2;

		Shape[i][0].x  = r->x*2.0;
		Shape[i][0].y  = r->y*2.0;
		Shape[i][1].x  = Shape[i][0].x + 2.0*r->width;
		Shape[i][1].y  = Shape[i][0].y + 2.0*r->height;
    }
	return true;
}
//
//============================================================================
bool VJfacedetect::DetectFace2(IplImage* image)
{
	IplImage* small_image = cvCreateImage
		(cvSize(image->width/2, image->height/2), image->depth, image->nChannels);
	cvPyrDown(image, small_image, CV_GAUSSIAN_5x5);

	CvSeq* pFaces = cvHaarDetectObjects(small_image, __cascade, __storage,
		1.1, 3, CV_HAAR_DO_CANNY_PRUNING);

	cvReleaseImage(&small_image);

    printf("Found quantity %d\n", pFaces->total);
	if(0 == pFaces->total)//can't find a face
		return false;


	for (int i = 0; i < pFaces->total; i++)
    {

		CvRect* r = (CvRect*)cvGetSeqElem(pFaces, i);

        CvPoint center;
        CvScalar color = colors[i%8];
        int radius;
        center.x = cvRound((r->x + r->width*0.5));
        center.y = cvRound((r->y + r->height*0.5));
        radius = cvRound((r->width + r->height)*0.25);
        cvCircle( image, center, radius, color, 3, 8, 0 );

    }
	return true;
}
