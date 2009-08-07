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

#include <fstream>
#include "AAM_Util.h"
#include "AAM_Basic.h"
#include "AAM_IC.h"
using namespace std;

//============================================================================
std::ostream& operator<<(std::ostream &os, const CvMat* mat)
{
	assert(CV_MAT_TYPE(mat->type) == CV_64FC1);
	for(int i = 0; i < mat->rows; i++)
	{
		for(int j = 0; j < mat->cols; j++)
		{
			os << CV_MAT_ELEM(*mat, double, i, j) << " ";
		}
		os << std::endl;
	}
	return os;
}

//============================================================================
std::istream& operator>>(std::istream &is, CvMat* mat)
{
	assert(CV_MAT_TYPE(mat->type) == CV_64FC1);
	for(int i = 0; i < mat->rows; i++)
	{
		for(int j = 0; j < mat->cols; j++)
		{
			is >> CV_MAT_ELEM(*mat, double, i, j);
		}
	}
	return is;
}

//============================================================================
// compare function for the qsort() call below
static int str_compare(const void *arg1, const void *arg2)
{
    return strcmp((*(std::string*)arg1).c_str(), (*(std::string*)arg2).c_str());
}

#ifdef WIN32
file_lists AAM_Common::ScanNSortDirectory(const std::string &path, const std::string &extension)
{
    WIN32_FIND_DATA wfd;
    HANDLE hHandle;
    string searchPath, searchFile;
    file_lists vFilenames;
	int nbFiles = 0;

	searchPath = path + "/*" + extension;
	hHandle = FindFirstFile(searchPath.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == hHandle)
    {
		fprintf(stderr, "ERROR(%s, %d): Cannot find (*.%s)files in directory %s\n",
			__FILE__, __LINE__, extension.c_str(), path.c_str());
		exit(0);
    }
    do
    {
        //. or ..
        if (wfd.cFileName[0] == '.')
        {
            continue;
        }
        // if exists sub-directory
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
	    }
        else//if file
        {
			searchFile = path + "/" + wfd.cFileName;
			vFilenames.push_back(searchFile);
			nbFiles++;
		}
    }while (FindNextFile(hHandle, &wfd));

    FindClose(hHandle);

	// sort the filenames
    qsort((void *)&(vFilenames[0]), (size_t)nbFiles, sizeof(string), str_compare);

    return vFilenames;
}

#else

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_PATH 1024

static int match(const char* s1, const char* s2)
{
	int diff = strlen(s1) - strlen(s2);
	if(diff >= 0 && !strcmp(s1+diff, s2))
		return 1;
	return 0;
}

file_lists AAM_Common::ScanNSortDirectory(const std::string &path, const std::string &extension)
{
	struct dirent *d;
	DIR* dir;
	struct stat s;
	char fullpath[MAX_PATH];
	file_lists allfiles;
	int num = 0;

	dir = opendir(path.c_str());
	if(dir == NULL)
	{
		 fprintf(stderr, "Can not open directory %s\n", path.c_str());
		 exit(0);
	}

	while(d = readdir(dir))
	{
		sprintf(fullpath, "%s/%s", path.c_str(), d->d_name);
		if(stat(fullpath, &s) != -1)
		{
			if(S_ISDIR(s.st_mode))
				continue;
			if(match(d->d_name, extension.c_str()))
			{
				allfiles.push_back(std::string(fullpath));
				num++;
			}
		}

	}
	closedir(dir);
	qsort((void*)&(allfiles[0]), size_t(num), sizeof(std::string), str_compare);
	return allfiles;
}
#endif

//============================================================================
void AAM_Common::DrawPoints(IplImage* image, const AAM_Shape& Shape)
{
	for(int i = 0; i < Shape.NPoints(); i++)
	{
		cvCircle(image, cvPointFrom32f(Shape[i]), 3, CV_RGB(255, 0, 0));
	}
}


//============================================================================
void AAM_Common::DrawTriangles(IplImage* image, const AAM_Shape& Shape, const std::vector<std::vector<int> >&tris)
{
	int idx1, idx2, idx3;
	for(int i = 0; i < tris.size(); i++)
	{
		idx1 = tris[i][0];
		idx2 = tris[i][1];
		idx3 = tris[i][2];
		cvLine(image, cvPointFrom32f(Shape[idx1]), cvPointFrom32f(Shape[idx2]),
			CV_RGB(255,255,255));
		cvLine(image, cvPointFrom32f(Shape[idx2]), cvPointFrom32f(Shape[idx3]),
			CV_RGB(255,255,255));
		cvLine(image, cvPointFrom32f(Shape[idx3]), cvPointFrom32f(Shape[idx1]),
			CV_RGB(255,255,255));
	}
}

