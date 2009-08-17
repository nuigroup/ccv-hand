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

#ifndef AAM_BASIC_H
#define AAM_BASIC_H

#include "AAM_Util.h"
#include "AAM_TDM.h"
#include "AAM_CAM.h"

/**
   The basic Active appearace model building and fitting method.
   Refer to: T.F. Cootes, G.J. Edwards and C.J. Taylor. "Active Appearance Models". ECCV 1998
*/
class AAM_Basic:public AAM
{
public:
	AAM_Basic();
	~AAM_Basic();

	virtual const int GetType()const { return TYPE_AAM_BASIC;	}

	//build cootes's basis aam
	void Train(const file_lists& pts_files,
		const file_lists& img_files,
		double scale = 1.0 ,
		double shape_percentage = 0.975,
		double texture_percentage = 0.975,
		double appearance_percentage = 0.975);

	virtual void Build(const file_lists& pts_files,
		const file_lists& img_files, double scale = 1.0)
	{	Train(pts_files, img_files, scale);	}

	//Fit the image using aam basic.
	virtual void Fit(const IplImage* image, AAM_Shape& Shape,
		int max_iter = 30, bool showprocess = false);

	//virtual void SetAllParamsZero();

	virtual const AAM_Shape GetMeanShape()const{ return __cam.__shape.GetMeanShape();	}

	//draw the image according the searching result(0:point, 1:triangle, 2:appearance)
	virtual void Draw(IplImage* image, const AAM_Shape& Shape, int type);

	// Read data from stream
	virtual void Read(std::ifstream& is);

	// write data to stream
	virtual void Write(std::ofstream& os);

	//set search parameters zero
	virtual void SetAllParamsZero();

	//init search parameters
	virtual void InitParams(const IplImage* image);

private:
	//Calculates the pixel difference from a model instance and an image
	double EstResidual(const IplImage* image, const CvMat* c_q,
		CvMat* s, CvMat* t_m, CvMat* t_s, CvMat* deltat);

	void CalcJacobianMatrix(const file_lists& pts_files,
		const file_lists& img_files,
		double disp_scale = 0.2, double disp_angle = 20,
		double disp_trans = 5.0, double disp_std = 1.0,
		int nExp = 30);

private:
	AAM_CAM __cam;
	CvMat*  __G;

private:
	//speed up for on-line alignment
	CvMat*  __current_c_q;  //current appearance+pose parameters
	CvMat*  __update_c_q;	//update appearance+pose parameters
	CvMat*  __delta_c_q;	//displacement appearance+pose parameters
	CvMat*	__c;			//appearance parameters
	CvMat*	__p;			//shape parameters
	CvMat*	__q;			//pose parameters
	CvMat*	__lamda;		//texture parameters
	CvMat*	__s;			//current shape
	CvMat*	__t_m;			//model texture instance
	CvMat*	__t_s;			//warped texture at current shape instance
	CvMat*	__delta_t;		//differnce between __ts and __tm
};

#endif //
