/****************************************************************************
* File Name:        VO_AAM.cpp                                              *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM Model, shape + texture                              *
* Used Library:     STL + OpenCV + Boost                                    *
****************************************************************************/


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <boost/filesystem.hpp>

#include "VO_AAM.h"

using namespace std;

/** Default Constructor */
VO_AAM::VO_AAM()
{
    this->init();
}


/** Initialization */
void VO_AAM::init()
{
    this->m_CVMTextures											= NULL;
    this->m_CVMNormalizedTextures								= NULL;
    this->m_CVMNormalizedTexturesProject2Truncated				= NULL;
    this->m_CVMTruancatedNormalizedTexturesMean					= NULL;
    this->m_CVMTruancatedNormalizedTexturesSD					= NULL;
    this->m_CVMNormalizedTexturesMean							= NULL;
    this->m_CVMNormalizedTexturesEigenVectors					= NULL;
    this->m_CVMNormalizedTexturesEigenValues					= NULL;
    this->m_CVMTruncatedNormalizedTexturesEigenVectors			= NULL;
    this->m_CVMTruncatedNormalizedTexturesEigenValues			= NULL;
    this->m_iTextureExtractionMethod                            = DIRECTTEXTURE;
    this->m_iNbOfChannels										= COLORCHANNELS;
    this->m_iNbOfPixels											= 0;
    this->m_iNbOfTextures										= 0;
    this->m_iNbOfTruncatedTextures								= 0;
    this->m_iNbOfEigenTexturesAtMost							= 0;
    this->m_fAverageTextureStandardDeviation					= 0.;
    this->m_fTruncatedPercent_Texture                           = 0.95f;
    this->m_CVSizeOfTemplateFace                                = cvSize(0, 0);
    this->m_IplTemplateFace										= NULL;
    this->m_IplEdges											= NULL;
    this->m_IplConvexHull										= NULL;
    this->m_IplConcave											= NULL;
    this->m_vIplTriangles.clear();
    this->m_vImages.clear();
}


VO_AAM::~VO_AAM()
{
    if (this->m_CVMTextures)									cvReleaseMat (&this->m_CVMTextures);
    if (this->m_CVMNormalizedTextures)							cvReleaseMat (&this->m_CVMNormalizedTextures);
    if (this->m_CVMNormalizedTexturesProject2Truncated)			cvReleaseMat (&this->m_CVMNormalizedTexturesProject2Truncated);
    if (this->m_CVMTruancatedNormalizedTexturesMean)			cvReleaseMat (&this->m_CVMTruancatedNormalizedTexturesMean);
    if (this->m_CVMTruancatedNormalizedTexturesSD)				cvReleaseMat (&this->m_CVMTruancatedNormalizedTexturesSD);
    if (this->m_CVMNormalizedTexturesMean)						cvReleaseMat (&this->m_CVMNormalizedTexturesMean);
    if (this->m_CVMNormalizedTexturesEigenVectors)				cvReleaseMat (&this->m_CVMNormalizedTexturesEigenVectors);
    if (this->m_CVMNormalizedTexturesEigenValues)				cvReleaseMat (&this->m_CVMNormalizedTexturesEigenValues);
    if (this->m_CVMTruncatedNormalizedTexturesEigenVectors)		cvReleaseMat (&this->m_CVMTruncatedNormalizedTexturesEigenVectors);
    if (this->m_CVMTruncatedNormalizedTexturesEigenValues)		cvReleaseMat (&this->m_CVMTruncatedNormalizedTexturesEigenValues);
    if (this->m_IplTemplateFace)								cvReleaseImage (&this->m_IplTemplateFace);
    if (this->m_IplEdges)										cvReleaseImage (&this->m_IplEdges);
    if (this->m_IplConvexHull)									cvReleaseImage (&this->m_IplConvexHull);
    if (this->m_IplConcave)										cvReleaseImage (&this->m_IplConcave);

    for (unsigned int i = 0; i <this->m_vImages.size (); i++ )
    {
        if (this->m_vImages[i])
        {
            cvReleaseImage (&this->m_vImages[i]);
        }
    }
    this->m_vImages.clear();

}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculate point warping information

@param      triangles           Input - a vector of all triangles

@param      ch                  Input - convex hull

@param      warpInfo            Input - information of a vector of warping points

@return     unsigned int        points (actually pixels) inside the template face

@note       Obviously, triangles and ch contain duplicated information

*/
unsigned int VO_AAM::VO_CalcPointWarpingInfo(const vector <VO_AAMTriangle2D>& triangles, const CvMat* ch, 
                                             vector<VO_AAMWarpingPoint>& warpInfo)
{
    unsigned int NbOfTriangles  = triangles.size();
    unsigned int NbOfPixels     = 0;
    bool isPointInTriangle      = false;

    CvRect rect                 = cvBoundingRect( const_cast<CvMat*>(ch), 0 );
    CvMat* tempVert             = cvCreateMat (1, 3, CV_32FC2); // just for judging whether this point is in which triangle
    CvPoint2D32f pt;

    for (unsigned int i = 0; i < rect.height; i++)
    {
        for (unsigned int j = 0; j < rect.width; j++)
        {
            // JIA Pei. 2006-11-25. You will see the following (int) is very important
            // without (int), the result here is not correct at all!!
            pt.x = (float)j;
            pt.y = (float)i;

            // the convex hull contains all points inside the template face which is composed by the triangles
            if( VO_ASM::VO_IsPointInConvexHull(pt, ch, true) ) // true - including the convex hull, false - excluding the convex hull
                //if( this->VO_IsPointInTemplateFace(pt) )
            {
                isPointInTriangle = false;

                for (unsigned int k = 0; k < NbOfTriangles; k++)
                {
                    CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 0 ) = triangles[k].GetVertex(0);
                    CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 1 ) = triangles[k].GetVertex(1);
                    CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 2 ) = triangles[k].GetVertex(2);

                    // Here, we can actually give out all the local variables except the BGR, 
                    // the BGR is not shape based and will be given by function VO_LoadTexturesFromShapes()
                    if( cvPointPolygonTest( tempVert, pt, 0 ) >= 0)
                    {
                        isPointInTriangle = true;
                        vector<int> Index;
                        Index.resize(3);
                        Index = triangles[k].GetVertexIndexes();

                        VO_AAMWarpingPoint tempPixelTriangle;

                        // necessary for all methods
                        tempPixelTriangle.SetPosition(pt);
                        tempPixelTriangle.SetTriangleIndex(k);
                        tempPixelTriangle.SetPointIndex(NbOfPixels);
                        tempPixelTriangle.SetAAMTriangle2d(triangles[tempPixelTriangle.GetTriangleIndex() ] );

                        // Very important!! Note by JIA Pei, push_back cannot perform on 2-D vectors
                        warpInfo.push_back (tempPixelTriangle);

                        NbOfPixels ++;
                    }
                }

                if(!isPointInTriangle)
                {
                    CvPoint iorg = cvPointFrom32f( pt );
                }

                // This is wrong!!!
                // NbOfPixels ++;
            }
        }
    }

    cvReleaseMat (&tempVert);

    return NbOfPixels;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculate point warping information

@param      iShape                  Input - the shape

@param      img                     Input - image

@param      templateTriangles       Input - the composed face template triangles

@param      warpInfo                Input - warping information for all template points

@param      oTexture                Output - the extracted texture

@param      tbm                     Input - texture extraction method

@return     bool                    loading succeed or not?

