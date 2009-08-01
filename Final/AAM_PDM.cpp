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

#include "AAM_PDM.h"
using namespace std;

//============================================================================
AAM_PDM::AAM_PDM()
{
	__MeanShape = 0;
	__ShapesEigenVectors = 0;
    __ShapesEigenValues = 0;
	__matshape = 0;
}

//============================================================================
AAM_PDM::~AAM_PDM()
{
	cvReleaseMat(&__MeanShape);
	cvReleaseMat(&__ShapesEigenVectors);
	cvReleaseMat(&__ShapesEigenValues);
	cvReleaseMat(&__matshape);
}

//============================================================================
void AAM_PDM::Train(const std::vector<AAM_Shape> &AllShapes,
					double scale /* = 1.0 */,
					double percentage /* = 0.975 */)
{
	int nSamples = AllShapes.size();
	int nPoints = AllShapes[0].NPoints();
	__matshape = cvCreateMat(1, nPoints*2, CV_64FC1);

	std::vector<AAM_Shape> AlignedShapes = AllShapes;
	for(int s = 0; s < AlignedShapes.size(); s++)
		AlignedShapes[s] *= scale;

	AAM_PDM::AlignShapes(AlignedShapes);

	// Assign all points positions from vector<AAM_Shape> to CvMat
	CvMat *m_CVShapes = cvCreateMat (nSamples, nPoints * 2, CV_64FC1);
    for(int i = 0; i < nSamples; i++)
    {
		for(int j = 0; j < nPoints; j++)
        {
            CV_MAT_ELEM(*m_CVShapes, double, i, 2*j  ) = AlignedShapes[i][j].x;
            CV_MAT_ELEM(*m_CVShapes, double, i, 2*j+1) = AlignedShapes[i][j].y;        }
    }

	DoPCA(m_CVShapes, percentage);
	__AAMRefShape.Mat2Point(__MeanShape);

	// this step is not necessary!
	__AAMRefShape.Translate(-__AAMRefShape.MinX(), -__AAMRefShape.MinY());

	cvReleaseMat(&m_CVShapes);
}

//============================================================================
void AAM_PDM::AlignShapes(std::vector<AAM_Shape> &AllShapes)
{
	printf("Align all shapes...\n");

	int nSamples = AllShapes.size();
	int nPoints = AllShapes[0].NPoints();

	for(int ii=0; ii < nSamples; ii++)	AllShapes[ii].Centralize();

	// calculate the mean shape
	AAM_Shape meanShape;
	AAM_PDM::CalcMeanShape(meanShape, AllShapes);

	// We choose an initial estimate AlignedShapes[0]
    AAM_Shape refShape(meanShape);
	const AAM_Shape ConstRefShape(meanShape);
	AAM_Shape NewMeanShape(meanShape);

    // do a number of alignment iterations until the mean shape estimate is stable
    double diff, diff_max = 0.0001;
	const int max_iter = 30;
    for(int iter = 0; iter < max_iter; iter++)
    {
		//align all shapes to the mean shape estimate
        for(int i = 0;i < nSamples;i++)
        {
			// align the i-th shape to the estimate of the mean shape
			AllShapes[i].AlignTo(refShape);
		}

        // Re-estimate new mean shape from aligned shapes
		AAM_PDM::CalcMeanShape(NewMeanShape, AllShapes);

		// Constrain new mean shape by aligning it to ref shape
		NewMeanShape.AlignTo(ConstRefShape);

		diff = (NewMeanShape-refShape).GetNorm2();

		printf("\tAlignment iteration #%i, mean shape est. diff. = %g\n", iter, diff );

		if(diff <= diff_max) break; //converged

		//if not converged, come on iterations
		refShape = NewMeanShape;
	}

	AAM_PDM::CalcMeanShape(meanShape, AllShapes);
}

//============================================================================
void AAM_PDM::CalcMeanShape(AAM_Shape &MeanShape,
							const std::vector<AAM_Shape> &AllShapes)
{
	MeanShape.resize(AllShapes[0].NPoints());
    MeanShape = 0;
    for(int i = 0; i < AllShapes.size(); i++)     MeanShape += AllShapes[i];
    MeanShape /= AllShapes.size();
}

//============================================================================
void AAM_PDM::DoPCA(const CvMat* AllShapes, double percentage)
{
	printf("Doing PCA of shapes datas...");

	int nSamples = AllShapes->rows;
	int nPointsby2 = AllShapes->cols;
    int nEigenAtMost = MIN(nSamples, nPointsby2);

    CvMat* tmpEigenValues = cvCreateMat(1, nEigenAtMost, CV_64FC1);
    CvMat* tmpEigenVectors = cvCreateMat(nEigenAtMost, nPointsby2, CV_64FC1);
    __MeanShape = cvCreateMat(1, nPointsby2, CV_64FC1 );

    cvCalcPCA(AllShapes, __MeanShape,
        tmpEigenValues, tmpEigenVectors, CV_PCA_DATA_AS_ROW);

	double allSum = cvSum(tmpEigenValues).val[0];
	double partSum = 0.0;
    int nTruncated = 0;
    double largesteigval = cvmGet(tmpEigenValues, 0, 0);
	for(int i = 0; i < nEigenAtMost; i++)
    {
		double thiseigval = cvmGet(tmpEigenValues, 0, i);
        if(thiseigval / largesteigval < 0.0001) break; // firstly check
		partSum += thiseigval;
		++ nTruncated;
        if(partSum/allSum >= percentage)	break;    //secondly check
    }

	__ShapesEigenValues = cvCreateMat(1, nTruncated, CV_64FC1);
	__ShapesEigenVectors = cvCreateMat(nTruncated, nPointsby2, CV_64FC1);

	CvMat G;
	cvGetCols(tmpEigenValues, &G, 0, nTruncated);
	cvCopy(&G, __ShapesEigenValues);

	cvGetRows(tmpEigenVectors, &G, 0, nTruncated);
	cvCopy(&G, __ShapesEigenVectors);

	cvReleaseMat(&tmpEigenVectors);
	cvReleaseMat(&tmpEigenValues);
	printf("Done (%d/%d)\n", nTruncated, nEigenAtMost);
}

