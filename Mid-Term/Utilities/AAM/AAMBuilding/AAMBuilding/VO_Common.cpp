/****************************************************************************
* File Name:        VO_Common.cpp                                           *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* create Date:      2008-03-04                                              *
* Main Content:     Common files                                            *
* Used Library:     STL + OpenCV + Boost                                    *
****************************************************************************/


#include "VO_Common.h"
#include "VO_AAMBasic.h"

using namespace std;

/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculation standard deviation for samples with known mean

@param      Input	    iSamples

@param      Input	    mSamples

@param      Output	    sdSamples

*/
void VO_Common::VO_CalcSamplesSDWithKnownMean(const CvMat* iSamples, const CvMat* mSamples, CvMat* sdSamples)
{
    unsigned int numberOfSamples    = iSamples->rows;
    unsigned int numberOfFeatures   = iSamples->cols;
    cvZero(sdSamples);

    CvMat* oneCol = cvCreateMat(numberOfSamples, 1, CV_32FC1);

    // calculate the column mean and variance
    for (unsigned int i = 0; i < numberOfFeatures; i++)
    {
        double mean, std_dev = 0;
        cvGetCol( iSamples, oneCol, i );
        mean = CV_MAT_ELEM( *mSamples, float, 0, i );

        for(unsigned int j = 0; j < numberOfSamples; j++)
             std_dev += pow( (CV_MAT_ELEM( *oneCol, float, j, 0 ) - mean), 2);

        CV_MAT_ELEM( *sdSamples, float, 0, i ) = (float) sqrt(std_dev);
    }

    cvReleaseMat(&oneCol);
}



/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculation mean and standard deviation for samples

@param      Input	    iSamples

@param      Output	    mSamples

@param      Output	    sdSamples

*/
void VO_Common::VO_CalcSamplesMeanNSD(const CvMat* iSamples, CvMat* mSamples, CvMat* sdSamples)
{
    unsigned int numberOfSamples    = iSamples->rows;
    unsigned int numberOfFeatures   = iSamples->cols;
    cvZero(sdSamples);

    CvMat* oneCol = cvCreateMat(numberOfSamples, 1, CV_32FC1);

    // calculate the column mean and variance
    for (unsigned int i = 0; i < numberOfFeatures; i++)
    {
        CvScalar mean, std_dev;

        cvGetCol( iSamples, oneCol, i );

        cvAvgSdv( oneCol, &mean, &std_dev, NULL );

        CV_MAT_ELEM( *mSamples, float, 0, i ) = (float) mean.val[0];
        CV_MAT_ELEM( *sdSamples, float, 0, i ) = (float) std_dev.val[0];
    }

    cvReleaseMat(&oneCol);
}



/**

@author     JIA Pei

@version    2008-03-24

@brief      Constrain model parameters

@param      inputDeltaParam     Input and output - input parameters' delta

@param      ParamSD             Input - parameters' standard deviation

@return     void

*/
void VO_Common::VO_ModelDeltaParamConstrain(CvMat* inputDeltaParam, const CvMat* ParamSD)
{
    assert (inputDeltaParam->cols = ParamSD->cols);
    for (unsigned int i = 0; i < inputDeltaParam->cols; i++)
    {
        if ( CV_MAT_ELEM( *inputDeltaParam, float, 0, i ) > 4*CV_MAT_ELEM( *ParamSD, float, 0, i ))
        {
            CV_MAT_ELEM( *inputDeltaParam, float, 0, i ) = 4*CV_MAT_ELEM( *ParamSD, float, 0, i );
        }
        else if ( CV_MAT_ELEM( *inputDeltaParam, float, 0, i ) < -4*CV_MAT_ELEM( *ParamSD, float, 0, i ))
        {
            CV_MAT_ELEM( *inputDeltaParam, float, 0, i ) = -4*CV_MAT_ELEM( *ParamSD, float, 0, i );
        }
    }
}



/**

@author     JIA Pei

@version    2008-03-24

@brief      Constrain pose parameters

@param      inputDeltaParam         Input and output - input parameters' delta

@return     void

*/
void VO_Common::VO_PoseDeltaParamConstrain(CvMat* inputDeltaParam)
{
    vector<float> iDeltaT, DeltaPose;
    iDeltaT.resize(inputDeltaParam->cols);
    DeltaPose.resize(4);
    for (int i = 0; i < iDeltaT.size(); i++)
    {
        iDeltaT[i] = CV_MAT_ELEM( *inputDeltaParam, float, 0, i );
    }

    VO_AAMBasic::VO_DisplacementVector2Pose(iDeltaT, DeltaPose);

    // for scale
    if ( DeltaPose[0] > 1.05 )
    {
        DeltaPose[0] = 1.05;
    }
    else if(DeltaPose[0] < 0.95)
    {
        DeltaPose[0] = 0.95;
    }

    // for theta
    if ( DeltaPose[1] > (15.0/180.0*CV_PI) )
    {
        DeltaPose[1] = 15.0/180.0*CV_PI;
    }
    else if(DeltaPose[1] < (-15.0/180.0*CV_PI))
    {
        DeltaPose[1] = -15.0/180.0*CV_PI;
    }

    // for tx
    if ( DeltaPose[2] > 0.02)
    {
        DeltaPose[2] = 0.02;
    }
    else if(DeltaPose[2] < -0.02)
    {
        DeltaPose[2] = -0.02;
    }

    // for ty
    if ( DeltaPose[3] > 0.02)
    {
        DeltaPose[3] = 0.02;
    }
    else if(DeltaPose[3] < -0.02)
    {
        DeltaPose[3] = -0.02;
    }

    // pose back to displacement parameters
    VO_AAMBasic::VO_Pose2DisplacementVector( DeltaPose, iDeltaT );

    for (int i = 0; i < iDeltaT.size(); i++)
    {
        CV_MAT_ELEM( *inputDeltaParam, float, 0, i ) = iDeltaT[i];
    }
}



/**

@author     JIA Pei

@version    2008-03-24

@brief      Range Constrain - shape should be within the image

@param      inputShape         Input and output - input shape parameters

@param      sizeConstrain      Input            - the size for constraining

@return     void

*/
void VO_Common::VO_RangeConstrain(CvMat* inputShape, const CvSize& sizeConstrain)
{
    for (unsigned int i = 0; i < inputShape->cols/2; i++)
    {
        if ( CV_MAT_ELEM( *inputShape, float, 0, 2*i ) < 1.0 )
        {
            CV_MAT_ELEM( *inputShape, float, 0, 2*i ) = 1.0f;
        }
        else if ( CV_MAT_ELEM( *inputShape, float, 0, 2*i ) > (sizeConstrain.width - 1.0) )
        {
            CV_MAT_ELEM( *inputShape, float, 0, 2*i ) = sizeConstrain.width - 1.0f;
        }

        if ( CV_MAT_ELEM( *inputShape, float, 0, 2*i+1 ) < 1.0 )
        {
            CV_MAT_ELEM( *inputShape, float, 0, 2*i+1 ) = 1.0f;
        }
        else if ( CV_MAT_ELEM( *inputShape, float, 0, 2*i+1 ) > (sizeConstrain.height - 1.0) )
        {
            CV_MAT_ELEM( *inputShape, float, 0, 2*i+1 ) = sizeConstrain.height - 1.0f;
        }
    }
}