*/
bool VO_AAM::VO_LoadOneTextureFromShape(const VO_AAMShape2D& iShape, const IplImage* img, 
                                        const vector<VO_AAMTriangle2D>& templateTriangles,
                                        const vector<VO_AAMWarpingPoint>& warpInfo, VO_AAMTexture2D3C& oTexture, int tbm)
{
    // make sure all shape points are inside the image
    if ( !VO_ASM::VO_IsShapeInsideImage(iShape, img) )
        return false;

    unsigned int NbOfPoints     = iShape.GetSize();
    unsigned int NbOfShapes     = NbOfPoints*2;
    unsigned int NbOfPixels     = warpInfo.size();
    unsigned int NbOfTextures   = NbOfPixels*img[0].nChannels;
    unsigned int NbOfTriangles  = templateTriangles.size();
    oTexture.SetSize(NbOfTextures);
    vector<float> tempTexture;
    tempTexture.resize(NbOfTextures);


    CvPoint2D32f* src = new CvPoint2D32f[3];
    CvPoint2D32f* dst = new CvPoint2D32f[3];

    vector<CvMat*> v_map_matrix;
    v_map_matrix.resize(NbOfTriangles);

    for(unsigned int i = 0; i < NbOfTriangles; i++ )
    {
        v_map_matrix[i] = cvCreateMat (2, 3, CV_32FC1);
    }

    CvMat* MappedPixel = cvCreateMat (2, 1, CV_32FC1);
    CvMat* TemplatePixel = cvCreateMat (3, 1, CV_32FC1);

    // calculate all the possible mapping (for speeding up) 95 mapping totally
    for (unsigned int j = 0; j < NbOfTriangles; j++ )
    {
        // Get the affine transformation for each triangle pair.
        src[0] = templateTriangles[j].GetVertex(0);
        src[1] = templateTriangles[j].GetVertex(1);
        src[2] = templateTriangles[j].GetVertex(2);

        dst[0] = iShape.GetPoint( templateTriangles[j].GetVertexIndex(0) ).GetXY();
        dst[1] = iShape.GetPoint( templateTriangles[j].GetVertexIndex(1) ).GetXY();
        dst[2] = iShape.GetPoint( templateTriangles[j].GetVertexIndex(2) ).GetXY();

        cvGetAffineTransform( src, dst, v_map_matrix[j] );
    }


    CvRect rect = VO_ASM::VO_CalcShapeBoundaryRect(iShape);
    rect.width +=1; rect.height +=1;    // Why +1? Make sure you have 1 pixel bigger for interpolation.

    IplImage *Img2BExtracted = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_32F, img->nChannels);
    cvZero(Img2BExtracted);

    switch(tbm)
    {
    case LAPLACETEXTURE:
        {
            IplImage *tempImg2BExtracted = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, img->nChannels);
            cvConvert( img, tempImg2BExtracted );

            // Explained by JIA Pei, 2008-03-09. 
            // Why I don't do cvCopy(tempImg2BExtracted, tempExcavated); right here, 
            // but after cvSmooth, cvLaplace, cvAbs? 
            // Because we may deal with the boundary and corners much better in this way.
            cvSmooth(tempImg2BExtracted, tempImg2BExtracted, CV_GAUSSIAN, 3 );
            cvLaplace( tempImg2BExtracted, tempImg2BExtracted, 5 );
            cvAbs(tempImg2BExtracted, tempImg2BExtracted);

            cvSetImageROI(tempImg2BExtracted, rect);
            cvCopy(tempImg2BExtracted, Img2BExtracted);
            cvResetImageROI(tempImg2BExtracted);

            cvReleaseImage(&tempImg2BExtracted);
        }
        break;
    case HARRISCORNERTEXTURE:
        {
            IplImage *imgCopy = cvCloneImage(img);
            IplImage *tempImg2BExtracted = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, Img2BExtracted->nChannels);

            // Explained by JIA Pei, 2008-03-09. 
            // Why I don't do cvCopy(tempImg2BExtracted, tempExcavated); right here, 
            // but after cvCornerHarris? 
            // Because we may deal with the boundary and corners much better in this way.
            // cvCornerHarris is for one channel

            switch (imgCopy->nChannels)
            {
            case GRAYCHANNELS:
                {
                    cvCornerHarris( imgCopy, tempImg2BExtracted, 3, 3, 0.02);
                    cvAbs(tempImg2BExtracted, tempImg2BExtracted);
                }
                break;
            case COLORCHANNELS:
            default:
                {
                    IplImage *b = cvCreateImage(cvGetSize(imgCopy), IPL_DEPTH_8U, 1);
                    IplImage *g = cvCreateImage(cvGetSize(imgCopy), IPL_DEPTH_8U, 1);
                    IplImage *r = cvCreateImage(cvGetSize(imgCopy), IPL_DEPTH_8U, 1);
                    IplImage *bHC = cvCreateImage(cvGetSize(imgCopy), IPL_DEPTH_32F, 1);
                    IplImage *gHC = cvCreateImage(cvGetSize(imgCopy), IPL_DEPTH_32F, 1);
                    IplImage *rHC = cvCreateImage(cvGetSize(imgCopy), IPL_DEPTH_32F, 1);
                    cvSplit(imgCopy, b, g, r, NULL);

                    cvCornerHarris( b, bHC, 3, 3, 0.02);
                    cvCornerHarris( g, gHC, 3, 3, 0.02);
                    cvCornerHarris( r, rHC, 3, 3, 0.02);

                    cvMerge(bHC, gHC, rHC, NULL, tempImg2BExtracted);
                    cvAbs(tempImg2BExtracted, tempImg2BExtracted);

                    cvReleaseImage(&b);
                    cvReleaseImage(&g);
                    cvReleaseImage(&r);
                    cvReleaseImage(&bHC);
                    cvReleaseImage(&gHC);
                    cvReleaseImage(&rHC);
                }            
                break;
            }

            cvSetImageROI(tempImg2BExtracted, rect);
            cvCopy(tempImg2BExtracted, Img2BExtracted);
            cvResetImageROI(tempImg2BExtracted);

            cvReleaseImage(&imgCopy);
            cvReleaseImage(&tempImg2BExtracted);            
        }
        break;
    case GABOR:	// This might be very slow
        {
        }
        break;
    case SEGMENTATION:
        {
            IplImage *imgCopy = cvCloneImage(img);
            IplImage *tempExcavated = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, Img2BExtracted->nChannels);

            cvSmooth(imgCopy, imgCopy, CV_GAUSSIAN);

            cvSetImageROI(imgCopy, rect);
            cvCopy(imgCopy, tempExcavated);
            cvResetImageROI(imgCopy);

            cvThreshold( tempExcavated, tempExcavated, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
            // cvAdaptiveThreshold( tempExcavated, tempExcavated, 255 );    // only for single channel images

            cvConvert( tempExcavated, Img2BExtracted );
            cvReleaseImage(&imgCopy);
            cvReleaseImage(&tempExcavated);
        }
        break;
    case HISTOGRAMEQUALIZED:
        {
            IplImage *imgCopy = cvCloneImage(img);
            IplImage *tempExcavated = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, Img2BExtracted->nChannels);

            // Obviously, for histogram equalization, excavate before cvEqualizeHist()
            cvSetImageROI(imgCopy, rect);
            cvCopy(imgCopy, tempExcavated);
            cvResetImageROI(imgCopy);

            switch (tempExcavated->nChannels)
            {
            case GRAYCHANNELS:
                cvEqualizeHist( tempExcavated, tempExcavated );
                break;
            case COLORCHANNELS:
            default:
                {
                    IplImage *b = cvCreateImage(cvGetSize(tempExcavated), IPL_DEPTH_8U, 1);
                    IplImage *g = cvCreateImage(cvGetSize(tempExcavated), IPL_DEPTH_8U, 1);
                    IplImage *r = cvCreateImage(cvGetSize(tempExcavated), IPL_DEPTH_8U, 1);
                    cvSplit(tempExcavated, b, g, r, NULL);

                    cvEqualizeHist( b, b );
                    cvEqualizeHist( g, g );
                    cvEqualizeHist( r, r );

                    cvMerge(b, g, r, NULL, tempExcavated);

                    cvReleaseImage(&b);
                    cvReleaseImage(&g);
                    cvReleaseImage(&r);
                }            
                break;
            }

            cvConvert( tempExcavated, Img2BExtracted );
            cvReleaseImage(&imgCopy);
            cvReleaseImage(&tempExcavated);
        }
        break;
    case DIRECTTEXTURE:
        {
            IplImage *imgCopy = cvCloneImage(img);
            IplImage *tempExcavated = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, Img2BExtracted->nChannels);

            cvSetImageROI(imgCopy, rect);
            cvCopy(imgCopy, tempExcavated);
            cvResetImageROI(imgCopy);

            cvConvert( tempExcavated, Img2BExtracted );
            cvReleaseImage(&imgCopy);
            cvReleaseImage(&tempExcavated);
        }
    default:
        break;
    }

    //IplImage* Img4Display = cvCreateImage(cvGetSize(Img2BExtracted), IPL_DEPTH_8U, Img2BExtracted->nChannels);
    //cvConvert(Img2BExtracted, Img4Display);
    //cvSaveImage("testtest.jpg", Img4Display);
    //cvReleaseImage(&Img4Display);

    // implement the real map
    for (unsigned int i = 0; i < NbOfPixels; i++ )
    {
        // warp from m_vAAMTriangle2D[j] (template pixel in the specific triangle) to each shape/image
        CV_MAT_ELEM( *TemplatePixel, float, 0, 0) = warpInfo[i].GetPosition().x;
        CV_MAT_ELEM( *TemplatePixel, float, 1, 0) = warpInfo[i].GetPosition().y;
        CV_MAT_ELEM( *TemplatePixel, float, 2, 0) = 1.0;
        cvMatMul( v_map_matrix[warpInfo[i].GetTriangleIndex() ], TemplatePixel, MappedPixel );

        // Explained by JIA Pei!!! This is quite slow!!!!!!!!!! So, we included everything here at the bottom!!
        float x = CV_MAT_ELEM( *MappedPixel, float, 0, 0) - rect.x;
        float y = CV_MAT_ELEM( *MappedPixel, float, 1, 0) - rect.y;
        register int X = (int)x;
        register int Y = (int)y;
        register float s=x-X;
        register float t=y-Y;

        for (int j = 0; j < Img2BExtracted->nChannels; j++)
        {
            if ( X > img->width || X < 0 || Y < 0 || Y > img->height)
                tempTexture[i*Img2BExtracted->nChannels + j] = 0.0f;

            else
                tempTexture[i*Img2BExtracted->nChannels + j] = 
                ( (1.0f-t) * CV_IMAGE_ELEM( Img2BExtracted, float, Y, X * Img2BExtracted->nChannels + j ) + 
                t * CV_IMAGE_ELEM( Img2BExtracted, float, Y+1, X * Img2BExtracted->nChannels + j ) ) * (1.0f-s) + 
                ( (1.0f-t) * CV_IMAGE_ELEM( Img2BExtracted, float, Y, (X+1) * Img2BExtracted->nChannels + j ) + 
                t * CV_IMAGE_ELEM( Img2BExtracted, float, Y+1, (X+1) * Img2BExtracted->nChannels + j ) ) * s;
        }

        oTexture.SetTextures(tempTexture);
    }

    // Till now it's correct.
    for(unsigned int i = 0; i < v_map_matrix.size(); i++)
    {
        cvReleaseMat (&v_map_matrix[i]);
    }
    cvReleaseMat (&MappedPixel);
    cvReleaseMat (&TemplatePixel);
    delete src;
    delete dst;

    cvReleaseImage(&Img2BExtracted);

    return true;

}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculate point warping information

