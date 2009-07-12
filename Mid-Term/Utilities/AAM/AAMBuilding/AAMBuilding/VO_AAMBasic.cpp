/****************************************************************************
* File Name:        VO_AAMBasic.cpp                                         *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     Basic AAM Model, shape + texture concatenation          *
* Used Library:     STL + OpenCV + Boost                                    *
****************************************************************************/


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

#include <boost/filesystem.hpp>

#include <gsl/gsl_multifit.h>

#include "VO_AAMBasic.h"

using namespace std;


VO_AAMBasic::VO_AAMBasic()
{
    this->init();
}


/** Initialization */
void VO_AAMBasic::init()
{
    this->m_CVMWeightsScaleShape2Texture		    = NULL;
    this->m_CVMConcatenated						    = NULL;
    this->m_CVMConcatenatedProject2Truncated	    = NULL;
    this->m_CVMTruancatedConcatenatedMean		    = NULL;
    this->m_CVMTruancatedConcatenatedSD			    = NULL;
    this->m_CVMConcatenatedMean					    = NULL;
    this->m_CVMConcatenatedSD                       = NULL;
    this->m_CVMConcatenatedEigenVectors			    = NULL;
    this->m_CVMConcatenatedEigenValues			    = NULL;
    this->m_CVMTruncatedConcatenatedEigenVectors    = NULL;
    this->m_CVMTruncatedConcatenatedEigenValues	    = NULL;
    this->m_CVMPcs								    = NULL;
    this->m_CVMPcg								    = NULL;
    this->m_CVMQs								    = NULL;
    this->m_CVMQg								    = NULL;
    this->m_CVMRc								    = NULL;
    this->m_CVMRt								    = NULL;
    this->m_CVMCParamGradientMatrix					= NULL;
    this->m_CVMPoseGradientMatrix				    = NULL;
    this->m_iNbOfConcatenated					    = 0;
    this->m_iNbOfTruncatedConcatenated			    = 0;
    this->m_iNbOfEigenConcatenatedAtMost		    = 0;
    this->m_fTruncatedPercent_Concatenated          = 0.95f;
}


VO_AAMBasic::~VO_AAMBasic()
{
    if (this->m_CVMWeightsScaleShape2Texture)		    cvReleaseMat (&this->m_CVMWeightsScaleShape2Texture);
    if (this->m_CVMConcatenated)					    cvReleaseMat (&this->m_CVMConcatenated);
    if (this->m_CVMConcatenatedProject2Truncated)	    cvReleaseMat (&this->m_CVMConcatenatedProject2Truncated);
    if (this->m_CVMTruancatedConcatenatedMean)		    cvReleaseMat (&this->m_CVMTruancatedConcatenatedMean);
    if (this->m_CVMTruancatedConcatenatedSD)		    cvReleaseMat (&this->m_CVMTruancatedConcatenatedSD);
    if (this->m_CVMConcatenatedMean)				    cvReleaseMat (&this->m_CVMConcatenatedMean);
    if (this->m_CVMConcatenatedSD)					    cvReleaseMat (&this->m_CVMConcatenatedSD);
    if (this->m_CVMConcatenatedEigenVectors)		    cvReleaseMat (&this->m_CVMConcatenatedEigenVectors);
    if (this->m_CVMConcatenatedEigenValues)			    cvReleaseMat (&this->m_CVMConcatenatedEigenValues);
    if (this->m_CVMTruncatedConcatenatedEigenVectors)   cvReleaseMat (&this->m_CVMTruncatedConcatenatedEigenVectors);
    if (this->m_CVMTruncatedConcatenatedEigenValues)    cvReleaseMat (&this->m_CVMTruncatedConcatenatedEigenValues);
    if (this->m_CVMPcs)								    cvReleaseMat (&this->m_CVMPcs);
    if (this->m_CVMPcg)								    cvReleaseMat (&this->m_CVMPcg);
    if (this->m_CVMQs)								    cvReleaseMat (&this->m_CVMQs);
    if (this->m_CVMQg)								    cvReleaseMat (&this->m_CVMQg);
    if (this->m_CVMRc)								    cvReleaseMat (&this->m_CVMRc);
    if (this->m_CVMRt)								    cvReleaseMat (&this->m_CVMRt);
    if (this->m_CVMCParamGradientMatrix)				cvReleaseMat (&this->m_CVMCParamGradientMatrix);
    if (this->m_CVMPoseGradientMatrix)				    cvReleaseMat (&this->m_CVMPoseGradientMatrix);

}



//void VO_AAMBasic::VO_ShapeTexture2Concatenated( VO_AAMShape2D iShape, VO_AAMTexture2D3C iTexture, vector<float>& b )
//{
//    CvMat* b_s_temp = cvCreateMat (1, this->m_CVMTruncatedAlignedShapesEigenValues->cols, CV_32FC1);
//    CvMat* b_s = cvCreateMat (1, this->m_CVMTruncatedAlignedShapesEigenValues->cols, CV_32FC1);
//    CvMat* b_t = cvCreateMat (1, this->m_CVMTruncatedNormalizedTexturesEigenValues->cols, CV_32FC1);
//
//    this->VO_ShapeProject2TruncatedEigenSpace(iShape, b_s_temp);			// 1*116->1*20
//
//    vector<float> v_b_s_temp;
//    v_b_s_temp.resize(b_s_temp->cols);
//    for (unsigned int i = 0; i < v_b_s_temp.size(); i++)
//    {
//        v_b_s_temp[i] =  CV_MAT_ELEM( *b_s_temp, float, 0, i );
//    }
//
//    cvMatMul( b_s_temp, this->m_CVMWeightsScaleShape2Texture, b_s );		// 1*20->1*20, just for rescaling
//
//    vector<float> v_b_s;
//    v_b_s.resize(b_s->cols);
//    for (unsigned int i = 0; i < v_b_s.size(); i++)
//    {
//        v_b_s[i] =  CV_MAT_ELEM( *b_s, float, 0, i );
//    }
//
//    this->VO_TextureProject2TruncatedEigenSpace(iTexture, b_t);     // 1*92385->1*127
//
//    b.clear();
//
//    for (unsigned int i = 0; i < b_s->cols; i++)
//    {
//        b.push_back ( CV_MAT_ELEM( *b_s, float, 0, i ) );
//    }
//
//    for (unsigned int i = 0; i < b_t->cols; i++)
//    {
//        b.push_back ( CV_MAT_ELEM( *b_t, float, 0, i ) );
//    }
//
//    // b is of 1*147
//    cvReleaseMat ( &b_s_temp );
//    cvReleaseMat ( &b_s );
//    cvReleaseMat ( &b_t );
//
//}


/**

@ note
Explained by JIA Pei, 2007-05-17
Refer to 
1) Cootes' "Comparing Variations on the Active Appearance Model Algorithm"
2) Cootes' "Statistical Models of Appearance for Computer Vision" page 56
3) Stegmann's" AAM-API, Basic Active Appearance Models"

This is for calculating the multivariate linear regression matrix
Explained by JIA Pei, 2007-05-17
According to Cootes' "Comparing Variations on the Active Appearance Model Algorithm",
there are three types of parameters, p = [c|t|u]. 
Here, we only consider the c and t parameters, why?
u is for texture transformation, so that the texture can be commensurate at the same scale 

Actually, the experiments designed in Stegmann's summary in AAM-API is just for estimating
the regression matrix R, in case of 1) the robustness; 2) make the training dataset more linear!!

It's explained in Cootes' Comparing Variations on the Active Appearance Model Algorithm that
"Delta(r)/Delta(p) is estimated by numeric differentiation, systematically displacing each parameter
from the known optimal value on typical images and computing an average over the training set."

Here, we could ignore the additional experimental samples, that's to say, the training data are just the 240
faces from the dataset. we set a variable to control this whether we need this additional man-made samples or not.
*/
void VO_AAMBasic::VO_BuildRegressionMatrices()
{
    // do model parameter experiments
    {
        // in VO_DoCParamExperiments, a matrix size of 92385*21120 is too big to be allocated
        this->VO_DoCParamExperiments();

        // estimate Rc using principal component regression	
        //this->VO_DoRegression( C, X,  m_pModel->m_R_c ); 
    }
    // do pose experiments
    {
        this->VO_DoPoseExperiments();

        // estimate Rt using principal component regression	
        //this->VO_DoRegression( C, X,  m_pModel->m_R_t );
    }

    // store negative versions to avoid any sign change at each iteration
    // m_pModel->m_R_c *= -1;
    // m_pModel->m_R_t *= -1;

    // gsl_multifit_linear_svd();
}



