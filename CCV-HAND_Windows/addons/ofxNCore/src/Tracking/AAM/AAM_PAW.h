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
#ifndef AAM_PAW_H
#define AAM_PAW_H

#include "AAM_Util.h"
#include "AAM_Shape.h"

class AAM_IC;
class AAM_Basic;
class AAM_CAM;

// 2D piecewise affine warp
class AAM_PAW
{
	friend class AAM_IC;
	friend class AAM_Basic;
	friend class AAM_CAM;
	friend class AAM_Common;
public:
	AAM_PAW();
	~AAM_PAW();

	// Build a piecewise affine warp
	void Train(const AAM_Shape& ReferenceShape,
		CvMat* Points,
		CvMemStorage* Storage,
		const std::vector<std::vector<int> >* tri = 0,
		bool buildVtri = true);

	// Read data from stream
	void Read(std::ifstream& is);

	// Write data to stream
	void Write(std::ofstream& os);

	inline const int nPoints()const { return __n;	}
	inline const int nPix()const	{ return __nPixels;	}
	inline const int nTri()const	{ return __nTriangles;	}

	inline const CvPoint2D32f Vertex(int i)const { return __referenceshape[i];	}

	inline const std::vector<std::vector<int> >* GetTri()const{  return &__tri;	}

	// index of point for j-th vertex of i-th triangle
	inline const int Tri(int i, int j)const{ return __tri[i][j];	}

	// index of triangle the pixel lies in
	inline const int PixTri(int i)const{ return __pixTri[i];	}

	// Coeffients of affine warp
	inline const double Alpha(int i)const { return __alpha[i];	}
	inline const double Belta(int i)const { return __belta[i];	}
	inline const double Gamma(int i)const { return __gamma[i];	}

	// width and height boundary
	inline const int Width()const { return __width;	}
	inline const int Height()const { return __height;	}
	// Is point(j,i) in boundary: not(-1), yes(index of pixel)
	inline const int Rect(int i, int j)const { return __rect[i][j]; }

	// Warp the image to the mean shape mesh to calculate shape free patch
	void CalcWarpTexture(const CvMat* s, const IplImage* image,	CvMat* t)const;

	// For display, translate the texture vector to the visual-able image format
	// and then save into an image
	void TextureToImage(IplImage* image, const CvMat* t)const;
	void SaveWarpTextureToImage(const char* filename, const CvMat* t)const;

	// Calculate the warp parameters for a specific point according the piecewise
	static void CalcWarpParameters(double x, double y, double x1, double y1,
								double x2, double y2, double x3, double y3,
								double &alpha, double &belta, double &gamma);

	// Warp from source triangle to destination triangle
	static void Warp(double x, double y, double x1, double y1,
		double x2, double y2, double x3, double y3,
		double& X, double& Y, double X1, double Y1,
		double X2, double Y2, double X3, double Y3);

private:
	// Build triangles
	void Delaunay(const CvSubdiv2D* Subdiv, const CvMat *ConvexHull);

	// Calculate all pixels in the triangles of mean shape mesh
	void CalcPixelPoint(const CvRect rect, CvMat *ConvexHull);
	void FastCalcPixelPoint(const CvRect rect);

	// Is the current edge (ind1, ind2) already in the AAM model edges?
	static bool IsEdgeIn(int ind1, int ind2,
						const std::vector<std::vector<int> > &edges);

	// Help to build up triangles in the mesh
	static bool IsTriangleNotIn(const std::vector<int>& one_tri,
							const std::vector<std::vector<int> > &tris);

	// Find triangles containing each landmark.
	void FindVTri();

private:
	int __n;						/*number of landmarks*/
	int __nPixels;					/*number of pixels*/
	int __nTriangles;				/*number of triangles*/
	int __width, __height, __xmin, __ymin; /*Domain of warp region*/

	std::vector<std::vector<int> > __tri;	/*triangle vertexes index*/
	std::vector<std::vector<int> > __vtri;	/*vertex vs triangle*/
	std::vector<int>			   __pixTri;
	std::vector<double>			   __alpha, __belta,  __gamma;
	std::vector<std::vector<int> > __rect; /*height by width */

	AAM_Shape __referenceshape;



};

#endif //