//============================================================================
void AAM_Common::DrawAppearance(IplImage*image, const AAM_Shape& Shape,
				const CvMat* t, const AAM_PAW& paw, const AAM_PAW& refpaw)
{
	int x1, x2, y1, y2, idx1, idx2;
	int xby3, idxby3;
	int minx, miny, maxx, maxy;
	int tri_idx, v1, v2, v3;
	byte* pimg;
	double* fastt = t->data.db;
	int nChannel = image->nChannels;
	const AAM_Shape& refShape = refpaw.__referenceshape;
	const std::vector<std::vector<int> >& tri = paw.__tri;
	const std::vector<std::vector<int> >& rect1 = paw.__rect;
	const std::vector<std::vector<int> >& rect2 = refpaw.__rect;
	const std::vector<int>& pixTri = paw.__pixTri;
	const std::vector<double>& alpha = paw.__alpha;
	const std::vector<double>& belta= paw.__belta;
	const std::vector<double>& gamma = paw.__gamma;

	minx = Shape.MinX(); miny = Shape.MinY();
	maxx = Shape.MaxX(); maxy = Shape.MaxY();
	for(int y = miny; y < maxy; y++)
	{
		y1 = y-miny;
		pimg = (byte*)(image->imageData + image->widthStep*y);
		for(int x = minx; x < maxx; x++)
		{
			x1 = x-minx;
			idx1 = rect1[y1][x1];
			if(idx1 >= 0)
			{
				tri_idx = pixTri[idx1];
				v1 = tri[tri_idx][0];
				v2 = tri[tri_idx][1];
				v3 = tri[tri_idx][2];

				x2 = alpha[idx1]*refShape[v1].x + belta[idx1]*refShape[v2].x +
					gamma[idx1]*refShape[v3].x;
				y2 = alpha[idx1]*refShape[v1].y + belta[idx1]*refShape[v2].y +
					gamma[idx1]*refShape[v3].y;

				idx2 = rect2[y2][x2];
				idxby3 = idx2 + (idx2<<1);	/* 3*idx2 */

				if(nChannel == 3)
				{
					xby3 = x + (x<<1);			/* 3*x */
					pimg[xby3  ] = fastt[idxby3  ];
					pimg[xby3+1] = fastt[idxby3+1];
					pimg[xby3+2] = fastt[idxby3+2];
				}
				else
				{
					pimg[x] = (fastt[idxby3]+fastt[idxby3+1]+fastt[idxby3+2])/3;
				}
			}
		}
	}
}

//===========================================================================
void AAM_Common::CheckShape(CvMat* s, int w, int h)
{
	double* fasts = s->data.db;
	int npoints = s->cols / 2;

	for(int i = 0; i < npoints; i++)
	{
		if(fasts[2*i] > w-1) fasts[2*i] = w-1;
		else if(fasts[2*i] < 0) fasts[2*i] = 0;

		if(fasts[2*i+1] > h-1) fasts[2*i+1] = h-1;
		else if(fasts[2*i+1] < 0) fasts[2*i+1] = 0;
	}
}

//============================================================================
AAM::AAM()
{
}

//============================================================================
AAM::~AAM()
{
}

//============================================================================
void AAM_Pyramid::BuildDetectMapping(const file_lists& pts_files,
							 const file_lists& img_files,
							 VJfacedetect& FaceDetect,
							 double refWidth)
{
	printf("########################################################\n");
	printf("Building Detect Mapping ...\n");

	int total = 0;
	__referenceWidth = refWidth;
	int nPoints = GetMeanShape().NPoints();
	__VJDetectShape.resize(nPoints);
	for(int i = 0; i < pts_files.size(); i++)
	{
		printf("%i of %i\r", i, pts_files.size());

		IplImage* image = cvLoadImage(img_files[i].c_str(), -1);

		std::vector<AAM_Shape> DetShape;
		bool flag = FaceDetect.DetectFace(DetShape, image);
		if(!flag) continue;

		AAM_Shape Shape;
		flag = Shape.ReadAnnotations(pts_files[i]);
		if(!flag)	Shape.ScaleXY(image->width, image->height);

		cvReleaseImage(&image);

		CvPoint2D32f  lt = DetShape[0][0], rb = DetShape[0][1];
		double x = (lt.x+rb.x)/2., y = (lt.y+rb.y)/2.;
		double w = (-lt.x+rb.x), h = (-lt.y+rb.y);

		Shape.Translate(-x, -y);
		Shape.ScaleXY(__referenceWidth/w, __referenceWidth/h);

		__VJDetectShape += Shape;
		total ++;
	}
	__VJDetectShape /= total;

	printf("########################################################\n");
}

