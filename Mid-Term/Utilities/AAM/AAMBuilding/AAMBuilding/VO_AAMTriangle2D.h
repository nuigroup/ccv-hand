/****************************************************************************
* File Name:        VO_AAMTriangle2D.h                                      *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     Triangle structure defined by 3 indexes and             *
*                   the coordinates of the 3 vertexes                       *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/


#ifndef __VO_AAMTriangle2D__
#define __VO_AAMTriangle2D__

#include <vector>

#include "cv.h"
#include "highgui.h"

#include "VO_Triangle2D.h"

using namespace std;


/****************************************************************************
* Class Name:       VO_AAMTriangle2D                                        *
* Inherited From:   VO_Triangle2D                                           *
* Author:           JIA Pei                                                 *
* Create Date:      2008-03-04                                              *
* Function:         Triangle structure defined by 3 indexes and             *
*                   the coordinates of the 3 vertexes                       *
****************************************************************************/
class VO_AAMTriangle2D : public VO_Triangle2D
{
private:
    /** Indexes of 3 triangle vertexes in the built AAM model vertex sequence */
    vector<int>             m_iVertexIndexes;

	/** Indexes of this triangle in the built AAM model triangle sequence */
	int						m_iTriangleIndex;

public:
    /** Default constructor to create a VO_AAMTriangle2D object */
    VO_AAMTriangle2D() {this->m_iVertexIndexes.resize(3);}

    /** Constructor to create a VO_AAMTriangle2D object with three coordinate vertexes in vector format */
    VO_AAMTriangle2D(const vector<CvPoint2D32f> &iVertexes):VO_Triangle2D(iVertexes)
    {
        this->m_iVertexIndexes.resize(3);
    }

    /** Constructor to create a VO_AAMTriangle2D object with three coordinate vertexes in a row in CvMat* format */
    VO_AAMTriangle2D(const CvMat* iVertexes):VO_Triangle2D(iVertexes)
    {
        this->m_iVertexIndexes.resize (3);
    }

    /** Constructor to create a VO_AAMTriangle2D object with three coordinate vertexes and 
        three corresponding vertex indexes in vector format */
    VO_AAMTriangle2D( const vector<CvPoint2D32f> &iVertexes, const vector<int> &iVertexIndexes )
        :VO_Triangle2D(iVertexes)
    {
        assert (iVertexIndexes.size () == 3); this->m_iVertexIndexes = iVertexIndexes;
    }

    /** Constructor to create a VO_AAMTriangle2D object with three coordinate vertexes in vector format
        and three corresponding vertex indexes in vector format */
    VO_AAMTriangle2D( const CvMat* iVertexes, const vector<int> &iVertexIndexes )
        :VO_Triangle2D(iVertexes)
    {
        assert (iVertexIndexes.size () == 3); this->m_iVertexIndexes = iVertexIndexes;
    }

    /** operator= overloading, similar to copy constructor */
    VO_AAMTriangle2D&           operator=(const VO_AAMTriangle2D &s)
    {
        this->m_iVertexIndexes    = s.GetVertexIndexes();
        this->SetdD( s.GetdD() );
        this->SetVertexes( s.GetVertexes() );
        return (*this);
    }

	/** Judge whether this triangle contains a vertex of index "iIndex" */
	bool Contains(int iIndex) const
	{
		if( (this->m_iVertexIndexes[0] == iIndex) || (this->m_iVertexIndexes[1] == iIndex) 
            || (this->m_iVertexIndexes[2] == iIndex) )
			return true;
		else
			return false;
	}

    /** Destructor */
    virtual ~VO_AAMTriangle2D() {this->m_iVertexIndexes.clear ();}

    /** Get three indexes of three vertexes as a vector */
    vector<int> GetVertexIndexes() const { return this->m_iVertexIndexes;}

    /** Get one index of one vertex, in the built AAM model vertex sequence. Apparently, ptIdx could only be 0,1,2 */
	int GetVertexIndex(int ptIdx) const {	return this->m_iVertexIndexes[ptIdx];}

    /** Get the index of this triangle, in the built AAM model triangle sequence */
	int GetTriangleIndex() const { return this->m_iTriangleIndex;}

    /** Set the indexes of three vertexes of this triangle */
    void SetVertexIndexes(const vector<int> &iVertexes) { this->m_iVertexIndexes = iVertexes;}

    /** Set the index of this triangle, in the built AAM model triangle sequence */
	void SetTriangleIndex(int iTriangleIndex) { this->m_iTriangleIndex = iTriangleIndex;}

};

#endif  // __VO_AAMTriangle2D__