@param      iShapes                 Input - the shapes

@param      img                     Input - images

@param      templateTriangles       Input - the composed face template triangles

@param      warpInfo                Input - warping information for all template points

@param      oTexture                Output - the extracted textures

@param      tbm                     Input - texture extraction method

@return     bool                    loading succeed or not?

*/
bool VO_AAM::VO_LoadTexturesFromShapes(const vector<VO_AAMShape2D>& iShapes, const vector<IplImage*> imgs, 
                                       const vector <VO_AAMTriangle2D>& triangles, const vector<VO_AAMWarpingPoint>& warpInfo,
                                       vector<VO_AAMTexture2D3C>& oTextures, int tbm)
{
    assert( iShapes.size() == imgs.size() );

    unsigned int NbOfSamples    = imgs.size();
    unsigned int NbOfPoints     = iShapes[0].GetSize();
    unsigned int NbOfShapes     = NbOfPoints*2;
    unsigned int NbOfPixels     = warpInfo.size();
    unsigned int NbOfTextures   = NbOfPixels*imgs[0]->nChannels;

    oTextures.resize(NbOfSamples);

    // Read all ASF file data into "m_vAAMAlignedShapes" and scale back to the original size
    for(unsigned int i=0; i < NbOfSamples; i++)
    {
        if ( !VO_AAM::VO_LoadOneTextureFromShape(iShapes[i], imgs[i], triangles, warpInfo, oTextures[i], tbm) )
            return false;
    }

    return true;

}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Normalize all textures - 2-norm all to "1"

@param      vTextures           Input - all textures before normalization

@param      normalizedTextures  Output - all textures after normalization

@return     float               return average texture size of all textures

*/
float VO_AAM::VO_NormalizeAllTextures(const vector<VO_AAMTexture2D3C>& vTextures, vector<VO_AAMTexture2D3C>& normalizedTextures)
{
    unsigned int NbOfSamples                    = vTextures.size();
    unsigned int NbOfTextures                   = vTextures[0].GetSize();
    float averageTextureStandardDeviation       = 0.0f;

    normalizedTextures = vTextures;

    VO_AAMTexture2D3C meanNormalizedTexture;
    meanNormalizedTexture.SetSize(NbOfTextures);

    for(unsigned int i = 0; i < NbOfSamples; i++)
    {
        averageTextureStandardDeviation += normalizedTextures[i].Standardize();
    }
    averageTextureStandardDeviation /= NbOfSamples;

    VO_AAM::VO_CalcMeanTexture ( normalizedTextures, meanNormalizedTexture );

    return averageTextureStandardDeviation;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Rescale the normalizetextures so that meantexture's 2-norm could be "1"

@param      meanNormalizeTexture    Input - meannormalizedtexture that all normalizedtextures should rescale to

@param      normalizedTextures      Input and Output - normalized texture and normalized rescaled texture

*/
void VO_AAM::VO_RescaleAllNormalizedTextures(const VO_AAMTexture2D3C& meanNormalizeTexture, 
                                             vector<VO_AAMTexture2D3C>& normalizedTextures)
{
    unsigned int NbOfSamples = normalizedTextures.size();

    // Explained by JIA Pei. 2008-03-04. scale back so that the mean shape size is equal to 1.0
    for(unsigned int i = 0; i < NbOfSamples; i++)
    {
        float ts = sqrt (normalizedTextures[i]*meanNormalizeTexture);
        normalizedTextures[i].Scale( 1.0f/ts );
    }
}



/**

@author     JIA Pei

@version    2008-03-24

@brief      Rescale the normalizedTexture to the already trained meanNormalizeTexture

@param      meanNormalizeTexture    Input - meanNormalizeTexture that all normalizedTextures should rescale to

@param      normalizedTexture       Input and Output - normalized texture and normalized rescaled texture

*/
void VO_AAM:: VO_RescaleOneNormalizedTexture(const VO_AAMTexture2D3C& meanNormalizeTexture, 
                                             VO_AAMTexture2D3C& normalizedTexture)
{
    float ts = sqrt (normalizedTexture*meanNormalizeTexture);
    normalizedTexture.Scale( 1.0f/ts );
}




/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculate mean texture

@param      vTextures     Input - all textures

@param      meanTexture   Output - mean texture

*/
void VO_AAM::VO_CalcMeanTexture(const vector<VO_AAMTexture2D3C>& vTextures, VO_AAMTexture2D3C &meanTexture)
{
    unsigned int NbOfSamples    = vTextures.size();
    unsigned int NbOfTextures   = vTextures[0].GetSize();
    meanTexture.SetSize(NbOfTextures);
    meanTexture = 0;

    for(unsigned int i = 0; i < NbOfSamples; i++)
    {
        meanTexture += vTextures[i];
    }

    meanTexture = meanTexture / (float)NbOfSamples;
}



/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculate point warping information

@param      triangles           Input - all possible triangles

@param      pt                  Input

@return     bool                whether the input pt in triangles

@note       Different from VO_ASM::VO_IsPointInConvexHull, because a trained template face may not be of convex,
For FRANCK and XM2VTS, the trained template face have concave hull

*/
bool VO_AAM::VO_IsPointInTemplateFace(const vector <VO_AAMTriangle2D>& triangles, const CvPoint2D32f& pt)
{
    bool isContaining = false;
    CvMat* tempVert = cvCreateMat (1, 3, CV_32FC2);

    //if( VO_ASM::VO_IsPointInConvexHull(pt, ch, true) )    // redundant. must be false here, excluding the convex hull (boundary)
    {
        for (unsigned int k = 0; k < triangles.size (); k++)
        {
            CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 0 ) = triangles[k].GetVertex(0);
            CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 1 ) = triangles[k].GetVertex(1);
            CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 2 ) = triangles[k].GetVertex(2);

            if (cvPointPolygonTest( tempVert, pt, 0 ) >= 0)
                isContaining = true;
        }
    }

    cvReleaseMat(&tempVert);

    return isContaining;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Put one texture into the template shape with a corresponding warp

@param      texture             Input

@param      triangles           Input - contains warping information

@param      img                 Output

@note       All texture vectors are of the same size. So, you can't put it into the respective triangulation 
for respective image. You've got to put a texture into a template shape first, then VO_WarpFromOneShapeToAnother

