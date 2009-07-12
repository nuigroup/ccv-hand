/****************************************************************************
* File Name:        AAMBuilding.cpp                                         *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM building test file                                  *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/


#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>

// used to distinguish POSIX and WIN32
#include <boost/regex/v4/fileiter.hpp>


#include "cv.h"
#include "highgui.h"
#include "ml.h"

#include "VO_Common.h"
#include "VO_IO.h"

#include "VO_AAMBasic.h"
//#include "VO_AAMICIA.h"


using namespace std;


int main ( int argc, char *argv[] )
{
	// specify the dataset to be used
	int ds 		= IMM;								// IMM, FRANCK, XM2VTS

	// specify the number of channels of the dataset
	int nChan 	= COLORCHANNELS;			        // GRAYCHANNELS, COLORCHANNELS

	// specify the method to be applied
	int med 	= AAM_BASIC;		                // AAM_BASIC, AAM_CMUINVERSECOMPOSITIONAL

	// specify the texture extraction method
	int tmd     = DIRECTTEXTURE;			        // DIRECTTEXTURE, LAPLACETEXTURE, HARRISCORNERTEXTURE, HISTOGRAMEQUALIZED, GABOR, SEGMENTATION

	string                  hostImagePath, hostAnnotatedPath, trainedDataPath;
	string                  prePath, strWidth, strHeight, strNumOfSamples;
	stringstream            ssWidth, ssHeight, ssNumOfSamples;
	vector<string>          AllAnnotatedFiles, AllImgFiles4Training;
	vector<IplImage*>       imgs;
	vector<VO_AAMShape2D>   shps;
	vector<VO_AAMShape2D>   shps_LeftEye;
	vector<VO_AAMShape2D>   shps_RightEye;
	vector<VO_AAMShape2D>   shps_LeftEyeBrow;
	vector<VO_AAMShape2D>   shps_RightEyeBrow;
	vector<VO_AAMShape2D>   shps_Nose;
	vector<VO_AAMShape2D>   shps_Mouth;
	vector<VO_AAMShape2D>   shps_Cheek;

	vector<int> set_LeftEye;
	vector<int> set_RightEye;
	vector<int> set_LeftEyeBrow;
	vector<int> set_RightEyeBrow;
	vector<int> set_Nose;
	vector<int> set_Mouth;
	vector<int> set_Cheek;


	// 1. Specify folders
	// For Windows
#ifdef BOOST_REGEX_FI_WIN32_MAP

	switch ( ds )
	{
		case XM2VTS:	// 720*576
			prePath             = "F:/Research/Database/XM2VTS/XM2VTS (CDS001+006)";
			break;
		case FRANCK:	// 720*576
			prePath             = "F:/Research/Database/FRANCK";
			break;
		case IMM:		// 640*480
		default:
			prePath             = "F:/Research/Database/IMM";
			break;
	}

	// For Posix -- Portable Operating System Interface for uniX
#elif defined(BOOST_REGEX_FI_POSIX_MAP)

	switch ( ds )
	{
		case XM2VTS:	// 720*576
			prePath             = "/media/Elements/Research/Database/XM2VTS/XM2VTS (CDS001+006)";
			break;
		case FRANCK:	// 720*576
			prePath             = "/media/Elements/Research/Database/FRANCK";
			break;
		case IMM:		// 640*480
		default:
			prePath             = "/media/Elements/Research/Database/IMM";
			break;
	}
#endif

	hostImagePath 		= prePath + "/images/all320X240";
	hostAnnotatedPath 	= prePath + "/annotations/all";

	// 2. Load all images
	AllImgFiles4Training 	= VO_IO::ScanNSortImagesInDirectory ( hostImagePath );
	switch ( nChan )
	{
		case GRAYCHANNELS:
		{
			for ( unsigned int i=0; i < AllImgFiles4Training.size(); i++ )
			{
				imgs.push_back ( cvLoadImage ( AllImgFiles4Training[i].c_str (), CV_LOAD_IMAGE_GRAYSCALE ) );
			}

			ssWidth << imgs[0]->width;
			ssWidth >> strWidth;
			ssHeight << imgs[0]->height;
			ssHeight >> strHeight;
			ssNumOfSamples << AllImgFiles4Training.size();
			ssNumOfSamples >> strNumOfSamples;
			trainedDataPath = prePath + "/traineddata/Gray/" + strWidth + "X" + strHeight + "_" + strNumOfSamples + "/face/frontal";
			//trainedDataPath = "/media/Elements/Research/Database/XM2VTS/XM2VTS (CDS001+006)/traineddata/Gray/360X288_2360/face/frontal";
		}
		break;
		case COLORCHANNELS:
		{
			for ( unsigned int i=0; i < AllImgFiles4Training.size(); i++ )
			{
				imgs.push_back ( cvLoadImage ( AllImgFiles4Training[i].c_str (), CV_LOAD_IMAGE_COLOR ) );
			}
			ssWidth << imgs[0]->width;
			ssWidth >> strWidth;
			ssHeight << imgs[0]->height;
			ssHeight >> strHeight;
			ssNumOfSamples << AllImgFiles4Training.size();
			ssNumOfSamples >> strNumOfSamples;
			trainedDataPath = prePath + "/traineddata/RGB/" + strWidth + "X" + strHeight + "_" + strNumOfSamples + "/face/frontal";
		}
		break;
	}

	// 3. Load the annotated information
	AllAnnotatedFiles	= VO_IO::ScanNSortAnnotationInDirectory ( hostAnnotatedPath );
	switch ( ds )
	{
		case XM2VTS:
		case FRANCK:
		{
			// Read all PTS file data into shps
			for ( unsigned int i=0; i < AllAnnotatedFiles.size(); i++ )
			{
				VO_AAMShape2D tempAAMShape;
				VO_AAMShape2D::ReadPTS ( AllAnnotatedFiles[i], tempAAMShape );
				tempAAMShape.ScaleXY ( 0.5, 0.5 );
				tempAAMShape.SetHostImage ( AllImgFiles4Training[i] );
				shps.push_back ( tempAAMShape );
			}
		}
		break;
		case IMM:
		default:
		{
			// Read all ASF file data into shps and scale back to the original size
			for ( unsigned int i=0; i < AllAnnotatedFiles.size(); i++ )
			{
				VO_AAMShape2D tempAAMShape;
				VO_AAMShape2D::ReadASF ( AllAnnotatedFiles[i], tempAAMShape );
				tempAAMShape.ScaleXY ( ( float ) imgs[i]->width, ( float ) imgs[i]->height );
				shps.push_back ( tempAAMShape );
			}
		}
		break;
	}

// 	// 4. Predefine the component information
// 	// For IMM
// 	// lefteye, 21-28
// 	for(int i = 21; i < 29; i++)
// 	{
// 		set_LeftEye.push_back(i);
// 	}
//
// 	// righteye, 13-20
// 	for(int i = 13; i < 21; i++)
// 	{
// 		set_RightEye.push_back(i);
// 	}
//
// 	// lefteyebrow, 35-38
// 	for(int i = 34; i < 39; i++)
// 	{
// 		set_LeftEyeBrow.push_back(i);
// 	}
//
// 	// righteyebrow, 29-33
// 	for(int i = 29; i < 34; i++)
// 	{
// 		set_RightEyeBrow.push_back(i);
// 	}
//
// 	// mouth, 39-46
// 	for(int i = 39; i < 47; i++)
// 	{
// 		set_Mouth.push_back(i);
// 	}
//
// 	// nose, 47-57
// 	for(int i = 47; i < 58; i++)
// 	{
// 		set_Nose.push_back(i);
// 	}
//
// 	// cheek, 0-12
// 	for(int i = 0; i < 13; i++)
// 	{
// 		set_Cheek.push_back(i);
// 	}
//
// 	shps_LeftEye.resize(shps.size());
// 	shps_RightEye.resize(shps.size());
// 	shps_LeftEyeBrow.resize(shps.size());
// 	shps_RightEyeBrow.resize(shps.size());
// 	shps_Nose.resize(shps.size());
// 	shps_Mouth.resize(shps.size());
// 	shps_Cheek.resize(shps.size());
//
// 	// 5. Load the annotated component information
// 	for(int i = 0; i < shps.size(); i++)
// 	{
// 		shps_LeftEye[i].SetHostImage( shps[i].GetHostImage() );
// 		shps_RightEye[i].SetHostImage( shps[i].GetHostImage() );
// 		shps_LeftEyeBrow[i].SetHostImage( shps[i].GetHostImage() );
// 		shps_RightEyeBrow[i].SetHostImage( shps[i].GetHostImage() );
// 		shps_Nose[i].SetHostImage( shps[i].GetHostImage() );
// 		shps_Mouth[i].SetHostImage( shps[i].GetHostImage() );
// 		shps_Cheek[i].SetHostImage( shps[i].GetHostImage() );
//
// 		for(int j = 0; j < shps[0].GetSize(); j++)
// 		{
// 			//if (vectorcontains(set_LeftEye, shps[i].m_vPoint[j].m_iindex) || vectorcontains(set_LeftEyeBrow, shps[i].m_vPoint[j].m_iindex) )
// 			if (vectorcontains(set_LeftEye, shps[i].GetPoint(j).GetIndex() ))
// 			{
// 				shps_LeftEye[i].InsertPoint(shps[i].GetPoint(j) );
// 			}
// 			//else if (vectorcontains(set_RightEye, shps[i].m_vPoint[j].m_iindex) || vectorcontains(set_RightEyeBrow, shps[i].m_vPoint[j].m_iindex) )
// 			else if (vectorcontains(set_RightEye, shps[i].GetPoint(j).GetIndex() ) )
// 			{
// 				shps_RightEye[i].InsertPoint(shps[i].GetPoint(j) );
// 			}
// 			else if (vectorcontains(set_Nose, shps[i].GetPoint(j).GetIndex() ))
// 			{
// 				shps_Nose[i].InsertPoint(shps[i].GetPoint(j) );
// 			}
// 			else if (vectorcontains(set_Mouth, shps[i].GetPoint(j).GetIndex() ))
// 			{
// 				shps_Mouth[i].InsertPoint(shps[i].GetPoint(j) );
// 			}
// 			//else if (vectorcontains(set_LeftEyeBrow, shps[i].m_vPoint[j].m_iindex))
// 			//{
// 			//	shps_LeftEyeBrow[i].InsertPoint(shps[i].GetPoint(j) );
// 			//}
// 			//else if (vectorcontains(set_RightEyeBrow, shps[i].m_vPoint[j].m_iindex))
// 			//{
// 			//	shps_RightEyeBrow[i].InsertPoint(shps[i].GetPoint(j) );
// 			//}
// 			else if (vectorcontains(set_Cheek, shps[i].GetPoint(j).GetIndex() ))
// 			{
// 				shps_Cheek[i].InsertPoint(shps[i].GetPoint(j) );
// 			}
// 		}
// 	}

	// We could train the components respectively here.
	switch (med)
	{
    case AAM_BASIC:
        {
            VO_AAMBasic testAAMBasic;
            testAAMBasic.VO_BuildAAMBasic ( shps, imgs, nChan, tmd, 0.95f, 0.95f, 0.95f );
            testAAMBasic.VO_Save(trainedDataPath);
        }
        break;
    case AAM_CMUINVERSECOMPOSITIONAL:
        {
            //VO_AAMICIA testAAMICIA;
            //testAAMICIA.VO_BuildAAMICIA ( shps, imgs, nChan, tmd, 0.95f, 0.95f );
            //testAAMICIA.VO_Save(trainedDataPath);
        }
    default:
        break;
	}
	
	return EXIT_SUCCESS;

}



