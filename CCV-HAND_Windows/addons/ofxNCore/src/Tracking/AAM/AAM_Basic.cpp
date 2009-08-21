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
#include <ctime>
#ifdef TARGET_WIN32
	#include <direct.h>
	#include <io.h>
#endif
#ifdef TARGET_UNIX
	#include <sys/stat.h>
#endif
#include "AAM_Basic.h"

#include <cv.h>
#include <highgui.h>
//============================================================================
AAM_Basic::AAM_Basic()
{
	__G = 0;
	__current_c_q = 0;
	__update_c_q = 0;
	__delta_c_q = 0;
	__c = 0;
	__p = 0;
	__q = 0;
	__lamda = 0;
	__s = 0;
	__t_m = 0;
	__t_s = 0;
	__delta_t = 0;
}

//============================================================================
AAM_Basic::~AAM_Basic()
{
	cvReleaseMat(&__G);				cvReleaseMat(&__current_c_q);
	cvReleaseMat(&__update_c_q);	cvReleaseMat(&__delta_c_q);
	cvReleaseMat(&__p);				cvReleaseMat(&__q);
	cvReleaseMat(&__c);				cvReleaseMat(&__lamda);
	cvReleaseMat(&__s);				cvReleaseMat(&__t_s);
	cvReleaseMat(&__t_m);			cvReleaseMat(&__delta_t);
}

//============================================================================
void AAM_Basic::Train(const file_lists& pts_files,
					  const file_lists& img_files,
					  double scale /* = 1.0 */,
					  double shape_percentage /* = 0.975 */,
					  double texture_percentage /* = 0.975 */,
					  double appearance_percentage /* = 0.975 */)
{
	if(pts_files.size() != img_files.size())
	{
		fprintf(stderr, "ERROE(%s, %d): #Shapes != #Images\n",
			__FILE__, __LINE__);
		exit(0);
	}

	printf("################################################\n");
	printf("Build Fixed Jocobian Active Appearance Model ...\n");

	__cam.Train(pts_files, img_files, scale, shape_percentage,
		texture_percentage, appearance_percentage);

	printf("Build Jacobian Matrix...\n");
	__G = cvCreateMat(__cam.nModes()+4, __cam.__texture.nPixels(), CV_64FC1);
	CalcJacobianMatrix(pts_files, img_files);

	//allocate memory for on-line fitting
	__current_c_q = cvCreateMat(1, __cam.nModes()+4, CV_64FC1);
	__update_c_q = cvCreateMat(1, __cam.nModes()+4, CV_64FC1);
	__delta_c_q = cvCreateMat(1, __cam.nModes()+4, CV_64FC1);
	__c = cvCreateMat(1, __cam.nModes(), CV_64FC1);
	__p = cvCreateMat(1, __cam.__shape.nModes(), CV_64FC1);
	__q = cvCreateMat(1, 4, CV_64FC1);
	__lamda = cvCreateMat(1, __cam.__texture.nModes(), CV_64FC1);
	__s = cvCreateMat(1, __cam.__shape.nPoints()*2, CV_64FC1);
	__t_s = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	__t_m = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	__delta_t = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);

	printf("################################################\n\n");
}

//============================================================================
static double rand_in_between(double a, double b)
{
	int A = rand() % 50;
	return a + (b-a)*A/49;
}