*/
void VO_AAM::VO_PutOneTextureToTemplateShape(const VO_AAMTexture2D3C &texture, const vector <VO_AAMTriangle2D>& triangles, IplImage* img)
{
    cvZero(img);

    // Get the image m_IplTemplateFace, for alignment!
    int PixelsInFaceTemplate = 0;
    for (unsigned int y = 0; y < img->height; y++)
    {
        for (unsigned int x = 0; x < img->width; x++)
        {
            CvPoint2D32f pt = cvPoint2D32f(x , y);

            if( VO_AAM::VO_IsPointInTemplateFace(triangles, pt) )
            {
                for(unsigned int k = 0; k < img->nChannels; k++)
                {
                    CV_IMAGE_ELEM(img, unsigned char, y, x * img->nChannels + k) = (unsigned char)texture.GetATexture(PixelsInFaceTemplate++);
                }
            }
        }
    }
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Put one texture into the template shape with a corresponding warp

@param      iShape              Input - shape warp from

@param      oShape              Input - shape warp to

@param      triangles           Input - contains warp information

@param      iImg                Input - input image, extracted texture from

@param      oImg                Output - output image, put the texture on to this image

@note       All texture vectors are of the same size. So, you can't put it into the respective triangulation 
for respective image. You've got to put a texture into a template shape first, then VO_WarpFromOneShapeToAnother

*/
void VO_AAM::VO_WarpFromOneShapeToAnother(const VO_AAMShape2D& iShape, const VO_AAMShape2D& oShape, const vector <VO_AAMTriangle2D>& triangles, const IplImage* iImg, IplImage* oImg)
{
    unsigned int NbOfPoints     = iShape.GetSize()/2;
    unsigned int NbOfTriangles  = triangles.size();
    unsigned int NbOfPixels     = 0;

    CvMat* pointIs = cvCreateMat (1, NbOfPoints, CV_32FC2);
    CvMat* pointOs = cvCreateMat (1, NbOfPoints, CV_32FC2);

    for( unsigned int i = 0; i < NbOfPoints; i++ )
    {
        CV_MAT_ELEM( *pointIs, CvPoint2D32f, 0, i ) = iShape.GetPoint(i).GetXY();
        CV_MAT_ELEM( *pointOs, CvPoint2D32f, 0, i ) = oShape.GetPoint(i).GetXY();
    }

    CvMat* IsConvexHull = cvCreateMat (1, NbOfPoints, CV_32FC2);
    CvMat* OsConvexHull = cvCreateMat (1, NbOfPoints, CV_32FC2);

    cvConvexHull2(pointIs, IsConvexHull, CV_CLOCKWISE, 0 );
    cvConvexHull2(pointOs, OsConvexHull, CV_CLOCKWISE, 0 );

    for (unsigned int y = 0; y < oImg->height; y++)
    {
        for (unsigned int x = 0; x < oImg->width; x++)
        {
            CvPoint2D32f pt = cvPoint2D32f(x, y);

            if( VO_AAM::VO_IsPointInConvexHull(pt, OsConvexHull, true) )
                //if( this->VO_IsPointInTemplateFace(pt) )
            {
                CvMat* tempVert = cvCreateMat (1, 3, CV_32FC2);
                for (unsigned int k = 0; k < NbOfTriangles; k++)
                {
                    CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 0 ) = oShape.GetPoint( triangles[k].GetVertexIndex(0) ).GetXY();
                    CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 1 ) = oShape.GetPoint( triangles[k].GetVertexIndex(1) ).GetXY();
                    CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 2 ) = oShape.GetPoint( triangles[k].GetVertexIndex(2) ).GetXY();

                    // Here, we can actually give out all the local variables except the BGR, 
                    // the BGR is not shape based and will be given by function VO_LoadTexturesFromShapes()
                    if( cvPointPolygonTest( tempVert, pt, 0 ) >= 0)
                    {
                        CvPoint2D32f* src = new CvPoint2D32f[3];
                        CvPoint2D32f* dst = new CvPoint2D32f[3];

                        CvMat* map_matrix = cvCreateMat (2, 3, CV_32FC1);
                        CvMat* MappedPixel = cvCreateMat (2, 1, CV_32FC1);
                        CvMat* TemplatePixel= cvCreateMat (3, 1, CV_32FC1);

                        // Get the affine transformation for each triangle pair.
                        src[0] = CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 0 );
                        src[1] = CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 1 );
                        src[2] = CV_MAT_ELEM( *tempVert, CvPoint2D32f, 0, 2 );

                        dst[0] = iShape.GetPoint(triangles[k].GetVertexIndex(0) ).GetXY();
                        dst[1] = iShape.GetPoint(triangles[k].GetVertexIndex(1) ).GetXY();
                        dst[2] = iShape.GetPoint(triangles[k].GetVertexIndex(2) ).GetXY();

                        cvGetAffineTransform( src, dst, map_matrix );

                        // warp from m_vAAMTriangle2D[j] (template pixel in the specific triangle) to each shape/image
                        CV_MAT_ELEM( *TemplatePixel, float, 0, 0) = pt.x;
                        CV_MAT_ELEM( *TemplatePixel, float, 1, 0) = pt.y;
                        CV_MAT_ELEM( *TemplatePixel, float, 2, 0) = 1.0;

                        cvMatMul( map_matrix, TemplatePixel, MappedPixel );

                        // extract the B,G,R on each shape/image respectively into textures
                        // CV_MAT_ELEM( *MappedPixel, float, 0, 0) is for x while
                        // CV_MAT_ELEM( *MappedPixel, float, 1, 0) is for y. But, 
                        // x determines cols, while y determines rows.
                        vector<float> OnePixelBGR = VO_AAM::VO_CalcSubPixelTexture (
                            CV_MAT_ELEM( *MappedPixel, float, 0, 0), 
                            CV_MAT_ELEM( *MappedPixel, float, 1, 0), iImg);

                        CV_IMAGE_ELEM(oImg, unsigned char, y, x * 3 + 0) = (unsigned char)OnePixelBGR[0];
                        CV_IMAGE_ELEM(oImg, unsigned char, y, x * 3 + 1) = (unsigned char)OnePixelBGR[1];
                        CV_IMAGE_ELEM(oImg, unsigned char, y, x * 3 + 2) = (unsigned char)OnePixelBGR[2];

                        // Till now it's correct.
                        cvReleaseMat (&map_matrix);
                        cvReleaseMat (&MappedPixel);
                        cvReleaseMat (&TemplatePixel);
                        delete src;
                        delete dst;
                    }
                }
                NbOfPixels ++;
                cvReleaseMat (&tempVert);
            }
        }
    }

    cvReleaseMat(&pointIs);
    cvReleaseMat(&pointOs);
    cvReleaseMat(&IsConvexHull);
    cvReleaseMat(&OsConvexHull);

}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculate sub pixel texture of a point in image

@return     vector<float>   - a vector of size 1 or 3

@note       In the pictures, x represents cols, y represents rows!
In IplImage* image, x represents cols, y represents rows as well!!!

