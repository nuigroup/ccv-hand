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

#include "AAM_CAM.h"

//============================================================================
AAM_CAM::AAM_CAM()
{
	__MeanAppearance = 0;
	__AppearanceEigenValues = 0;
	__AppearanceEigenVectors = 0;
	__Qs = 0;
	__Qg = 0;
	__MeanS = 0;
	__MeanG = 0;

	__Points = 0;
	__Storage = 0;
	__pq = 0;

	__a = 0;
}

//============================================================================
AAM_CAM::~AAM_CAM()
{
	cvReleaseMat(&__MeanAppearance);
	cvReleaseMat(&__AppearanceEigenValues);
	cvReleaseMat(&__AppearanceEigenVectors);
	cvReleaseMat(&__Qg);
	cvReleaseMat(&__Qs);
	cvReleaseMat(&__MeanS);
	cvReleaseMat(&__MeanG);

	cvReleaseMat(&__Points);
	cvReleaseMemStorage(&__Storage);
	cvReleaseMat(&__pq);

	cvReleaseMat(&__a);
}

//============================================================================
void AAM_CAM::Train(const file_lists& pts_files,
					const file_lists& img_files,
					double scale /* = 1.0 */,
					double shape_percentage /* = 0.95 */,
					double texture_percentage /* = 0.95 */,
					double appearance_percentage /* = 0.95 */)
{
	//building shape and texture distribution model
	std::vector<AAM_Shape> AllShapes;
	for(int ii = 0; ii < pts_files.size(); ii++)
	{
		AAM_Shape Shape;
		bool flag = Shape.ReadAnnotations(pts_files[ii]);
		if(!flag)
		{
			IplImage* image = cvLoadImage(img_files[ii].c_str(), -1);
			Shape.ScaleXY(image->width, image->height);
			cvReleaseImage(&image);
		}
		AllShapes.push_back(Shape);
	}

	printf("Build point distribution model...\n");
	__shape.Train(AllShapes, scale, shape_percentage);

	printf("Build warp information of mean shape mesh...");
	__Points = cvCreateMat (1, __shape.nPoints(), CV_32FC2);
	__Storage = cvCreateMemStorage(0);
	AAM_Shape refShape = __shape.__AAMRefShape/* * scale */;
	__paw.Train(refShape, __Points, __Storage);
	printf("[%d by %d, %d triangles, %d*3 pixels]\n",
		__paw.Width(), __paw.Height(), __paw.nTri(), __paw.nPix());

	printf("Build texture distribution model...\n");
	__texture.Train(pts_files, img_files, __paw, texture_percentage, true);
	__pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);

	printf("Build combined appearance model...\n");
	int nsamples = pts_files.size();
	int npointsby2 = __shape.nPoints()*2;
	int npixels = __texture.nPixels();
	int nfeatures = __shape.nModes() + __texture.nModes();
	CvMat* AllAppearances = cvCreateMat(nsamples, nfeatures, CV_64FC1);
	CvMat* s = cvCreateMat(1, npointsby2, CV_64FC1);
	CvMat* t = cvCreateMat(1, npixels, CV_64FC1);
	__MeanS = cvCreateMat(1, npointsby2, CV_64FC1);
	__MeanG = cvCreateMat(1, npixels, CV_64FC1);
	cvCopy(__shape.GetMean(), __MeanS);
	cvCopy(__texture.GetMean(), __MeanG);

	//calculate ratio of shape to appearance
	CvScalar Sum1 = cvSum(__shape.__ShapesEigenValues);
    CvScalar Sum2 = cvSum(__texture.__TextureEigenValues);
    __WeightsS2T = sqrt(Sum2.val[0] / Sum1.val[0]);

	for(int i = 0; i < nsamples; i++)
	{
		//Get Shape and Texture respectively
		IplImage* image = cvLoadImage(img_files[i].c_str(), -1);

		AAM_Shape Shape;
		if(!Shape.ReadAnnotations(pts_files[i]))
			Shape.ScaleXY(image->width, image->height);
		Shape.Point2Mat(s);

		__paw.CalcWarpTexture(s, image, t);
		__texture.NormalizeTexture(__MeanG, t);

		//combine shape and texture parameters
		CvMat OneAppearance;
		cvGetRow(AllAppearances, &OneAppearance, i);
		ShapeTexture2Combined(s, t, &OneAppearance);

		cvReleaseImage(&image);
	}

	//Do PCA of appearances
	DoPCA(AllAppearances, appearance_percentage);

	int np = __AppearanceEigenVectors->rows;

	printf("Extracting the shape and texture part of the combined eigen vectors..\n");

	// extract the shape part of the combined eigen vectors
    CvMat Ps;
	cvGetCols(__AppearanceEigenVectors, &Ps, 0, __shape.nModes());
	__Qs = cvCreateMat(np, npointsby2, CV_64FC1);
	cvMatMul(&Ps, __shape.GetBases(), __Qs);
	cvConvertScale(__Qs, __Qs, 1.0/__WeightsS2T);

	// extract the texture part of the combined eigen vectors
    CvMat Pg;
	cvGetCols(__AppearanceEigenVectors, &Pg, __shape.nModes(), nfeatures);
	__Qg = cvCreateMat(np, npixels, CV_64FC1);
	cvMatMul(&Pg, __texture.GetBases(), __Qg);

	__a = cvCreateMat(1, __AppearanceEigenVectors->cols, CV_64FC1);
}