//============================================================================
void AAM_Basic::CalcJacobianMatrix(const file_lists& pts_files,
								   const file_lists& img_files,
								   double disp_scale /* = 0.2 */,
								   double disp_angle /* = 20 */,
								   double disp_trans /* = 5.0 */,
								   double disp_std /* = 1.0 */,
								   int nExp /* = 30 */)
{
	CvMat* J = cvCreateMat(__cam.nModes()+4, __cam.__texture.nPixels(), CV_64FC1);
	CvMat* d = cvCreateMat(1, __cam.nModes()+4, CV_64FC1);
	CvMat* o = cvCreateMat(1, __cam.nModes()+4, CV_64FC1);
	CvMat* oo = cvCreateMat(1, __cam.nModes()+4, CV_64FC1);
	CvMat* t = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	CvMat* t_m = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	CvMat* t_s = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	CvMat* t1 = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	CvMat* t2 = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	CvMat* u = cvCreateMat(1, __cam.nModes()+4, CV_64FC1);
	CvMat* c = cvCreateMat(1, __cam.nModes(), CV_64FC1);
    CvMat* s = cvCreateMat(1, __cam.__shape.nPoints()*2, CV_64FC1);
    CvMat* q = cvCreateMat(1, 4, CV_64FC1);
	CvMat* p = cvCreateMat(1, __cam.__shape.nModes(),CV_64FC1);
	CvMat* lamda = cvCreateMat(1, __cam.__texture.nModes(), CV_64FC1);

	double theta = disp_angle * CV_PI / 180;
	double aa = MAX(fabs(disp_scale*cos(theta)), fabs(disp_scale*sin(theta)));
	cvmSet(d,0,0,aa); cvmSet(d,0,1,aa); cvmSet(d,0,2,disp_trans); cvmSet(d,0,3,disp_trans);
	for(int nmode = 0; nmode < __cam.nModes(); nmode++)
		cvmSet(d,0,4+nmode,disp_std*sqrt(__cam.Var(nmode)));

	srand(unsigned(time(0)));
	cvSetZero(u);cvSetZero(J);
	for(int i = 0; i < pts_files.size(); i++)
	{
		IplImage* image = cvLoadImage(img_files[i].c_str(), -1);
		AAM_Shape Shape;
		if(!Shape.ReadAnnotations(pts_files[i]))
			Shape.ScaleXY(image->width, image->height);
		Shape.Point2Mat(s);

		//calculate current texture vector
		__cam.__paw.CalcWarpTexture(s, image, t);
		__cam.__texture.NormalizeTexture(__cam.__MeanG, t);

		//calculate appearance parameters
		__cam.__shape.CalcParams(s, p, q);
		__cam.__texture.CalcParams(t, lamda);
		__cam.CalcParams(c, p, lamda);

		//update appearance and pose parameters
		CvMat subo;
		cvGetCols(o, &subo, 0, 4); cvCopy(q, &subo);
		cvGetCols(o, &subo, 4, 4+__cam.nModes()); cvCopy(c, &subo);

		//get optimal EstResidual
		EstResidual(image, o, s, t_m, t_s, t1);

		for(int j = 0; j < nExp; j++)
		{
			printf("Pertubing (%d/%d) for image (%d/%d)...\r", j, nExp, i, pts_files.size());

			for(int l = 0; l < 4+__cam.nModes(); l++)
			{
				double D = cvmGet(d,0,l);
				double v = rand_in_between(-D, D);
				cvCopy(o, oo); CV_MAT_ELEM(*oo,double,0,l) += v;
				EstResidual(image, oo, s, t_m, t_s, t2);

				cvSub(t1, t2, t2);
				cvConvertScale(t2, t2, 1.0/v);

				//accumulate into l-th row
				CvMat Jl; cvGetRow(J, &Jl, l);
				cvAdd(&Jl, t2, &Jl);

				CV_MAT_ELEM(*u, double, 0, l) += 1.0;
			}
		}
		cvReleaseImage(&image);
	}

	//normalize
	for(int l = 0; l < __cam.nModes()+4; l++)
	{
		CvMat Jl; cvGetRow(J, &Jl, l);
		cvConvertScale(&Jl, &Jl, 1.0/cvmGet(u,0,l));
	}

	CvMat* JtJ = cvCreateMat(__cam.nModes()+4, __cam.nModes()+4, CV_64FC1);
	CvMat* InvJtJ = cvCreateMat(__cam.nModes()+4, __cam.nModes()+4, CV_64FC1);
	cvGEMM(J, J, 1, NULL, 0, JtJ, CV_GEMM_B_T);
    cvInvert(JtJ, InvJtJ, CV_SVD);
	cvMatMul(InvJtJ, J, __G);

	cvReleaseMat(&J);	cvReleaseMat(&d); 	cvReleaseMat(&o);
	cvReleaseMat(&oo); 	cvReleaseMat(&t);	cvReleaseMat(&t_s);
	cvReleaseMat(&t_m);	cvReleaseMat(&t1); 	cvReleaseMat(&t2);
	cvReleaseMat(&u); 	cvReleaseMat(&c);	cvReleaseMat(&s);
	cvReleaseMat(&q); 	cvReleaseMat(&p);	cvReleaseMat(&lamda);
	cvReleaseMat(&JtJ); cvReleaseMat(&InvJtJ);
}