*/
vector<float> VO_AAM::VO_CalcSubPixelTexture(float x, float y, const IplImage* image)
{
    vector<float> result;

    register int Y = (int)y;
    register int X = (int)x;

    register float s=x-X;
    register float t=y-Y;

    vector<float> LeftTop        =   VO_AAM::VO_CalcPixelRGB(X, Y, image);
    vector<float> LeftBottom     =   VO_AAM::VO_CalcPixelRGB(X , Y + 1, image);
    vector<float> RightTop       =   VO_AAM::VO_CalcPixelRGB(X + 1, Y, image);
    vector<float> RightBottom    =   VO_AAM::VO_CalcPixelRGB(X + 1, Y + 1, image);

    vector<float> tempA, tempB;
    tempA.resize (image->nChannels);
    tempB.resize (image->nChannels);

    for (int i = 0; i < image->nChannels; i++)
    {
        tempA[i] += (1.0f-t) * LeftTop[i] + t * LeftBottom[i];
        tempB[i] += (1.0f-t) * RightTop[i] + t * RightBottom[i];
    }

    for (int i = 0; i < image->nChannels; i++)
    {
        result.push_back (tempA[i] * (1.0f-s) + tempB[i] * s);
    }

    return result;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculate RGB value for a point in image

@param      image

@return     vector<float>   - a vector of size 1 or 3

*/
vector<float> VO_AAM::VO_CalcPixelRGB(int x, int y, const IplImage* image)
{
    vector<float> result;

    switch(image->depth)
    {
    case IPL_DEPTH_8U:
        //case IPL_DEPTH_8S:
        for (unsigned int i = 0; i < image->nChannels; i++)
        {
            result.push_back ( (float)CV_IMAGE_ELEM( image, unsigned char, y, x * image->nChannels + i ) );
        }
        break;
    case IPL_DEPTH_16S:
        for (unsigned int i = 0; i < image->nChannels; i++)
        {
            result.push_back ( (float)CV_IMAGE_ELEM( image, short int , y, x * image->nChannels + i ) );
        }
        break;
        // 	case IPL_DEPTH_16U:
        // 		for (unsigned int i = 0; i < image->nChannels; i++)
        // 		{
        // 			result.push_back ( (float)CV_IMAGE_ELEM( image, unsigned short int , y, x * image->nChannels + i ) );
        // 		}
        // 	break;
    case IPL_DEPTH_64F:
        for (unsigned int i = 0; i < image->nChannels; i++)
        {
            result.push_back ( (float)CV_IMAGE_ELEM( image, float , y, x * image->nChannels + i ) );
        }
        break;
        // 	case IPL_DEPTH_32F:
        // 		for (unsigned int i = 0; i < image->nChannels; i++)
        // 		{
        // 			result.push_back ( (float)CV_IMAGE_ELEM( image, float , y, x * image->nChannels + i ) );
        // 		}
        // 	break;
    case IPL_DEPTH_32S:
    default:
        for (unsigned int i = 0; i < image->nChannels; i++)
        {
            result.push_back ( (float)CV_IMAGE_ELEM( image, signed long int , y, x * image->nChannels + i ) );
        }
        break;

    }

    return result;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Normalized texture to reference scale texture

@param      inTexture       Input - input texture

@param      textureSD       Input - texture standard deviation

@param      outTexture      Output - output texture in reference scale

*/
void VO_AAM::VO_NormalizedTexture2ReferenceScale(const VO_AAMTexture2D3C& inTexture, float textureSD, VO_AAMTexture2D3C& outTexture)
{
    outTexture = const_cast<VO_AAMTexture2D3C&> (inTexture) * textureSD + AVERAGEFACETEXTURE;

    // To judge whether the pixel value (gray level value) is bigger than 255 or smaller than 0
    for (unsigned int i = 0; i < outTexture.GetSize(); i++)
    {
        if (outTexture.GetATexture(i) < FLT_EPSILON)
        {
            outTexture.SetATexture(i, 0.0f);
        }
        else if ( (outTexture.GetATexture(i) - 255.0f) > FLT_EPSILON)
        {
            outTexture.SetATexture(i, 255.0f);
        }
    }
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Reference scale texture back to normalized one

@param      inTexture       Input - reference texture

@param      textureSD       Input - texture standard deviation

@param      outTexture      Output - output normalized texture

*/
void VO_AAM::VO_ReferenceTextureBack2Normalize(const VO_AAMTexture2D3C& inTexture, float textureSD, VO_AAMTexture2D3C& outTexture)
{
    outTexture = ( const_cast<VO_AAMTexture2D3C&> (inTexture) - AVERAGEFACETEXTURE ) / textureSD;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Put edges on template face

@param      edges           Input - all edges information

@param      templateShape   Input - template shape

@param      iImg            Input - template image

@param      oImg            Output - output image with edges on oImg

*/
void VO_AAM::VO_PutEdgesOnTemplateFace(const vector<VO_AAMEdge>& edges, const VO_AAMShape2D& templateShape,  
                                       const IplImage* iImg, IplImage* oImg)
{
    unsigned int NbOfEdges = edges.size();
    CvPoint iorg,idst;

    cvCopy(iImg, oImg);

    for (unsigned int i = 0; i < NbOfEdges; i++)
    {
        iorg = templateShape.GetPoint(edges[i].GetIndex1() ).GetXYInt();
        idst = templateShape.GetPoint(edges[i].GetIndex2() ).GetXYInt();
        cvLine( oImg, iorg, iorg, colors[8], 2, 0, 0 );     // emphasize the vertex
        cvLine( oImg, iorg, idst, colors[8], 1, 0, 0 );
    }
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Put convex hull on template face

@param      ch              Input - convex hull

@param      iImg            Input - template image

@param      oImg            Output - output images with convex hull on oImg

*/
void VO_AAM::VO_PutConvexHullOnTemplateFace(const CvMat* ch, const IplImage* iImg, IplImage* oImg)
{
    unsigned int NbOfPointsOnConvexHull = ch->cols;
    CvPoint iorg,idst;

    cvCopy(iImg, oImg);

    for (unsigned int i = 0; i < NbOfPointsOnConvexHull; i++)
    {
        if(i != NbOfPointsOnConvexHull - 1 )
        {
            iorg = cvPointFrom32f( CV_MAT_ELEM( *ch, CvPoint2D32f, 0, i ) );
            idst = cvPointFrom32f( CV_MAT_ELEM( *ch, CvPoint2D32f, 0, i+1 ) );
            cvLine( oImg, iorg, iorg, colors[8], 2, 0, 0 );     // emphasize the vertex
            cvLine( oImg, iorg, idst, colors[8], 1, 0, 0 );
        }
        else
        {
            iorg = cvPointFrom32f( CV_MAT_ELEM( *ch, CvPoint2D32f, 0, i ) );
            idst = cvPointFrom32f( CV_MAT_ELEM( *ch, CvPoint2D32f, 0, 0 ) );
            cvLine( oImg, iorg, iorg, colors[8], 2, 0, 0 );     // emphasize the vertex
            cvLine( oImg, iorg, idst, colors[8], 1, 0, 0 );
        }
    }
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Put concave pixels on template face - inside convex hull but out of template face

@param      ch              Input - convex hull

@param      triangles       Input - all triangles information

@param      iImg            Input - template image

@param      oImg            Output - output images with concave pixels on oImg

*/
void VO_AAM::VO_PutConcavePixelsOnTemplateFace(const CvMat* ch, const vector <VO_AAMTriangle2D>& triangles, 
                                               const IplImage* iImg, IplImage* oImg)
{
    cvCopy(iImg, oImg);

    CvPoint2D32f pt;
    for (unsigned int i = 0; i < iImg->height; i++)
    {
        for (unsigned int j = 0; j < iImg->width; j++)
        {
            pt.x = (float)j;
            pt.y = (float)i;

            if( VO_AAM::VO_IsPointInConvexHull(pt, ch, true) && !VO_AAM::VO_IsPointInTemplateFace(triangles, pt) )
            {
                CvPoint iorg = cvPointFrom32f(pt);
                cvLine( oImg, iorg, iorg, colors[8], 1, 0, 0 );   // Here, must not emphasize the points
            }
        }
    }
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Put every single triangle onto template face

@param      triangles       Input - all triangles information

@param      iImg            Input - template image

@param      oImgs           Output - output images with single triangle on every single oImg

*/
void VO_AAM::VO_PutTrianglesOnTemplateFace(const vector <VO_AAMTriangle2D>& triangles, 
                                           const IplImage* iImg, vector<IplImage*>& oImgs)
{
    unsigned int NbOfTriangles = triangles.size();

    for(unsigned int i = 0; i < NbOfTriangles; i++)
    {
        cvCopy(iImg, oImgs[i]);
        vector<CvPoint> iorg, idst;
        vector<CvPoint2D32f> iorgFloat, idstFloat;
        iorg.resize(3);
        idst.resize(3);
        iorgFloat.resize(3);
        idstFloat.resize(3);

        iorgFloat       = triangles[i].GetVertexes();
        idstFloat[0]    = triangles[i].GetVertex(1);
        idstFloat[1]    = triangles[i].GetVertex(2);
        idstFloat[2]    = triangles[i].GetVertex(0);

        iorg[0]         = cvPointFrom32f(iorgFloat[0]);
        iorg[1]         = cvPointFrom32f(iorgFloat[1]);
        iorg[2]         = cvPointFrom32f(iorgFloat[2]);
        idst[0]         = cvPointFrom32f(idstFloat[0]);
        idst[1]         = cvPointFrom32f(idstFloat[1]);
        idst[2]         = cvPointFrom32f(idstFloat[2]);

        cvLine( oImgs[i], iorg[0], idst[0], colors[8], 1, 0, 0 );
        cvLine( oImgs[i], iorg[1], idst[1], colors[8], 1, 0, 0 );
        cvLine( oImgs[i], iorg[2], idst[2], colors[8], 1, 0, 0 );
    }
}



/**

@author     JIA Pei

@version    2008-03-20

@brief      texture parameters back project to aligned shape

@param      inC             Input - input concatenated parameters

@return     VO_AAMShape2D   - the back projected shape

*/
VO_AAMTexture2D3C VO_AAM::VO_TParamBackProjectToNormalizedTexture(CvMat* inT)
{
    CvMat* oTexture = cvCreateMat(1, this->m_iNbOfTextures, CV_32FC1);

    // Q back to texture
    cvBackProjectPCA(inT, this->m_CVMNormalizedTexturesMean, this->m_CVMTruncatedNormalizedTexturesEigenVectors, oTexture);
    VO_AAMTexture2D3C res   =   VO_AAMTexture2D3C::VO_CvMat2AAMTexture(oTexture);

    cvReleaseMat(&oTexture);

    return res;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      build AAM

@param      vShapes     Input - all input shapes

@param      imgs        Input - all input images

@param      channel     Input - 1 or 3

@param      tbm         Input - texture build method

@param      TPShape     Input - truncated percentage for shape model

@param      TPTexture   Input - truncated percentage for texture model
*/
void VO_AAM::VO_BuildAAM(const vector<VO_AAMShape2D> &vShapes, const vector<IplImage*> &imgs, 
                         int channel, int tbm, float TPShape, float TPTexture)
{
    this->m_iNbOfChannels               = channel;
    this->m_iTextureExtractionMethod    = tbm;
    this->m_vImages                     = imgs;
    this->m_fTruncatedPercent_Texture   = TPTexture;

    this->VO_BuildASM(vShapes, TPShape);

    this->m_iNbOfPixels = VO_AAM::VO_CalcPointWarpingInfo(this->m_vAAMTriangle2D, this->m_CVMConvexHull, this->m_vPointWarpInfo);
    this->m_iNbOfTextures = this->m_iNbOfPixels*this->m_iNbOfChannels;

    this->VO_LoadTexturesFromShapes(this->m_vAAMShapes, this->m_vImages, this->m_vAAMTriangle2D, this->m_vPointWarpInfo, 
        this->m_vAAMTextures, this->m_iTextureExtractionMethod);

    this->m_fAverageTextureStandardDeviation = this->VO_NormalizeAllTextures(this->m_vAAMTextures, this->m_vAAMNormalizedTextures);
    VO_AAM::VO_CalcMeanTexture(this->m_vAAMNormalizedTextures, this->m_VOAAMMeanNormalizedTexturePrior);
    VO_AAM::VO_RescaleAllNormalizedTextures(this->m_VOAAMMeanNormalizedTexturePrior,this->m_vAAMNormalizedTextures);
    // Calculate meantexture, now, bear in mind that all normalizedtexture and the meantexture are all of size "1"
    VO_AAM::VO_CalcMeanTexture(this->m_vAAMNormalizedTextures, this->m_VOAAMMeanNormalizedTexturePost);// actually, "1" already
    this->m_VOAAMMeanNormalizedTexturePost.Scale( 1.0f/this->m_VOAAMMeanNormalizedTexturePost.Norm2());  // "1" again

    this->m_CVMTextures = cvCreateMat(this->m_iNbOfSamples, this->m_iNbOfTextures, CV_32FC1);
    this->m_CVMNormalizedTextures = cvCreateMat(this->m_iNbOfSamples, this->m_iNbOfTextures, CV_32FC1);
    // VO_AAMTexture2CvMat() is not available here. For a vector of VO_AAMTexture2D3C
    for(unsigned int i=0; i < this->m_iNbOfSamples; i++)
    {
        for(unsigned int j = 0; j < this->m_iNbOfTextures; j++)
        {
            CV_MAT_ELEM( *this->m_CVMTextures, float, i, j ) = this->m_vAAMTextures[i].GetATexture(j);

            CV_MAT_ELEM( *this->m_CVMNormalizedTextures, float, i, j ) = this->m_vAAMNormalizedTextures[i].GetATexture(j);
        }
    }

    this->m_iNbOfEigenTexturesAtMost = MIN(this->m_iNbOfSamples, this->m_iNbOfTextures);

    this->m_CVMNormalizedTexturesEigenValues = cvCreateMat( 1, this->m_iNbOfEigenTexturesAtMost, CV_32FC1 );
    this->m_CVMNormalizedTexturesEigenVectors = cvCreateMat(this->m_iNbOfEigenTexturesAtMost, this->m_iNbOfTextures, CV_32FC1);

    this->m_CVMNormalizedTexturesMean = cvCreateMat( 1, this->m_iNbOfTextures, CV_32FC1 );
    this->m_CVMNormalizedTexturesSD   = cvCreateMat( 1, this->m_iNbOfTextures, CV_32FC1);
    this->m_VOAAMMeanNormalizedTexturePost.VO_AAMTexture2CvMat(this->m_CVMNormalizedTexturesMean);

    cvCalcPCA( this->m_CVMNormalizedTextures, this->m_CVMNormalizedTexturesMean, 
        this->m_CVMNormalizedTexturesEigenValues, this->m_CVMNormalizedTexturesEigenVectors,
        CV_PCA_DATA_AS_ROW/* + CV_PCA_USE_AVG */);

    VO_Common::VO_CalcSamplesSDWithKnownMean(this->m_CVMNormalizedTextures, this->m_CVMNormalizedTexturesMean, this->m_CVMNormalizedTexturesSD);

    CvScalar SumOfEigenValues = cvSum( this->m_CVMNormalizedTexturesEigenValues );

    float limit = (float)(FLT_EPSILON * SumOfEigenValues.val[0]);

    unsigned int i = 0;
    for(i = 0; i < this->m_iNbOfEigenTexturesAtMost; i++)
    { 
        if ( CV_MAT_ELEM( *this->m_CVMNormalizedTexturesEigenValues, float, 0, i ) < limit) break;
    }

    // NonZero EigenValues
    unsigned int NbOfNonZeroEigenValues = i;

    for(i = NbOfNonZeroEigenValues; i < this->m_iNbOfEigenTexturesAtMost; i++)
    {
        CV_MAT_ELEM( *this->m_CVMNormalizedTexturesEigenValues, float, 0, i ) = 0.0f;
    }

    SumOfEigenValues = cvSum( this->m_CVMNormalizedTexturesEigenValues );
    float ps = 0.0f;
    this->m_iNbOfTruncatedTextures = 0;

    for(unsigned int i = 0; i < NbOfNonZeroEigenValues; i++)
    {
        ps += CV_MAT_ELEM( *this->m_CVMNormalizedTexturesEigenValues, float, 0, i );
        ++this->m_iNbOfTruncatedTextures;
        if( ps/SumOfEigenValues.val[0] >= this->m_fTruncatedPercent_Texture) break;
    }

    this->m_CVMTruncatedNormalizedTexturesEigenValues = cvCreateMat( 1, this->m_iNbOfTruncatedTextures, CV_32FC1);
    this->m_CVMTruncatedNormalizedTexturesEigenVectors = cvCreateMat( this->m_iNbOfTruncatedTextures, this->m_iNbOfTextures, CV_32FC1);

    for(unsigned int i = 0; i < this->m_iNbOfTruncatedTextures; i++)
    {
        CV_MAT_ELEM( *this->m_CVMTruncatedNormalizedTexturesEigenValues, float, 0, i )
            = CV_MAT_ELEM( *this->m_CVMNormalizedTexturesEigenValues, float, 0, i );

        for (unsigned int j = 0; j < this->m_iNbOfTextures; j++)
        {
            CV_MAT_ELEM( *this->m_CVMTruncatedNormalizedTexturesEigenVectors, float, i, j )
                = CV_MAT_ELEM( *this->m_CVMNormalizedTexturesEigenVectors, float, i, j );
        }
    }

    this->m_CVMNormalizedTexturesProject2Truncated = cvCreateMat( this->m_iNbOfSamples, this->m_iNbOfTruncatedTextures, CV_32FC1);
    cvProjectPCA( this->m_CVMNormalizedTextures, this->m_CVMNormalizedTexturesMean, this->m_CVMTruncatedNormalizedTexturesEigenVectors, this->m_CVMNormalizedTexturesProject2Truncated );

    this->m_CVMTruancatedNormalizedTexturesMean = cvCreateMat( 1, this->m_iNbOfTruncatedTextures, CV_32FC1);
    this->m_CVMTruancatedNormalizedTexturesSD   = cvCreateMat( 1, this->m_iNbOfTruncatedTextures, CV_32FC1);
    // This time, the mean value is not specified beforehand
    VO_Common::VO_CalcSamplesMeanNSD(this->m_CVMNormalizedTexturesProject2Truncated, this->m_CVMTruancatedNormalizedTexturesMean, this->m_CVMTruancatedNormalizedTexturesSD);

    // Calculate reference texture 
    VO_AAM::VO_NormalizedTexture2ReferenceScale(this->m_VOAAMMeanNormalizedTexturePost, this->m_fAverageTextureStandardDeviation, this->m_VOAAMReferenceTexture);


    //////////////////////////////////////////////////////////////////////////
    // Calculate m_vNormalizedPointWarpInfo
    //////////////////////////////////////////////////////////////////////////
    CvPoint2D32f* src = new CvPoint2D32f[3];
    CvPoint2D32f* dst = new CvPoint2D32f[3];

    // warp from reference triangles to normalized triangles
    vector<CvMat*> v_map_matrix;
    v_map_matrix.resize(this->m_vAAMTriangle2D.size ());
    for(unsigned int i = 0; i < v_map_matrix.size (); i++ )
    {
        v_map_matrix[i] = cvCreateMat (2, 3, CV_32FC1);
    }

    // calculate all the possible mapping (for speeding up) 95 mapping totally
    // Here, actually, a translation and a scale can do this as well!
    for (unsigned int k = 0; k < v_map_matrix.size (); k++ )
    {
        // Get the affine transformation for each triangle pair.
        src[0] = this->m_vAAMTriangle2D[k].GetVertex(0);
        src[1] = this->m_vAAMTriangle2D[k].GetVertex(1);
        src[2] = this->m_vAAMTriangle2D[k].GetVertex(2);

        dst[0] = this->m_VOAAMMeanAlignedShapePost.GetPoint( this->m_vAAMTriangle2D[k].GetVertexIndex(0)).GetXY();
        dst[1] = this->m_VOAAMMeanAlignedShapePost.GetPoint( this->m_vAAMTriangle2D[k].GetVertexIndex(1)).GetXY();
        dst[2] = this->m_VOAAMMeanAlignedShapePost.GetPoint( this->m_vAAMTriangle2D[k].GetVertexIndex(2)).GetXY();

        cvGetAffineTransform( src, dst, v_map_matrix[k] );
    }

    delete src;
    delete dst;


    CvMat* MappedPixel = cvCreateMat (2, 1, CV_32FC1);
    CvMat* TemplatePixel= cvCreateMat (3, 1, CV_32FC1);
    CvPoint2D32f pt, pt0;
    for (unsigned int i = 0; i < this->m_iNbOfPixels; i++)
    {
        // JIA Pei. 2006-11-25. You will see the following (int) is very important
        // without (int), the result here is not correct at all!!
        pt = this->m_vPointWarpInfo[i].GetPosition();
        unsigned int triangleIndex = this->m_vPointWarpInfo[i].GetTriangleIndex();

        CV_MAT_ELEM( *TemplatePixel, float, 0, 0) = pt.x;
        CV_MAT_ELEM( *TemplatePixel, float, 1, 0) = pt.y;
        CV_MAT_ELEM( *TemplatePixel, float, 2, 0) = 1.0;

        cvMatMul( v_map_matrix[triangleIndex], TemplatePixel, MappedPixel );
        pt0.x = CV_MAT_ELEM( *MappedPixel, float, 0, 0);
        pt0.y = CV_MAT_ELEM( *MappedPixel, float, 1, 0);

        VO_AAMWarpingPoint tempNormalizedPixelTriangle;

        tempNormalizedPixelTriangle.SetPosition(pt0);
        tempNormalizedPixelTriangle.SetTriangleIndex(triangleIndex);
        tempNormalizedPixelTriangle.SetPointIndex(i);
        tempNormalizedPixelTriangle.SetAAMTriangle2d(this->m_vAAMNormalizedTriangle2D[triangleIndex] );

        // Very important!! Note by JIA Pei, push_back cannot perform on vector< vector < > >
        this->m_vNormalizedPointWarpInfo.push_back (tempNormalizedPixelTriangle);
    }

    cvReleaseMat(&MappedPixel);
    cvReleaseMat(&TemplatePixel);

    //////////////////////////////////////////////////////////////////////////


    // Calculate template images
    CvRect rect = VO_ASM::VO_CalcShapeBoundaryRect(this->m_VOAAMReferenceShape);
    this->m_CVSizeOfTemplateFace = cvSize(rect.width, rect.height);

    this->m_IplTemplateFace = cvCreateImage(this->m_CVSizeOfTemplateFace, IPL_DEPTH_8U, this->m_iNbOfChannels);
    this->m_IplEdges        = cvCreateImage(this->m_CVSizeOfTemplateFace, IPL_DEPTH_8U, this->m_iNbOfChannels);
    this->m_IplConvexHull   = cvCreateImage(this->m_CVSizeOfTemplateFace, IPL_DEPTH_8U, this->m_iNbOfChannels);
    this->m_IplConcave      = cvCreateImage(this->m_CVSizeOfTemplateFace, IPL_DEPTH_8U, this->m_iNbOfChannels);
    this->m_vIplTriangles.resize(m_iNbOfTriangles);
    for (unsigned int i = 0; i < this->m_iNbOfTriangles; i++)
    {
        this->m_vIplTriangles[i] = cvCreateImage(this->m_CVSizeOfTemplateFace, IPL_DEPTH_8U, this->m_iNbOfChannels);
    }
    cvZero(this->m_IplTemplateFace);

    VO_AAM::VO_PutOneTextureToTemplateShape(this->m_VOAAMReferenceTexture, this->m_vAAMTriangle2D, this->m_IplTemplateFace);
    VO_AAM::VO_PutEdgesOnTemplateFace(this->m_vAAMEdge, this->m_VOAAMReferenceShape, this->m_IplTemplateFace, this->m_IplEdges);
    VO_AAM::VO_PutConvexHullOnTemplateFace(this->m_CVMConvexHull, this->m_IplTemplateFace, this->m_IplConvexHull);
    VO_AAM::VO_PutConcavePixelsOnTemplateFace(this->m_CVMConvexHull, this->m_vAAMTriangle2D, this->m_IplTemplateFace, 
       this->m_IplConcave);
    VO_AAM::VO_PutTrianglesOnTemplateFace(this->m_vAAMTriangle2D, this->m_IplTemplateFace, this->m_vIplTriangles);


    //cvSaveImage("Reference.jpg",this->m_IplTemplateFace);
    //cvSaveImage("edges.jpg",this->m_IplEdges);
    //cvSaveImage("convexhull.jpg",this->m_IplConvexHull);
    //cvSaveImage("concave.jpg",this->m_IplConcave);

    //string trianglestr;
    //stringstream ssi;
    //string stri;
    //for (unsigned int i = 0; i < this->m_iNbOfTriangles; i++)
    //{
    //    ssi << i;
    //    ssi >> stri;
    //    if(stri.length() == 2)
    //        trianglestr = "triangle0" + stri + ".jpg";
    //    else if(stri.length() == 1)
    //        trianglestr = "triangle00" + stri + ".jpg";
    //    else
    //        trianglestr = "triangle" + stri + ".jpg";

    //    cvSaveImage(trianglestr.c_str(), this->m_vIplTriangles[i] );

    //    ssi.clear();
    //}

    //if (this->m_IplTemplateFace)    cvReleaseImage (&this->m_IplTemplateFace);
    //if (this->m_IplEdges)			cvReleaseImage (&this->m_IplEdges);
    //if (this->m_IplConvexHull)		cvReleaseImage (&this->m_IplConvexHull);
    //if (this->m_IplConcave)			cvReleaseImage (&this->m_IplConcave);

    //for (unsigned int i = 0; i <this->m_vImages.size (); i++ )
    //{
    //    if (this->m_vImages[i])
    //    {
    //        cvReleaseImage (&this->m_vImages[i]);
    //    }
    //}
    //this->m_vImages.clear();


}



/**

@author     JIA Pei

@version    2008-03-18

@brief      Save AAM to a specified folder

@param      fn         Input - the folder that AAM to be saved to

*/
void VO_AAM ::VO_Save(const string& fd)
{
    VO_ASM::VO_Save(fd);
    
    string fn = fd+"/AAM";
    if (!boost::filesystem::is_directory(fn) )
        boost::filesystem::create_directory( fn );

    fstream fp;
    string tempfn;

    // AAM
    tempfn = fn + "/AAM" + ".txt";
    fp.open(tempfn.c_str (), ios::out);

    // m_iTextureExtractionMethod
    fp << "m_iTextureExtractionMethod" << endl;
    fp << this->m_iTextureExtractionMethod << endl;

    // m_iNbOfChannels
    fp << "m_iNbOfChannels" << endl;
    fp << this->m_iNbOfChannels << endl;

    // m_iNbOfPixels
    fp << "m_iNbOfPixels" << endl;
    fp << this->m_iNbOfPixels << endl;

    // m_iNbOfTextures
    fp << "m_iNbOfTextures" << endl;
    fp << this->m_iNbOfTextures << endl;

    // m_iNbOfTruncatedTextures
    fp << "m_iNbOfTruncatedTextures" << endl;
    fp << this->m_iNbOfTruncatedTextures << endl;

    // m_iNbOfEigenTexturesAtMost
    fp << "m_iNbOfEigenTexturesAtMost" << endl;
    fp << this->m_iNbOfEigenTexturesAtMost << endl;

    // m_fAverageTextureStandardDeviation
    fp << "m_fAverageTextureStandardDeviation" << endl;
    fp << this->m_fAverageTextureStandardDeviation << endl;

    // m_fTruncatedPercent_Texture
    fp << "m_fTruncatedPercent_Texture" << endl;
    fp << this->m_fTruncatedPercent_Texture << endl;

    fp.close();

    // m_CVMTextures
    tempfn = fn + "/m_CVMTextures" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTextures" << endl;
    for (unsigned int i = 0; i < this->m_CVMTextures->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMTextures->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMTextures, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMNormalizedTextures
    tempfn = fn + "/m_CVMNormalizedTextures" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMNormalizedTextures" << endl;
    for (unsigned int i = 0; i < this->m_CVMNormalizedTextures->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMNormalizedTextures->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMNormalizedTextures, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMNormalizedTexturesProject2Truncated
    tempfn = fn + "/m_CVMNormalizedTexturesProject2Truncated" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMNormalizedTexturesProject2Truncated" << endl;
    for (unsigned int i = 0; i < this->m_CVMNormalizedTexturesProject2Truncated->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMNormalizedTexturesProject2Truncated->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMNormalizedTexturesProject2Truncated, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMTruancatedNormalizedTexturesMean
    tempfn = fn + "/m_CVMTruancatedNormalizedTexturesMean" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruancatedNormalizedTexturesMean" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruancatedNormalizedTexturesMean->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMTruancatedNormalizedTexturesMean, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMTruancatedNormalizedTexturesSD
    tempfn = fn + "/m_CVMTruancatedNormalizedTexturesSD" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruancatedNormalizedTexturesSD" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruancatedNormalizedTexturesSD->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMTruancatedNormalizedTexturesSD, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMNormalizedTexturesMean
    tempfn = fn + "/m_CVMNormalizedTexturesMean" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMNormalizedTexturesMean" << endl;
    for (unsigned int i = 0; i < this->m_CVMNormalizedTexturesMean->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMNormalizedTexturesMean, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMNormalizedTexturesSD
    tempfn = fn + "/m_CVMNormalizedTexturesSD" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMNormalizedTexturesSD" << endl;
    for (unsigned int i = 0; i < this->m_CVMNormalizedTexturesSD->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMNormalizedTexturesSD, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMNormalizedTexturesEigenVectors
    tempfn = fn + "/m_CVMNormalizedTexturesEigenVectors" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMNormalizedTexturesEigenVectors" << endl;
    for (unsigned int i = 0; i < this->m_CVMNormalizedTexturesEigenVectors->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMNormalizedTexturesEigenVectors->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMNormalizedTexturesEigenVectors, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMNormalizedTexturesEigenValues
    tempfn = fn + "/m_CVMNormalizedTexturesEigenValues" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMNormalizedTexturesEigenValues" << endl;
    for (unsigned int i = 0; i < this->m_CVMNormalizedTexturesEigenValues->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMNormalizedTexturesEigenValues, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMTruncatedNormalizedTexturesEigenVectors
    tempfn = fn + "/m_CVMTruncatedNormalizedTexturesEigenVectors" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruncatedNormalizedTexturesEigenVectors" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruncatedNormalizedTexturesEigenVectors->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMTruncatedNormalizedTexturesEigenVectors->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMTruncatedNormalizedTexturesEigenVectors, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMTruncatedNormalizedTexturesEigenValues
    tempfn = fn + "/m_CVMTruncatedNormalizedTexturesEigenValues" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruncatedNormalizedTexturesEigenValues" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruncatedNormalizedTexturesEigenValues->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMTruncatedNormalizedTexturesEigenValues, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_VOAAMReferenceTexture
    tempfn = fn + "/m_VOAAMReferenceTexture" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_VOAAMReferenceTexture" << endl;
    for (unsigned int i = 0; i < this->m_iNbOfTextures; i++)
    {
        fp << this->m_VOAAMReferenceTexture.GetATexture(i) << " ";
    }
    fp << endl;
    fp.close();

    // m_vAAMTextures
    tempfn = fn + "/m_vAAMTextures" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vAAMTextures" << endl;
    for (unsigned int i = 0; i < this->m_vAAMTextures.size(); i++)
    {
        for (unsigned int j = 0; j < this->m_iNbOfTextures; j++)
        {
            fp << this->m_vAAMTextures[i].GetATexture(j)<< " ";
        }
        fp << endl;
    }
    fp.close();

    // m_vAAMNormalizedTextures
    tempfn = fn + "/m_vAAMNormalizedTextures" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vAAMNormalizedTextures" << endl;
    for (unsigned int i = 0; i < this->m_vAAMNormalizedTextures.size(); i++)
    {
        for (unsigned int j = 0; j < this->m_iNbOfTextures; j++)
        {
            fp << this->m_vAAMNormalizedTextures[i].GetATexture(j)<< " ";
        }
        fp << endl;
    }
    fp.close();

    // m_VOAAMMeanNormalizedTexturePrior
    tempfn = fn + "/m_VOAAMMeanNormalizedTexturePrior" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_VOAAMMeanNormalizedTexturePrior" << endl;
    for (unsigned int j = 0; j < this->m_iNbOfTextures; j++)
    {
        fp << this->m_VOAAMMeanNormalizedTexturePrior.GetATexture(j)<< " ";
    }
    fp << endl;
    fp.close();

    // m_VOAAMMeanNormalizedTexturePost
    tempfn = fn + "/m_VOAAMMeanNormalizedTexturePost" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_VOAAMMeanNormalizedTexturePost" << endl;
    for (unsigned int j = 0; j < this->m_iNbOfTextures; j++)
    {
        fp << this->m_VOAAMMeanNormalizedTexturePost.GetATexture(j)<< " ";
    }
    fp << endl;
    fp.close();

    // m_vPointWarpInfo
    tempfn = fn + "/m_vPointWarpInfo" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vPointWarpInfo" << endl;
    for (unsigned int i = 0; i < this->m_vPointWarpInfo.size (); i++)
    {
        fp << this->m_vPointWarpInfo[i].GetPointIndex() << " "
            << this->m_vPointWarpInfo[i].GetPosition().x << " "
            << this->m_vPointWarpInfo[i].GetPosition().y << " "
            << this->m_vPointWarpInfo[i].GetTriangleIndex() << " " << endl;
    }
    fp.close();

    // m_vNormalizedPointWarpInfo
    tempfn = fn + "/m_vNormalizedPointWarpInfo" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vNormalizedPointWarpInfo" << endl;
    for (unsigned int i = 0; i < this->m_vNormalizedPointWarpInfo.size (); i++)
    {
        fp << this->m_vNormalizedPointWarpInfo[i].GetPointIndex() << " "
            << this->m_vNormalizedPointWarpInfo[i].GetPosition().x << " "
            << this->m_vNormalizedPointWarpInfo[i].GetPosition().y << " "
            << this->m_vNormalizedPointWarpInfo[i].GetTriangleIndex() << " " << endl;
    }
    fp.close();

    /** Template face image */
    tempfn = fn + "/Reference.jpg";
    cvSaveImage(tempfn.c_str(), this->m_IplTemplateFace);

    /** Image of edges */
    tempfn = fn + "/edges.jpg";
    cvSaveImage(tempfn.c_str(), this->m_IplEdges);

    /** Image of convex hull */
    tempfn = fn + "/convexhull.jpg";
    cvSaveImage(tempfn.c_str(), this->m_IplConvexHull);

    /** If the out boundary edges compose a concave hull, instead of a convex hull */
    tempfn = fn + "/concave.jpg";
    cvSaveImage(tempfn.c_str(), this->m_IplConcave);

    /** How many triangles totally */
    string trianglestr;
    stringstream ssi;
    string stri;
    for (unsigned int i = 0; i < this->m_vIplTriangles.size(); i++)
    {
        ssi << i;
        ssi >> stri;
        if(stri.length() == 2)
            trianglestr = "triangle0" + stri + ".jpg";
        else if(stri.length() == 1)
            trianglestr = "triangle00" + stri + ".jpg";
        else
            trianglestr = "triangle" + stri + ".jpg";

        cvSaveImage(trianglestr.c_str(), this->m_vIplTriangles[i] );

        ssi.clear();
    }

}

