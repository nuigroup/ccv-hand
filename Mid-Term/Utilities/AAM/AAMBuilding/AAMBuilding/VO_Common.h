/****************************************************************************
* File Name:        VO_Triangle2D.h                                         *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* create Date:      2008-03-04                                              *
* Main Content:     Common files                                            *
* Used Library:     STL + OpenCV + Boost                                    *
****************************************************************************/

#ifndef __VO_Common__
#define __VO_Common__

#include <climits>
#include <vector>
#include <iostream>

#include "cv.h"
#include "highgui.h"

#define AAM_BASIC									1
#define AAM_DIRECT									2
#define AAM_CMUINVERSECOMPOSITIONAL					3
#define AAM_ADDITIVECOMPOSITIONAL					4
#define AAM_ADDITIVECOMPOSITIONALWITHLOCKING		5
#define AAM_ADDITIVECOMPOSITIONALWITHSNAKE			6
#define AAM_ADDITIVECOMPOSITIONALWITHCOMPONENT		7
#define AAM_3D										8

// For generic dataset with lots of persons, < 0
#define IMM											-1
#define XM2VTS										-2
#define CALTECH										-3
#define BIOID										-4
// For person specific dataset with only 1 person, > 0
#define FRANCK							            1
// Otherwise, no dataset is used, not training at all, == 0

/** For detected face or tracking face constrain */
#define FACESMALLESTSIZE							80		// for both detection and tracking
#define FACEBIGGESTSIZE								480		// for both detection and tracking

#define AAMADABOOSTDOWNPERCENTAGE					0.2

#define GRAYCHANNELS								1
#define COLORCHANNELS								3		// R G B 3 channels

#define AVERAGEFACETEXTURE							128		// must be between 0-255, here, we choose 128

#define DIRECTMAP									1
#define LINEARIZEMAP								2

/** For texture extraction */
#define DIRECTTEXTURE								1
#define LAPLACETEXTURE								2
#define HARRISCORNERTEXTURE							3
#define HISTOGRAMEQUALIZED							4
#define GABOR										5
#define SEGMENTATION								6

/** For face detection */
#define HAARCASCADE         						0
#define TEMPLATE            						1
#define WAVELETTEMPLATE     						2


#define LOCKEDQUALIFICATION							0.99

#define LEFTEYEBROW									1
#define	RIGHTEYEBROW								2
#define LEFTEYE								     	3
#define	RIGHTEYE								    4
#define NOSE										5
#define	MOUTH										6
#define CHIN    									7
#define	LEFTEAR								        8
#define RIGHTEAR								    9
#define	LEFTIRIS								    10
#define RIGHTIRIS									11
#define CHEEK                                       12

#define MEAN0NORM1                                  1
#define VARY01                                      2

/** For general iteration */
#define EPOCH                                       30

static CvScalar colors[] = 
{
    {{0,0,255}},
    {{0,128,255}},
    {{0,255,255}},
    {{0,255,0}},
    {{255,128,0}},
    {{255,255,0}},
    {{255,0,0}},
    {{255,0,255}},
	{{255,255,255}}
};


/** static template function to judge whether the vector contains a value */
template <class T>
static bool vectorcontains(std::vector<T> v, T t)
{
	for(unsigned int i = 0; i < v.size(); i++)
	{
		if (v[i] == t)
			return true;
	}

	return false;
}

/** static template function to judge whether the vector contains another vector */
template <class T>
static bool vectorcontains(std::vector<T> v, std::vector<T> t)
{
	for(unsigned int i = 0; i < t.size(); i++)
	{
		if(!vectorcontains(v, t[i]))
			return false;
	}

	return true;
}

/** static template function to truncate an image with upper and lower limits */
template <class T>
static bool VO_cvTruncate(IplImage* iImg, T vLower, T vUpper)
{
    for( int i = 0; i < iImg->height; i++)
    {
        for (int j = 0; j < iImg->width; j++)
        {
            if ( CV_IMAGE_ELEM(iImg, T, i, j) < vLower)
            {
                CV_IMAGE_ELEM(iImg, T, i, j) = vLower;
            }
            else if ( CV_IMAGE_ELEM(iImg, T, i, j) > vUpper)
            {
                CV_IMAGE_ELEM(iImg, T, i, j) = vUpper;
            }
        }
    }        

    return true;
}

class VO_Common
{
public:
    static void     VO_CalcSamplesSDWithKnownMean(const CvMat* iSamples, const CvMat* mSampes, CvMat* sdSamples);

    static void     VO_CalcSamplesMeanNSD(const CvMat* iSamples, CvMat* mSampes, CvMat* sdSamples);

    static void     VO_ModelDeltaParamConstrain(CvMat* inputDeltaParam, const CvMat* ParamSD);

    static void     VO_PoseDeltaParamConstrain(CvMat* inputDeltaParam);

    static void     VO_RangeConstrain(CvMat* inputShape, const CvSize& sizeConstrain);
};


#endif  // __VO_Common__