//============================================================================
bool AAM_Pyramid::InitShapeFromDetBox(AAM_Shape& Shape,const IplImage* image , VJfacedetect& facedetect)
{
	std::vector<AAM_Shape> DetShape;

	bool flag = facedetect.DetectFace(DetShape, image);

	if(!flag)	return false;

	CvPoint2D32f  lt = DetShape[0][0], RB = DetShape[0][1];
	Shape = __VJDetectShape;
	Shape.ScaleXY((-lt.x+RB.x)/__referenceWidth, (-lt.y+RB.y)/__referenceWidth);
	Shape.Translate((lt.x+RB.x)/2., (lt.y+RB.y)/2. );
	return true;
}


//============================================================================
AAM_Pyramid::AAM_Pyramid()
{
	__model.resize(0);
}

//============================================================================
AAM_Pyramid::~AAM_Pyramid()
{
	for(int i = 0; i < __model.size(); i++)
		delete __model[i];
}

//============================================================================
void AAM_Pyramid::Build(const file_lists& pts_files,
						const file_lists& img_files,
						int type /* = TYPE_AAM_IC */,
						int level /* = 1 */)
{
	if(level <= 0)
	{
		fprintf(stderr, "ERROE(%s,%d): The pyramid level must be at least 1!\n",
			__FILE__, __LINE__);
		exit(0);
	}
	__model.resize(0);

	for(int i = 0; i < level; i++)
	{
		if(type == TYPE_AAM_BASIC)		__model.push_back(new AAM_Basic);
		else if(type == TYPE_AAM_IC)	__model.push_back(new AAM_IC);
		else
		{
			printf("Unsupported aam type!\n");
			exit(0);
		}
		__model[i]->Build(pts_files, img_files, 1.0/pow(2.0, i));
	}
}

//============================================================================
void AAM_Pyramid::Fit(const IplImage* image,
					  AAM_Shape& Shape,
					  int max_iter /* = 30 */,
					  bool showprocess /* = false */)
{
	// the images used during search
	int w = image->width;
	int h = image->height;

	int startlev = __model.size() - 1;
	int iter = max_iter/(startlev+1);
	//double PyrScale = pow(2.0, startlev);
	//Shape /= PyrScale;

	// for each level in the image pyramid
	for (int iLev = startlev; iLev > 0; iLev--)
    {
		printf("Level %d: ", iLev);
		//IplImage* fitimage = cvCreateImage(cvSize(w/PyrScale, h/PyrScale), image->depth, image->nChannels);
		//cvResize(image, fitimage);

		__model[iLev]->Fit(/*fit*/image, Shape, iter, showprocess);
		//cvReleaseImage(&fitimage);

		//PyrScale /= 2.0;
		//Shape *= 2.0;
	}
	printf("Level 0: ");
	__model[0]->Fit(image, Shape, iter, showprocess);
}


//void AAM_Pyramid::SetAllParamsZero()
//{
//	for(int i = 0; i < __model.size(); i++)
//		__model[i]->SetAllParamsZero();
//}

//============================================================================
const AAM_Shape AAM_Pyramid::GetMeanShape()const
{
	return __model[0]->GetMeanShape();
}

//============================================================================
void AAM_Pyramid::Draw(IplImage* image, const AAM_Shape& Shape, int type)
{
	__model[0]->Draw(image, Shape, type);
}

//============================================================================
void AAM_Pyramid::WriteModel(const std::string& filename)
{
	ofstream os(filename.c_str());
	if(!os){
		fprintf(stderr, "ERROR(%s, %d): CANNOT create model \"%s\"\n",
			__FILE__, __LINE__, filename.c_str());
		exit(0);
	}

	os << __model[0]->GetType() << " " << __model.size() << std::endl;

	for(int i = 0; i < __model.size(); i++)
	{
		printf("Writing (level %d) active appearance model to file...\n", i);
		__model[i]->Write(os);
	}
	printf("Done\n");

	os << __referenceWidth << std::endl;
	__VJDetectShape.Write(os);

	os.close();
}

//============================================================================
void AAM_Pyramid::ReadModel(const std::string& filename)
{
	ifstream is(filename.c_str());
	if(!is){
		fprintf(stderr, "ERROR(%s, %d): CANNOT load model \"%s\"\n",
			__FILE__, __LINE__, filename.c_str());
		exit(0);
	}

	int level, type;
	is >> type >> level;

	for(int i = 0; i < level; i++)
	{
		if(type == TYPE_AAM_BASIC)	__model.push_back(new AAM_Basic);
		else if(type == TYPE_AAM_IC)	__model.push_back(new AAM_IC);
		else {
			fprintf(stderr, "Unsupported AAM type\n");
			exit(0);
		}
		printf("Reading (level %d) active appearance model from file...\n", i);
		__model[i]->Read(is);
	}
	printf("Done\n");

	is >> __referenceWidth;
	__VJDetectShape.resize(GetMeanShape().NPoints());
	__VJDetectShape.Read(is);
	is.close();
}
