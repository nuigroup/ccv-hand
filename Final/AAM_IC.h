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


#ifndef AAM_IC_H
#define AAM_IC_H

#include "AAM_Util.h"
#include "AAM_Shape.h"
#include "AAM_PDM.h"
#include "AAM_TDM.h"
#include "AAM_PAW.h"

/**
   Active appearace model using project-out inverse compositional fitting method.
   Refer to: I. Matthews and S. Baker. "Active Appearance Models Revisited". 2004
*/
class AAM_IC  :public AAM
{
public:
	AAM_IC();
	~AAM_IC();

	virtual const int GetType()const { return TYPE_AAM_IC;	}

	//build aam inverse compositional model
	virtual void Build(const file_lists& pts_files, const file_lists& img_files,
		double scale = 1.0)
	{	Train(pts_files, img_files, scale); }

	void Train(const file_lists& pts_files, const file_lists& img_files,
		double scale = 1.0, double shape_percentage = 0.975,
		double texture_percentage = 0.975);

	//Fit the image using Inverse Compositional.
	virtual void Fit(const IplImage* image, AAM_Shape& Shape,
		int max_iter = 30, bool showprocess = false);

	//set all search parameters zero
	virtual void SetAllParamsZero();

	//init search parameters
	virtual void InitParams(const IplImage* image){}

	//draw the image according the searching result(0:point, 1:triangle, 2:appearance)
	virtual void Draw(IplImage* image, const AAM_Shape& Shape, int type);

	// Read data from stream
	virtual void Read(std::ifstream& is);

	// write data to stream
	virtual void Write(std::ofstream& os);

	//Get Mean Shape of IC model
	inline const AAM_Shape GetMeanShape()const{ return __sMean;	}

private:

	//calclulate the texture parameters project to linear subspace span(A)
	void CalcAppearanceVariation(const CvMat* error_t, CvMat* lamda);

	//Evaluate the Jacobians dN_dq and dW_dp of piecewise affine warp at(x;0)
	void CalcWarpJacobian(CvMat* Jx, CvMat* Jy);

    //Calculate index of gradients for every point in texture.
    //If point is outside texture, set to -1.
	CvMat* CalcGradIdx();

	//Calculate the gradient of texture template A0.
	void CalcTexGrad(const CvMat* texture, CvMat* dTx, CvMat* dTy);

	//Calculating the modified steepest descent image.
	void CalcModifiedSD(CvMat* SD, const CvMat* dTx, const CvMat* dTy,
		const CvMat* Jx, const CvMat* Jy);

	//Inverse compose current warp with shape parameter update.
	//Update warp N.W(x;p,q)<-- N.W(x;p,q) . N.W(x;delta_p,delta_q)^-1.
	void InverseCompose(const CvMat* dpq, const CvMat* s, CvMat* NewS);

	//Compute the Hessian matrix using modified steepest descent image.
	void CalcHessian(CvMat* H, const CvMat* SD);

private:

	//these variables are used for train PAW
	CvMat*			__Points;
	CvMemStorage*	__Storage;

private:

	AAM_PDM		__shape;		/*shape distribution model*/
	AAM_TDM		__texture;		/*shape distribution model*/
	AAM_PAW		__paw;			/*piecewise affine warp*/
	AAM_Shape	__sMean;		/*mean shape of model*/
	AAM_Shape	__sStar1, __sStar2, __sStar3, __sStar4;/*global shape transform bases*/
	CvMat*		__G;			/*Update matrix*/
						/*product of inverse Hessian with steepest descent image*/

private:
	//pre-allocated stuff for online alignment
	CvMat*		__update_s0;	/*shape change at the base mesh */
	CvMat*		__inv_pq;		/*inverse parameters at the base mesh*/

	CvMat*		__warp_t;		/*warp image to base mesh*/
	CvMat*		__error_t;		/*error between warp image and template image A0*/
	CvMat*		__search_pq;	/*search parameters */
	CvMat*		__delta_pq;		/*parameters change to be updated*/
	CvMat*		__current_s;		/*current search shape*/
	CvMat*		__update_s;		/*shape after composing the warp*/
	CvMat*		__lamda;		/*appearance parameters*/
};

#endif