//============================================================================
double AAM_Basic::EstResidual(const IplImage* image, const CvMat* c_q,
						   CvMat* s, CvMat* t_m,
						   CvMat* t_s, CvMat* deltat)
{
	CvMat c, q;
	cvGetCols(c_q, &q, 0, 4);
	cvGetCols(c_q, &c, 4, 4+__cam.nModes());

	// generate model texture
	__cam.CalcTexture(t_m, &c);

	// generate model shape
	__cam.CalcShape(s, c_q);

	// generate warped texture
	AAM_Common::CheckShape(s, image->width, image->height);
	__cam.__paw.CalcWarpTexture(s, image, t_s);
	__cam.__texture.NormalizeTexture(__cam.__MeanG, t_s);

	// calculate pixel difference
	cvSub(t_m, t_s, deltat);

	return cvNorm(deltat);
}

//============================================================================
void AAM_Basic::SetAllParamsZero()
{
	cvSetZero(__q);
	cvSetZero(__c);
}

//============================================================================
void AAM_Basic::InitParams(const IplImage* image)
{
	//shape parameter
	__cam.__shape.CalcParams(__s, __p, __q);

	//texture parameter
	__cam.__paw.CalcWarpTexture(__s, image, __t_s);
	__cam.__texture.NormalizeTexture(__cam.__MeanG, __t_s);
	__cam.__texture.CalcParams(__t_s, __lamda);

	//combined appearance parameter
	__cam.CalcParams(__c, __p, __lamda);
}

