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


#include <iostream>
#include "AAM_MovieAVI.h"

//============================================================================
void AAM_MovieAVI::Open(const char* videofile)
{
	capture = cvCaptureFromAVI(videofile);
	if(!capture)
	{
		fprintf(stderr, "could not open video file %s!\n", videofile);
		exit(0);
	}
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0);
	capimg = cvQueryFrame( capture );
	image = cvCreateImage(cvGetSize(capimg), capimg->depth, capimg->nChannels);
}

//============================================================================
void AAM_MovieAVI::Close()
{
	cvReleaseCapture(&capture);
	capture = 0;
	cvReleaseImage(&image);
	image = 0;
}

//============================================================================
IplImage* AAM_MovieAVI:: ReadFrame(int frame_no )
{
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, frame_no);
	capimg = cvQueryFrame( capture );

	if(capimg->origin == 0)
		cvCopy(capimg, image);
	else
		cvFlip(capimg, image);

	return image;
}
