/****************************************************************************
* File Name:        VO_AAMWarpingPoint.cpp                                  *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM model warping information -                         * 
*                   pixel texture relationship                              *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/


#include "VO_AAMWarpingPoint.h"


/**

    @author     JIA Pei

    @version    2007-06-13

    @brief      Calculate the first item of Jacobian

    @return     void

    @note       http://visionopen.com/cv/aam.php

*/
void VO_AAMWarpingPoint::CalcJacobianOne()
{
	float x, y, x1, x2, x3, y1, y2, y3;
	this->m_Jacobian_One.resize(3);

	x = this->m_CVPosition.x;
	y = this->m_CVPosition.y;

	x1 = this->m_oAAMTriangle2d.GetVertex(0).x;
	x2 = this->m_oAAMTriangle2d.GetVertex(1).x;
	x3 = this->m_oAAMTriangle2d.GetVertex(2).x;
	y1 = this->m_oAAMTriangle2d.GetVertex(0).y;
	y2 = this->m_oAAMTriangle2d.GetVertex(1).y;
	y3 = this->m_oAAMTriangle2d.GetVertex(2).y;

	this->m_Jacobian_One[0] = y*x3-y3*x+x*y2-x2*y+x2*y3-x3*y2;
	this->m_Jacobian_One[1] = -y*x3+x1*y+x3*y1+y3*x-x1*y3-x*y1;
	this->m_Jacobian_One[2] = -x*y2+x*y1+x1*y2+x2*y-x2*y1-x1*y;

	// make sure m_Jacobian_One is bigger than 0
	for (unsigned int i = 0; i < this->m_Jacobian_One.size(); i++)
	{
		this->m_Jacobian_One[i] /= this->m_oAAMTriangle2d.GetdD();
		if (this->m_Jacobian_One[i] < 1e-15)
			this->m_Jacobian_One[i] = 0.0;
	}

}


/**

    @author     JIA Pei

    @version    2007-06-13

    @brief      Calculate the Jacobian matrix

    @return     void

*/
void VO_AAMWarpingPoint::CalcJacobianMatrix(const CvMat* TruncatedAlignedShapesEigenVectors)
{
	unsigned int NbOfShapesParameters = TruncatedAlignedShapesEigenVectors->rows;
	// for a certain temppixeltriangle.m_CVPosition, the Jacobian[i] are different

	this->m_JacobianMatrix.resize (2);
	this->m_JacobianMatrix[0].resize(NbOfShapesParameters);
	this->m_JacobianMatrix[1].resize(NbOfShapesParameters);

	for (unsigned int np = 0; np < NbOfShapesParameters; np++)
	{
		this->m_JacobianMatrix[0][np] = this->m_Jacobian_One[0] 
		* CV_MAT_ELEM( *TruncatedAlignedShapesEigenVectors, float, np, this->m_oAAMTriangle2d.GetVertexIndex(0) * 2)
			+ this->m_Jacobian_One[1] 
		* CV_MAT_ELEM( *TruncatedAlignedShapesEigenVectors, float, np, this->m_oAAMTriangle2d.GetVertexIndex(1) * 2)
			+ this->m_Jacobian_One[2] 
		* CV_MAT_ELEM( *TruncatedAlignedShapesEigenVectors, float, np, this->m_oAAMTriangle2d.GetVertexIndex(2) * 2);

		this->m_JacobianMatrix[1][np] =  this->m_Jacobian_One[0] 
		* CV_MAT_ELEM( *TruncatedAlignedShapesEigenVectors, float, np, this->m_oAAMTriangle2d.GetVertexIndex(0) * 2 + 1)
			+ this->m_Jacobian_One[1]														   
		* CV_MAT_ELEM( *TruncatedAlignedShapesEigenVectors, float, np, this->m_oAAMTriangle2d.GetVertexIndex(1) * 2 + 1)
			+ this->m_Jacobian_One[2]														   
		* CV_MAT_ELEM( *TruncatedAlignedShapesEigenVectors, float, np, this->m_oAAMTriangle2d.GetVertexIndex(2) * 2 + 1);
	}
}