//============================================================================
void AAM_Basic::Fit(const IplImage* image, AAM_Shape& Shape,
					int max_iter /* = 30 */,bool showprocess /* = false */)
{
	//intial some stuff
	double t = gettime;
	double e1, e2;
	const int np = 5;
	double k_values[np] = {1, 0.5, 0.25, 0.125, 0.0625};
	int k;
	IplImage* Drawimg = 0;

	Shape.Point2Mat(__s);
	InitParams(image);
	CvMat subcq;
	cvGetCols(__current_c_q, &subcq, 0, 4); cvCopy(__q, &subcq);
	cvGetCols(__current_c_q, &subcq, 4, 4+__cam.nModes()); cvCopy(__c, &subcq);

	//calculate error
	e1 = EstResidual(image, __current_c_q, __s, __t_m, __t_s, __delta_t);

	//do a number of iteration until convergence
	for(int iter = 0; iter <max_iter; iter++)
	{
		if(showprocess)
		{
			if(Drawimg == 0)	Drawimg = cvCloneImage(image);
			else cvCopy(image, Drawimg);
			__cam.CalcShape(__s, __current_c_q);
			Shape.Mat2Point(__s);
			Draw(Drawimg, Shape, 2);
						#ifdef TARGET_WIN32
		mkdir("result");
    #endif

	#ifdef TARGET_UNIX

			mkdir("result", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#endif
			char filename[100];
			sprintf(filename, "result/ter%d.bmp", iter);
			cvSaveImage(filename, Drawimg);
		}

		// predict parameter update
		cvGEMM(__delta_t, __G, 1, NULL, 0, __delta_c_q, CV_GEMM_B_T);

		//force first iteration
		if(iter == 0)
		{
			cvAdd(__current_c_q, __delta_c_q, __current_c_q);
			CvMat c; cvGetCols(__current_c_q, &c, 4, 4+__cam.nModes());
			//constrain parameters
			__cam.Clamp(&c);
			e1 = EstResidual(image, __current_c_q, __s, __t_m, __t_s, __delta_t);
		}

		//find largest step size which reduces texture EstResidual
		else
		{
			for(k = 0; k < np; k++)
			{
				cvScaleAdd(__delta_c_q, cvScalar(k_values[k]), __current_c_q,  __update_c_q);
				//constrain parameters
				CvMat c; cvGetCols(__update_c_q, &c, 4, 4+__cam.nModes());
				__cam.Clamp(&c);

				e2 = EstResidual(image, __update_c_q, __s, __t_m, __t_s, __delta_t);
				if(e2 <= e1)	break;
			}
		}

		//check for convergence
		if(iter > 0)
		{
			if(k == np)
			{
				e1 = e2;
				cvCopy(__update_c_q, __current_c_q);
			}

			else if(fabs(e2-e1)<0.001*e1)	break;
			else if (cvNorm(__delta_c_q)<0.001)	break;
			else
			{
				cvCopy(__update_c_q, __current_c_q);
				e1 = e2;
			}
		}
	}

	cvReleaseImage(&Drawimg);
	__cam.CalcShape(__s, __current_c_q);
	Shape.Mat2Point(__s);
	t = gettime - t;
	printf("AAM-Basic Fitting time cost: %.3f millisec\n", t);
}


//===========================================================================
void AAM_Basic::Draw(IplImage* image, const AAM_Shape& Shape, int type)
{
    printf("AAM drawing(freedom!)");
	if(type == 0) AAM_Common::DrawPoints(image, Shape);
	else if(type == 1) AAM_Common::DrawTriangles(image, Shape, __cam.__paw.__tri);
	else if(type == 2)
	{
		double minV, maxV;
		cvMinMaxLoc(__t_m, &minV, &maxV);
		cvConvertScale(__t_m, __t_m, 255/(maxV-minV), -minV*255/(maxV-minV));
		AAM_PAW paw;
		paw.Train(Shape, __cam.__Points, __cam.__Storage, __cam.__paw.GetTri(), false);
		AAM_Common::DrawAppearance(image, Shape, __t_m, paw, __cam.__paw);
	}
	else fprintf(stderr, "ERROR(%s, %d): Unsupported drawing type\n",
		__FILE__, __LINE__);
}

//===========================================================================
void AAM_Basic::Write(std::ofstream& os)
{
	__cam.Write(os);
	os << __G;
}

//===========================================================================
void AAM_Basic::Read(std::ifstream& is)
{
	__cam.Read(is);
	 __G = cvCreateMat(__cam.nModes()+4, __cam.__texture.nPixels(), CV_64FC1);
	 is >> __G;

	 //allocate memory for on-line fitting
	__current_c_q = cvCreateMat(1, __cam.nModes()+4, CV_64FC1);
	__update_c_q = cvCreateMat(1, __cam.nModes()+4, CV_64FC1);
	__delta_c_q = cvCreateMat(1, __cam.nModes()+4, CV_64FC1);
	__c = cvCreateMat(1, __cam.nModes(), CV_64FC1);
	__p = cvCreateMat(1, __cam.__shape.nModes(), CV_64FC1);
	__q = cvCreateMat(1, 4, CV_64FC1);
	__lamda = cvCreateMat(1, __cam.__texture.nModes(), CV_64FC1);
	__s = cvCreateMat(1, __cam.__shape.nPoints()*2, CV_64FC1);
	__t_s = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	__t_m = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	__delta_t = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
}
