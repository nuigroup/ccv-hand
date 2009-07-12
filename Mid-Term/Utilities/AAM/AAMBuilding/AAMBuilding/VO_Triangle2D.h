/****************************************************************************
* File Name:        VO_Triangle2D.h                                         *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     Coordinates of three vertexes, cached denominator       *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/



#ifndef __VO_Triangle2D__
#define __VO_Triangle2D__

#include <vector>

#include "cv.h"
#include "highgui.h"

using namespace std;


/****************************************************************************
* Class Name:       VO_Triangle2D                                           *
* Author:           JIA Pei                                                 *
* Create Date:      2008-03-04                                              *
* Function:         Coordinates of three vertexes, cached denominator       *
****************************************************************************/
class VO_Triangle2D
{
protected:

    /** Coordinates of three vertexes */
    vector<CvPoint2D32f>    m_vVertexes;

    /** Cached denominator for later Jacobian calculation */
    float                   m_dD;

    /** private function to calculate cached denominator */
	float Calc_dD()
    {
        float x1, x2, x3, y1, y2, y3;

        x1 = this->m_vVertexes[0].x;
        x2 = this->m_vVertexes[1].x;
        x3 = this->m_vVertexes[2].x;
        y1 = this->m_vVertexes[0].y;
        y2 = this->m_vVertexes[1].y;
        y3 = this->m_vVertexes[2].y;

        return (+x2*y3-x2*y1-x1*y3-x3*y2+x3*y1+x1*y2);
    }

public:
    /** Default constructor to create a VO_Triangle2D object */
    VO_Triangle2D() {this->m_vVertexes.resize (3); this->m_dD = 0;}

    /** Constructor to create a VO_Triangle2D object with three coordinate vertexes in vector format */
    VO_Triangle2D(const vector<CvPoint2D32f> &iVertexes)
    {
        assert (iVertexes.size () == 3);
        this->m_vVertexes.clear ();
        this->m_vVertexes = iVertexes;
        this->m_dD = this->Calc_dD ();
    }

    /** Constructor to create a VO_Triangle2D object with three coordinate vertexes in a row in CvMat* format */
    VO_Triangle2D(const CvMat* iVertexes)
    {
        assert (iVertexes->cols == 3);
        this->m_vVertexes.resize (3);
        for (unsigned int i = 0; i < this->m_vVertexes.size (); i++)
        {
            this->m_vVertexes[i] = CV_MAT_ELEM( *iVertexes, CvPoint2D32f, 0, i );
        }
        this->m_dD = this->Calc_dD ();
    }

    /** Destructor */
    virtual ~VO_Triangle2D() {this->m_vVertexes.clear(); }

    /** operator= overloading, similar to copy constructor */
    VO_Triangle2D&           operator=(const VO_Triangle2D &s)
    {
        this->m_vVertexes   = s.GetVertexes();
        this->m_dD          = s.GetdD();
        return (*this);
    }

    /** Get coordinates of three vertexes */
	vector<CvPoint2D32f>    GetVertexes() const { return this->m_vVertexes;}

    /** Get one vertex' coordinates in terms of index */
    CvPoint2D32f            GetVertex(int index) const { return this->m_vVertexes[index];}

    /** Get cached denominator of this triangle */
    float                   GetdD() const { return this->m_dD;}

    /** Set cached denominator for this triangle */
    void                    SetdD(float idD) { this->m_dD = idD;}

    /** Set coordinates with three coordinate vertexes in vector format */
    void                    SetVertexes(const vector<CvPoint2D32f> &iVertexes) { this->m_vVertexes = iVertexes;}

    /** Set coordinates with three coordinate vertexes in a row in CvMat* format */
    void                    SetVertexes(const  CvMat* iVertexes) 
    {
        assert (iVertexes->cols == 3);
        this->m_vVertexes.resize (3);
        for (unsigned int i = 0; i < this->m_vVertexes.size (); i++)
        {
            this->m_vVertexes[i] = CV_MAT_ELEM( *iVertexes, CvPoint2D32f, 0, i );
        }
    }
};

#endif  // __VO_Triangle2D__

