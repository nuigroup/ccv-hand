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

#ifndef AAM_PDM_H
#define AAM_PDM_H

#include "AAM_Util.h"
#include "AAM_Shape.h"

class AAM_CAM;

//2D point distribution model
class AAM_PDM
{
	friend class AAM_CAM;
public:
	AAM_PDM();
	~AAM_PDM();

	// Build shape distribution model
	void Train(const std::vector<AAM_Shape> &AllShapes,
		double scale = 1.0, double percentage = 0.975);

	// Read data from stream
	void Read(std::ifstream& is);

	// write data to stream
	void Write(std::ofstream& os);

	// Calculate shape according to parameters p and q
	void CalcLocalShape(const CvMat* p, CvMat* s);
	void CalcGlobalShape(const CvMat* q, CvMat* s);
	void CalcShape(const CvMat* p, const CvMat* q, CvMat* s);
	void CalcShape(const CvMat* pq, CvMat* s);

	// Calculate parameters p and q according to shape
	void CalcParams(const CvMat* s, CvMat* p, CvMat* q);
	void CalcParams(const CvMat* s, CvMat* pq);

	// Limit shape parameters.
    void Clamp(CvMat* p, double s_d = 3.0);

	// Get number of points in shape model
	inline const int nPoints()const{return __MeanShape->cols / 2;}

	// Get number of modes of shape variation
	inline const int nModes()const{return __ShapesEigenVectors->rows;}

	// Get mean shape
	inline const CvMat* GetMean()const{ return __MeanShape;	}
	inline const AAM_Shape GetMeanShape()const{ return __AAMRefShape;	}

	// Get shape eigen-vectors of PCA (shape modes)
	inline const CvMat* GetBases()const{ return __ShapesEigenVectors;	}

	inline const double Var(int i)const{ return cvmGet(__ShapesEigenValues,0,i);	}

private:
	// Align shapes using procrustes analysis
	static void AlignShapes(std::vector<AAM_Shape> &AllShapes);

	// Calculate mean shape of all shapes
	static void CalcMeanShape(AAM_Shape &MeanShape,
							const std::vector<AAM_Shape> &AllShapes);

	// Do PCA of shape data
	void DoPCA(const CvMat* AllShapes, double percentage);

private:

	CvMat*		__MeanShape;
	CvMat*		__ShapesEigenVectors;
    CvMat*		__ShapesEigenValues;
	AAM_Shape	__AAMRefShape;

	CvMat*		__matshape;
	AAM_Shape   __x;
	AAM_Shape   __y;
};

#endif //