void AAM_PDM::CalcLocalShape(const CvMat* p, CvMat* s)
{
	cvBackProjectPCA(p, __MeanShape, __ShapesEigenVectors,	s);
}

void AAM_PDM::CalcGlobalShape(const CvMat* q, CvMat* s)
{
	int npoints = nPoints();
	double* fasts = s->data.db;
	double a=cvmGet(q,0,0)+1, b=cvmGet(q,0,1),
		tx=cvmGet(q,0,2), ty=cvmGet(q,0,3);
	double x, y;
	for(int i = 0; i < npoints; i++)
	{
		x = fasts[2*i  ];
		y = fasts[2*i+1];

		fasts[2*i  ] = a*x-b*y+tx;
		fasts[2*i+1] = b*x+a*y+ty;
	}
}

void AAM_PDM::CalcShape(const CvMat* p, const CvMat* q, CvMat* s)
{
	CalcLocalShape(p, s);
	CalcGlobalShape(q, s);
}

void AAM_PDM::CalcShape(const CvMat* pq, CvMat* s)
{
	CvMat p, q;
	cvGetCols(pq, &q, 0, 4);
	cvGetCols(pq, &p, 4, 4+nModes());
	CalcShape(&p, &q, s);
}

void AAM_PDM::CalcParams(const CvMat* s, CvMat* pq)
{
	CvMat p, q;
	cvGetCols(pq, &q, 0, 4);
	cvGetCols(pq, &p, 4, 4+nModes());

	CalcParams(s, &p, &q);
}

void AAM_PDM::CalcParams(const CvMat* s, CvMat* p, CvMat* q)
{
	int nmodes = nModes(), npoints = nPoints();

	double a, b, tx, ty;
	double a_, b_, tx_, ty_;
	double norm;

	__y.Mat2Point(s);
	__y.COG(tx, ty);
	__y.Translate(-tx, -ty);
	cvmSet(q,0,2,tx);
	cvmSet(q,0,3,ty);

	// do a few iterations to get (s, theta, p)
	cvZero(p);
	for(int iter = 0; iter < 2; iter++)
	{
		cvBackProjectPCA(p, __MeanShape, __ShapesEigenVectors,	__matshape);
		__x.Mat2Point(__matshape);

		__x.AlignTransformation(__y, a, b, tx, ty); //in fact, tx = ty = 0

		norm = a*a + b*b;
		a_ = a / norm; b_ = -b / norm;
		tx_ = (-a*tx - b*ty) / norm; ty_ = (b*tx - a*ty) / norm;
		__x = __y;
		__x.TransformPose(a_, b_, tx_, ty_);

		__x.Point2Mat(__matshape);
		cvProjectPCA(__matshape, __MeanShape, __ShapesEigenVectors, p);
	}

	cvmSet(q, 0, 0, a -1);
	cvmSet(q, 0, 1, b);
	Clamp(p, 1.8);
}

void AAM_PDM::Clamp(CvMat* p, double s_d /* = 3.0 */)
{
	double* fastp = p->data.db;
	double* fastv = __ShapesEigenValues->data.db;
	int nmodes = nModes();
	double limit;

	for(int i = 0; i < nmodes; i++)
	{
		limit = s_d*sqrt(fastv[i]);
		if(fastp[i] > limit) fastp[i] = limit;
		else if(fastp[i] < -limit) fastp[i] = -limit;
	}
}

void AAM_PDM::Write(std::ofstream& os)
{
	os << nPoints() << " " << nModes() << std::endl;
	os << __MeanShape << std::endl;
	os << __ShapesEigenValues << std::endl;
	os << __ShapesEigenVectors << std::endl;
	__AAMRefShape.Write(os);
	os << std::endl;
}

void AAM_PDM::Read(std::ifstream& is)
{
	int _nPoints, _nModes;
	is >> _nPoints >> _nModes;

	__MeanShape = cvCreateMat(1, _nPoints*2, CV_64FC1);
	__ShapesEigenValues = cvCreateMat(1, _nModes, CV_64FC1);
	__ShapesEigenVectors = cvCreateMat(_nModes, _nPoints*2, CV_64FC1);
	__AAMRefShape.resize(_nPoints);

	is >> __MeanShape;
	is >> __ShapesEigenValues;
	is >> __ShapesEigenVectors;
	__AAMRefShape.Read(is);

	__matshape  = cvCreateMat(1, nPoints()*2, CV_64FC1);
}