// Totally, there are 17280 samples for the experiments, every is of size 92385. 
// 92385*17280, too big to allocate. So, here, we use another methods for multivariate regression --
// by saving all those data in files.
void VO_AAMBasic::VO_DoCParamExperiments()
{
    int nExperiment = 0;
    CvMat* currentConcatenatedParameters = cvCreateMat(1, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    CvMat* currentTexture = cvCreateMat(1, this->m_iNbOfTextures, CV_32FC1);
    CvMat* currentShape = cvCreateMat(1, this->m_iNbOfShapes, CV_32FC1);
    VO_AAMShape2D currentShapeInstance;					// built from the parameters
    VO_AAMTexture2D3C currentTextureInstance;			// built from the parameters, but not sampled by using the shape parameters
    VO_AAMTexture2D3C delta_g;

    // for each training example in the training set
    for(unsigned int i = 0; i < this->m_iNbOfSamples; i ++)
    {
        nExperiment = 0;
        int totalExp4OneSample = this->m_vCDisps.size() * this->m_vCDisps[0].size();
        CvMat *X = cvCreateMat(this->m_iNbOfTextures, totalExp4OneSample, CV_32FC1);					// 92385*72
        CvMat *C = cvCreateMat(this->m_iNbOfTruncatedConcatenated, totalExp4OneSample, CV_32FC1);		// 18*72
        cvZero(C);
        CvPoint2D32f tempCvPoint2D32f;
        VO_AAMPoint2D tempPoint2d;

        for(unsigned int j = 0; j < this->m_vCDisps.size(); j++)			// 4
        {
            for (unsigned int k = 0; k < this->m_vCDisps[0].size(); k++)	// 18
            {
                // do displacement measures
                cvGetRow( this->m_CVMConcatenatedProject2Truncated, currentConcatenatedParameters, i );

                // adjust(shift) currentConcatenatedParameters to implement the experiments
                CV_MAT_ELEM(*currentConcatenatedParameters, float, 0, k) += this->m_vCDisps[j][k];

                // According to Cootes' "Comparing Variations on the Active Appearance Model Algorithm" - Equation (3)

                // Build the shape instance
                cvGEMM(currentConcatenatedParameters, this->m_CVMQs, 1, this->m_CVMAlignedShapesMean, 1, currentShape, CV_GEMM_B_T );

                // Build the texture instance
                cvGEMM(currentConcatenatedParameters, this->m_CVMQg, 1, this->m_CVMNormalizedTexturesMean, 1, currentTexture, CV_GEMM_B_T );

                currentShapeInstance.SetSize(this->m_iNbOfPoints);
                for (unsigned int n = 0; n < this->m_iNbOfPoints; n++)
                {
                    tempCvPoint2D32f.x = CV_MAT_ELEM(*currentShape, float, 0, n*2 );
                    tempCvPoint2D32f.y = CV_MAT_ELEM(*currentShape, float, 0, n*2+1 );
                    tempPoint2d.SetXY(tempCvPoint2D32f);
                    currentShapeInstance.SetPoint(tempPoint2d, n);
                }

                currentTextureInstance.SetSize(this->m_iNbOfTextures);
                for (unsigned int n = 0; n < this->m_iNbOfTextures; n++)
                {
                    currentTextureInstance.SetATexture(n, CV_MAT_ELEM(*currentTexture, float, 0, n ) );
                }


                //////////////////////////////////////////////////////////////////////////
                // Explained by JIA Pei, 2007-05-30
                // The following part is just for having a look at those shifted images during experiments

                //IplImage *TempFace = cvCloneImage(this->m_vImages[i]);
                //for (unsigned int j = 0; j < this->m_iNbOfPoints; j++)
                //{
                //	CvPoint pt1, pt2;
                //	pt1.x = currentShapeInstance.m_vPoint[j].m_CVxy.x;
                //	pt2.x = currentShapeInstance.m_vPoint[j].m_CVxy.x;
                //	pt1.y = currentShapeInstance.m_vPoint[j].m_CVxy.y;
                //	pt2.y = currentShapeInstance.m_vPoint[j].m_CVxy.y;

                //	// Never draw a rectangle on the face before AAM tracking/fitting
                //	cvRectangle( TempFace, pt1, pt2, colors[0], 3);
                //}
                //
                //stringstream ssj, ssk;
                //string strj, strk;
                //ssj << j;
                //ssk << k;
                //ssj >> strj;
                //ssk >> strk;
                //string filestr = this->m_vimgFiles[i].substr (14,9); 

                //string folderstr1 = "./cexperiment";
                //string folderstr2 = folderstr1 + "/" + strj;
                //string folderstr3 = folderstr2 + "/" + strk;
                //string folderfilestr = folderstr3 + "/" + filestr;

                //boost::filesystem::create_directory( folderstr1 );
                //boost::filesystem::create_directory( folderstr2 );
                //boost::filesystem::create_directory( folderstr3 );

                //cvSaveImage(folderfilestr.c_str(), TempFace);
                //cvReleaseImage(&TempFace);
                //////////////////////////////////////////////////////////////////////////

                // Align the built shape instance to the aligned annotated shape
                currentShapeInstance.AlignTo(this->m_vAAMShapes[i]);
                //				delta_g = this->VO_LoadOneStandardizedTextureFromShape(currentShapeInstance, this->m_vImages[i]) - currentTextureInstance;

                // Explained by JIA Pei. Here, the X matrix is too big to allocate a memory for calculation, 2007-05-30
                // insert the results into X and C
                for (unsigned int n = 0; n < this->m_iNbOfTextures; n++)
                {
                    CV_MAT_ELEM(*X, float, n, nExperiment) = delta_g.GetATexture(n);
                }
                CV_MAT_ELEM(*C, float, k, nExperiment) = this->m_vCDisps[j][k];

                ++nExperiment;
            }
            //cout << "Experiment" << nExperiment << "of" << X->cols << "done (c)..." << endl;
        }

        //////////////////////////////////////////////////////////////////////////
        // just in order to save the data
        // X, C

        /*
        string filestr = this->m_vimgFiles[i].substr (14,5) + ".txt"; 
        string folderstr1 = "./cexperiment";
        string folderstrX = folderstr1 + "/" + "X";
        string folderstrC = folderstr1 + "/" + "C";
        string folderfilestrX = folderstrX + "/" + filestr;
        string folderfilestrC = folderstrC + "/" + filestr;

        boost::filesystem::create_directory( folderstr1 );
        boost::filesystem::create_directory( folderstrX );
        boost::filesystem::create_directory( folderstrC );

        fstream fp;
        fp.open(folderfilestrX.c_str (), ios::out);
        fp << this->m_vimgFiles[i].substr (14,5) << "-X" << endl;
        for (unsigned int m = 0; m < X->rows; m++)
        {
        for (unsigned int n = 0; n < X->cols; n++)
        {			
        fp << CV_MAT_ELEM( *X, float, m, n) << " ";
        }
        fp << endl;
        }
        fp.close();

        fp.open(folderfilestrC.c_str (), ios::out);
        fp << this->m_vimgFiles[i].substr (14,5) << "-C" << endl;
        for (unsigned int m = 0; m < C->rows; m++)
        {
        for (unsigned int n = 0; n < C->cols; n++)
        {			
        fp << CV_MAT_ELEM( *C, float, m, n) << " ";
        }
        fp << endl;
        }
        fp.close();

        cvReleaseMat(&X);
        cvReleaseMat(&C);*/
        //////////////////////////////////////////////////////////////////////////
    }

    cvReleaseMat(&currentConcatenatedParameters);
    cvReleaseMat(&currentTexture);
    cvReleaseMat(&currentShape);
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Carry out pose displacement experiments

*/
void VO_AAMBasic::VO_DoPoseExperiments()
{
    int totalExp = this->m_vPoseDisps.size() * this->m_vPoseDisps[0].size() * this->m_iNbOfSamples;
    int nExperiment = 0;
    //X.resize( this->m_iNbOfTextures, totalExp );
    //C.resize( 4, totalExp );
    vector<float> delta_g;

    // for each training example in the (subsampled) training set
    for(unsigned int nShape = 0; nShape < this->m_iNbOfSamples; nShape ++)
    {
        // get the shape image
        // m_vImages

        /*
        for(unsigned int i=0;i<vPoseDisps.GetSize_Row();i++)
        {
        // do displacement measures
        this->PoseDisplacement( image, m_Shapes.m_vShapes[nShape], cVectors.GetARow (nShape), 
        vPoseDisps.GetARow(i), delta_g );

        // insert the results into X and C
        X.SetACol( nExperiment, delta_g );
        lv_vector<float> med = vPoseDisps.GetARow(i);
        C.SetACol( nExperiment, med);
        ++nExperiment;
        }
        cout << "Experiment" << nExperiment << "of" << totalExp << "done (pose)..." << endl;
        */
    }
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Carry out multi variate linear regression experiments

*/
void VO_AAMBasic::VO_DoRegression()
{

}



/**

@author     JIA Pei

@version    2008-03-04

@brief      Build gradient matrices

*/
void VO_AAMBasic::VO_BuildGradientMatrices()
{
    // do model parameter experiments
    {
        this->m_CVMCParamGradientMatrix = cvCreateMat(this->m_iNbOfTextures, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
        cvZero(this->m_CVMCParamGradientMatrix);

        this->VO_EstCParamGradientMatrix(this->m_CVMCParamGradientMatrix);

        // estimate Rc
        this->m_CVMRc = cvCreateMat(this->m_iNbOfTruncatedConcatenated, this->m_iNbOfTextures, CV_32FC1);

        CvMat* temp = cvCreateMat(this->m_CVMCParamGradientMatrix->cols, this->m_CVMCParamGradientMatrix->cols, CV_32FC1);

        cvGEMM(this->m_CVMCParamGradientMatrix, this->m_CVMCParamGradientMatrix, 1, NULL, 0, temp, CV_GEMM_A_T);
        cvInvert( temp, temp, CV_SVD );
        cvGEMM(temp, this->m_CVMCParamGradientMatrix, 1, NULL, 0, this->m_CVMRc, CV_GEMM_B_T);
        cvReleaseMat(&temp);
    }

    // do pose experiments, this is for global shape normalization
    {
        this->m_CVMPoseGradientMatrix = cvCreateMat(this->m_iNbOfTextures, 4, CV_32FC1);
        cvZero(this->m_CVMPoseGradientMatrix);

        this->VO_EstPoseGradientMatrix(this->m_CVMPoseGradientMatrix);

        // estimate Rt
        this->m_CVMRt = cvCreateMat(4, this->m_iNbOfTextures, CV_32FC1);

        CvMat* temp = cvCreateMat(this->m_CVMPoseGradientMatrix->cols, this->m_CVMPoseGradientMatrix->cols, CV_32FC1);

        cvGEMM(this->m_CVMPoseGradientMatrix, this->m_CVMPoseGradientMatrix, 1, NULL, 0, temp, CV_GEMM_A_T);
        cvInvert( temp, temp, CV_SVD );
        cvGEMM(temp, this->m_CVMPoseGradientMatrix, 1, NULL, 0, this->m_CVMRt, CV_GEMM_B_T);
        cvReleaseMat(&temp);
    }
}



/**

@author     JIA Pei

@version    2008-03-04

@brief      Build gradient matrices in terms of C parameters

@param      oCParamGM   - Output    the built gradient matrix in terms of C parameters

*/
void VO_AAMBasic::VO_EstCParamGradientMatrix(CvMat* oCParamGM)
{
    int nExperiment = 0;
    CvMat* currentConcatenatedParameters = cvCreateMat(1, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    CvMat* currentConcatenatedParametersPositiveDisp = cvCreateMat(1, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    CvMat* currentConcatenatedParametersNegativeDisp = cvCreateMat(1, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    CvMat* currentShapePositive = cvCreateMat(1, this->m_iNbOfShapes, CV_32FC1);
    CvMat* currentShapeNegative = cvCreateMat(1, this->m_iNbOfShapes, CV_32FC1);
    CvMat* currentTexturePositive = cvCreateMat(1, this->m_iNbOfTextures, CV_32FC1);
    CvMat* currentTextureNegative = cvCreateMat(1, this->m_iNbOfTextures, CV_32FC1);
    VO_AAMShape2D currentShapeInstancePositive;
    VO_AAMShape2D currentShapeInstanceNegative;
    VO_AAMTexture2D3C currentTextureInstancePositive;
    VO_AAMTexture2D3C currentTextureInstanceNegative;
    currentShapeInstancePositive.SetSize(this->m_iNbOfPoints);
    currentShapeInstanceNegative.SetSize(this->m_iNbOfPoints);
    currentTextureInstancePositive.SetSize(this->m_iNbOfTextures);
    currentTextureInstanceNegative.SetSize(this->m_iNbOfTextures);

    VO_AAMTexture2D3C delta_g1, delta_g2, cDiff;

    // for each training example in the training set
    for(unsigned int i = 0; i < this->m_iNbOfSamples; i ++)
    {
        for(unsigned int j = 0; j < this->m_vCDisps.size(); j = j+2)		// 4
        {
            for (unsigned int k = 0; k < this->m_vCDisps[0].size(); k++)	// 18
            {
                // do displacement measures
                cvGetRow( this->m_CVMConcatenatedProject2Truncated, currentConcatenatedParameters, i );

                cvCopy(currentConcatenatedParameters, currentConcatenatedParametersNegativeDisp);
                cvCopy(currentConcatenatedParameters, currentConcatenatedParametersPositiveDisp);

                // adjust(shift) currentConcatenatedParameters ... for calculating the Jacobian Matrix
                CV_MAT_ELEM(*currentConcatenatedParametersNegativeDisp, float, 0, k) = 
                    CV_MAT_ELEM(*currentConcatenatedParameters, float, 0, k) + this->m_vCDisps[j][k];
                CV_MAT_ELEM(*currentConcatenatedParametersPositiveDisp, float, 0, k) = 
                    CV_MAT_ELEM(*currentConcatenatedParameters, float, 0, k) + this->m_vCDisps[j+1][k];

                // Build the shape instance from the combined model
                cvGEMM(currentConcatenatedParametersPositiveDisp, this->m_CVMQs, 1, this->m_CVMAlignedShapesMean, 1, currentShapePositive, CV_GEMM_B_T );
                cvGEMM(currentConcatenatedParametersNegativeDisp, this->m_CVMQs, 1, this->m_CVMAlignedShapesMean, 1, currentShapeNegative, CV_GEMM_B_T );
                currentShapeInstancePositive = VO_AAMShape2D::VO_CvMat2AAMShape2D(currentShapePositive);
                currentShapeInstanceNegative = VO_AAMShape2D::VO_CvMat2AAMShape2D(currentShapeNegative);

                // Build the texture instance from the combined model
                cvGEMM(currentConcatenatedParametersPositiveDisp, this->m_CVMQg, 1, this->m_CVMNormalizedTexturesMean, 1, currentTexturePositive, CV_GEMM_B_T );
                cvGEMM(currentConcatenatedParametersNegativeDisp, this->m_CVMQg, 1, this->m_CVMNormalizedTexturesMean, 1, currentTextureNegative, CV_GEMM_B_T );
                currentTextureInstancePositive = VO_AAMTexture2D3C::VO_CvMat2AAMTexture(currentTexturePositive);
                currentTextureInstanceNegative = VO_AAMTexture2D3C::VO_CvMat2AAMTexture(currentTextureNegative);

                // Align from the displacement alignedshape to the shape of original size
                currentShapeInstancePositive.AlignTo(this->m_vAAMShapes[i]);
                currentShapeInstanceNegative.AlignTo(this->m_vAAMShapes[i]);

                // Obtain the original texture information from shape instance
                VO_AAM::VO_LoadOneTextureFromShape(currentShapeInstancePositive, this->m_vImages[i], this->m_vAAMTriangle2D, this->m_vPointWarpInfo, delta_g2);
                VO_AAM::VO_LoadOneTextureFromShape(currentShapeInstanceNegative, this->m_vImages[i], this->m_vAAMTriangle2D, this->m_vPointWarpInfo, delta_g1);

                // The following codes are just for intermediate display
                //IplImage *tempImage2 = cvCloneImage(this->m_vImages[i]);
                //IplImage *tempImage1 = cvCloneImage(this->m_vImages[i]);
                //VO_AAM::VO_PutOneTextureToTemplateShape(delta_g2, this->m_vAAMTriangle2D, tempImage2);
                //VO_AAM::VO_PutOneTextureToTemplateShape(delta_g1, this->m_vAAMTriangle2D, tempImage1);

                //IplImage *resImage2 = cvCloneImage(this->m_vImages[i]);
                //IplImage *resImage1 = cvCloneImage(this->m_vImages[i]);
                //VO_AAM::VO_WarpFromOneShapeToAnother(this->m_VOAAMReferenceShape, currentShapeInstancePositive, this->m_vAAMTriangle2D, tempImage2, resImage2);
                //VO_AAM::VO_WarpFromOneShapeToAnother(this->m_VOAAMReferenceShape, currentShapeInstanceNegative, this->m_vAAMTriangle2D, tempImage1, resImage1);

                //cvSaveImage("tempImage2.jpg", tempImage2);
                //cvSaveImage("tempImage1.jpg", tempImage1);
                //cvSaveImage("resImage2.jpg", resImage2);
                //cvSaveImage("resImage1.jpg", resImage1);

                // Normalize the extracted(loaded) textures
                delta_g2.Standardize();
                delta_g1.Standardize();

                delta_g2 -= currentTextureInstancePositive;
                delta_g1 -= currentTextureInstanceNegative;

                // form central difference
                cDiff = (delta_g2-delta_g1)/(this->m_vCDisps[j+1][k] - this->m_vCDisps[j][k]);

                for (unsigned int n = 0; n < this->m_iNbOfTextures; n++)
                {
                    CV_MAT_ELEM(*oCParamGM, float, n, k) += cDiff.GetATexture(n);
                }

                nExperiment++;
            }
        }
    }

    // normalize
    for(unsigned int i = 0; i < this->m_iNbOfTextures; i++)
    {
        for (unsigned int j = 0; j < this->m_iNbOfTruncatedConcatenated; j++)
        {
            // this->m_CVMCParamGradientMatrix is a summation for 240 pictures (size of this->m_iNbOfSamples)
            // 4 -- refer to 4 possible shifts for just one parameter (18 parameters altogether)
            // 2 -- every pair of shifts, we got only one cDiff, which is actually stored in this->m_CVMCParamGradientMatrix
            CV_MAT_ELEM(*oCParamGM, float, i, j) /= (this->m_iNbOfSamples * 4 / 2);
        }
    }

    cvReleaseMat(&currentConcatenatedParameters);
    cvReleaseMat(&currentConcatenatedParametersPositiveDisp);
    cvReleaseMat(&currentConcatenatedParametersNegativeDisp);
    cvReleaseMat(&currentShapePositive);
    cvReleaseMat(&currentShapeNegative);
    cvReleaseMat(&currentTexturePositive);
    cvReleaseMat(&currentTextureNegative);

}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Build gradient matrices in terms of pose

@param      oPoseGM   - Output    the built gradient matrix in terms of poses

*/
void VO_AAMBasic::VO_EstPoseGradientMatrix(CvMat* oPoseGM)
{
    int nExperiment = 0;
    CvMat* currentConcatenatedParameters = cvCreateMat(1, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    CvMat* currentTexture = cvCreateMat(1, this->m_iNbOfTextures, CV_32FC1);
    CvMat* currentShape = cvCreateMat(1, this->m_iNbOfShapes, CV_32FC1);

    VO_AAMShape2D currentShapeInstance, currentShapeInstance1, currentShapeInstance2;
    VO_AAMTexture2D3C currentTextureInstance;
    VO_AAMTexture2D3C delta_g1, delta_g2, cDiff;
    currentShapeInstance.SetSize(this->m_iNbOfPoints);
    currentShapeInstance1.SetSize(this->m_iNbOfPoints);
    currentShapeInstance2.SetSize(this->m_iNbOfPoints);
    currentTextureInstance.SetSize(this->m_iNbOfTextures);
    vector<float> disp1, disp2;
    disp1.resize(4);
    disp2.resize(4);
    vector<float> posedisp1, posedisp2;
    posedisp1.resize(4);
    posedisp2.resize(4);

    // just for displacement
    CvPoint2D32f translation = cvPoint2D32f(0.0, 0.0);
    float scale = 1.0f, theta = 0.0f;

    // for each training example in the training set
    for(unsigned int i = 0; i < this->m_iNbOfSamples; i ++)
    {
        for(unsigned int j = 0; j < this->m_vPoseDisps.size(); j = j+2)		// 4
        {
            for (unsigned int k = 0; k < this->m_vPoseDisps[0].size(); k++)	// 4
            {
                for (unsigned int p = 0; p < posedisp1.size(); p++)
                {
                    posedisp1[p] = 0;
                    posedisp2[p] = 0;
                }

                posedisp1[k] = this->m_vPoseDisps[j][k];
                posedisp2[k] = this->m_vPoseDisps[j+1][k];

                this->VO_DisplacementVector2Pose(posedisp1, disp1);
                this->VO_DisplacementVector2Pose(posedisp2, disp2);

                cvGetRow( this->m_CVMConcatenatedProject2Truncated, currentConcatenatedParameters, i );

                // Build the shape instance from the combined model
                cvGEMM(currentConcatenatedParameters, this->m_CVMQs, 1, this->m_CVMAlignedShapesMean, 1, currentShape, CV_GEMM_B_T );
                currentShapeInstance = VO_AAMShape2D::VO_CvMat2AAMShape2D(currentShape);

                // Build the texture instance from the combined model
                cvGEMM(currentConcatenatedParameters, this->m_CVMQg, 1, this->m_CVMNormalizedTexturesMean, 1, currentTexture, CV_GEMM_B_T );
                currentTextureInstance = VO_AAMTexture2D3C::VO_CvMat2AAMTexture(currentTexture);

                // Calculate the align to information
                currentShapeInstance.AlignTransformation(this->m_vAAMShapes[i], scale, theta, translation);

                currentShapeInstance1 = currentShapeInstance;
                currentShapeInstance2 = currentShapeInstance;

                currentShapeInstance1.Scale(disp1[0] * scale);
                currentShapeInstance2.Scale(disp2[0] * scale);
                currentShapeInstance1.Rotate(disp1[1] + theta);
                currentShapeInstance2.Rotate(disp2[1] + theta);
                currentShapeInstance1.Translate(disp1[2] + translation.x, disp1[3] + translation.y);
                currentShapeInstance2.Translate(disp2[2] + translation.x, disp2[3] + translation.y);

                VO_AAM::VO_LoadOneTextureFromShape(currentShapeInstance1, this->m_vImages[i], this->m_vAAMTriangle2D, this->m_vPointWarpInfo, delta_g1);
                VO_AAM::VO_LoadOneTextureFromShape(currentShapeInstance2, this->m_vImages[i], this->m_vAAMTriangle2D, this->m_vPointWarpInfo, delta_g2);

                // The following codes are just for intermediate display
                //IplImage *tempImage2 = cvCloneImage(this->m_vImages[i]);
                //IplImage *tempImage1 = cvCloneImage(this->m_vImages[i]);
                //VO_AAM::VO_PutOneTextureToTemplateShape(delta_g2, this->m_vAAMTriangle2D, tempImage2);
                //VO_AAM::VO_PutOneTextureToTemplateShape(delta_g1, this->m_vAAMTriangle2D, tempImage1);

                //IplImage *resImage2 = cvCloneImage(this->m_vImages[i]);
                //IplImage *resImage1 = cvCloneImage(this->m_vImages[i]);
                //VO_AAM::VO_WarpFromOneShapeToAnother(this->m_VOAAMReferenceShape, currentShapeInstance2, this->m_vAAMTriangle2D, tempImage2, resImage2);
                //VO_AAM::VO_WarpFromOneShapeToAnother(this->m_VOAAMReferenceShape, currentShapeInstance1, this->m_vAAMTriangle2D, tempImage1, resImage1);

                //// Normalize the extracted(loaded) textures
                //delta_g2.Standardize();
                //delta_g1.Standardize();

                //cvSaveImage("tempImage2.jpg", tempImage2);
                //cvSaveImage("tempImage1.jpg", tempImage1);
                //cvSaveImage("resImage2.jpg", resImage2);
                //cvSaveImage("resImage1.jpg", resImage1);

                delta_g2 -= currentTextureInstance;
                delta_g1 -= currentTextureInstance;

                // form central difference
                cDiff = (delta_g2-delta_g1)/(this->m_vPoseDisps[j+1][k] - this->m_vPoseDisps[j][k]);

                for (unsigned int n = 0; n < this->m_iNbOfTextures; n++)
                {
                    CV_MAT_ELEM(*oPoseGM, float, n, k) += cDiff.GetATexture(n);
                }

                nExperiment++;
            }
        }
    }

    // normalize
    for(unsigned int i = 0; i < this->m_iNbOfTextures; i++)
    {
        for (unsigned int j = 0; j < 4; j++)
        {
            // this->m_CVMCParamGradientMatrix is a summation for 240 pictures (size of this->m_iNbOfSamples)
            // 4 -- refer to 4 possible shifts for just one parameter (18 parameters altogether)
            // 2 -- every pair of shifts, we got only one cDiff, which is actually stored in this->m_CVMCParamGradientMatrix
            CV_MAT_ELEM(*oPoseGM, float, i, j) /= (this->m_iNbOfSamples * 4 / 2);
        }
    }

    cvReleaseMat(&currentConcatenatedParameters);
    cvReleaseMat(&currentTexture);
    cvReleaseMat(&currentShape);

}



/**

@author     JIA Pei

@version    2008-03-20

@brief      Concatenated parameters back project to shape and texture parameters

@param      inC             Input - input concatenated parameters

@param      outS            Output - back projected shape model parameters

@param      outT            Output - back projected texture model parameters

@return     nothing

*/
void VO_AAMBasic::VO_CParamBackProjectToSParamTParam(const CvMat* inC, CvMat* outS, CvMat* outT)
{
    CvMat* tempConcatenated = cvCreateMat(1, this->m_iNbOfConcatenated, CV_32FC1);
    cvZero(tempConcatenated);
    cvBackProjectPCA(inC, this->m_CVMConcatenatedMean, this->m_CVMTruncatedConcatenatedEigenVectors, tempConcatenated);

    for (unsigned int i = 0; i < this->m_iNbOfTruncatedShapes; i++)
    {
        CV_MAT_ELEM( *outS, float, 0, i ) = CV_MAT_ELEM( *tempConcatenated, float, 0, i );
    }

    CvMat* Inverseofm_CVMWeightsScaleShape2Texture = cvCreateMat(this->m_iNbOfTruncatedShapes, this->m_iNbOfTruncatedShapes, CV_32FC1);
    cvInvert( this->m_CVMWeightsScaleShape2Texture, Inverseofm_CVMWeightsScaleShape2Texture, CV_SVD );
    cvMatMul( outS, Inverseofm_CVMWeightsScaleShape2Texture, outS );

    for (unsigned int i = 0; i < this->m_iNbOfTruncatedTextures; i++)
    {
        CV_MAT_ELEM( *outT, float, 0, i ) = CV_MAT_ELEM( *tempConcatenated, float, 0, i+this->m_iNbOfTruncatedShapes );
    }

    cvReleaseMat(&Inverseofm_CVMWeightsScaleShape2Texture);
    cvReleaseMat(&tempConcatenated);
}



/**

@author     JIA Pei

@version    2008-03-20

@brief      Concatenated parameters back project to aligned shape

@param      inC             Input - input concatenated parameters

@return     VO_AAMShape2D   - the back projected shape

*/
VO_AAMShape2D VO_AAMBasic::VO_CParamBackProjectToAlignedShape(const CvMat* inC)
{
    CvMat* oShape = cvCreateMat(1, this->m_iNbOfShapes, CV_32FC1);

    // c back to shape
    cvGEMM(inC, this->m_CVMQs, 1, this->m_CVMAlignedShapesMean, 1, oShape, CV_GEMM_A_T + CV_GEMM_B_T);
    VO_AAMShape2D res   =   VO_AAMShape2D::VO_CvMat2AAMShape2D(oShape);

    cvReleaseMat(&oShape);

    return res;

}



/**

@author     JIA Pei

@version    2008-03-20

@brief      Concatenated parameters back project to aligned shape

@param      inC             Input - input concatenated parameters

@return     VO_AAMShape2D   - the back projected shape

*/
VO_AAMTexture2D3C VO_AAMBasic::VO_CParamBackProjectToNormalizedTexture(const CvMat* inC)
{
    CvMat* oTexture = cvCreateMat(1, this->m_iNbOfTextures, CV_32FC1);

    // c back to texture
    cvGEMM(inC, this->m_CVMQg, 1, this->m_CVMNormalizedTexturesMean, 1, oTexture, CV_GEMM_A_T + CV_GEMM_B_T);

    VO_AAMTexture2D3C res   =   VO_AAMTexture2D3C::VO_CvMat2AAMTexture(oTexture);

    cvReleaseMat(&oTexture);

    return res;
}




/**

@author     JIA Pei

@version    2008-03-04

@brief      generate displacement vectors for later numerical analysis

@note       Explained by Stegmann: sets must *always* be in anti-symmetric pairs with the largest displacements first, 
            e.g. [ -.5, .5, -.25, .25 ]
            According to (7.7) from Stegmann's AAM-API, there will be n=m(4k+24)=240*(4*18+24)=23040 displacements.
            However, here, for the pose displacements, the amount is not 24 but 16 instead. n=m(4k+16)=240*(4*18+16)=21120

@ref        1) Cootes' "Comparing Variations on the Active Appearance Model Algorithm"
            2) Cootes' "Statistical Models of Appearance for Computer Vision" page 56
            3) Stegmann's" AAM-API, Basic Active Appearance Models". 

*/
void VO_AAMBasic::VO_CreateDisplacementSets()
{
    // generate c and pose displacement sets
    vector<float> vStdDisp;
    vector<float> vXYDisp;
    vector<float> vScaleDisp;
    vector<float> vRotDisp;
    vStdDisp.resize(4);
    vXYDisp.resize(4);
    vScaleDisp.resize(4);
    vRotDisp.resize(4);

    // Mikkel B. Stegmann's master's thesis displacement set
    vStdDisp[0] = -.5f;
    vStdDisp[1] =  .5f;
    vStdDisp[2] = -.25f;
    vStdDisp[3] =  .25f;

    // relative displacement set
    vScaleDisp[0] =  .95f;
    vScaleDisp[1] = 1.05f;
    vScaleDisp[2] =  .85f;
    vScaleDisp[3] = 1.15f;

    // in radian measure
    vRotDisp[0] = (float)(-5.0 / 180.0 * M_PI);
    vRotDisp[1] = (float)( 5.0 / 180.0 * M_PI);
    vRotDisp[2] = (float)(-15.0 / 180.0 * M_PI);
    vRotDisp[3] = (float)( 15.0 / 180.0 * M_PI);

    vXYDisp[0] = -.05f;
    vXYDisp[1] =  .05f;
    vXYDisp[2] = -.10f;
    vXYDisp[3] =  .10f;

    // Pre-process for X, Y pose parameters
    vector<float> vXDisp, vYDisp;
    vXDisp.resize (vXYDisp.size());
    vYDisp.resize (vXYDisp.size());
    for (unsigned int i = 0; i < vXYDisp.size(); i++)
    {
        vXDisp[i] = vXYDisp[i] * this->m_CVSizeOfTemplateFace.width;
        vYDisp[i] = vXYDisp[i] * this->m_CVSizeOfTemplateFace.height;
    }

    this->m_vCDisps = VO_AAMBasic::VO_CalcCParamDisplacementVectors( vStdDisp, this->m_CVMTruancatedConcatenatedSD );
    this->m_vPoseDisps = VO_AAMBasic::VO_CalcPoseDisplacementVectors( vScaleDisp, vRotDisp, vXDisp, vYDisp );

}



/**

@author		Stegmann, JIA Pei

@version	2008-03-04

@brief		Generates a set of combined model parameter displacement vectors
            where each parameter is displaced at a time according to the values in vStdDisp.

@param		vStdDisp	    Input - A vector of parameter displacements in standard deviations of the corresponding parameter.

@param		ConcatenatedSD	Input - Concatenated standard deviation

@return		A vector of displacement vectors.

*/
vector<vector<float> > VO_AAMBasic::VO_CalcCParamDisplacementVectors(const vector<float>& vStdDisp, const CvMat* ConcatenatedSD)
{
    vector<vector<float> >	cDisplacements;
    cDisplacements.resize( vStdDisp.size() );
    for (unsigned int i = 0; i < cDisplacements.size(); i++)
    {
        cDisplacements[i].resize( ConcatenatedSD->cols );
    }

    // calculate displacements, for each parameter
    for (unsigned int i = 0; i < cDisplacements.size(); i++)
    {
        for(unsigned int j = 0; j < cDisplacements[0].size(); j++)
        {
            // for each displacement
            cDisplacements[i][j] = vStdDisp[i] * CV_MAT_ELEM(*ConcatenatedSD, float, 0, j);
        }
    }

    return cDisplacements;
}


/**

@author		Stegmann, JIA Pei

@version	2008-03-04

@brief		Generates a set of pose parameter displacement vectors
            where each parameter is displaced at a time according to the values in vPose.
            Cootes' "Statistical Models of Appearance for Computer Vision" page 45
            (s, theta, tx, ty) ->( sx, sy, tx, ty)

@param		vPose	    Input   - A vector of pose displacement parameters.

@param		vDisp	    Output  - A vector of real displacements.

@return		A vector of displacement vectors.

@note       For angle - vPose[1], always in radian measure.

*/
void VO_AAMBasic::VO_Pose2DisplacementVector( const vector<float>& vPose, vector<float> &vDisp, int method)
{
    vDisp.resize( vPose.size() );

    switch(method)
    {
    case DIRECTMAP:
        // add scale, rotation, x and y displacements
        {
            vDisp[0] = vPose[0] - 1.0f;
            vDisp[1] = vPose[1];
            vDisp[2] = vPose[2];
            vDisp[3] = vPose[3];
        }
        break;
    case LINEARIZEMAP:
        // add scale, rotation, x and y displacements
        {
            vDisp[0] = vPose[0]*cos( vPose[1] ) - 1.0f;
            vDisp[1] = vPose[0]*sin( vPose[1] );
            vDisp[2] = vPose[2];
            vDisp[3] = vPose[3];
        }
        break;
    }

}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Change from displacement vector to pose parameters

*/
void VO_AAMBasic::VO_DisplacementVector2Pose(const vector<float> &vDisp, vector<float>& vPose, int method)
{
    vPose.resize(4);

    switch(method)
    {
    case DIRECTMAP:
        {
            vPose[0] = vDisp[0]+1.0f;
            vPose[1] = vDisp[1];
            vPose[2] = vDisp[2];
            vPose[3] = vDisp[3];
        }
        break;
    case LINEARIZEMAP:
        {
            vPose[0] = sqrt ( (1.0f + vDisp[0]) * (1.0f + vDisp[0] ) + vDisp[1] * vDisp[1] );		// scale
            vPose[1] = (float) atan2( (float)vDisp[1], (float)(1.0 + vDisp[0]) );					// rotation
            vPose[2] = vDisp[2];																	// translation x
            vPose[3] = vDisp[3];																	// translation y
        }
        break;
    }

}



/**

@author     JIA Pei

@version    2008-03-04

@brief      Build displacement sets for Pose parameters

@param      vScaleDisp      Input - all input shapes

@param      vRotDisp        Input - all input images

@param      vXDisp          Input - 1 or 3

@param      vYDisp          Input - texture build method

@param      method          Input - truncated percentage for shape model

@return     vector<vector<float> > - all pose displacement vectors
*/
vector<vector<float> >	VO_AAMBasic::VO_CalcPoseDisplacementVectors(const vector<float> &vScaleDisp, 
                                                                    const vector<float> &vRotDisp,
                                                                    const vector<float> &vXDisp,
                                                                    const vector<float> &vYDisp, 
                                                                    int method)
{
    vector<vector<float> > poseDisplacements;
    poseDisplacements.resize( vXDisp.size() );
    for (unsigned int i = 0; i < poseDisplacements.size(); i++)
    {
        poseDisplacements[i].resize( 4 );	// 4 refers to X, Y, Scale, and Rotate
    }

    vector<float> singlePose;
    singlePose.resize( 4 );

    for(unsigned int i = 0; i < poseDisplacements.size(); i++)
    {
        singlePose[0] = vScaleDisp[i];
        singlePose[1] = vRotDisp[i];
        singlePose[2] = vXDisp[i];
        singlePose[3] = vYDisp[i];

        VO_AAMBasic::VO_Pose2DisplacementVector( singlePose, poseDisplacements[i], method);
    }

    return poseDisplacements;
}





/**
Load concatenated data
*/
void VO_AAMBasic::VO_LoadConcatenated()
{
    this->m_CVMWeightsScaleShape2Texture	= cvCreateMat ( this->m_iNbOfTruncatedShapes, this->m_iNbOfTruncatedShapes, CV_32FC1);
    CvScalar SumOfEigenValues_Shape			= cvSum( this->m_CVMTruncatedAlignedShapesEigenValues );
    CvScalar SumOfEigenValues_Texture		= cvSum( this->m_CVMTruncatedNormalizedTexturesEigenValues );
    float val = (float)(SumOfEigenValues_Texture.val[0] / SumOfEigenValues_Shape.val[0]);
    cvSetIdentity( this->m_CVMWeightsScaleShape2Texture, cvRealScalar(val) );

    CvMat* WeightedAlignedShapesProject2Truncated = cvCreateMat (this->m_iNbOfSamples, this->m_iNbOfTruncatedShapes, CV_32FC1);
    cvMatMul( this->m_CVMAlignedShapesProject2Truncated, this->m_CVMWeightsScaleShape2Texture, WeightedAlignedShapesProject2Truncated );		// (240*20)*(20*20)->240*20, just for rescaling

    this->m_iNbOfConcatenated = this->m_iNbOfTruncatedShapes + this->m_iNbOfTruncatedTextures;
    this->m_CVMConcatenated = cvCreateMat( this->m_iNbOfSamples, this->m_iNbOfConcatenated, CV_32FC1 );
    this->m_vAAMConcatenated.resize(this->m_iNbOfSamples);
    for (unsigned int i = 0; i < this->m_vAAMConcatenated.size(); i++)
    {
        this->m_vAAMConcatenated[i].resize(this->m_iNbOfConcatenated);
    }

    for(unsigned int i=0; i < this->m_iNbOfSamples; i++)
    {
        for(unsigned int j = 0; j < this->m_iNbOfTruncatedShapes; j++)
        {
            CV_MAT_ELEM(*this->m_CVMConcatenated, float, i, j) = CV_MAT_ELEM(*WeightedAlignedShapesProject2Truncated, float, i, j);
            this->m_vAAMConcatenated[i][j] = CV_MAT_ELEM( *this->m_CVMConcatenated, float, i, j );
        }
        for(unsigned int j = this->m_iNbOfTruncatedShapes; j < this->m_iNbOfConcatenated; j++)
        {
            CV_MAT_ELEM(*this->m_CVMConcatenated, float, i, j) = CV_MAT_ELEM(*this->m_CVMNormalizedTexturesProject2Truncated, float, i, j - this->m_iNbOfTruncatedShapes );
            this->m_vAAMConcatenated[i][j] = CV_MAT_ELEM( *this->m_CVMConcatenated, float, i, j );
        }
    }
    
    cvReleaseMat(&WeightedAlignedShapesProject2Truncated);
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      build Basic AAM - Cootes method

@param      vShapes     Input - all input shapes

@param      imgs        Input - all input images

@param      channel     Input - 1 or 3

@param      tbm         Input - texture build method

@param      TPShape     Input - truncated percentage for shape model

@param      TPTexture   Input - truncated percentage for texture model

@param      TPConcatenated   Input - truncated percentage for concatenated

@note       Refer to "Statistical Models of Appearance for Computer Vision" page 31, Cootes
*/
void VO_AAMBasic::VO_BuildAAMBasic(const vector<VO_AAMShape2D> &vShapes, const vector<IplImage*> &imgs, 
                                   int channel, int tbm, float TPShape, float TPTexture, float TPConcatenated)
{
    this->VO_BuildAAM(vShapes, imgs, channel, tbm, TPShape, TPTexture);
    this->m_fTruncatedPercent_Concatenated = TPConcatenated;

    this->VO_LoadConcatenated();

    this->m_iNbOfEigenConcatenatedAtMost = MIN(this->m_iNbOfSamples, this->m_iNbOfConcatenated);

    this->m_CVMConcatenatedEigenValues  = cvCreateMat( 1, this->m_iNbOfEigenConcatenatedAtMost, CV_32FC1 );
    this->m_CVMConcatenatedEigenVectors = cvCreateMat( this->m_iNbOfEigenConcatenatedAtMost, this->m_iNbOfConcatenated, CV_32FC1 );
    this->m_CVMConcatenatedMean         = cvCreateMat( 1, this->m_iNbOfConcatenated, CV_32FC1 );

    // do normal PCA
    cvCalcPCA( this->m_CVMConcatenated, this->m_CVMConcatenatedMean, this->m_CVMConcatenatedEigenValues, this->m_CVMConcatenatedEigenVectors, CV_PCA_DATA_AS_ROW /*+ CV_PCA_USE_AVG*/ );

    this->m_vAAMMeanConcatenated.resize(this->m_iNbOfConcatenated);
    for(unsigned int i = 0; i < this->m_iNbOfConcatenated; i++)
        this->m_vAAMMeanConcatenated[i] = CV_MAT_ELEM( *this->m_CVMConcatenatedMean, float, 0, i );

    CvScalar SumOfEigenValues = cvSum( this->m_CVMConcatenatedEigenValues );

    float limit = (float)( FLT_EPSILON * SumOfEigenValues.val[0] );

    unsigned int i = 0;
    for(i = 0; i < this->m_iNbOfEigenConcatenatedAtMost; i++)
    {
        if ( CV_MAT_ELEM( *this->m_CVMConcatenatedEigenValues, float, 0, i ) < limit) break;
    }

    // NonZero EigenValues
    unsigned int NbOfNonZeroEigenValues = i;

    for(i = NbOfNonZeroEigenValues; i < this->m_iNbOfEigenConcatenatedAtMost; i++)
    {
        CV_MAT_ELEM( *this->m_CVMConcatenatedEigenValues, float, 0, i ) = 0.0f;
    }

    SumOfEigenValues = cvSum( this->m_CVMConcatenatedEigenValues );
    float ps = 0.0f;
    this->m_iNbOfTruncatedConcatenated = 0;

    for(unsigned int i = 0; i < NbOfNonZeroEigenValues; i++)
    {
        ps += CV_MAT_ELEM( *this->m_CVMConcatenatedEigenValues, float, 0, i );
        ++this->m_iNbOfTruncatedConcatenated;
        if( ps/SumOfEigenValues.val[0] >= this->m_fTruncatedPercent_Concatenated) break;
    }

    this->m_CVMTruncatedConcatenatedEigenValues = cvCreateMat( 1, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    this->m_CVMTruncatedConcatenatedEigenVectors = cvCreateMat( this->m_iNbOfTruncatedConcatenated, this->m_iNbOfConcatenated, CV_32FC1);

    for(unsigned int i = 0; i < this->m_iNbOfTruncatedConcatenated; i++)
    {
        CV_MAT_ELEM( *this->m_CVMTruncatedConcatenatedEigenValues, float, 0, i )
            = CV_MAT_ELEM( *this->m_CVMConcatenatedEigenValues, float, 0, i );

        for (unsigned int j = 0; j < this->m_iNbOfConcatenated; j++)
        {
            CV_MAT_ELEM( *this->m_CVMTruncatedConcatenatedEigenVectors, float, i, j )
                = CV_MAT_ELEM( *this->m_CVMConcatenatedEigenVectors, float, i, j );
        }
    }

    this->m_CVMConcatenatedProject2Truncated = cvCreateMat(this->m_iNbOfSamples, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    cvProjectPCA( this->m_CVMConcatenated, this->m_CVMConcatenatedMean, this->m_CVMTruncatedConcatenatedEigenVectors, this->m_CVMConcatenatedProject2Truncated );

    this->m_CVMTruancatedConcatenatedMean   = cvCreateMat( 1, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    this->m_CVMTruancatedConcatenatedSD     = cvCreateMat( 1, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    this->m_CVMConcatenatedSD               = cvCreateMat( 1, this->m_iNbOfConcatenated, CV_32FC1);

    VO_Common::VO_CalcSamplesMeanNSD(this->m_CVMConcatenatedProject2Truncated, this->m_CVMTruancatedConcatenatedMean, this->m_CVMTruancatedConcatenatedSD);
    VO_Common::VO_CalcSamplesMeanNSD(this->m_CVMConcatenated, this->m_CVMConcatenatedMean, this->m_CVMConcatenatedSD);

    // extract the shape part of the combined eigen vectors
    CvRect rects = cvRect( 0, 0, this->m_iNbOfTruncatedShapes, this->m_iNbOfTruncatedConcatenated);
    this->m_CVMPcs = cvCreateMat (this->m_iNbOfTruncatedConcatenated, this->m_iNbOfTruncatedShapes, CV_32FC1);
    cvGetSubRect( this->m_CVMTruncatedConcatenatedEigenVectors, this->m_CVMPcs, rects );

    // extract the texture part of the combined eigen vectors
    CvRect rectg = cvRect( this->m_iNbOfTruncatedShapes, 0, this->m_iNbOfTruncatedTextures, this->m_iNbOfTruncatedConcatenated);
    this->m_CVMPcg = cvCreateMat (this->m_iNbOfTruncatedConcatenated, this->m_iNbOfTruncatedTextures, CV_32FC1);
    cvGetSubRect( this->m_CVMTruncatedConcatenatedEigenVectors, this->m_CVMPcg, rectg );

    // calculate m_CVMQs = m_CVMTruncatedAlignedShapesEigenVectors * m_CVMWeightsScaleShape2Texture^{-1} * m_CVMPcs
    CvMat* inverseOf_m_CVMWeightsScaleShape2Texture = cvCreateMat (this->m_CVMWeightsScaleShape2Texture->cols, 
        this->m_CVMWeightsScaleShape2Texture->rows, CV_32FC1);
    cvInvert( this->m_CVMWeightsScaleShape2Texture, inverseOf_m_CVMWeightsScaleShape2Texture, CV_SVD );

    CvMat* temp = cvCreateMat (this->m_iNbOfShapes, this->m_iNbOfTruncatedShapes, CV_32FC1);
    cvGEMM(this->m_CVMTruncatedAlignedShapesEigenVectors, inverseOf_m_CVMWeightsScaleShape2Texture, 1, NULL, 0, temp, CV_GEMM_A_T);

    this->m_CVMQs = cvCreateMat (this->m_iNbOfShapes, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    cvGEMM(temp, this->m_CVMPcs, 1, NULL, 0, this->m_CVMQs, CV_GEMM_B_T);

    // calculate m_CVMQg = m_CVMTruncatedNormalizedTexturesEigenVectors * m_CVMPcg
    this->m_CVMQg = cvCreateMat (this->m_iNbOfTextures, this->m_iNbOfTruncatedConcatenated, CV_32FC1);
    cvGEMM(this->m_CVMTruncatedNormalizedTexturesEigenVectors, this->m_CVMPcg, 1, NULL, 0, this->m_CVMQg, CV_GEMM_A_T+CV_GEMM_B_T);

    cvReleaseMat(&inverseOf_m_CVMWeightsScaleShape2Texture);
    cvReleaseMat(&temp);

    this->VO_CreateDisplacementSets( );

    // this->VO_BuildRegressionMatrices();
    this->VO_BuildGradientMatrices();
 
}


/**

@author     JIA Pei

@version    2008-03-18

@brief      Save AAMBasic to a specified folder

@param      fn         Input - the folder that AAMBasic to be saved to

*/
void VO_AAMBasic::VO_Save(const string& fd)
{
    VO_AAM::VO_Save(fd);

    string fn = fd+"/AAMBasic";
    if (!boost::filesystem::is_directory(fn) )
        boost::filesystem::create_directory( fn );

    fstream fp;
    string tempfn;

    // AAMBasic
    tempfn = fn + "/AAMBasic" + ".txt";
    fp.open(tempfn.c_str (), ios::out);

    // m_iNbOfConcatenated
    fp << "m_iNbOfConcatenated" << endl;
    fp << this->m_iNbOfConcatenated << endl;

    // m_iNbOfTruncatedConcatenated
    fp << "m_iNbOfTruncatedConcatenated" << endl;
    fp << this->m_iNbOfTruncatedConcatenated << endl;

    // m_iNbOfEigenConcatenatedAtMost
    fp << "m_iNbOfEigenConcatenatedAtMost" << endl;
    fp << this->m_iNbOfEigenConcatenatedAtMost << endl;

    // m_fTruncatedPercent_Concatenated
    fp << "m_fTruncatedPercent_Concatenated" << endl;
    fp << this->m_fTruncatedPercent_Concatenated << endl;

    fp.close();

    // m_CVMWeightsScaleShape2Texture
    tempfn = fn + "/m_CVMWeightsScaleShape2Texture" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMWeightsScaleShape2Texture" << endl;
    for (unsigned int i = 0; i < this->m_CVMWeightsScaleShape2Texture->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMWeightsScaleShape2Texture->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMWeightsScaleShape2Texture, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMConcatenated
    tempfn = fn + "/m_CVMConcatenated" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMConcatenated" << endl;
    for (unsigned int i = 0; i < this->m_CVMConcatenated->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMConcatenated->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMConcatenated, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMConcatenatedProject2Truncated
    tempfn = fn + "/m_CVMConcatenatedProject2Truncated" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMConcatenatedProject2Truncated" << endl;
    for (unsigned int i = 0; i < this->m_CVMConcatenatedProject2Truncated->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMConcatenatedProject2Truncated->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMConcatenatedProject2Truncated, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMTruancatedConcatenatedMean
    tempfn = fn + "/m_CVMTruancatedConcatenatedMean" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruancatedConcatenatedMean" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruancatedConcatenatedMean->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMTruancatedConcatenatedMean, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMTruancatedConcatenatedSD
    tempfn = fn + "/m_CVMTruancatedConcatenatedSD" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruancatedConcatenatedSD" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruancatedConcatenatedSD->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMTruancatedConcatenatedSD, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMConcatenatedMean
    tempfn = fn + "/m_CVMConcatenatedMean" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMConcatenatedMean" << endl;
    for (unsigned int i = 0; i < this->m_CVMConcatenatedMean->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMConcatenatedMean, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMConcatenatedSD
    tempfn = fn + "/m_CVMConcatenatedSD" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMConcatenatedSD" << endl;
    for (unsigned int i = 0; i < this->m_CVMConcatenatedSD->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMConcatenatedSD, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMConcatenatedEigenVectors
    tempfn = fn + "/m_CVMConcatenatedEigenVectors" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMConcatenatedEigenVectors" << endl;
    for (unsigned int i = 0; i < this->m_CVMConcatenatedEigenVectors->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMConcatenatedEigenVectors->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMConcatenatedEigenVectors, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMConcatenatedEigenValues
    tempfn = fn + "/m_CVMConcatenatedEigenValues" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMConcatenatedEigenValues" << endl;
    for (unsigned int i = 0; i < this->m_CVMConcatenatedEigenValues->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMConcatenatedEigenValues, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMTruncatedConcatenatedEigenVectors
    tempfn = fn + "/m_CVMTruncatedConcatenatedEigenVectors" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruncatedConcatenatedEigenVectors" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruncatedConcatenatedEigenVectors->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMTruncatedConcatenatedEigenVectors->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMTruncatedConcatenatedEigenVectors, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMTruncatedConcatenatedEigenValues
    tempfn = fn + "/m_CVMTruncatedConcatenatedEigenValues" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruncatedConcatenatedEigenValues" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruncatedConcatenatedEigenValues->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMTruncatedConcatenatedEigenValues->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMTruncatedConcatenatedEigenValues, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMPcs	
    tempfn = fn + "/m_CVMPcs" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMPcs" << endl;
    for (unsigned int i = 0; i < this->m_CVMPcs->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMPcs->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMPcs, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMPcg
    tempfn = fn + "/m_CVMPcg" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMPcg" << endl;
    for (unsigned int i = 0; i < this->m_CVMPcg->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMPcg->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMPcg, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMQs
    tempfn = fn + "/m_CVMQs" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMQs" << endl;
    for (unsigned int i = 0; i < this->m_CVMQs->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMQs->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMQs, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMQg
    tempfn = fn + "/m_CVMQg" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMQg" << endl;
    for (unsigned int i = 0; i < this->m_CVMQg->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMQg->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMQg, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMRc
    tempfn = fn + "/m_CVMRc" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMRc" << endl;
    for (unsigned int i = 0; i < this->m_CVMRc->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMRc->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMRc, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMRt, we ignore this now
    tempfn = fn + "/m_CVMRt" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMRt" << endl;
    for (unsigned int i = 0; i < this->m_CVMRt->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMRt->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMRt, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMCParamGradientMatrix
    tempfn = fn + "/m_CVMCParamGradientMatrix" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMCParamGradientMatrix" << endl;
    for (unsigned int i = 0; i < this->m_CVMCParamGradientMatrix->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMCParamGradientMatrix->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMCParamGradientMatrix, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMPoseGradientMatrix, we ignore this now
    tempfn = fn + "/m_CVMPoseGradientMatrix" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMPoseGradientMatrix" << endl;
    for (unsigned int i = 0; i < this->m_CVMPoseGradientMatrix->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMPoseGradientMatrix->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMPoseGradientMatrix, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_vAAMConcatenated
    tempfn = fn + "/m_vAAMConcatenated" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vAAMConcatenated" << endl;
    for (unsigned int i = 0; i < this->m_vAAMConcatenated.size(); i++)
    {	
        for (unsigned int j = 0; j < this->m_vAAMConcatenated[0].size(); j++)
        {
            fp << this->m_vAAMConcatenated[i][j]<< " ";
        }
        fp << endl;
    }
    fp.close();

    // m_vAAMMeanConcatenated
    tempfn = fn + "/m_vAAMMeanConcatenated" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vAAMMeanConcatenated" << endl;
    for (unsigned int i = 0; i < this->m_vAAMMeanConcatenated.size(); i++)
    {	
        fp << this->m_vAAMMeanConcatenated[i] << " ";
    }
    fp << endl;
    fp.close();

}