/**

    @author     JIA Pei

    @version    2007-06-13

    @brief      Calculate the Jacobian matrix for global shape normalization

    @return     void

*/
void VO_AAMWarpingPoint::CalcJacobianMatrix4GlobalShapeNorm(const CvMat* SimilarityTransformMatrix)
{
	unsigned int NbOfSimilarityTransformParameters = SimilarityTransformMatrix->rows;
	// for a certain temppixeltriangle.m_CVPosition, the Jacobian[i] are different

	this->m_JacobianMatrix4GlobalShapeNorm.resize (2);
    this->m_JacobianMatrix4GlobalShapeNorm[0].resize(NbOfSimilarityTransformParameters);
    this->m_JacobianMatrix4GlobalShapeNorm[1].resize(NbOfSimilarityTransformParameters);

    for (unsigned int np = 0; np < NbOfSimilarityTransformParameters; np++)
	{
		this->m_JacobianMatrix4GlobalShapeNorm[0][np] = this->m_Jacobian_One[0] 
		* CV_MAT_ELEM( *SimilarityTransformMatrix, float, np, this->m_oAAMTriangle2d.GetVertexIndex(0) * 2)
			+ this->m_Jacobian_One[1]												  
		* CV_MAT_ELEM( *SimilarityTransformMatrix, float, np, this->m_oAAMTriangle2d.GetVertexIndex(1) * 2)
			+ this->m_Jacobian_One[2]												  
		* CV_MAT_ELEM( *SimilarityTransformMatrix, float, np, this->m_oAAMTriangle2d.GetVertexIndex(2) * 2);

		this->m_JacobianMatrix4GlobalShapeNorm[1][np] =  this->m_Jacobian_One[0] 
		* CV_MAT_ELEM( *SimilarityTransformMatrix, float, np, this->m_oAAMTriangle2d.GetVertexIndex(0) * 2 + 1)
			+ this->m_Jacobian_One[1]												  
		* CV_MAT_ELEM( *SimilarityTransformMatrix, float, np, this->m_oAAMTriangle2d.GetVertexIndex(1) * 2 + 1)
			+ this->m_Jacobian_One[2]												  
		* CV_MAT_ELEM( *SimilarityTransformMatrix, float, np, this->m_oAAMTriangle2d.GetVertexIndex(2) * 2 + 1);
	}
}


/**

    @author     JIA Pei

    @version    2007-06-13

    @brief      Calculate the Steepest Descent Image (SDI)

    @return     void

*/
void VO_AAMWarpingPoint::CalcSteepestDescentImages(unsigned int inChannels)
{
	this->m_SteepestDescentImages.resize (inChannels);

	for (unsigned int i = 0; i < this->m_SteepestDescentImages.size (); i++)
	{
		this->m_SteepestDescentImages[i].resize (this->m_JacobianMatrix[0].size ());
	}

	for (unsigned int i = 0; i < this->m_SteepestDescentImages[0].size (); i++)
	{
		for (unsigned int k = 0; k < inChannels; k++)
		{
			this->m_SteepestDescentImages[k][i] = this->m_Gradients[k][0] * this->m_JacobianMatrix[0][i]
			+ this->m_Gradients[k][1] * this->m_JacobianMatrix[1][i];
		}
	}
}



/**

    @author     JIA Pei

    @version    2007-06-13

    @brief      Calculate the Steepest Descent Image (SDI) for global shape normalization

    @return     void

*/
void VO_AAMWarpingPoint::CalcSteepestDescentImages4GlobalShapeNorm(unsigned int inChannels)
{
	this->m_SteepestDescentImages4GlobalShapeNorm.resize (inChannels);

	for (unsigned int i = 0; i < this->m_SteepestDescentImages4GlobalShapeNorm.size (); i++)
	{
		this->m_SteepestDescentImages4GlobalShapeNorm[i].resize (this->m_JacobianMatrix4GlobalShapeNorm[0].size ());
	}

	for (unsigned int i = 0; i < this->m_SteepestDescentImages4GlobalShapeNorm[0].size (); i++)
	{
		for (unsigned int k = 0; k < inChannels; k++)
		{
			this->m_SteepestDescentImages4GlobalShapeNorm[k][i] = 
				this->m_Gradients[k][0] * this->m_JacobianMatrix4GlobalShapeNorm[0][i] +
				this->m_Gradients[k][1] * this->m_JacobianMatrix4GlobalShapeNorm[1][i];
		}
	}
}