//============================================================================
void AAM_CAM::ShapeTexture2Combined(const CvMat* Shape, const CvMat* Texture,
									CvMat* Appearance)
{
	__shape.CalcParams(Shape, __pq);
	CvMat mat1, mat2;
	cvGetCols(__pq, &mat1, 4, 4+__shape.nModes());
	cvGetCols(Appearance, &mat2, 0, __shape.nModes());
	cvCopy(&mat1, &mat2);
	cvConvertScale(&mat2, &mat2, __WeightsS2T);

	cvGetCols(Appearance, &mat2, __shape.nModes(), __shape.nModes()+__texture.nModes());
	__texture.CalcParams(Texture, &mat2);
}

//============================================================================
void AAM_CAM::DoPCA(const CvMat* AllAppearances, double percentage)
{
	printf("Doing PCA of appearance datas...");

	int nSamples = AllAppearances->rows;
	int nfeatures = AllAppearances->cols;
    int nEigenAtMost = MIN(nSamples, nfeatures);

    CvMat* tmpEigenValues = cvCreateMat(1, nEigenAtMost, CV_64FC1);
    CvMat* tmpEigenVectors = cvCreateMat(nEigenAtMost, nfeatures, CV_64FC1);
    __MeanAppearance = cvCreateMat(1, nfeatures, CV_64FC1 );

    cvCalcPCA(AllAppearances, __MeanAppearance,
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

	__AppearanceEigenValues = cvCreateMat(1, nTruncated, CV_64FC1);
	__AppearanceEigenVectors = cvCreateMat(nTruncated, nfeatures, CV_64FC1);

	CvMat G;
	cvGetCols(tmpEigenValues, &G, 0, nTruncated);
	cvCopy(&G, __AppearanceEigenValues);

	cvGetRows(tmpEigenVectors, &G, 0, nTruncated);
	cvCopy(&G, __AppearanceEigenVectors);

	cvReleaseMat(&tmpEigenVectors);
	cvReleaseMat(&tmpEigenValues);
	printf("Done (%d/%d)\n", nTruncated, nEigenAtMost);
}

//============================================================================
void AAM_CAM::CalcLocalShape(CvMat* s, const CvMat* c)
{
	cvMatMul(c, __Qs, s);
	cvAdd(s, __MeanS, s);
}

//============================================================================
void AAM_CAM::CalcGlobalShape(CvMat* s, const CvMat* pose)
{
	int npoints = s->cols/2;
	double* fasts = s->data.db;
	double a=cvmGet(pose,0,0)+1, b=cvmGet(pose,0,1),
		tx=cvmGet(pose,0,2), ty=cvmGet(pose,0,3);
	double x, y;
	for(int i = 0; i < npoints; i++)
	{
		x = fasts[2*i  ];
		y = fasts[2*i+1];

		fasts[2*i  ] = a*x-b*y+tx;
		fasts[2*i+1] = b*x+a*y+ty;
	}
}

//============================================================================
void AAM_CAM::CalcTexture(CvMat* t, const CvMat* c)
{
	cvMatMul(c, __Qg, t);
	cvAdd(t, __MeanG, t);
}

//============================================================================
void AAM_CAM::CalcParams(CvMat* c, const CvMat* bs, const CvMat* bg)
{
	double* fasta = __a->data.db;
	double* fastbs = bs->data.db;
	double* fastbg = bg->data.db;

	int i;
	for(i = 0; i < bs->cols; i++)	fasta[i] = __WeightsS2T * fastbs[i];
	for(i = 0; i < bg->cols; i++)   fasta[i+bs->cols] = fastbg[i];

	cvProjectPCA(__a, __MeanAppearance, __AppearanceEigenVectors, c);
}

//============================================================================
void AAM_CAM::Clamp(CvMat* c, double s_d /* = 3.0 */)
{
	double* fastc = c->data.db;
	double* fastv = __AppearanceEigenValues->data.db;
	int nmodes = nModes();
	double limit;

	for(int i = 0; i < nmodes; i++)
	{
		limit = s_d*sqrt(fastv[i]);
		if(fastc[i] > limit) fastc[i] = limit;
		else if(fastc[i] < -limit) fastc[i] = -limit;
	}
}

//============================================================================
void AAM_CAM::DrawAppearance(IplImage* image, const AAM_Shape& Shape, CvMat* Texture)
{
	AAM_PAW paw;
	int x1, x2, y1, y2, idx1 = 0, idx2 = 0;
	int tri_idx, v1, v2, v3;
	int minx, miny, maxx, maxy;
	paw.Train(Shape, __Points, __Storage, __paw.GetTri(), false);
	AAM_Shape refShape = __paw.__referenceshape;
	double minV, maxV;
	cvMinMaxLoc(Texture, &minV, &maxV);
	cvConvertScale(Texture, Texture, 1/(maxV-minV)*255, -minV*255/(maxV-minV));

	minx = Shape.MinX(); miny = Shape.MinY();
	maxx = Shape.MaxX(); maxy = Shape.MaxY();
	for(int y = miny; y < maxy; y++)
	{
		y1 = y-miny;
		for(int x = minx; x < maxx; x++)
		{
			x1 = x-minx;
			idx1 = paw.Rect(y1, x1);
			if(idx1 >= 0)
			{
				tri_idx = paw.PixTri(idx1);
				v1 = paw.Tri(tri_idx, 0);
				v2 = paw.Tri(tri_idx, 1);
				v3 = paw.Tri(tri_idx, 2);

				x2 = paw.Alpha(idx1)*refShape[v1].x + paw.Belta(idx1)*refShape[v2].x +
					paw.Gamma(idx1)*refShape[v3].x;
				y2 = paw.Alpha(idx1)*refShape[v1].y + paw.Belta(idx1)*refShape[v2].y +
					paw.Gamma(idx1)*refShape[v3].y;

				idx2 = __paw.Rect(y2, x2);
				if(idx2 < 0) continue;

				CV_IMAGE_ELEM(image, byte, y, 3*x) = cvmGet(Texture, 0, 3*idx2);
				CV_IMAGE_ELEM(image, byte, y, 3*x+1) = cvmGet(Texture, 0, 3*idx2+1);
				CV_IMAGE_ELEM(image, byte, y, 3*x+2) = cvmGet(Texture, 0, 3*idx2+2);
			}
		}
	}
}

//============================================================================
void AAM_CAM::Write(std::ofstream& os)
{
	__shape.Write(os);
	__texture.Write(os);
	__paw.Write(os);

	os << __AppearanceEigenVectors->rows << " " << __AppearanceEigenVectors->cols
		<< " " << __WeightsS2T << std::endl;
	os << __MeanAppearance;
	os << __AppearanceEigenValues;
	os << __AppearanceEigenVectors;
	os << __Qs;
	os << __Qg;
	os << __MeanS;
	os << __MeanG;
}

//============================================================================
void AAM_CAM::Read(std::ifstream& is)
{
	__shape.Read(is);
	__texture.Read(is);
	__paw.Read(is);

	int np, nfeatures;
	is >> np >> nfeatures >> __WeightsS2T;

	__MeanAppearance = cvCreateMat(1, nfeatures, CV_64FC1);
	__AppearanceEigenValues = cvCreateMat(1, np, CV_64FC1);
	__AppearanceEigenVectors = cvCreateMat(np, nfeatures, CV_64FC1);
	__Qs = cvCreateMat(np, __shape.nPoints()*2, CV_64FC1);
	__Qg = cvCreateMat(np, __texture.nPixels(), CV_64FC1);
	__MeanS = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__MeanG = cvCreateMat(1, __texture.nPixels(), CV_64FC1);

	is >> __MeanAppearance;
	is >> __AppearanceEigenValues;
	is >> __AppearanceEigenVectors;
	is >> __Qs;
	is >> __Qg;
	is >> __MeanS;
	is >> __MeanG;

	__Points = cvCreateMat (1, __shape.nPoints(), CV_32FC2);
	__Storage = cvCreateMemStorage(0);
	__pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__a = cvCreateMat(1, __AppearanceEigenVectors->cols, CV_64FC1);
}

//============================================================================
static AAM_CAM *g_cam;
static const int n = 6;//appearance modes
static int b_c[n];
static const int offset = 40;
static CvMat* c = 0;//conbined appearance parameters
static CvMat* s = 0;//shape instance
static CvMat* t = 0;//texture instance
static IplImage* image = 0;
static AAM_Shape aam_s;
//============================================================================

//============================================================================
void ontrackcam(int pos)
{
	if(c == 0)
	{
		c = cvCreateMat(1, g_cam->nModes(), CV_64FC1);cvZero(c);
		s = cvCreateMat(1, g_cam->__shape.nPoints()*2, CV_64FC1);
		t = cvCreateMat(1, g_cam->__texture.nPixels(), CV_64FC1);
	}

	double var;
	//registrate appearance parameters
	for(int i = 0; i < n; i++)
	{
		var = 3*sqrt(g_cam->Var(i))*(double(b_c[i])/offset-1.0);
		cvmSet(c, 0, i, var);
	}

	//generate shape and texture instance
	g_cam->CalcLocalShape(s, c);
	g_cam->CalcTexture(t, c);

	//warp texture instance from base mesh to current shape instance
	aam_s.Mat2Point(s);
	int w = aam_s.GetWidth(), h = aam_s.MaxY()-aam_s.MinY();
	 aam_s.Translate(w, h);
	if(image == 0)image = cvCreateImage(cvSize(w*2,h*2), 8, 3);
	cvSet(image, cvScalar(128, 128, 128));
	g_cam->DrawAppearance(image, aam_s, t);

	cvNamedWindow("Combined Appearance Model",1);
	cvShowImage("Combined Appearance Model", image);

	if(cvWaitKey(10) == '27')
	{
		cvReleaseImage(&image);
		cvReleaseMat(&s);
		cvReleaseMat(&t);
		cvReleaseMat(&c);
		cvDestroyWindow("Parameters");
		cvDestroyWindow("Combined Appearance Model");
	}
}

//============================================================================
void AAM_CAM::ShowVariation()
{
	printf("Show modes of appearance variations...\n");
	cvNamedWindow("Parameters",1);

	//create trackbars for appearance
	for(int i = 0; i < n; i++)
	{
		char barname[100];
		sprintf(barname, "a %d", i);
		b_c[i] = offset;
		cvCreateTrackbar(barname, "Parameters", &b_c[i], 2*offset+1, ontrackcam);
	}

	g_cam = this;
	ontrackcam(1);
    cvWaitKey(0);
}
